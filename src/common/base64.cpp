#include "base64.h"
#include <string.h>
//base64��/�����õĻ����ַ���
const char kBase64Alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";
/*******************************************************************************
* ����: base64_encode
* ����: ascii����Ϊbase64��ʽ
* �β�: bindata : ascii�ַ�������
*            base64 : base64�ַ������
*          binlength : bindata�ĳ���
* ����: base64�ַ�������
* ˵��: ��
******************************************************************************/
int base64_encode(const unsigned char *bindata, char *base64, int binlength)
{
	int i, j;
	unsigned char current;
	for (i = 0, j = 0; i < binlength; i += 3)
	{
		current = (bindata[i] >> 2);
		current &= (unsigned char)0x3F;
		base64[j++] = kBase64Alphabet[(int)current];
		current = ((unsigned char)(bindata[i] << 4)) & ((unsigned char)0x30);
		if (i + 1 >= binlength)
		{
			base64[j++] = kBase64Alphabet[(int)current];
			base64[j++] = '=';
			base64[j++] = '=';
			break;
		}
		current |= ((unsigned char)(bindata[i + 1] >> 4)) & ((unsigned char)0x0F);
		base64[j++] = kBase64Alphabet[(int)current];
		current = ((unsigned char)(bindata[i + 1] << 2)) & ((unsigned char)0x3C);
		if (i + 2 >= binlength)
		{
			base64[j++] = kBase64Alphabet[(int)current];
			base64[j++] = '=';
			break;
		}
		current |= ((unsigned char)(bindata[i + 2] >> 6)) & ((unsigned char)0x03);
		base64[j++] = kBase64Alphabet[(int)current];
		current = ((unsigned char)bindata[i + 2]) & ((unsigned char)0x3F);
		base64[j++] = kBase64Alphabet[(int)current];
	}
	base64[j] = '\0';
	return j;
}
/*******************************************************************************
* ����: base64_decode
* ����: base64��ʽ����Ϊascii
* �β�: base64 : base64�ַ�������
*            bindata : ascii�ַ������
* ����: ���������ascii�ַ�������
* ˵��: ��
******************************************************************************/
int base64_decode(const char *base64, unsigned char *bindata)
{
	int i, j;
	unsigned char k;
	unsigned char temp[4];
	for (i = 0, j = 0; base64[i] != '\0'; i += 4)
	{
		memset(temp, 0xFF, sizeof(temp));
		for (k = 0; k < 64; k++)
		{
			if (kBase64Alphabet[k] == base64[i])
				temp[0] = k;
		}
		for (k = 0; k < 64; k++)
		{
			if (kBase64Alphabet[k] == base64[i + 1])
				temp[1] = k;
		}
		for (k = 0; k < 64; k++)
		{
			if (kBase64Alphabet[k] == base64[i + 2])
				temp[2] = k;
		}
		for (k = 0; k < 64; k++)
		{
			if (kBase64Alphabet[k] == base64[i + 3])
				temp[3] = k;
		}
		bindata[j++] = ((unsigned char)(((unsigned char)(temp[0] << 2)) & 0xFC)) | \
			((unsigned char)((unsigned char)(temp[1] >> 4) & 0x03));
		if (base64[i + 2] == '=')
			break;
		bindata[j++] = ((unsigned char)(((unsigned char)(temp[1] << 4)) & 0xF0)) | \
			((unsigned char)((unsigned char)(temp[2] >> 2) & 0x0F));
		if (base64[i + 3] == '=')
			break;
		bindata[j++] = ((unsigned char)(((unsigned char)(temp[2] << 6)) & 0xF0)) | \
			((unsigned char)(temp[3] & 0x3F));
	}
	return j;
}