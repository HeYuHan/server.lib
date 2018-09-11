json = require "json"
require 'config'
--define
REDIS_CMD_HASH = 1
REDIS_CMD_STRING = 2
REDIS_CMD_SORT_STRING =3

----------------------------------------------------------
--db script hash
local redis_script_get_guid = nil
local redis_script_set_session = nil
local redis_script_get_user_by_guid = nil
local redis_script_save_user_by_guid_and_unionid = nil
local redis_script_save_room_card = nil
-------------------------------------------------------
------------------------------------------------
--tools
function EnumTable(tbl, index) 
    local enumtbl = {} 
    local enumindex = index or 0 
    for i, v in pairs(tbl) do 
        enumtbl[v] = enumindex + i 
    end 
    return enumtbl 
end
function EnumToString(tbl,value)
    for i, v in pairs(tbl) do 
        if v == value then return i end
    end 
    return 'enum not found'
end

function CreateObject(class,o)
    o = o or {}
    setmetatable(o,{__index = class})
    if(o.OnCreate ~= nil) then o:OnCreate() end
    return o;
end
function CopyTable(class)
    local o = {}
    for i, v in pairs(class) do 
        if type(v) == 'table' then
            o[i] = CopyTable(v)
        else
            o[i] = v
        end
        

    end 
    return o
end
function PrintTable(t)
    for i, v in pairs(t) do 
        if type(v) == 'table' then
            PrintTable(v)
        else
            print(tostring(i) .. ':' .. tostring(v))
        end
    end 
end
function bubbleSort(arr,func,length)
    local len = length or #arr
    for i=1,len do
        for j=1,len - i do
            if func(arr[j],arr[j+1]) > 0 then
                local temp = arr[j+1]
                arr[j+1] = arr[j]
                arr[j] = temp
            end
        end
    end
    return arr
end
--==============================--
--desc:types
--time:2018-09-02 05:43:54
--@o:
--@return 
UserInfo = {
    guid = '',
    nick = '',
    unionid = '',
    headimgurl = '',
    diamond = INIT_DIAMOND_COUNT,
    gold = INIT_GOLD_COUNT,
    isProxy = false

}
PayType = EnumTable({
    'None','Host','AA','Winer'
})
RoomCard = {
    guid = '',
    owner = '',
    maxUseCount = 5,
    used = 0,
    currency = '',
    rate = {1,1,1},
    limit = 0,
    xi = false,
    pay = PayType.Winer
}
--==============================--
function InitRedisScript(o)
    local free_db = o.db == nil
    db = o.db
    if (o.db == nil) then
        db = RedisHelper()
        local ok = db:Connect(o.addr)
        if (ok == false) then
            log_error('cant connect db : ' .. o.addr)
            return false
        end
    end
    
    
    local res = RedisResponse()
    db:ScriptCmd(res,'script flush','')
    log_info('remove all script : ' .. res:String())
    --redis_script_get_guid
    db:ScriptCmd(res,'script load',"local ret = redis.call('get',KEYS[1]);\
    if (ret == false) then redis.call('set',KEYS[1],ARGV[1]+1) ret = ARGV[1] else redis.call('set',KEYS[1],ret + 1) end\
    return tostring(ret);")
    if res:Valid() then
        log_info('redis_script_get_guid: ' .. res:String())
        redis_script_get_guid = res:String()
    else
        log_error('cached redis_script_get_guid failed')
    end
    --redis_script_set_session

    db:ScriptCmd(res,'script load',"local ret = redis.call('set',KEYS[1],ARGV[1]);\
    ret = redis.call('set','backup:' .. KEYS[1],ARGV[1]);\
    if (ret) then redis.call('expire',KEYS[1],ARGV[2]) end \
    return ret;")
    if res:Valid() then
        log_info('redis_script_get_guid: ' .. res:String())
        redis_script_set_session = res:String()
    else
        log_error('cached redis_script_get_guid failed')
    end
    --redis_script_save_user_by_guid_and_unionid
    db:ScriptCmd(res,'script load',"local ret = redis.call('hset','users:map',KEYS[1],ARGV[2]);\
    ret = redis.call('set','users:guid:' .. ARGV[1],KEYS[1]);\
    return ret;")
    if res:Valid() then
        log_info('redis_script_save_user_by_guid_and_unionid: ' .. res:String())
        redis_script_save_user_by_guid_and_unionid = res:String()
    else
        log_error('cached redis_script_save_user_by_guid_and_unionid failed')
    end

    --redis_script_get_user_by_guid
    db:ScriptCmd(res,'script load',"local ret = redis.call('get','users:guid:' .. KEYS[1]);\
    if (ret) then return redis.call('hget','users:map',ret) end return nil")

    if res:Valid() then
        log_info('redis_script_get_user_by_guid: ' .. res:String())
        redis_script_get_user_by_guid = res:String()
    else
        log_error('cached redis_script_get_user_by_guid failed')
    end

    --redis_script_save_room_card
    db:ScriptCmd(res,'script load',"local ret = redis.call('hset','rooms:card',KEYS[1],ARGV[1]);\
    if (ret) then ret = redis.call('set','session:rooms:card:' .. KEYS[1],ARGV[1]) end \
    if (ret) then return redis.call('expire','session:rooms:card:' .. KEYS[1],ARGV[2]) end \
    return nil")
    if res:Valid() then
        log_info('redis_script_save_room_card: ' .. res:String())
        redis_script_save_room_card = res:String()
    else
        log_error('cached redis_script_save_room_card failed')
    end

    if(free_db) then db:Close() end
    print('free db:' .. tostring(free_db))
    db = nil
    res = nil
    return true
