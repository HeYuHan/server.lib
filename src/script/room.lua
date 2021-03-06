allRoom = {}
Room = {
    guid = 0
}
RoomPlayers = {
    client = nil,
    info = nil
}
TYPE_TEST_PENG = 1
TYPE_TEST_GANG = 2
TYPE_TEST_ANGANG = 3
TYPE_TEST_HU = 4
TYPE_TEST_ZIMO = 5

function RoomPlayers:OnCreate()
    self.shou_pai = CreateObject(Array)
    self.di_pai = CreateObject(Array)
    self.an_gang_array = CreateObject(Array)
    self.jiao_pai_array = CreateObject(Array)
    self.qi_pai_array = CreateObject(Array)
    self.hu_pai_info = nil
    self.maizhuang = 0
    self.test = nil
    self.ready = false
    
end
function RoomPlayers:Reset()
    self.shou_pai:Clear()
    self.di_pai:Clear()
    self.an_gang_array:Clear()
    self.jiao_pai_array:Clear()
    self.qi_pai_array:Clear()
    self.hu_pai_info = nil
    self.maizhuang = 0
    self.test = nil
    self.ready = false
end
function RoomPlayers:ChuPai(pai)
    if self.shou_pai:Remove(pai) then 
        --self.qi_pai_array:Push(pai)
        return true
    else
        return false
    end
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
function RoomPlayers:TestPeng(value)
    local temp_shou ={}
    local temp_di = {}
    for i=1,self.shou_pai:Size() do
        local pai = self.shou_pai:At(i)
        if #temp_di<2 and PaiEqual(pai,value) then
            table.insert( temp_di,pai)
        else
            table.insert( temp_shou,pai)
        end
    end
    if #temp_di > 1 then
        table.insert( temp_di,value)
        self.test = {
            type = TYPE_TEST_PENG,
            data = temp_di,
            shou = temp_shou,
            value = value
        }
        return true
    end
    self.test = nil
    return false
end

function RoomPlayers:TestGang(value)
    local temp_shou ={}
    local temp_di = {}
    for i=1,self.shou_pai:Size() do
        local pai = self.shou_pai:At(i)
        if #temp_di<3 and PaiEqual(pai,value) then
            table.insert( temp_di,pai)
        else
            table.insert( temp_shou,pai)
        end
    end
    if #temp_di > 2 then
        table.insert( temp_di,value)
        self.test = {
            type = TYPE_TEST_GANG,
            data = temp_di,
            shou = temp_shou,
            value = value
        }
        return true
    end
    self.test = nil
    return false
end
function RoomPlayers:TestAnGang()
    self.shou_pai:Sort(SortPaiNumber)
    local back_up = self.shou_pai:Clone()
    for i=1,self.shou_pai:Size()-3 do
        local pai1 = self.shou_pai:At(i)
        local pai2 = self.shou_pai:At(i+1)
        local pai3 = self.shou_pai:At(i+2)
        local pai4 = self.shou_pai:At(i+3)
        if PaiEqual(pai1,pai2) and PaiEqual(pai2,pai3) and PaiEqual(pai3,pai4) then
            self.shou_pai:Splice(i,4)
            self.test = {
                type = TYPE_TEST_ANGANG,
                data = {pai1,pai2,pai3,pai4},
                shou = self.shou_pai,
                value = pai1
            }
            self.shou_pai = back_up
            return true
        end
    end
    self.test = nil
    return false
end
function RoomPlayers:TestHu(pai,puke)
    local ret = false
    local caulater = CreateObject(CheckPaiNode)
    self.shou_pai:Each(function ( k,v )
        caulater:AddOriginPai(puke:GetDetail(v))
    end)
    caulater:AddOriginPai(puke:GetDetail(pai))
    local result_array = caulater:CheckWin()
    ret = #result_array > 0
    if ret then
        self.test = {
            type = TYPE_TEST_HU,
            data = result_array,
            value = pai
        }
    else
        self.test = nil
    end
    return ret
end
function RoomPlayers:ApplyTest(test)
    self.test = test
    if self.test.type == TYPE_TEST_GANG then
        self.shou_pai:SetTable(self.test.shou)
        self.an_gang_array:Push(self.test.value)
        self.di_pai:PushRange(self.test.data)
    elseif self.test.type == TYPE_TEST_ANGANG then
        self.shou_pai:SetTable(self.test.shou)
        self.di_pai:PushRange(self.test.data)
    elseif self.test.type == TYPE_TEST_PENG then
        self.shou_pai:SetTable(self.test.shou)
        self.di_pai:PushRange(self.test.data)
    end
end
function RoomPlayers:CaculateScore(puke)
    if self.test and (self.test.type == TYPE_TEST_HU or self.test.type == TYPE_TEST_ZIMO)then
        local di_info_array = {}
        self.di_pai:Sort(SortPaiNumber)
        for k,v in pairs(self.test.data) do
            local shou = DetailToNumberArray(v)
            local info = puke:CaculateDiHu(shou,self.di_pai,self.an_gang_array,self.jiao_pai_array)
            info.hu_pai_array = shou
            if self.test.type == TYPE_TEST_ZIMO then info.hu_pai_type = bit_or(info.hu_pai_type,TYPE_HU_ZIMO) end
            info:CaculateTotleScore(true,puke.xi)
            table.insert( di_info_array,info)
        end
        self.hu_pai_info = di_info_array[1]
        for i=2,#di_info_array do
            if self.hu_pai_info.totle_socre < di_info_array[i].totle_socre then
                self.hu_pai_info = di_info_array[i]
            end
        end
    else
        self.hu_pai_info = puke:CaculateDiHu(self.shou_pai,self.di_pai,self.an_gang_array,self.jiao_pai_array)
        self.hu_pai_info:CaculateTotleScore(false,puke.xi)
    end
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
function DetailToNumberArray(details)
    local ret =CreateObject(Array)
    for k,v in pairs(details) do
        ret:Push(v.pai.num)
    end
    return ret
