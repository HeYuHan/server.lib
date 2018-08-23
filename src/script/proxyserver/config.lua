ProxyConfigExt = {}
ProxyConfigExt.__index = ProxyConfigExt

function ProxyConfigExt:OnEnter(config)
    config:SetMaxRoom(50)
    config:SetMaxChannel(10)
    config:SetEeventPoolSize(4)
    config:SetMaxClient(10)
end