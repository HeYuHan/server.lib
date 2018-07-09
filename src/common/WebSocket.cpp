#include "WebSocket.h"
#include "common.h"

#ifndef WIN32
#include <netinet/in.h>
#else
#include <winsock.h>
#endif // !WIN32
#include <string.h>
USING_NS_CORE
enum WS_Status
{
	WS_STATUS_CONNECT = 0,
	WS_STATUS_UNCONNECT = 1,
};
void inverted_string(char *str, int len)
{
	int i; char temp;
	for (i = 0; i<len / 2; ++i)
	{
		temp = *(str + i);
		*(str + i) = *(str + len - i - 1);
		*(str + len - i - 1) = temp;
	}
}



WebSokcetParser::WebSokcetParser()
{
}
int WebSokcetParser::DecodeFrame(char * frameData, int frameSize, int &outHead, int &outSize)
{
	outHead = 0;
	outSize = 0;
	int ret = WS_PARSE_RESULT_OK;
	const int frameLength = frameSize;
	// 检查扩展位并忽略

	if ((frameData[0] & 0x70) != 0x0)
	{
		ret = WS_PARSE_RESULT_ERROR;
	}

	// fin位: 为1表示已接收完整报文, 为0表示继续监听后续报文
	int fin = (frameData[0] & 0x80);
	if (fin == 0)
	{
		ret = WS_PARSE_RESULT_WAIT_NETX_FRAME;
	}
	else if (frameLength < 2)
	{
		outHead = 0;
		outSize = 0;
		return WS_PARSE_RESULT_WAIT_NEXT_DATA;
	}
	// mask位, 为1表示数据被加密
	if ((frameData[1] & 0x80) != 0x80)
	{
		ret = WS_PARSE_RESULT_ERROR;
	}
	// 操作码
	unsigned long long payloadLength = 0;
	int payloadFieldExtraBytes = 0;
	char opcode = frameData[0] & 0x0f;
	if (opcode == WS_TEXT_FRAME || opcode == WS_BINARY_FRAME)
	{
		// 处理utf-8编码的文本帧
		payloadLength = frameData[1] & 0x7f;
		if (payloadLength == 126)
		{
			payloadFieldExtraBytes = 2;
			payloadLength = (frameData[2] & 0xFF) << 8 | (frameData[3] & 0xFF);
		}
		else if (payloadLength == 127)
		{
			payloadFieldExtraBytes = 8;
			inverted_string(&frameData[2], 8);
			memcpy(&payloadLength, &frameData[2], 8);
		}
		else if(payloadLength > 127)
		{
			ret = WS_PARSE_RESULT_ERROR;
			//too long
		}
	}
	else if (opcode == WS_PING_FRAME || opcode == WS_PONG_FRAME)
	{
		outHead = 1;
		outSize = 0;
		ret = WS_PARSE_RESULT_SKIP;
	}
	else if (opcode == WS_CLOSING_FRAME)
	{
		ret = WS_PARSE_RESULT_ERROR;
	}
	else
	{
		ret = WS_PARSE_RESULT_ERROR;
	}
	if (payloadLength > (unsigned long long)(frameLength - (2 + 4 + payloadFieldExtraBytes)))
	{
		outHead = 0;
		outSize = 0;
		ret = WS_PARSE_RESULT_WAIT_NEXT_DATA;
	}
	// 数据解码
	if ((ret == WS_PARSE_RESULT_OK||ret == WS_PARSE_RESULT_WAIT_NETX_FRAME) && (payloadLength > 0))
	{
		// header: 2字节, masking key: 4字节
		const char *maskingKey = &frameData[2 + payloadFieldExtraBytes];
		char *payloadData = &frameData[2 + payloadFieldExtraBytes+4];
		for (size_t i = 0; i < payloadLength; i++)
		{
			payloadData[i] = payloadData[i] ^ maskingKey[i % 4];
		}
		outSize = payloadLength;
		outHead = 2 + payloadFieldExtraBytes + 4;
	}

	return ret;
}

int WebSokcetParser::EncodeFrame(char * frameData, int frameSize, int emptySize, int &outSize)
{
	outSize = 0;
	int ret = WS_EMPTY_FRAME;
	const int messageLength = frameSize;
	uint8 payloadFieldExtraBytes = 0;
	// header: 2字节, mask位设置为0(不加密), 则后面的masking key无须填写, 省略4字节
	//uint8 frameHeaderSize = 2;
	char frameHeader[12] = { 0 };

	// fin位为1, 扩展位为0, 操作位为frameType
	frameHeader[0] = static_cast<uint8>(0x80 | WS_TEXT_FRAME);

	// 填充数据长度
	if (messageLength <126)
	{
		payloadFieldExtraBytes = 0;
		frameHeader[1] = static_cast<uint8>(messageLength);
	}
	else if(messageLength < 0xffff)
	{
		frameHeader[1] = 126;
		payloadFieldExtraBytes = 2;
		frameHeader[2] = (messageLength >> 8 & 0xFF);
		frameHeader[3] = (messageLength & 0xFF);
	}
	else
	{
		payloadFieldExtraBytes = 10;
		memcpy(&frameHeader[2], &messageLength, sizeof(unsigned long long));
		inverted_string(&frameHeader[2], sizeof(unsigned long long));
	}
	if (emptySize < (2 + payloadFieldExtraBytes))
	{
		return WS_ERROR_FRAME;
	}
	memmove(&frameData[2 + payloadFieldExtraBytes], frameData, messageLength);
	memcpy(frameData, frameHeader, 2 + payloadFieldExtraBytes);
	outSize = messageLength + 2 + payloadFieldExtraBytes;
	return ret;
}

//int wsEncodeFrame(string inMessage, string &outFrame, enum WS_FrameType frameType)
//{
//	int ret = WS_EMPTY_FRAME;
//	const uint32_t messageLength = inMessage.size();
//	if (messageLength > 32767)
//	{
//		// 暂不支持这么长的数据
//		return WS_ERROR_FRAME;
//	}
//
//	uint8_t payloadFieldExtraBytes = (messageLength <= 0x7d) ? 0 : 2;
//	// header: 2字节, mask位设置为0(不加密), 则后面的masking key无须填写, 省略4字节
//	uint8_t frameHeaderSize = 2 + payloadFieldExtraBytes;
//	uint8_t *frameHeader = new uint8_t[frameHeaderSize];
//	memset(frameHeader, 0, frameHeaderSize);
//	// fin位为1, 扩展位为0, 操作位为frameType
//	frameHeader[0] = static_cast<uint8_t>(0x80 | frameType);
//
//	// 填充数据长度
//	if (messageLength <= 0x7d)
//	{
//		frameHeader[1] = static_cast<uint8_t>(messageLength);
//	}
//	else
//	{
//		frameHeader[1] = 0x7e;
//		uint16_t len = htons(messageLength);
//		memcpy(&frameHeader[2], &len, payloadFieldExtraBytes);
//	}
//
//	// 填充数据
//	uint32_t frameSize = frameHeaderSize + messageLength;
//	char *frame = new char[frameSize + 1];
//	memcpy(frame, frameHeader, frameHeaderSize);
//	memcpy(frame + frameHeaderSize, inMessage.c_str(), messageLength);
//	frame[frameSize] = '\0';
//	outFrame = frame;
//
//	delete[] frame;
//	delete[] frameHeader;
//	return ret;
//}