
json = require "json"
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
        self.socket:Send(msg)
    end
end





local server = WebListenner()
server:Listen('0.0.0.0:9796',10)

local function OnServerAccept(socket)
    print('accept client uid' .. tostring(socket.uid))
    local client = Client:new({socket = socket})
   
end



server:RegisterCallBack({OnAccept = OnServerAccept})

t = Timer()
t = nil



Timer:Loop()