end
function GetUserGuid(db)
    local res = RedisResponse()
    GetGuid(db,res,'user',100000)
    if res:Valid() then return res:String() end
    return "-1"
end
function GetCardGuid(db)
    local res = RedisResponse()
    GetGuid(db,res,'card',100000)
    if res:Valid() then return res:String() end
    return "-1"
end
function GetGuid(db,res,map,start)
    db:ScriptEval(res,'EVALSHA',redis_script_get_guid,1,string.format('guid:%s %d',map,start))
end
function SaveRecoderFile(db,res,name)
    SaveSession(db,res,'recoder',name,tostring(os.time()),7*24*60*60)
end

function SaveSession(db,res,map,key,value,time)
    db:ScriptEval(res,'EVALSHA',redis_script_set_session,1,string.format('session:%s:%s %s %d',map,key,value,time))
end
function SaveUser(db,user)
    local res = RedisResponse()
    db:ScriptEval(res,'EVALSHA',redis_script_save_user_by_guid_and_unionid,1,string.format('%s %s %s',tostring(user.unionid),tostring(user.guid),json.encode(user)))
    if res:Valid() then
        return true
    else
        return false
    end
end

function SaveRoomCard(db,card)
    local res = RedisResponse()
    db:ScriptEval(res,'EVALSHA',redis_script_save_room_card,1,string.format('%s %s %d',tostring(card.guid),json.encode(card),60*60))
    if res:Valid() then
        return true
    else
        return false
    end
end
function GetRoomSession(db,guid)
    local res = RedisResponse()
    db:GetValue(REDIS_CMD_STRING,res,'session','rooms:card:' .. tostring(guid))
    if res:Valid() then
        return json.decode(res:String())
    else
        return nil
    end
end
function UseRoomCard(db,guid)
    if db:ExpireKey(REDIS_CMD_STRING,'session','rooms:card:' .. tostring(guid),0) then
        return true
    else
        return false
    end
end

function GetUserByGuid(db,guid,parse)
    local res = RedisResponse()
    db:ScriptEval(res,'EVALSHA',redis_script_get_user_by_guid,1,tostring(guid))
    if res:Valid() then
        if parse then
            return res:String()
        else
            return json.decode(res:String())
        end
    else
        return nil
    end
end
function GetUserByUnionid(db,unionid,parse)
    local res = RedisResponse()
    db:GetValue(REDIS_CMD_HASH,res,'users:map',tostring(unionid))
    if res:Valid() then
        if parse then
            return res:String()
        else
            return json.decode(res:String())
        end
    else
        return nil
    end
end

function GetSystemTable(db)
    local res = RedisResponse()
    db:GetValue(REDIS_CMD_STRING,res,'config','sys')
    if res:Valid() then
        return json.decode(res:String())
    else
        return nil
    end
end

