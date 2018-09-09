allRoom = {}
Room = {
    guid = 0
}
RoomPlayers = {
    client = nil,
    info = nil
}
function RoomPlayers:OnCreate()
    self.shou_pai = CreateObject(Array)
    self.di_pai = CreateObject(Array)
    self.maizhuang = 0
end
function RoomPlayers:MoPai(pai)
    self.shou_pai:Push(pai)
end
function RoomPlayers:DiPai(pai)
    self.di_pai:Push(pai)
end
function RoomPlayers:MaiZhuang()
    return self.maizhuang == 2
end
---------------------------------------------------------------------------
TYPE_PAI_NONE = 0
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


TYPE_HU_WENQIAN = 1
TYPE_HU_DANGDIAO = bit_left(1,1)
TYPE_HU_YAZI = bit_left(1,2)
TYPE_HU_BAINZHANG = bit_left(1,3)
TYPE_HU_ZIMO = bit_left(1,4)
TYPE_HU_TIANHU = bit_left(1,5)
TYPE_HU_TIANTING = bit_left(1,6)
TYPE_HU_QIONGXI = bit_left(1,7)

TYPE_JIANG_NONE = 0
TYPE_JIANG_NORMAL = 1
TYPE_JIANG_NORMALSAME = 2
TYPE_JIANG_HIGHSAME = 3
TYPE_JIANG_HIGHNORMAL = 4

TYPE_RATE_MINGKE = 1
TYPE_RATE_ANKE = 2
TYPE_RATE_MINGGANG = 3
TYPE_RATE_ANGANG = 4
TYPE_RATE_JIAOPAI = 5



Pai = {value = nil,type = TYPE_PAI_NONE,num = nil}
Pai_All = {}
PaiDetail = {pai=nil,is_jiang=false,is_laojiang=false,equal_jiang_value=0}
function CreatePai(number)
    if Pai_All[number] then
        return Pai_All[number]
    end
    local ret = CreateObject(Pai)
    Pai_All[number] = ret
    ret.type=TYPE_PAI_NONE
    ret.num = number
    if (number<41) then
        ret.value=number%10
        if ret.value == 0 then ret.type = TYPE_PAI_HONG else ret.type = TYPE_PAI_WNAG end
        return ret
    elseif(number<81) then
        ret.value=number%10
        if ret.value == 0 then ret.type = TYPE_PAI_BAI else ret.type = TYPE_PAI_TIAO end
        return ret;
    elseif(number<121) then
        ret.value=number%10
        if ret.value == 0 then ret.type = TYPE_PAI_QIAN else ret.type = TYPE_PAI_TONG end
        return ret;
    else
        ret.value=number%120
        ret.type= TYPE_PAI_XI
        return ret
    end
end 
function PaiEqual(p1,p2)
    if type(p1) == 'table' then
        return (p1.value == p2.value) and (p1.type == p2.type)
    else
        local p11 = CreatePai(p1)
        local p22 = CreatePai(p2)
        return PaiEqual(p11,p22)
    end
end
function IsLaoJiang(ret)
    if((ret.type == TYPE_PAI_HONG) or (ret.type == TYPE_PAI_QIAN) or (ret.type == TYPE_PAI_BAI) or ((ret.type == TYPE_PAI_TIAO) and ret.value==9))
    then return true
    end
    return false;
end
function PaiValueToNumber(type,value)
    ret=0
    if(type==TYPE_PAI_HONG) then return 10 end
    if(type==TYPE_PAI_WNAG) then return value end
    if(type==TYPE_PAI_BAI)  then return 50 end
    if(type==TYPE_PAI_TIAO) then return value+40 end
    if(type==TYPE_PAI_QIAN) then return 90 end
    if(type==TYPE_PAI_TONG) then return value+80 end
    if(type==TYPE_PAI_XI)   then return 121 end
    return ret
end

PuKe = {}
function PuKe:OnCreate()
    self.pais = CreateObject(RandomInt)
    self.jiang_pai = {}
    self.jiang_pai_type = TYPE_JIANG_NONE
    self.pai_detail_array = {}
    self.xi = false
end

function PuKe:Init(xi)
    self.xi = xi
    self.jiang_pai[1]=math.floor(math.random()*120)+1
    self.jiang_pai[2]=math.floor(math.random()*120)+1
    if xi then
        self.pais:Init(1,125)
    else
        self.pais:Init(1,120)
    end
    self.pais:PopValue(self.jiang_pai[1])
    self.pais:PopValue(self.jiang_pai[2])
    self:CaculateJiangPaiType()
