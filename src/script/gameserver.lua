require('script/client')
require('script/config')
gRoom ={}
gServer = nil
Server = {}
function Server:new(o)
    o = o or {}
    setmetatable(o,self)
    self.__index = self
    return o
end
function Server:OnAccept(socket)
    print('accept client uid' .. tostring(socket.uid))
    local client = Client:new({socket = socket})
end
function Server:OnUpdate(frame)
    --log_info('game server update:' .. tostring(frame))
    for k,v in pairs(gRoom) do
        if (v) then v:OnUpdate(frame) end
    end
    for k,v in pairs(allOnlineClients) do
        if (v) then v:OnUpdate(frame) end
    end
end
function Server:run()
    self.db = RedisHelper()
    local ok = self.db:Connect(REDIS_ADDR)
    if (ok == false) then
        print('cant connect redis')
        return
    end
    InitRedisScript({db=self.db})
    self.listenner = WebListenner()
    self.listenner:Listen(GAME_LISTEN_ADDR,GAME_LISTEN_COUNT)
    self.listenner:RegisterCallBack(self)

    self.timer = Timer()
    self.timer:OnRegister(self)
    self.timer:Init(1,true)
    self.timer:Begin()
    Timer:Loop()
end


gServer = Server:new()
gServer:run()