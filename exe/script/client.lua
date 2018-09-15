require('common')
require('room')
CLIENT_MSG = EnumTable({
    'CM_CHECK_IN_ROOM',
    'CM_ENTER_ROOM',
    'CM_LEAVE_ROOM',
    'CM_READY_GAME',
    'CM_START_GAME',
    'CM_GET_ROOM_INFO',
    'CM_CHU_PAI',
    --'CM_RESPON_CHU_PAI',
    'CM_TEST_PAI',
    'CM_MAI_ZHUANG',
    'CM_BROADCAST',
    'CM_DISMISS_GAME'
})
SERVER_MSG = EnumTable({
    'SM_CHECK_IN_ROOM',
    'SM_ENTER_ROOM',
    'SM_LEAVE_ROOM',
    'SM_READY_GAME',
    'SM_START_GAME',
    'SM_MO_PAI',
    --'SM_CHU_PAI',
    'SM_REFRESH_PAI',
    'SM_TEST_PAI',
    -- 'SM_HUAN_PAI',--//换牌
    'SM_PENG_PAI',--//碰牌
    'SM_GANG_PAI',--//杠牌
    'SM_HU_PAI',--//胡牌
    'SM_GAME_BALANCE',--//进入结算,
    'SM_SYNC_ROOM_STATE',--//断线重新连接
    'SM_MAI_ZHUANG',--//买庄
    'SM_BROADCAST',
    'SM_DISMISS_GAME',
    'SM_DISMISS_GAME_RESULT'
})
allOnlineClients = {}
Client = {socket = nil,uid=0}
Client.__index = Client
function Client:new(o)
    o = o or {}
    setmetatable(o,self)
    self.__index = self
    o.socket:RegisterCallBack(o)
    o.auth_time = 0
    o.authed = false
    o.uid = o.socket.uid
    o.guid = nil
    o.player = nil
    o.info = nil
    o.room = nil
    return o
end
function Client:OnUpdate(frame)
    print('client update frame:' .. tostring(frame))
end
function Client:OnConnected()
    print('client connected uid:' .. tostring(self.uid))
    --self:Send(json.encode({key1="1234",key2=234,key3=true}))
end
function Client:OnDisconnected()
    self.room = nil
    self.socket = nil
    if self.player then self.player.client = nil end
    self.player = nil
    self.info = nil
    print('client disconnected uid:' .. tostring(self.uid))
    collectgarbage("collect")
end

function Client:RegisterMessageHandle()
    local messageHandle = {}
    messageHandle[CLIENT_MSG.CM_CHECK_IN_ROOM]=self.CheckInRoom
    messageHandle[CLIENT_MSG.CM_ENTER_ROOM]=self.EnterRoom;
    messageHandle[CLIENT_MSG.CM_LEAVE_ROOM]=self.LeaveRoom;
    messageHandle[CLIENT_MSG.CM_READY_GAME]=self.ReadyGame;
    messageHandle[CLIENT_MSG.CM_CHU_PAI]=self.ChuPai;
    messageHandle[CLIENT_MSG.CM_TEST_PAI]=self.TestPai;
    messageHandle[CLIENT_MSG.CM_MAI_ZHUANG]=self.MaiZhuang;
    messageHandle[CLIENT_MSG.CM_BROADCAST]=self.Broadcast;
    messageHandle[CLIENT_MSG.CM_DISMISS_GAME]=self.GameLeave;
    self.messageHandle = messageHandle
end
--==============================--
--desc:MessageHandle
--time:2018-09-02 05:52:52
--@msg:
--@return 
function Client:SendMessage(type,msg)
    self:Send({type-1,msg})
end
function Client:CheckInRoom(msg)
    -- body
    print('CheckInRoom=>'..msg)
end

function Client:EnterRoom(msg)
    local r = Room.Get(msg.guid)
    if not(r) then
        local session = GetRoomSession(gServer.db,msg.guid)
        if session then
            print('get room session count:' .. json.encode(session))
            UseRoomCard(gServer.db,msg.guid)
            r = Room.Create(msg.guid)
            r.card = session
        end
    end
    if r then
        if not(r:Enter(self)) then
            self:Send({
                error = 'CANT_ENTER_ROOM'
            })
        end
    else
        self:Send({
            error = 'ROOM_NOT_FOUND'
        })
    end
   
end

function Client:LeaveRoom(msg)
    -- body
    print('LeaveRoom=>'..msg)
end

function Client:ReadyGame(msg)
    if self.room then self.room:Ready(self,msg.ready) end
end
function Client:ChuPai(msg)
    -- body
    if self.room then self.room:ChuPai(self,msg) end
end
function Client:TestPai(msg)
    -- body
    if self.room then self.room:Test(self,msg) end
end
function Client:MaiZhuang(msg)
    if self.room then
        self.room:MaiZhuang(self,msg)
    end
end
function Client:Broadcast(msg)
    if self.room then
        self.room.playrs:Each(function ( k,v )
            if v.client then
                v.client:SendMessage(SERVER_MSG.SM_BROADCAST,{guid=self.info.guid,msg=msg})
            end
        end)
    end
end
function Client:GameLeave(msg)
    -- body
    print('GameLeave=>'..msg)
end
--==============================--


function Client:Auth(msg)
    log_info('client auth msg:' .. msg)
    ok,ret = pcall(json.decode,msg)
    if(ok and ret.unionid and ret.guid) then
        local user = GetUserByUnionid(gServer.db,ret.unionid)
        if(user and user.guid == ret.guid) then
            self.authed = true
            self:RegisterMessageHandle()
            self:Send('{"error":0}')
            self.info = user
            return
        end
    end
    self:Disconnect()
end
function Client:OnHandle(msg)
    ok= xpcall(self.messageHandle[msg[1]+1],function(err) log_error(err) log_error(debug.traceback())  end,self,msg[2])
    if ok == false then
        log_error('cant handle message : ' .. EnumToString(CLIENT_MSG,msg[1]+1))
        self:Disconnect()
    end
end
function Client:OnMessage(msg)
    print('client uid:' .. tostring(self.uid) .. ' message => ' .. msg)
    if (self.authed == false) then
        self:Auth(msg)
        return
    end

    ok,ret = pcall(json.decode,msg)
    if ok then
        self:OnHandle(ret)
    else
        log_error('cant parse msg : ' .. msg)
        self:Disconnect()
    end

    
end
function Client:Send(msg)
    if self.socket then
        if type(msg) == 'table' then
            self.socket:Send(json.encode(msg))
        else
            self.socket:Send(msg)
        end
    end
end
function Client:Disconnect()
    if self.socket then
        self.socket:Disconnect()
    end
end