end
function PuKe:IsJiangPai(number)
    for i=1,2 do
        if PaiEqual(self.jiang_pai[i],number) then
            return true
        end
    end
    return false
end


function PuKe:CaculateJiangPaiType()
    local ret1 = CreatePai(self.jiang_pai[1])
    local ret2 = CreatePai(self.jiang_pai[2])
    local lao_jiang={IsLaoJiang(ret1),IsLaoJiang(ret2)}
    local number_equal = ret1.value == ret2.value
    local equal = number_equal and ret1.type == ret2.type
    local this = self
    if(lao_jiang[1] and lao_jiang[2]) then
    
        this.jiang_pai_type=TYPE_JIANG_HIGHSAME
    elseif(lao_jiang[1] or lao_jiang[2]) then
        this.jiang_pai_type=TYPE_JIANG_HIGHNORMAL
    elseif(equal or number_equal) then
        this.jiang_pai_type=TYPE_JIANG_NORMALSAME
    else
        this.jiang_pai_type=TYPE_JIANG_NORMAL
    end

end
function PuKe:GetDetail(num)
    local this = self
    local ret = this.pai_detail_array[num];
    if not(ret) then
        ret = CreateObject(PaiDetail)
        ret.pai=CreatePai(num)
        ret.is_jiang=self:IsJiangPai(num)
        ret.is_laojiang=IsLaoJiang(ret.pai)
        local j1=CreatePai(this.jiang_pai[0])
        local j2=CreatePai(this.jiang_pai[1])
        if(j1.value==ret.pai.value)then ret.equal_jiang_value=1 + ret.equal_jiang_value end
        if(j2.value==ret.pai.value)then ret.equal_jiang_value=1 + ret.equal_jiang_value end
        this.pai_detail_array[num]=ret
    end
    return ret
end
function PuKe:Get()
    return self.pais:Get()
end
function PuKe:GetSize()
    return self.pais:Size()
end

function PuKe.GetHuPaiRate(jiang_pai_type,pai_detail,type)
    local hu=2
    local rate=1
    if(pai_detail.is_jiang) then
        if jiang_pai_type == TYPE_JIANG_HIGHSAME then
            rate = 16
        elseif jiang_pai_type == TYPE_JIANG_HIGHNORMAL then
            if(pai_detail.is_laojiang) then 
                rate = 8
            else 
                rate = 4
            end
        elseif jiang_pai_type == TYPE_JIANG_NORMALSAME then
            rate = 8
        elseif jiang_pai_type == TYPE_JIANG_NORMAL then
            rate = 4
        end
    elseif(pai_detail.is_laojiang) then
        if jiang_pai_type == TYPE_JIANG_HIGHSAME then
            rate = 8
        elseif jiang_pai_type == TYPE_JIANG_HIGHNORMAL then
            if(pai_detail.is_laojiang) then 
                rate =4
            else 
                rate = 1
            end
        elseif jiang_pai_type == TYPE_JIANG_NORMALSAME then
            rate = 2
        elseif jiang_pai_type == TYPE_JIANG_NORMAL then
            rate = 2
        end
       
    elseif(pai_detail.equal_jiang_value>0) then
        rate=2*pai_detail.equal_jiang_value
    end
    hu = hu *rate;
    if(pai_detail.is_jiang) then
        if(type == TYPE_RATE_ANKE) then
            hu =3*hu
        elseif(type == TYPE_RATE_JIAOPAI)then
            hu =4*hu
        end
    else
        if(type == TYPE_RATE_ANKE) then
            hu =2*hu
        elseif(type == TYPE_RATE_MINGGANG)then
            hu =4*hu
        elseif(type == TYPE_RATE_ANGANG)then
            hu =6*hu
        elseif(TYPE_RATE_JIAOPAI)then
            hu =8*hu
        end
    end
    return hu
end

PaiNode = {}
function PaiNode:OnCreate()
    self:Reset()
end
function PaiNode:Reset()
    self.check = 1
    self.parent = nil
    self.brother = {}
end
function PaiNode:Init(detail)
    self.detail = detail
    self.type = detail.pai.type
    self.value = detail.pai.value
end