function Random(min,max)
    math.randomseed(os.time())
    local x = math.random()*(max - min) + min
    return math.ceil(x);
end

Array = {tbl=nil,size=0}
function Array:OnCreate()
    self.tbl = {}
    self.size = 0
end
function Array:SetTable(t)
    self.size = #t
    self.tbl = t
end
function Array:Push(e)
    self.size = self.size + 1
    self.tbl[self.size]=e
end
function Array:At(index)
    return self.tbl[index]
    -- 5
end
function Array:IndexOf(e)
    for i=1,self.size do
        if self.tbl[i] == e then
            return i
        end
    end
    return 0
end
function Array:Set(index,e)
    if index <= self.size and index > 0 then
        self.tbl[index]=e
        return true
    else
        return false
    end
end
function Array:RemoveAt(index)
    if index <= self.size and index > 0 then
        local ret = self.tbl[index]
        self.size = self.size - 1
        for i=index,self.size do
            self.tbl[i]=self.tbl[i+1]
        end
        return ret
    else
        return nil
    end
    return nil
end
function Array:Splice(index,count)
    count = count or 1
    if (index + count) > self.size then return false end
    for i=1,count do
        self:RemoveAt(index)
    end
    return true
end
function Array:Remove(e)
    local index = self:IndexOf(e)
    if index > 0 then
        return self:RemoveAt(index) ~= nil
    else
        return false
    end
end

function Array:Size()
    return self.size
end
function Array:Pop()
    assert(self.size > 0)
    self.size = self.size -1
    return self.tbl[self.size + 1]
end
function Array:Last()
    if self.size > 0 then return self.tbl[self.size] end
    return nil
end

function Array:Each(func)
    for i=1,self.size do
        func(i,self.tbl[i])
    end
end
function Array:Sort(func)
    bubbleSort(self.tbl,func,self.size)
end
function Array:Data()
    local data = {}
    for i=1,self.size do
        data[i]=self.tbl[i]
    end
    return data
end
function Array:Print()
    for i=1,self.size do
        print('index = '..tostring(i) .. ',value = ' .. tostring(self.tbl[i]))
    end
end
function Array:Clone()
    return CreateObject(Array,{tbl=self:Data(),size=self.size})
end
function Array:Clear()
    self.size = 0
end

--array test
-- local array = CreateObject(Array)
-- array:Push(1)
-- array:Push(2)
-- array:Push(6)
-- array:Print()
-- print('--------------------------------------------')

-- array:Push(4)
-- array:RemoveAt(3)
-- array:Print()
-- print('--------------------------------------------')
-- print('array size = ' .. tostring(array:Size()))
-- print('index of 4 = ' .. tostring(array:IndexOf(4)))


--------------------------------------------------------------------------------------
RandomInt={min=0,max=0,allowrepeat=false,recoders=nil}
function RandomInt:OnCreate()
    self.recoders = CreateObject(Array)
end
function RandomInt:Init(min,max,allowrepeat)
    self.min = min
    self.max = max
    self.allowrepeat = allowrepeat
    if not(allowrepeat) then
        for i=min,max do
            self.recoders:Push(i)
        end
        if not(DEFINE_RANDOM_TEST) then
            local len = max - min
            if (len > 1) then
                for i = 1,len-1 do
                    local value = self.recoders:At(i)
                    local rand_index = (Random(min,max) % (len-i)) + 1
                    self.recoders:Set(i,self.recoders:At(rand_index))
                    self.recoders:Set(rand_index,value)
                end
            end
        end
    end
end
function RandomInt:Print()
    self.recoders:Print()
end
function RandomInt:Get()
    if self.allowrepeat then
        local range=self.max-self.min;
        return math.floor(math.random()*range)+self.min;
    else
        return self.recoders:Pop()
    end
end
function RandomInt:PopValue(value)
    self.recoders:Remove(value)
end
function RandomInt:Size()
    return self.recoders:Size()
end
--test randomint
-- local r = CreateObject(RandomInt)
-- r:Init(1,10)
-- r:Print()
-------------------------------------------------------------------
--test sort
-- local arr={11,5,28,4,6,7}
-- bubbleSort(arr,function (a,b)
--     return a-b
-- end)
-- PrintTable(arr)
-------------------------------------------------------------------
