require('script/common')
require('script/config')
Client = {}
Server = {}
sysTable = {}

TYPE_ACCOUNT_ROOT = 1
TYPE_ACCOUNT_PROXY = 2
function Client:OnCreate()
    print('login client oncreate')
    self.socket:RegisterCallBack(self)
    self.uid = self.socket.uid
    self.auth = false
    self.account = TYPE_ACCOUNT_PROXY
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
    ok,ret = xpcall(Client['Public_'..msg.path],function(err) log_error(err) end,self,msg.data)
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
    if not(self.auth) then
        self:Auth(msg)
        return
    end
    ok,ret = pcall(json.decode,msg)
    
    if ok then
        self:OnRequest(ret)
    else
        log_error('cant parse msg : ' .. msg)
        self:Disconnect()
    end
end
function Client:OnConnected()
    --print('client connected uid:' .. tostring(self.uid))
    --self:Send(json.encode({key1="1234",key2=234,key3=true}))
end
function Client:OnDisconnected()
    self.socket = nil
    self.uid = 0
    --print('client disconnected uid:' .. tostring(self.uid))
    --collectgarbage("collect")
end
--删除代理
function Client:Public_DeleteProxy(msg)
    if self.account ~= TYPE_ACCOUNT_ROOT then
        return {error = 'ERROR_ACCOUNT_LEVEL_LOW'}
    end
    local user = GetProxyInfo(gServer.db,msg.guid)
    if user then
        if DeleteProxyInfo(gServer.db,user.guid) then
            return {error = 0}
        else
            return {error = 'ERROR_CANT_DELETE_PROXY'}
        end
    else
        return {error = 'ERROR_USER_NOT_FOUND'}
    end
end

--添加或者更新代理
function Client:Public_UpdateProxy(msg)
    if self.account ~= TYPE_ACCOUNT_ROOT then
        return {error = 'ERROR_ACCOUNT_LEVEL_LOW'}
    end
    local user = GetUserByGuid(gServer.db,msg.guid)
    if user then
        msg.password = msg.password or tostring(Random(100000,999999))
        if UpdateOrSaveProxyInfo(gServer.db,user.guid,msg.password) then
            return {password = msg.password,account = user.guid}
        else
            return {error = 'ERROR_CANT_UPDATE_PROXY'}
        end
    else
        return {error = 'ERROR_USER_NOT_FOUND'}
    end
end

--root账户,设置值
function Client:Public_SetMoney(msg)
    if self.account ~= TYPE_ACCOUNT_ROOT then
        return {error = 'ERROR_ACCOUNT_LEVEL_LOW'}
    end
    local user = GetUserByGuid(gServer.db,msg.guid)
    if user then
        local gold = msg.gold or user.gold
        local diamond = msg.diamond or user.diamond
        user.gold = gold
        user.diamond = diamond
        if SaveUser(gServer.db,user) then
            return {guid=user.guid,gold = user.gold,diamond = user.diamond}
        else
            return {error = 'ERROR_CANT_UPDATE_USER'}
        end
    else
        return {error = 'ERROR_USER_NOT_FOUND'}
    end
end

--充值，只增不减
function Client:Public_UpdateMoney(msg)
    local user = GetUserByGuid(gServer.db,msg.guid)
    if user then
        local gold = msg.gold or 0
        local diamond = msg.diamond or 0

        --查询代理账户钱是否够
        if(self.account == TYPE_ACCOUNT_PROXY) then
            local myinfo = GetUserByGuid(gServer.db,self.guid)
            if myinfo.gold < gold or myinfo.diamond < diamond then
                return {error = 'ERROR_PROXY_ACCOUNT_NOT_FULL_MONEY',gold = myinfo.gold,diamond = myinfo.diamond}
            else
                myinfo.gold = myinfo.gold - gold
                myinfo.diamond = myinfo.diamond - diamond
                if not(SaveUser(gServer.db,myinfo)) then
                    return {error = 'ERROR_CANT_UPDATE_USER'}
                end
            end
        end

        user.gold = user.gold + gold
        user.diamond = user.diamond + diamond
        if SaveUser(gServer.db,user) then
            return {guid=user.guid,gold = user.gold,diamond = user.diamond}
        else
            return {error = 'ERROR_CANT_UPDATE_USER'}
        end
    else
        return {error = 'ERROR_USER_NOT_FOUND'}
    end
end
--获取用户信息

function Client:Public_GetUserInfo(msg)
    local user = GetUserByGuid(gServer.db,msg.guid)
    if user then
        return user
    else
        return {error = 'ERROR_USER_NOT_FOUND'}
    end
end

function Client:Auth(msg)
    ok,ret = pcall(json.decode,msg)
    if ok then
        local error = false
        if msg.password and msg.type == TYPE_ACCOUNT_ROOT then
            local pwd = GetAdminInfo(gServer.db)
            if pwd and pwd == msg.password then
                self.account = TYPE_ACCOUNT_ROOT
            else
                error = true
            end
        elseif msg.password and msg.type == TYPE_ACCOUNT_PROXY then
            local pwd = GetProxyInfo(gServer.db,msg.guid)
            if pwd and pwd == msg.password then
                self.account = TYPE_ACCOUNT_PROXY
                self.guid = msg.guid
            else
                error = true
            end
        else
            error = true
        end

        if error then
            log_error('cant auth msg : ' .. msg)
            self.Send(json.encode({
                error = 'ERROR_AUTH_FAILED'
            }))
        else
            self.auth = true
            self.Send(json.encode({
                error = 0
            }))
        end


        
    else
        log_error('cant auth msg : ' .. msg)
        self.Send(json.encode({
            error = 'ERROR_AUTH_FAILED'
        }))
    end
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
    local admin = GetAdminInfo(self.db)
    if not(admin) then
        local ok = UpdateRootPassword(self.db,INIT_ROOT_PASSWORD)
        log_info('create root password:'..ok)
        admin = INIT_ROOT_PASSWORD
    end
    log_info('root password:'..admin)



    self.listenner = WebListenner()
    self.listenner:Listen(ADMIN_LISTEN_ADDR,ADMIN_LISTEN_COUNT)
    self.listenner:RegisterCallBack(self)
    self.timer = Timer()
    self.timer:OnRegister(self)
    self.timer:Init(1,true)
    self.timer:Begin()
    self.sysTableUpdateTimer = 0
    Timer:Loop()
end

function Server:SaveUserInfo(user)
    return SaveUser(self.db,user)
end

gServer = CreateObject(Server)
gServer:run()