CheckPaiNode = {}
function CheckPaiNode:OnCreate()
    self.pai_list = CreateObject(Array)
    self.is_win = false
    self.last_node = nil
    self.win_node = {}
end
function CheckPaiNode:Reset()
    self.is_win = false
    self.pai_list:Each(function ( k,v )
        v:Reset()
    end)
end
function CheckPaiNode:AddPai( node )
    self.pai_list:Push(node)
end
function CheckPaiNode:SetPais( nodes )
    self.pai_list = nodes
end
function CheckPaiNode:AddOriginPai( detail )
    local node = CreateObject(PaiNode)
    node:Init(detail)
    self.pai_list:Push(node)
end
function CheckPaiNode.SortNode(p1,p2)
    if(p1.type==p2.type) then
        if(p1.value==p2.value) then
            return p1.detail.pai.num-p2.detail.pai.num

        else
            return p1.value-p2.value
        end
    else
        return p1.type-p2.type
    end
end

function CheckPaiNode:CheckWin()
    local this = self
    if(this.pai_list:Size()==2) then
        if(PaiEqual(this.pai_list:At(1).detail.pai,this.pai_list:At(2).detail.pai)) then

            return {{this.pai_list:At(1).detail,this.pai_list:At(2).detail}}
        
        else
            return {}
        end
    end
    this.win_node={}
    this.pai_list:Sort(CheckPaiNode.SortNode)
    local type = 0
    local num = 0;
    self.pai_list:Each(function ( k,v )
        local p = v;
        if(p.type ~= type or p.value ~= num) then
            type = p.type
            num = p.value
            for j=i+1,i+3 do
                if(j>this.pai_list:Size()) then break end
                local p2 = this.pai_list:At(j)
                if(p2.type == p.type and p2.value==p.value) then
                    this:Reset()
                    p.brother[1] = p2
                    p.check = bit_left(1,4)
                    p2.check = bit_left(1,4)
                    for k = 1,self.pai_list:Size() do
                        if(this.pai_list:At(k).check == 1) then
                            this:Check(this.pai_list:At(k))
                            break
                        end
                    end
                    break
                end
            end

        end
    end)
    return this.win_node
end
function CheckPaiNode:Check(p)
    local this = self
    this:ClearBrother(p)
    if p.check == 1 then
        this:Check1(p)
    elseif p.check == bit_left(1,1) then
        this:Check2(p)
    elseif p.check == bit_left(1,2) then
        this:Check3(p)
    else
        this:CheckLast(p)
    end
end
function CheckPaiNode:ClearBrother(p)

    for k,v in paris(p.brother) do
        if v then
            v.check = 1
        end
        p.brother[k] = nil
    end
end
function CheckPaiNode:Check1(p) 
    p.check = bit_left(1,1)
    self:CheckBrother(p, 1)
    if (p.brother[1] and p.brother[2]) then
    
        self:CheckNext(p)
    
    else
    
        self:Check(p)
    end
end

function CheckPaiNode:Check2(p) 
    p.check = bit_left(1,2)
    self:CheckBrother(p, 2)
    if (p.brother[1] and p.brother[2]) then
    
        self:CheckNext(p)
    
    else
    
        self:Check(p)
    end
end

function CheckPaiNode:Check3(p)
    p.check = bit_left(1,3)
    self:CheckBrother(p, 3)
    if (p.brother[1] and p.brother[2] and p.brother[3]) then
    
        self:CheckNext(p)
    
    else
    
        self:Check(p)
    end
end
function CheckPaiNode:CheckNext(p)

    for i=1,self.pai_list:Size() do
        local t = self.pai_list:At(i)
        if t.check == 1 then
            t.parent = p
            self.last_node = t
            self:Check(t)
            return
        end
    end
    self:GetResult(true)
    if not(self.last_node) then return end
    self.last_node.check = bit_left(self.last_node.check,1)
    self:Check(self.last_node);
