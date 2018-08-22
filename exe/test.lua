LuaTestExt = {}
LuaTestExt.__index = LuaTestExt

function LuaTestExt:OnEnter(config)
    print('lua enter....')
    print(config:GetClassName())
    config.intData=100
    config.testData=1234
    print(config.testData)
end