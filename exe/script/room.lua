allRoom = {}
Room = {
    guid = 0
}
RoomPlayers = {
    client = nil,
    info = nil
}
---------------------------------------------------------------------------
TYPE_PAI_WNAG = 1
TYPE_PAI_TIAO = 2
TYPE_PAI_TONG = 3
TYPE_PAI_HONG = 4
TYPE_PAI_BAI = 5
TYPE_PAI_QIAN = 6
TYPE_PAI_XI =7


TYPE_HU_NONE = 0
TYPE_HU_PIAO = 1
TYPE_HU_QING = 2
TYPE_HU_TAZI = 3


TYPE_HU_WENQIAN = 1 << 0
TYPE_HU_DANGDIAO = 1 << 1
TYPE_HU_YAZI = 1 << 2
TYPE_HU_BAINZHANG = 1 << 3
TYPE_HU_ZIMO = 1 << 4
TYPE_HU_TIANHU = 1 << 5
TYPE_HU_TIANTING = 1 << 6
TYPE_HU_QIONGXI = 1 << 7

----------------------------------------------------------------------------
ROOM_STATE_WAIT = 1
ROOM_STATE_PLAY = 2
ROOM_STATE_BALANCE = 3
function Room.Create(guid)
    local r = CreateObject(Room)
    r.guid = guid
    allRoom[guid] = r
end
function Room.Get(guid)
    return allRoom[r.guid]
end
function Room:Free()
    allRoom[self.guid] = nil
    self.players = nil

end
function Room:OnCreate()
    print('room create guid:'..tostring(self.guid))
    self.state = ROOM_STATE_WAIT
    self.players = {}
end
function Room:RefreshRoomEnter()
    local room_players = {}
    local index = 1
    for k,v in pairs(self.players) do
        local p = {}
        --掉线设置为未准备好
        p.ready = v.ready and v.client
        p.guid = v.info.guid
        p.headimgurl = v.info.headimgurl
        room_players[index] = p
        index = index + 1
    end
    self:BroadCastMessage(nil,SERVER_MSG.SM_ENTER_ROOM,room_players)
    room_players = nil
end

function Room:Each(callback)
    for k,v in pairs(self.players) do
        callback(v)
    end
end

function Room:BroadCastMessage(sender,type,msg,other)
    for k,v in pairs(self.players) do
        if other then
            if v.client and v.client.guid ~= sender.client.guid then v.client:SendMessage(type,msg) end
        else
            if (v.clilent) then v.client:SendMessage(type,msg) end
        end
        
    end
end

function Room:BroadCastDiffMessage(sender,type,msg1,msg2)
    for k,v in pairs(self.players) do
        if v.client and v.client.guid ~= sender.client.guid then 
            v.client:SendMessage(type,msg1) 
        else if v.client then
            v.client:SendMessage(type,msg2) 
        end

        end
        
    end
end
function Room:SwapPlayer()
    local len = table.getn(self.players)
    if len > 1 then
        local first = self.players[1]
        for i = 2 , len do
            self.players[i-1]= self.players[i]
        end
        self.players[len]=first
    end
end

function Room:Check(client)
    return client.room and client.room.guid == self.guid
end
function Room:Start()
    if self.state == ROOM_STATE_PLAY then return end
    for k,v in pairs(self.players) do
        if not(v.ready) then return end
    end
    self.state = ROOM_STATE_PLAY
    return true
end
function Room:Enter(client)
    local len = table.getn(self.players) + 1
    if len > GAME_MAX_PLAYERS then return false end
    local player  = CreateObject(RoomPlayers)
    player.info = client.info
    player.client = client
    player.ready = false
    client.player = player
    client.room = self
    self.players[len] = player
    self:RefreshRoomEnter()
end
function Room:Leave(client)
    if self:Check(client) then
        self:Each(function (c)
            if(c.client and c.client.guid ~= client.guid) then
                c.client:SendMessage(SERVER_MSG.SM_LEAVE_ROOM,{guid = client.guid})
            end
        end)
        client.room = nil
        client.player.client = nil
        client.player = nil
    end
end
function Room:Ready(client,ready)
    if self:Check(client) then
        client.player.ready = ready
        if not(self:Start()) then
            --self:RefreshRoomEnter()
            self:Each(function (c)
                if(c.client) then
                    c.client:SendMessage(SERVER_MSG.SM_READY_GAME,{guid = client.guid,ready = ready})
                end
            end)
        end
    else
        log_error('client not in this room : ' .. tostring(self.guid))
    end
end
