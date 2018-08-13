print('student lua enter')
StudentExt = {var1 = 1}
StudentExt.__index = StudentExt

function StudentExt:OnEnter(native)
    --native:Run()
    native:Run(111)
    print('Student:OnEnter=>' .. self.var1)
    native:PrintArg()
   
end
local s = Student:new()
s:Run(12345)