end
function CheckPaiNode:CheckBrother(p,index)
    local this = self
    if index == 1 then
        for i=1,self.pai_list:Size() do
            local v = self.pai_list:At(i)
            if (v.check == 1) then
                if v.type == p.type and v.value == p.value + 1 then
                    p.brother[1] = v
                    p.brother[1].check = bit_left(1,4)
                    for j = i+1,i+3 do
                        if j >= self.pai_list:Size() then break end
                        local p2 = self.pai_list:At(j)
                        if p2.check == 1 then
                            if (p2.type == p.type and p2.value == p.value + 2)then
                                p.brother[2] = p2
                                p.brother[2].check = bit_left(1,4)
                            end
                        end
                    end
                end
            end
        end
    elseif index == 2 then
        for i=1,self.pai_list:Size() do
            local v = self.pai_list:At(i)
            if (v.check == 1) then
                if i > self.pai_list:Size() - 1 then break end

                if v.type == p.type and v.value == p.value then
                    local p2 = self.pai_list:At(i+1)
                    if (p2.type == p.type and p2.value == p.value) then
                            
                        p.brother[1] = v
                        p.brother[1].check = bit_left(1,4)
                        p.brother[2] = v
                        p.brother[2].check = bit_left(1,4)
                        break
                    end
                end
            end
        end
    elseif index ==3 then
        for i=1,self.pai_list:Size() do
            local v = self.pai_list:At(i)
            if (v.check == 1) then
                if i > self.pai_list:Size() - 2 then break end

                if v.type == p.type and v.value == p.value then
                    local p2 = self.pai_list:At(i+1)
                    local p3 = self.pai_list:At(i+2)
                    if (p2.type == p.type and p2.value == p.value and p3.type == p.type and p3.value == p.value) then
                            
                        p.brother[1] = v
                        p.brother[1].check = bit_left(1,4)
                        p.brother[2] = p2
                        p.brother[2].check = bit_left(1,4)
                        p.brother[3] = p3
                        p.brother[3].check = bit_left(1,4)
                        break
                    end
                end
            end
        end
    end
    
end
function CheckPaiNode:CheckLast(p)
    local this = self
    this:ClearBrother(p)
    if (p.parent) then
    
        p.check = 1
        this.last_node = p.parent
        this:Check(this.last_node)
    
    else
    
        this:GetResult(false)
    end
end
function CheckPaiNode.SortArray(a,b)
    return #b-#a
