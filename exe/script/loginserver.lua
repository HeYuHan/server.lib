require('common')
require('config')
Client = {}
Server = {}
sysTable = {}

function Client:OnCreate()
    print('login client oncreate')
    self.socket:RegisterCallBack(self)
    self.uid = self.socket.uid
end
function Client:Send(msg)
    if self.socket then
        self.socket:Send(msg)
    end
end

function Client:Disconnect()
    if self.socket then
        self.socket:Disconnect()
    end
end

function Client:OnRequest(msg)
    if not(msg.path) then
        log_error('call path is nil')
        self:Disconnect()
        return
    end
    ok,ret = xpcall(Client[msg.path],function(err) log_error(err) end,self,msg.data)
    if ok == false then
        log_error('cant call path : ' .. msg.path)
        self:Disconnect()
    else
        if (type(ret) == 'table') then
            self:Send(json.encode(ret))
        else
            self:Send(ret)
        end
    end
end
function Client:OnMessage(msg)
    ok,ret = pcall(json.decode,msg)
    
    if ok then
        self:OnRequest(ret)
    else
        log_error('cant parse msg : ' .. msg)
        self:Disconnect()
    end
end
function Client:OnConnected()
    print('client connected uid:' .. tostring(self.uid))
    --self:Send(json.encode({key1="1234",key2=234,key3=true}))
end
function Client:OnDisconnected()
    self.socket = nil
    self.uid = 0
    print('client disconnected uid:' .. tostring(self.uid))
    collectgarbage("collect")
end

function Client:TestUserInfo(msg)
    local test = CopyTable(UserInfo)
    test.nick = 'test' .. tostring(RandomInt(100000,999999))
    test.unionid = 'unionid' .. tostring(RandomInt(100000,999999))
    test.guid = GetUserGuid(gServer.db)
    gServer:SaveUserInfo(test)
    print('test:' .. json.encode(test))
    return test
end
function Client:CreateRoomCard(msg)
    local err = ''
    local user = nil
    if(msg.unionid) then
        user = GetUserByUnionid(gServer.db,msg.unionid)
    end
    if(user == nil and msg.guid) then
        user = GetUserByGuid(gServer.db,msg.guid)
    end
    
    if user then
        local currency = sysTable[msg.currency]
        if currency then
            local cost = currency[tostring(msg.count)]
            if cost then
                local user_money = user[msg.currency]
                if user_money < cost then
                    err = 'MONEY_NOT_FULL'
                else
                    local card = CopyTable(RoomCard)
                    card.guid = GetCardGuid(gServer.db)
                    card.maxUseCount = msg.count
                    card.owner = user.unionid
                    card.used = 0
                    card.rate = msg.balanceRate
                    local limit = tonumber(msg.limit)
                    if limit>0 then card.limit=limit end
                    if msg.includexi then card.xi = true end
                    card.pay = PayType[msg.payType]
                    card.currency = msg.currency
                    if (SaveRoomCard(gServer.db,card)) then
                        user[msg.currency] = user_money - cost
                        SaveUser(gServer.db,user)
                        card.gold = user.gold
                        card.diamond = user.diamond
                        return card
                    else
                        err = 'CANT_CREATE_ROOM_CARD'
                    end
                    

                end
            else
                err = 'CURRENCY_COUNT_NOT_FOUND'
            end
        else
            err = 'CURRENCY_NOT_FOUND'
        end
    else
        err = 'USER_NOT_FOUND'
        print('user not fuond ' .. msg.unionid)
    end
    return {
        error=err
    }
end

function Client:GetUserInfo(msg)
    if (msg.code) then
        http = Http()
        local ret = http:Get(string.format('%s?appid=%s&secret=%s&code=%s&grant_type=authorization_code',WX_TOKEN_PATH,WX_APPID,WX_KEY,msg.code))
        log_info('request token : ' .. http:Body())
        local wx1 = json.decode(http:Body())
        if (not(ret) or wx1.errcode) then
            return {
                error = 'WX_ATUH_FAILED',
                code = wx1.errcode,
                message = wx1.errmsg
            }
        end
        
        local user = GetUserByUnionid(gServer.db,wx1.unionid,true)
        if user then 
            return user 
        else
            ret = http:Get(string.format('%s?openid=%s&access_token=%s',WX_USER_PATH,wx1.openid,wx1.access_token))
            log_info('request user : ' .. http:Body())
            local wx2 = json.decode(http:Body())
            if (not(ret) or wx2.errcode) then
                return {
                    error = 'WX_ATUH_FAILED',
                    code = wx2.errcode,
                    message = wx2.errmsg
                }
            end
            user = CopyTable(UserInfo)
            user.nick = wx2.nickname
            user.unionid = wx1.unionid
            user.headimgurl = wx2.headimgurl
            user.guid = GetUserGuid(gServer.db)
            gServer:SaveUserInfo(user)
            return user
        end
    return ret
    end
    if (msg.unionid) then
        ret = GetUserByUnionid(gServer.db,msg.unionid,true)
        if ret then return ret end
    end
    if (msg.guid) then
        ret = GetUserByGuid(gServer.db,msg.guid,true)
        if ret then return ret end
        
    end
    
    --get token
    
    
    return {
        error = 'USER_NOT_FOUND'
    }
end






















function Server:OnAccept(socket)
    print('accept client uid' .. tostring(socket.uid))
    local client = CreateObject(Client,{socket = socket})
end
function Server:OnUpdate(frame)
    self.sysTableUpdateTimer = self.sysTableUpdateTimer + frame
    if (self.sysTableUpdateTimer > SYS_TABLE_UPATE_TIMER) then
        self.sysTableUpdateTimer = 0
        sysTable = GetSystemTable(self.db)
        --PrintTable(sysTable)
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
    self.listenner:Listen(LOGIN_LISTEN_ADDR,LOGIN_LISTEN_COUNT)
    self.listenner:RegisterCallBack(self)
    self.timer = Timer()
    self.timer:OnRegister(self)
    self.timer:Init(1,true)
    self.timer:Begin()
    self.sysTableUpdateTimer = 0
    Timer:Loop()
end
function Server:GetUserInfo(guid)
    local res = RedisResponse()
    self.db:GetValue(REDIS_CMD_HASH,res,"Users",tostring(guid))
    if res:Valid() then
        return json.decode(res:String())
    else
        return nil
    end
end

function Server:SaveUserInfo(user)
    return SaveUser(self.db,user)
end

gServer = CreateObject(Server)
gServer:run()