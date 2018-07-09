print('student lua enter')
StudentExt = {var1 = 0}
StudentExt.__index = StudentExt

function StudentExt:OnEnter(native)
    self.native:Run()
    self.native:Run(111)
    print('Student:OnEnter=>' .. self.var1)
   
end
