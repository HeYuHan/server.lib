
json = require "common"

InitRedisScript('127.0.0.1:6379')








Client = {socket = nil,uid=0}
Client.__index = Client
function Client:new(o)
    o = o or {}
    setmetatable(o,self)
    self.__index = self
    o.socket:RegisterCallBack(o)
    
    o.uid = o.socket.uid
    o.timer = Timer()
    o.timer:OnRegister(o)
    o.timer:Init(1,true)
    o.timer:Begin()
    return o
end
function Client:OnUpdate(frame)
    print('client update frame:' .. tostring(frame))
end
function Client:OnConnected()
    print('client connected uid:' .. tostring(self.uid))
    self:Send(json.encode({key1="1234",key2=234,key3=true}))
end
function Client:OnDisconnected()
    print('client disconnected uid:' .. tostring(self.uid))
    self.timer:Stop()
    self.timer = nil
    self.socket = nil
    collectgarbage("collect")
end
function Client:OnMessage(msg)
    print('client uid:' .. tostring(self.uid) .. ' message => ' .. msg)
end
function Client:Send(msg)

    print('send msg:--' .. msg)
    if self.socket then
        if type(msg) == 'table' then
            print('send' .. json.encode(msg))
            self.socket:Send(json.encode(msg))
        else
            self.socket:Send(msg)
        end
    end
end





local server = WebListenner()
server:Listen('0.0.0.0:9796',10)

local function OnServerAccept(socket)
    print('accept client uid' .. tostring(socket.uid))
    local client = Client:new({socket = socket})
   
end



server:RegisterCallBack({OnAccept = OnServerAccept})
REDIS_CMD_HASH = 1
REDIS_CMD_STRING = 2
REDIS_CMD_SORT_STRING =3

db = RedisHelper()
ok = db:Connect('127.0.0.1:6379')
print('connect db ' .. tostring(ok))
res = RedisResponse()
db:SaveValue(REDIS_CMD_HASH,res,'testMap','testKey','testValue456')
db:GetValue(REDIS_CMD_HASH,res,'testMap','testKey')
if res:Valid() then
    print(res:String())
end
GetUserGuid(db,res)
if res:Valid() then
    log_info('get_guid: ' .. res:String())
end

SaveSession(db,res,'card','id123','testValue',20)
SaveRecoderFile(db,res,'testFile')
if res:Valid() then
    log_info('save session: ' .. res:String())
end

db2 = RedisHelper()

RedisCallbackHandle = {}
function RedisCallbackHandle:OnMessage(res)
    if res:Valid() then
        print('async message:' .. res:String())
    end
end

function RedisCallbackHandle:OnConnect(status)
    print('connect status:' .. tostring(status))
end

function RedisCallbackHandle:OnDisconnect(status)
end

handle = RedisCallback()

handle:OnRegister(RedisCallbackHandle)

ok = db2:AsyncConnect('127.0.0.1:6379',handle)

db2:AsyncGetValue(handle,REDIS_CMD_HASH,'testMap','testKey')
AsyncHttp = {}
function AsyncHttp:OnMessage()
    print('body->' .. self.http.status)
end
InitMessageQueue()
-- http = Http('www.baidu.com',80,500)
-- http:OnRegister(AsyncHttp)
-- AsyncHttp.http = http
-- ok = http:AsyncGet('/')



Timer:Loop()