end
function CheckPaiNode:GetResult(tag) 
    local this = self
    this.is_win = tag
    if (this.is_win) then
        local win_info={}
        for i=1,self.pai_list:Size() do
            local p = self.pai_list:At(i)
            if p.brother[1] then
                local temp = {}
                table.insert(temp,p.detail)
                for j=1,#p.brother.length do
                    if p.brother[j] then
                        table.insert(temp,p.brother[j].detail)
                    end
                end
                table.insert(win_info,temp)
            end
        end
        bubbleSort(win_info,CheckPaiNode.SortArray)
        local win_info2={}
        for k,v in paris(win_info) do
            for k2,v2 in paris(v) do
                table.insert(win_info2,v2)
            end
        end
        if(#win_info2 > 0) then table.insert( self.win_node,win_info2) end
    end
end


----------------------------------------------------------------------------
ROOM_STATE_WAIT = 1
ROOM_STATE_PLAY = 2
ROOM_STATE_BALANCE = 3
function Room.Create(guid)
    local r = CreateObject(Room,{guid=guid})
    allRoom[guid] = r
    return r
end
function Room.Get(guid)
    return allRoom[guid]
end
function Room:Free()
    allRoom[self.guid] = nil
    self.players = nil

end
function Room:OnCreate()
    print('room create guid:'..tostring(self.guid))
    self.state = ROOM_STATE_WAIT
    self.players = CreateObject(Array)
    self.puke = CreateObject(PuKe)
    self.maizhuang_count = 0
end
function Room:RefreshRoomEnter()
    local room_players = {}
    self.players:Each(function (k,v)
        local p = {}
        --掉线设置为未准备好
        p.ready = v.ready and v.client
        p.guid = v.info.guid
        p.headimgurl = v.info.headimgurl
        p.nick = v.info.nick
        room_players[k] = p
    end)
    
    self:BroadCastMessage(nil,SERVER_MSG.SM_ENTER_ROOM,{card=self.card,players=room_players})
    room_players = nil
end

function Room:BroadCastMessage(sender,type,msg,other)
    self.players:Each(function (k,v)
        
        if other then
            if v.client and v.client.guid ~= sender.client.guid then v.client:SendMessage(type,msg) end
        else
            if (v.client) then v.client:SendMessage(type,msg) end
        end
    end)
   
end

function Room:BroadCastDiffMessage(sender,type,msg1,msg2)
    self.players:Each(function (k,v)
        if v.client and v.client.guid ~= sender.client.guid then 
            v.client:SendMessage(type,msg1) 
        elseif v.client then
            v.client:SendMessage(type,msg2) 
        end
    end)
end


function Room:SwapPlayer()
    local len = self.players:Size()
    if len > 1 then
        local first = self.players:At(1)
        first.index = len
        for i = 2 , len do
            local temp = self.players:At(i)
            temp.index = i-1
            self.players:Set(i-1,temp)
        end
        self.players:Set(len,first)
    end
end

function Room:Check(client)
    return client.room and client.room.guid == self.guid
end
function Room:Start()
    if self.state == ROOM_STATE_PLAY then return false end
    if self.players:Size() < GAME_MAX_PLAYERS then return false end
    self.players:Each(function (k,v)
        if not(v.ready) then return false end
    end)
    self.state = ROOM_STATE_PLAY
--游戏开始
    self.maizhuang_count=0
    self.card.used = self.card.used + 1
    self.puke:Init(self.card.xi)
    self:SwapPlayer()
    self.next_mopai_palyer = self.players:At(1)
--摸牌
    
    self.players:Each(function (k,v)
        for i=1,22 do
            local number = self.puke:Get()
            local pai = CreatePai(number)
            while pai.type == TYPE_PAI_XI do
                v:DiPai(number)
                number = self.puke:Get()
                pai = CreatePai(number)
            end
            v:MoPai(number)
        end
    end)
    self.players:Each(function (k,v)
       if(v.client) then
        v.client:SendMessage(SERVER_MSG.SM_START_GAME,{cardused=self.card.used,guid=v.info.guid,di=v.di_pai:Data(),shou=v.shou_pai:Data(),jiang=self.puke.jiang_pai,size2=self.puke:GetSize()})
       end
    end)
    return true
end
function Room:Enter(client)
    local len = self.players:Size()
    if len > GAME_MAX_PLAYERS then return false end
    local player  = CreateObject(RoomPlayers)
    player.info = client.info
    player.client = client
    player.ready = false
    client.player = player
    client.room = self
    self.players:Push(player)
    self:RefreshRoomEnter()
    return true
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
        client.player.ready = not(not(ready))
        self.players:Each(function (k,v)
            if(v.client) then
                v.client:SendMessage(SERVER_MSG.SM_READY_GAME,{guid = client.info.guid,ready = client.player.ready})
            end
        end)
        self:Start()
    else
        log_error('client not in this room : ' .. tostring(self.guid))
    end
end

function Room:MoPai()
    local number = self.puke:Get()
    local pai = CreatePai(number)
    local refresh_dipai = false
    while pai.type == TYPE_PAI_XI do
        refresh_dipai = true
        self.next_mopai_palyer:DiPai(number)
        number = self.puke:Get()
        pai = CreatePai(number)
    end
    self.next_mopai_palyer:DiPai(number)
    local di = nil
    if refresh_dipai then di=self.next_mopai_palyer.di_pai:Data() end
    self.players:Each(function ( k,v )
        if not(v.client) then return end
        if v.info.guid == self.next_mopai_palyer.info.guid then
            v.client:SendMessage(SERVER_MSG.SM_MO_PAI,{
                guid=self.next_mopai_palyer.info.guid,
                pai=number,
                di = di,
                size1=self.next_mopai_palyer.shou_pai:Size(),
                size2=self.puke:GetSize()
            })
        else
            v.client:SendMessage(SERVER_MSG.SM_MO_PAI,{
                guid=self.next_mopai_palyer.info.guid,
                di = di,
                size1=self.next_mopai_palyer.shou_pai:Size(),
                size2=self.puke:GetSize()
            })
        end
    end)

end

function Room:MaiZhuang(client,msg)
    if self.state ~= ROOM_STATE_PLAY then return end
    if(client.player.maizhuang ~= 0) then return end
    if not(not(msg.maizhuang)) then client.player.maizhuang =2
    else
        client.player.maizhuang = 1
    end
    self:BroadCastMessage(nil,SERVER_MSG.SM_MAI_ZHUANG,{
        guid=client.info.guid,
        maizhuang=client.player:MaiZhuang()
    })
    self.maizhuang_count =self.maizhuang_count + 1
    if(self.maizhuang_count == GAME_MAX_PLAYERS) then
        self:MoPai()
    end
end