end
function  PaiEqual(p1,p2)
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
function SortPaiNumber(a,b)
    local p1=CreatePai(a)
    local p2=CreatePai(b)
    if(p1.type==p2.type) then
    
        if(p1.value==p2.value) then
        
            return p1.num-p2.num
        
        else
            return p1.value-p2.value
        end
    
    else
        return p1.type-p2.type
    end
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
    self.pai_detail_array = {}
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
    local ret = self.pai_detail_array[num];
    if not(ret) then
        ret = CreateObject(PaiDetail)
        ret.pai=CreatePai(num)
        ret.is_jiang=self:IsJiangPai(num)
        ret.is_laojiang=IsLaoJiang(ret.pai)
        local j1=CreatePai(self.jiang_pai[1])
        local j2=CreatePai(self.jiang_pai[2])
        if(j1.value==ret.pai.value)then ret.equal_jiang_value=1 + ret.equal_jiang_value end
        if(j2.value==ret.pai.value)then ret.equal_jiang_value=1 + ret.equal_jiang_value end
        self.pai_detail_array[num]=ret
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


function PuKe:CaculateDiHu(shou_pai,di_pai,an_gang,jiao_pai)

    local ming_ke_array = CreateObject(Array)
    local an_ke_array = CreateObject(Array)
    local ming_gang_array = CreateObject(Array)
    local an_gang_array = CreateObject(Array)
    local xi_pai_array = CreateObject(Array)
    local sun_zi_array = CreateObject(Array)
    local dui_zi_array = CreateObject(Array)
    local jiao_pai_array = CreateObject(Array)
    local temp_array = CreateObject(Array)

    --判断三花聚会
    local san_hua=0;
    local have_san_hua=false
    --手牌(计算暗刻,重新排序)
    local shou_pai2=shou_pai:Clone()
    shou_pai:Sort(SortPaiNumber)
    shou_pai2:Each(function ( k,v )
        local detail = self:GetDetail(v)
        if(detail.pai.type == TYPE_PAI_XI)then
            xi_pai_array:Push(detail)
            return
        end
        if(detail.is_laojiang) then
        
            if(detail.pai.type==TYPE_PAI_HONG) then san_hua = bit_left(1,1) 
            elseif(detail.pai.type==TYPE_PAI_BAI) then san_hua = bit_left(1,2)
            elseif(detail.pai.type==TYPE_PAI_QIAN) then san_hua = bit_left(1,3) 
            end
        end
        temp_array:Push(detail)
        --类型不同跳过第一张
        if(temp_array:Size()>1 and temp_array:At(1).pai.type ~= temp_array:At(2).pai.type) then
        
            temp_array:RemoveAt(1)
            return
        end
        if(temp_array:Size()<2) then return end
        if(true) then
    

            if(temp_array:Size()<3)then return end
            if(temp_array:At(3).pai.type ~= temp_array:At(1).pai.type) then
            
                local t1 = temp_array:At(2)
                local t2 = temp_array:At(3)
                temp_array:Clear()
                temp_array:Push(t1)
                temp_array:Push(t2)
                return
            end
            --暗刻
            if(temp_array:At(1).pai.value == temp_array:At(2).pai.value and temp_array:At(1).pai.value == temp_array:At(2).pai.value) then
            
                if(jiao_pai:indexOf(temp_array:At(1).pai.num)<0) then an_ke_array:Push(temp_array:At(1)) end
                temp_array:Clear()
                return
            else
                local t1 = temp_array:At(2)
                local t2 = temp_array:At(3)
                temp_array:Clear()
                temp_array:Push(t1)
                temp_array:Push(t2)
                return
            end
        end
    end)
    --//计算顺子,和对子
    temp_array:Clear()
    for k = 1,shou_pai:Size() do
        while true do
            local detail = self:GetDetail(shou_pai:At(k))
            if(detail.pai.type == TYPE_PAI_XI) then break end
            temp_array:Push(detail)
            --类型不同跳过第一张
            if(temp_array:Size()>1 and temp_array:At(1).pai.type ~= temp_array:At(2).pai.type) then
            
                local t = temp_array:At(2)
                temp_array:Clear()
                temp_array:Push(t)
                break
            end
            if (temp_array:Size()<2) then break end
            if(temp_array:At(1).pai.value == temp_array:At(2).pai.value) then
                --取出下一张
                if(k<shou_pai:Size()) then
                    local detail2 = self:GetDetail(shou_pai:At(k+1))
                    if PaiEqual(temp_array:At(1).pai,detail2.pai) then
                        --暗刻,剔除叫牌
                        temp_array:Clear()
                        k = k + 1
                        break
                    --对子
                    else
                        dui_zi_array.Push(temp_array:At(1))
                        temp_array:Clear()
                        break
                    end
                --对子
                else
                    dui_zi_array:Push(temp_array:At(1))
                    temp_array:Clear()
                    break
                end
            else
                --取出下一张
                if(K<shou_pai:Size()) then
                
                    local detail2 = self:GetDetail(shou_pai:At(k+1))
                    --顺子
                    if((temp_array:At(1).pai.value == detail2.pai.value-2) and detail2.pai.type == temp_array:At(1).pai.type)then
                        sun_zi_array:Push(temp_array:At(1))
                        temp_array:Clear()
                        k = k +1
                        break
                    
                    else
                        local t = temp_array:At(1)
                        temp_array:Clear()
                        temp_array:Push(t)
                        break
                    end
                
                else
                    local t = temp_array:At(1)
                    temp_array:Clear()
                    temp_array:Push(t)
                    break
                end
            end
            
        end
            break
        end
        

        
    --底牌,明刻,明杠,暗杠
    temp_array:Clear()
    if di_pai then
        for k = 1,di_pai:Size() do
            while true do
                local detail=self:GetDetail(di_pai:At(k))
                if(detail.pai.type == TYPE_PAI_XI)then
                    xi_pai_array:Push(detail)
                    break
                end
                if(detail.is_laojiang) then
                    if(detail.pai.type==TYPE_PAI_HONG) then san_hua = bit_left(1,1) 
                    elseif(detail.pai.type==TYPE_PAI_BAI) then san_hua = bit_left(1,2)
                    elseif(detail.pai.type==TYPE_PAI_QIAN) then san_hua = bit_left(1,3) 
                    end
                end
                temp_array:Push(detail)
                --类型不同跳过第一张
                if(temp_array:Size()>1 and temp_array:At(1).pai.type ~= temp_array:At(2).pai.type) then
                    
                    local t = temp_array:At(2)
                    temp_array:Clear()
                    temp_array:Push(t)
                    break
                end
                --类型不同跳过第二张
                if(temp_array:Size()>2 and temp_array:At(1).pai.type ~= temp_array:At(3).pai.type) then
                    
                    local t = temp_array:At(3)
                    temp_array:Clear()
                    temp_array:Push(t)
                    break
                end
                if(temp_array:Size()<3)then break end
                if(temp_array:At(1).pai.value == temp_array:At(2).pai.value and temp_array:At(1).pai.value == temp_array:At(3).pai.value) then
                
                    if(k<di_pai:Size()) then
                    
                        local detail2 = self:GetDetail(di_pai:At(k+1))
                        if( PaiEqual(temp_array:At(1).pai,detail2.pai)) then
                            local is_an_gang=false
                            for m=1,an_gang:Size() do
                                if(PaiEqual(an_gang:At(m),temp_array:At(1).pai)) then
                                
                                    an_gang_array:Push(temp_array:At(1))
                                    is_an_gang=true
                                    break
                                end
                            end
                            
                            if not(is_an_gang) then ming_gang_array:Push(temp_array:At(1)) 
                                temp_array:Clear()
                                k = k + 1
                                break
                            end
                        --明刻,剔除叫牌
                        else
                        
                            if(jiao_pai:IndexOf(temp_array:At(1))==0)then ming_ke_array:Push(temp_array:At(1))end
                            temp_array:Clear()
                        end
                    
                    else
                    
                        if(jiao_pai:IndexOf(temp_array:At(1))==0)then ming_ke_array:Push(temp_array:At(1))end
                            temp_array:Clear()
                    end
                
                else
                
                    temp_array:Clear()
                end
                break
            end
        end
    end
    local di_hu=0
    local ming_ke_hu=0
    local an_ke_hu = 0
    local ming_gang_hu =0
    local an_gang_hu =0
    local jiao_hu=0

    have_san_hua = ((bit_and(san_hua,bit_left(1,1)))>0) and ((bit_and(san_hua,bit_left(1,2)))>0) and ((bit_and(san_hua,bit_left(1,3)))>0)

    --将手牌上含有明刻的放到明杠
    sun_zi_array:Each(function ( k,v )
        for k=1,3 do
            local detail2 = self:GetDetail(v.pai.num+k-1);
            local index=ming_ke_array:IndexOf(detail2);
            if(index>0)then
                ming_ke_array:RemoveAt(index)
                ming_gang_array:Push(detail2)
            end
        end
    end)
    --明刻
    ming_ke_array:Each(function ( k,detail )
        local hu = PuKe.GetHuPaiRate(self.jiang_pai_type,detail,TYPE_RATE_MINGKE)
        ming_ke_hu = hu+ming_ke_hu
        if(detail.is_laojiang and detail.pai.type ~= TYPE_PAI_TIAO and have_san_hua)then ming_ke_hu=hu+ming_ke_hu end
    end)
    --暗刻
    an_ke_array:Each(function ( k,detail )
        local hu = PuKe.GetHuPaiRate(self.jiang_pai_type,detail,TYPE_RATE_ANKE)
        an_ke_hu = an_ke_hu + hu
        if(detail.is_laojiang and detail.pai.type ~= TYPE_PAI_TIAO and have_san_hua)then an_ke_hu=hu+an_ke_hu end
    end)
    --明杠
    ming_gang_array:Each(function ( k, detail)
        local hu = PuKe.GetHuPaiRate(self.jiang_pai_type,detail,TYPE_RATE_MINGGANG)
        ming_gang_hu = hu+ming_gang_hu
        if(detail.is_laojiang and detail.pai.type ~= TYPE_PAI_TIAO and have_san_hua)then ming_gang_hu=hu+ming_gang_hu end
    end)
    --按杠
    an_gang_array:Each(function ( k, detail)
        local hu = PuKe.GetHuPaiRate(self.jiang_pai_type,detail,TYPE_RATE_ANGANG)
        an_gang_hu = hu+an_gang_hu
        if(detail.is_laojiang and detail.pai.type ~= TYPE_PAI_TIAO and have_san_hua)then an_gang_hu=hu+an_gang_hu end
    end)
    --叫牌
    jiao_pai:Each(function ( k,v )
        local detail = self:GetDetail(v)
        local hu = PuKe.GetHuPaiRate(self.jiang_pai_type,detail,TYPE_RATE_JIAOPAI)
        jiao_hu = hu+jiao_hu
        if(detail.is_laojiang and detail.pai.type ~= TYPE_PAI_TIAO and have_san_hua)then jiao_hu=hu+jiao_hu end
    end)

    --LogInfo("ming ke:"+ming_ke_hu+" an ke:"+an_ke_hu+" ming gang:"+ming_gang_hu+" an gang:"+an_gang_hu+" jiao hu:"+jiao_hu);
    local di_hu = ming_ke_hu+an_ke_hu+ming_gang_hu+an_gang_hu+jiao_hu
    local ret_info = CreateObject(HuPaiInfo)
    ret_info.ming_ke_array=ming_ke_array
    ret_info.an_ke_array=an_ke_array
    ret_info.ming_gang_array=ming_gang_array
    ret_info.an_gang_array=an_gang_array
    ret_info.sun_zi_array=sun_zi_array
    ret_info.dui_zi_array=dui_zi_array
    ret_info.jiao_pai_array=jiao_pai_array
    ret_info.di_hu_score=di_hu
    ret_info.xi_array=xi_pai_array
    return ret_info

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
    --print('begin check win list size : ' .. tostring(self.pai_list:Size()))

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
    local num = 0
    self.pai_list:Each(function ( i,v )
        --print(i,v.type,v.detail.pai.num,v.value)
        local p = v;
        if(p.type ~= type or p.value ~= num) then
            type = p.type
            num = p.value
            for j=i+1,i+3 do
                --print(tostring(i),'j:'..tostring(j))
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
    --print('cehck win end size:',#self.win_node)
    return this.win_node
end
function CheckPaiNode:Check(p)
    --print('check',p.detail.pai.num,p.type,p.value,p.check)
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
    for i=1,3 do
        local b = p.brother[i]
        if b then
            b.check = 1
        end
        p.brother[i] = nil
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
            if v.check == 1 and v.type == p.type and v.value == p.value + 1 then
                p.brother[1] = v
                p.brother[1].check = bit_left(1,4)
                for j = i+1,i+3 do
                    if j > self.pai_list:Size() then break end
                    local p2 = self.pai_list:At(j)
                    if (p2.check == 1 and p2.type == p.type and p2.value == p.value + 2)then
                        p.brother[2] = p2
                        p.brother[2].check = bit_left(1,4)
                        --print('num',p2.detail.pai.num)
                        break
                    end
                end
                break
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
                        p.brother[2] = p2
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
    -- for i=1,3 do
    --     if(p.brother[i]) then
    --         print('p'..tostring(i),p.brother[i].detail.pai.num,p.brother[i].type,p.brother[i].value,p.brother[i].check)
    --     end

    -- end
    -- print('brother',p.detail.pai.num,p.type,p.value,p.check)
    -- print('---------------------------------------------------------------------')
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
                for j=1,#p.brother do
                    if p.brother[j] then
                        --print('insert table')
                        table.insert(temp,p.brother[j].detail)
                    end
                end
                --print('insert table')
                table.insert(win_info,temp)
            end
        end
        bubbleSort(win_info,CheckPaiNode.SortArray)
        local win_info2={}
        for k,v in pairs(win_info) do
            for k2,v2 in pairs(v) do
                table.insert(win_info2,v2)
            end
        end
        if(#win_info2 > 0) then table.insert( self.win_node,win_info2) end
    end
end

HuPaiInfo = {}

function HuPaiInfo:OnCreate()
    self.hu_type = TYPE_HU_NONE
    self.hu_pai_type = TYPE_HU_NONE
    self.ming_ke_array = CreateObject(Array)
    self.an_ke_array = CreateObject(Array)
    self.ming_gang_array = CreateObject(Array)
    self.an_gang_array = CreateObject(Array)
    self.xi_array = CreateObject(Array)
    self.sun_zi_array = CreateObject(Array)
    self.dui_zi_array = CreateObject(Array)
    self.jiao_pai_array = CreateObject(Array)
    self.hu_pai_array = CreateObject(Array)
    self.di_hu_score = 0
    self.totle_socre = 0
    self.wen_qiang_score = 0
    self.xi_pai_score = 0
    self.san_long_ju_hu_score = 0
end
function HuPaiInfo.SortInfo(a,b)
    return a.totle_socre - b.totle_socre
end
function HuPaiInfo:CaculateTotleScore(hu_pai,includ_xipai)
    local this = self
    local qiong_xi = false
    local xi_len = self.xi_array:Size()
    if xi_len == 0 then
        qiong_xi = true
        self.xi_pai_score = 0
    elseif xi_len == 1 then
        self.xi_pai_score = 10
    elseif xi_len == 2 then
        self.xi_pai_score = 30
    elseif xi_len == 3 then
        self.xi_pai_score = 50
    elseif xi_len == 4 then
        self.xi_pai_score = 100
    elseif xi_len == 5 then
        self.xi_pai_score = 200
    end

    if not(hu_pai) then
        self.totle_socre=self.di_hu_score + self.san_long_ju_hu_score + self.xi_pai_score
        self.totle_socre=math.ceil(self.totle_socre/10)*10
        return
    end
    if hu_pai then
        local wen_qiang_count=0
        local temp_hu_pai_type=TYPE_HU_NONE
        --单吊
        self.dui_zi_array:Each(function ( k,v )
            if(v.pai.value == hu_pai.pai.value and d.pai.type == hu_pai.pai.type) then
                temp_hu_pai_type = bit_or(TYPE_HU_DANGDIAO,temp_hu_pai_type)
            end
        end)
        --边张
        local have_bian_zhang=false
        self.sun_zi_array:Each(function ( k,d)
            if(d.pai.type == hu_pai.pai.type and ((d.pai.value == 1 and hu_pai.pai.value == 3) or (hu_pai.pai.value == 7 and d.pai.value == 7))) then
            
                have_bian_zhang = true
            end
            if(d.pai.value==1 and d.pai.type == TYPE_PAI_TONG)then
                wen_qiang_count = wen_qiang_count + 1
            end
            --丫子
            if(temp_hu_pai_type == TYPE_HU_NONE and (d.pai.value == hu_pai.pai.value - 1) and d.pai.type == hu_pai.pai.type) then

                temp_hu_pai_type = bit_or(temp_hu_pai_type,TYPE_HU_YAZI)
                
            end
        end)

        if(temp_hu_pai_type == TYPE_HU_NONE and have_bian_zhang) then
            temp_hu_pai_type = bit_or(temp_hu_pai_type,TYPE_HU_BAINZHANG)
        end
        self.hu_pai_type = bit_or(self.hu_pai_type,temp_hu_pai_type)
        if wen_qiang_count > 0 then
            self.hu_pai_type = bit_or(self.hu_pai_type,TYPE_HU_WENQIAN)
            if wen_qiang_count == 1 then
                self.wen_qiang_score = 20
            elseif wen_qiang_count == 2 then
                self.wen_qiang_score = 50
            elseif wen_qiang_count == 3 then
                self.wen_qiang_score = 100
            elseif wen_qiang_count == 4 then
                self.wen_qiang_score = 200
            end
        end
            
        self.totle_socre=self.di_hu_score + self.san_long_ju_hu_score + self.wen_qiang_score + self.xi_pai_score;
        if(bit_and(self.hu_pai_type,TYPE_HU_DANGDIAO)>0) then
        
            this.totle_socre = 10 + this.totle_socre
        end
        if(bit_and(self.hu_pai_type,TYPE_HU_YAZI)>0) then
        
            this.totle_socre = 10 + this.totle_socre
        end
        if(bit_and(this.hu_pai_type,TYPE_HU_BAINZHANG)>0) then
        
            this.totle_socre = 10 + this.totle_socre
        end
        if(bit_and(this.hu_pai_type,TYPE_HU_ZIMO)>0) then
        
            this.totle_socre = 10 + this.totle_socre
        end
        --天胡
        local  socre_rate=1;
        if(bit_and(this.hu_pai_type,TYPE_HU_TIANHU)>0) then
            socre_rate = 4
        elseif(bit_and(this.hu_pai_type,TYPE_HU_TIANTING)>0) then
            socre_rate = 2
        elseif(bit_and(this.hu_pai_type,TYPE_HU_QIONGXI)>0) then
            socre_rate = 2
        end
        --全是对子或者文钱
        if(self.sun_zi_array:Size() == 0  or (wen_qiang_count==self.sun_zi_array:Size()))then
            self.hu_type = TYPE_HU_PIAO
            self.totle_socre = 50 +self.totle_socre
            socre_rate = 2 *socre_rate
        --清胡,7个顺子
        elseif(self.sun_zi_array:Size()==7) then
            self.hu_type = TYPE_HU_QING
            self.totle_socre = 100+self.totle_socre
        
        --塔子湖
        else
            self.hu_type=TYPE_HU_TAZI
            self.totle_socre =20+ self.totle_socre
        end
        if(qiong_xi and includ_xipai) then
            socre_rate=2*socre_rate
            self.hu_pai_type =bit_or(TYPE_HU_QIONGXI,self.hu_pai_type)
        end
        self.totle_socre=math.ceil(self.totle_socre/10)*10*socre_rate
    end

end


----------------------------------------------------------------------------
ROOM_STATE_WAIT = 1
ROOM_STATE_PLAY = 2
ROOM_STATE_BALANCE = 3
ROOM_STATE_FREE = 4

allClientRoom = {}

function Room.CheckInRoom(client)
    local room_guid = allClientRoom[client.info.guid]
    if room_guid then
        local r = Room.Get(room_guid)
        if r then
            for i=1,r.players:Size() do
                local p = r.players:At(i)
                if p.info.guid == client.info.guid then
                    p.client = client
                    client.player = p
                    
                    return r
                end
            end
        end
        
    end
    return nil
end

function Room.Create(guid)
    local r = CreateObject(Room,{guid=guid})
    allRoom[guid] = r
    return r
end
function Room.Get(guid)
    return allRoom[guid]
end
function Room:Free()
    self.state = ROOM_STATE_FREE
    self.players:Each(function (k,v)
        
        allClientRoom[v.info.guid]=nil
        if v.client then 
            v.client.player = nil
            v.client.room = nil
          
            v.client.recoder_writer = nil
            v.client = nil
        end
        
    end)
    allRoom[self.guid] = nil
    self.players = nil
    if self.recoder_writer then 
        self.recoder_writer:Close()
    end

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
        if v.ready==true and v.client ~=nil then
            p.ready = true
        else
            p.ready = false
        end
        p.guid = v.info.guid
        p.headimgurl = v.info.headimgurl
        p.nick = v.info.nick
        room_players[k] = p
    end)
    self:BroadCastMessage(nil,SERVER_MSG.SM_ENTER_ROOM,{card = self.card,players = room_players})
    room_players = nil
end

function Room:BroadCastMessage(sender,type,msg,other)
    self.players:Each(function (k,v)
        
        if other then
            if v.client and v.client.info.guid ~= sender.client.info.guid then v.client:SendMessage(type,msg) end
        else
            if (v.client) then v.client:SendMessage(type,msg) end
        end
    end)
   
end

function Room:BroadCastDiffMessage(sender,type,msg1,msg2)
    self.players:Each(function (k,v)
        if v.client and v.client.info.guid == sender.client.info.guid then 
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
    elseif len > 0 then
        self.players:At(1).index = 1
    end
end

function Room:Check(client)
    return client.room and client.room.guid == self.guid
end
function Room:SaveRecoderFile()
    if (WRITE_RECODER) and self.recoder_writer == nil then 
        
       
        local file_name = string.format('%s_%s',tostring(self.guid),tostring(os.time()))
        self.recoder_writer = AsyncFileWriter()
        self.recoder_writer:Create(RECODER_PATH .. file_name)
        self.recoder_file_name = file_name
        local write_info = {}
        self.players:Each(function (k,v)
            v.client.recoder_writer = self.recoder_writer
            table.insert( write_info,v.info)
            
        end)

        self.recoder_writer:PushContentLine(json.encode(write_info))

        write_info = nil
    end
    
end
function Room:Start()
    if self.state == ROOM_STATE_PLAY then return false end
    if self.players:Size() < GAME_MAX_PLAYERS then return false end
    local all_ready = true
    self.players:Each(function (k,v)
        if not(v.ready) then 
            all_ready = false
            if(self.card.used == 0) then allClientRoom[v.info.guid] = nil end
        else
            allClientRoom[v.info.guid] = self.guid
        end
    end)
    if not(all_ready) then return false end
    self.state = ROOM_STATE_PLAY
    self:SaveRecoderFile()
--游戏开始
    self.maizhuang_count=0
    self.card.used = self.card.used + 1
    self.puke:Init(self.card.xi)
    self:SwapPlayer()
    self.next_mopai_palyer = self.players:At(1)
    self.next_chupai_palyer = nil
    self.test_palyers = nil
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
    if len > GAME_MAX_PLAYERS or self:Check(client) then return false end
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
        local free = true
        self.players:Each(function ( k,v )
            if(v.client and v.info.guid ~= client.info.guid) then
                free = false
                v.client:SendMessage(SERVER_MSG.SM_LEAVE_ROOM,{guid = client.info.guid})
            end
        end)
        client.room = nil
        client.player.client = nil
        client.player = nil
        client.recoder_writer=nil
        if(free) then 
            if self.state ~= ROOM_STATE_FREE then 
                self:Balance(true)
            end
        end
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

TYPE_REFRESH_MOPAI = 1
TYPE_REFRESH_CHUPAI = 2

function Room:RefreshPlayer(player,type,pai)
    local di = player.di_pai:Data()
    local shou = player.shou_pai:Data()
    local qi = player.qi_pai_array:Data()
    self.players:Each(function ( k,v )
        if not(v.client) then return end
        if v.info.guid == player.info.guid then
            v.client:SendMessage(SERVER_MSG.SM_REFRESH_PAI,{
                guid=player.info.guid,
                di = di,
                shou = shou,
                qi = qi,
                type= type,
                pai = pai,
                size1 = player.shou_pai:Size(),
                size2=self.puke:GetSize()
            })
        else
            v.client:SendMessage(SERVER_MSG.SM_REFRESH_PAI,{
                guid=player.info.guid,
                di = di,
                qi = qi,
                pai = pai,
                type= type,
                size1 = player.shou_pai:Size(),
                size2=self.puke:GetSize()
            })
        end
    end)
end
function Room:MoPai()
    if self.next_chupai_palyer then self:RefreshPlayer(self.next_chupai_palyer,TYPE_REFRESH_CHUPAI,self.next_chupai_palyer.qi_pai_array:Last()) end
    self:EndTest()
    local number = self.puke:Get()
    local pai = CreatePai(number)
    local refresh_dipai = false
    while pai.type == TYPE_PAI_XI do
        refresh_dipai = true
        self.next_mopai_palyer:DiPai(number)
        number = self.puke:Get()
        pai = CreatePai(number)
    end
    self.test_palyers=CreateObject(Array)


    if self.next_mopai_palyer:TestHu(number,self.puke) then
        self.test_hu_players = CreateObject(Array)
        self.test_hu_players:Push({player = self.next_mopai_palyer,test = self.next_mopai_palyer.test,type = TYPE_TEST_ZIMO})
        log_info('begin test zi mo:' .. self.next_mopai_palyer.info.nick)
    end
    self.next_mopai_palyer:MoPai(number)
    if self.next_mopai_palyer:TestAnGang() then
        self.test_gan_players = CreateObject(Array)
        self.test_gan_players:Push({player = self.next_mopai_palyer,test = self.next_mopai_palyer.test,type = TYPE_TEST_ANGANG})
        log_info('begin test angang:' .. self.next_mopai_palyer.info.nick)
    end


    self:RefreshPlayer(self.next_mopai_palyer,TYPE_REFRESH_MOPAI,number)

    -- local di = nil
    -- if refresh_dipai then di=self.next_mopai_palyer.di_pai:Data() end
    -- self.players:Each(function ( k,v )
    --     if not(v.client) then return end
    --     if v.info.guid == self.next_mopai_palyer.info.guid then
    --         v.client:SendMessage(SERVER_MSG.SM_MO_PAI,{
    --             guid=self.next_mopai_palyer.info.guid,
    --             pai=number,
    --             di = di,
    --             size1=self.next_mopai_palyer.shou_pai:Size(),
    --             size2=self.puke:GetSize()
    --         })
    --     else
    --         v.client:SendMessage(SERVER_MSG.SM_MO_PAI,{
    --             guid=self.next_mopai_palyer.info.guid,
    --             di = di,
    --             size1=self.next_mopai_palyer.shou_pai:Size(),
    --             size2=self.puke:GetSize()
    --         })
    --     end
    -- end)


    if not(self:BeginTest()) then
        self.next_chupai_palyer = self.next_mopai_palyer
        self.next_mopai_palyer = self.players:At((self.next_chupai_palyer.index % GAME_MAX_PLAYERS) + 1)
    end
        
    
   
    

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
function Room:CreateTestPlayers()
    self.test_palyers=CreateObject(Array)
    local len = self.players:Size() - 2
    for i=0,len do
        self.test_palyers:Push(self.players:At(((self.next_chupai_palyer.index+i) % GAME_MAX_PLAYERS) + 1))
    end
end

function Room:BeginTest()
    if self.test_hu_players and self.test_hu_players:Size() > 0 then
        self.current_test_info = self.test_hu_players:At(1)
        self.test_hu_players:RemoveAt(1)
        self.current_test_info.player.client:SendMessage(SERVER_MSG.SM_TEST_PAI,{type = self.current_test_info.type,value = self.current_test_info.test.value})
        log_info('begin test hu')
        return true
    end
    if self.test_gan_players and self.test_gan_players:Size() > 0 then
        local player = self.test_gan_players:At(1)
        self.current_test_info = self.test_gan_players:At(1)
        self.test_gan_players:Clear()
        self.current_test_info.player.client:SendMessage(SERVER_MSG.SM_TEST_PAI,{type = self.current_test_info.type,value = self.current_test_info.test.value})
        log_info('begin test gang')
        return true
    end
    if self.test_peng_players and self.test_peng_players:Size() > 0 then
        local player = self.test_peng_players:At(1)
        self.current_test_info = self.test_peng_players:At(1)
        self.test_peng_players:Clear()
        self.current_test_info.player.client:SendMessage(SERVER_MSG.SM_TEST_PAI,{type = self.current_test_info.type,value = self.current_test_info.test.value})
        log_info('begin test peng')
        return true
    end
    return false
end
function Room:EndTest()
    self.current_test_info = nil
    if self.test_gan_players then self.test_peng_players:Clear() end
    if self.test_gan_players then self.test_gan_players:Clear() end
    if self.test_hu_players then self.test_hu_players:Clear() end
end


function Room:ChuPai(client,msg)
    if client.player ~= self.next_chupai_palyer then return end
    if not(self.next_chupai_palyer:ChuPai(msg.pai)) then 
        log_error('client not have pai : ' .. tostring(msg.pai))
        return 
    end
    self:BroadCastMessage(nil,SERVER_MSG.SM_CHU_PAI,{guid = client.info.guid,pai = msg.pai})
    --判断其余两家是否要这张牌
    self:CreateTestPlayers()
    self.test_hu_players = CreateObject(Array)
    self.test_gan_players = CreateObject(Array)
    self.test_peng_players = CreateObject(Array)
    for i=1,self.test_palyers:Size() do
        if(self.test_palyers:At(i):TestHu(msg.pai,self.puke)) then
            local p = self.test_palyers:At(i);
            self.test_hu_players:Push({player = p,test = p.test,type = TYPE_TEST_HU})
        end
        if(self.test_gan_players:Size() == 0 and self.test_palyers:At(i):TestGang(msg.pai)) then
            local p = self.test_palyers:At(i);
            self.test_gan_players:Push({player = p,test = p.test,type = TYPE_TEST_GANG})
        end
        if(self.test_peng_players:Size() == 0 and self.test_palyers:At(i):TestPeng(msg.pai)) then
            local p = self.test_palyers:At(i);
            self.test_peng_players:Push({player = p,test = p.test,type = TYPE_TEST_PENG})
        end
    end
    if self:BeginTest() then
    else
        self.next_chupai_palyer.qi_pai_array:Push(msg.pai)
        self:MoPai()
    end
end
function Room:Test(client,msg)
    msg = msg or {test = false}
    if(not(self.current_test_info) or self.current_test_info.player ~= client.player) then return end
    local current_test = self.current_test_info.test;
    if(self.current_test_info.type == TYPE_TEST_ZIMO) then
        if msg.test then
            --胡牌，结束游戏
            client.player:ApplyTest(current_test)
            self:BroadCastMessage(nil,SERVER_MSG.SM_HU_PAI,{type = TYPE_TEST_ZIMO,guid=client.info.guid})
            print('zimo',client.info.nick)
            self:Balance()
        elseif not(self:BeginTest()) then
            self.next_chupai_palyer = self.next_mopai_palyer
            self.next_mopai_palyer = self.players:At((self.next_chupai_palyer.index % GAME_MAX_PLAYERS) + 1)
        end

    elseif(self.current_test_info.type == TYPE_TEST_HU) then
        if msg.test then
            --胡牌，结束游戏
            client.player:ApplyTest(current_test)
            self:BroadCastMessage(nil,SERVER_MSG.SM_HU_PAI,{type = TYPE_TEST_HU,guid=client.info.guid})
            print('hu',client.info.nick)
            self:Balance()
        elseif not(self:BeginTest()) then
            --没人要，为出牌玩家添加弃牌
            self.next_chupai_palyer.qi_pai_array:Push(current_test.value)
            self:MoPai()
        end
    elseif (self.current_test_info.type == TYPE_TEST_ANGANG) then
        if msg.test then
            local pai = current_test.value
            client.player:ApplyTest(current_test)
            self.next_mopai_palyer = client.player
            self:BroadCastDiffMessage(client.player,SERVER_MSG.SM_GANG_PAI,
            {type = TYPE_TEST_ANGANG,guid=client.info.guid,pai=pai,shou = client.player.shou_pai:Data(),di = client.player.di_pai:Data(),size1=self.next_mopai_palyer.shou_pai:Size(),
            size2=self.puke:GetSize()},
            {type = TYPE_TEST_ANGANG,guid=client.info.guid,pai=pai,di = client.player.di_pai:Data(),size1=self.next_mopai_palyer.shou_pai:Size(),
            size2=self.puke:GetSize()})
            print('angan',client.info.nick)
            self:MoPai()
        elseif not(self:BeginTest()) then
            self.next_chupai_palyer = self.next_mopai_palyer
            self.next_mopai_palyer = self.players:At((self.next_chupai_palyer.index % GAME_MAX_PLAYERS) + 1)
        end
    elseif (self.current_test_info.type == TYPE_TEST_GANG) then
        if msg.test then
            local pai = current_test.value
            client.player:ApplyTest(current_test)
            self.next_mopai_palyer = client.player
            self:BroadCastDiffMessage(client.player,SERVER_MSG.SM_GANG_PAI,
            {type = TYPE_TEST_GANG,guid=client.info.guid,pai=pai,shou = client.player.shou_pai:Data(),di = client.player.di_pai:Data(),size1=self.next_mopai_palyer.shou_pai:Size(),
            size2=self.puke:GetSize()},
            {type = TYPE_TEST_GANG,guid=client.info.guid,pai=pai,di = client.player.di_pai:Data(),size1=self.next_mopai_palyer.shou_pai:Size(),
            size2=self.puke:GetSize()})
            print('gang',client.info.nick)
            self:MoPai()
        elseif not(self:BeginTest()) then
            --没人要，为出牌玩家添加弃牌
            self.next_chupai_palyer.qi_pai_array:Push(current_test.value)
            self:MoPai()
        end
    elseif (self.current_test_info.type == TYPE_TEST_PENG) then
        if msg.test then
            local pai = current_test.value
            client.player:ApplyTest(current_test)
            self.next_chupai_palyer = client.player
            self.next_mopai_palyer = self.players:At((self.next_chupai_palyer.index % GAME_MAX_PLAYERS) + 1)
            self:BroadCastDiffMessage(client.player,SERVER_MSG.SM_PENG_PAI,
            {type = TYPE_TEST_PENG,guid=client.info.guid,pai=pai,shou = client.player.shou_pai:Data(),di = client.player.di_pai:Data(),size1=self.next_mopai_palyer.shou_pai:Size(),
            size2=self.puke:GetSize()},
            {type = TYPE_TEST_PENG,guid=client.info.guid,pai=pai,di = client.player.di_pai:Data(),size1=self.next_mopai_palyer.shou_pai:Size(),
            size2=self.puke:GetSize()})
            print('peng',client.info.nick)
            
        elseif not(self:BeginTest()) then
            --没人要，为出牌玩家添加弃牌
            self.next_chupai_palyer.qi_pai_array:Push(current_test.value)
            self:MoPai()
        end
    -- else
    --     --没人要，为出牌玩家添加弃牌
    --     self.next_chupai_palyer.qi_pai_array:Push(client.player.test.value)
    --     self:MoPai()
    end
end


function Room:Reconnect(client)
    local all_sync_message = {}
    if self.current_test_info then all_sync_message.test = {guid =self.current_test_info.player.info.guid, type = self.current_test_info.type,value = self.current_test_info.test.value} end
    if self.next_mopai_palyer then all_sync_message.current = self.next_mopai_palyer.info.guid end
    all_sync_message.size2=self.puke:GetSize()
    all_sync_message.card = self.card
    all_sync_message.jiangpai = self.puke.jiang_pai
    all_sync_message.players = {}

    self.players:Each(function ( k,v )
        if not(v.client) then return end
        local di = v.di_pai:Data()
        local shou = v.shou_pai:Data()
        local qi = v.qi_pai_array:Data()
        if v.info.guid == client.player.info.guid then
            table.insert( all_sync_message.players, {
                guid = v.info.guid,
                nick = v.info.nick,
                headimgurl =v.info.headimgurl,
                di = di,
                shou = shou,
                qi = qi,
                size1 = v.shou_pai:Size(),
                maizhuag=v:MaiZhuang()
                
            })
        else
            table.insert( all_sync_message.players, {
                guid = v.info.guid,
                nick = v.info.nick,
                headimgurl =v.info.headimgurl,
                di = di,
                qi = qi,
                size1 = v.shou_pai:Size(),
                maizhuag=v:MaiZhuang()
            })
            v.client:SendMessage(SERVER_MSG.SM_RECONNECT,{guid = client.info.guid})
        end
    end)

    client:SendMessage(SERVER_MSG.SM_SYNC_ROOM_STATE,all_sync_message)

end


function Room:Balance(payOnly)
    self.state = ROOM_STATE_BALANCE
    local blance_msg = CreateObject(Array)
    
    --self.score_recoder = self.score_recoder or CreateObject(Array)
    self.player_score_map = self.player_score_map or {}
    local current_score = {}
    if not(payOnly) then 
        local recoder = CreateObject(Array)
        self.players:Each(function ( k,v )
            v:CaculateScore(self.puke)
            local msg = {}
            msg.guid = v.info.guid
            msg.hu = v.hu_pai_info.totle_socre
            if self.card.limit > 0 then 
                msg.hu = math.min(self.card.limit,msg.hu)
            end
            msg.type1 = v.hu_pai_info.hu_pai_type
            msg.type2 = v.hu_pai_info.hu_type
            msg.shou = v.shou_pai:Data()
            msg.di = v.di_pai:Data()

            blance_msg:Push(msg)
            print(v.info.nick,'score',msg.hu)
            --记录小局结算数据
            recoder:Push({
                guid = v.info.guid,
                hu = msg.hu,
                maizhuang = v:MaiZhuang()
            })


        end)

        
        recoder:Each(function ( j,v )
            local p1 = v
            local p2 = recoder:At((j % recoder:Size())+1)
            local p3 = recoder:At(((j+1) % recoder:Size())+1)
            local s1 = Room.CaculateScore(p1,p2,self.card.rate)
            local s2 = Room.CaculateScore(p1,p3,self.card.rate)
            local list = self.player_score_map[p1.guid]
            if not(list) then
                list = {
                }
                self.player_score_map[p1.guid] = list
            end
            table.insert(current_score,{guid= p1.guid,score= s1+s2})
            table.insert( list,s1+s2)
        end)
    end
    




    local winner_guid = nil
    local loser_guid = nil
    if self.current_test_info and self.current_test_info.player then winner_guid = self.current_test_info.player.info.guid end
    if self.next_chupai_palyer then loser_guid = self.next_chupai_palyer.info.guid end
    
    --self.card.maxUseCount = 2
    
    if self.card.maxUseCount > self.card.used and not(payOnly) then
        self:BroadCastMessage(nil,SERVER_MSG.SM_GAME_BALANCE,{score=current_score,loser = loser_guid,winner=winner_guid,balance = blance_msg.tbl})
        self:Clean()
    else
        --大结算
        -- local player_score_map = {}
        -- self.score_recoder:Each(function ( i,v )
        --     local scores = v
        --     scores:Each(function ( j,v )
        --         local p1 = v
        --         local p2 = scores:At((j % scores:Size())+1)
        --         local p3 = scores:At(((j+1) % scores:Size())+1)
        --         local s1 = Room.CaculateScore(p1,p2,self.card.rate)
        --         local list = player_score_map[p1.guid]
        --         if not(list) then
        --             list = {
        --                 score={}
        --             }
        --             player_score_map[p1.guid] = list
        --         end
        --         table.insert( list.score,s1+s2)
        --     end)
        -- end)
        local winnerguid=nil
        local winnerScore=-1
        local final_score={}
        local total = {}
        for key,list in pairs(self.player_score_map) do
            table.insert( total,{guid=key,score=list})
            local sum = 0
            for k,v in pairs(list) do
                sum = sum + v
            end 
            table.insert( final_score,{guid = key,socre= sum})
            if sum > winnerScore then
                winnerScore = sum
                winnerguid = key
            end
        end
        if self.recoder_writer then
            local res = RedisResponse()
            local json_score = json.encode(final_score)
            self.players:Each(function ( k,v )
                SaveSession(gServer.db,res,'record',string.format('%s_%s',tostring(v.info.guid),self.recoder_file_name),json_score,7*24*60*60)
            end)
            
        end
        

        if self.card.pay == PayType.Host or winnerScore <= 0 or winnerguid == nil then
            local user = GetUserByUnionid(gServer.db,self.card.owner)
            if user then
                user[self.card.currency] = user[self.card.currency] - self.card.cost
                SaveUser(gServer.db,user)
            end
           
        elseif self.card.pay == PayType.AA then
            local d = math.ceil(self.card.cost /self.players:Size())
            self.players:Each(function ( k,v )
                
                local user = GetUserByGuid(gServer.db,v.info.guid)
                if user then
                    user[self.card.currency] = user[self.card.currency] - d
                    SaveUser(gServer.db,user)
                end
            end)
        elseif self.card.pay == PayType.Winer then
            local user = GetUserByGuid(gServer.db,winnerguid)
            if user then
                user[self.card.currency] = user[self.card.currency] - self.card.cost
                SaveUser(gServer.db,user)
            end
        end

       print(winner_guid,winnerScore)
       print(json.encode(final_score))
        self:BroadCastMessage(nil,SERVER_MSG.SM_GAME_BALANCE,{score=current_score,loser = loser_guid,winner=winner_guid,balance = blance_msg.tbl,total=total})
        --self:BroadCastMessage(nil,SERVER_MSG.SM_GAME_BALANCE,{loser = loser_guid,winner=winner_guid,balance = blance_msg.tbl,score=self.player_score_map})
        
        
        
        self:Free()
    end
end

function Room.CaculateScore(p1,p2,rate)
    local d=p1.hu-p2.hu
    local scale=rate[1]
    if(p1.maizhuang and p2.maizhuang) then
        scale=rate[3]
    elseif(p1.maizhuang or p2.maizhuang) then
        scale=rate[2]
    end
    return d*scale
end

function Room:Clean()
    self.state= ROOM_STATE_WAIT
    self.maizhuang_count = 0
    self.players:Each(function (k,v)
        v:Reset()
    end)
end