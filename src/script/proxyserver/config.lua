ProxyConfigExt = {}
ProxyConfigExt.__index = ProxyConfigExt

function ProxyConfigExt:OnEnter(config)
    config:SetMaxRoom(50)
    config:SetMaxChannel(10)
    config:SetEeventPoolSize(10)
    config:SetMaxClient(10)
end