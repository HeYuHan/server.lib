#CCFLAGS = ['-I/usr/include/lua5.1', '-O2', '-ansi']
CCFLAGS = ['-I/usr/include', '-O2', '-ansi']
LIBPATH = ['/usr/local/lib']
LIBS = ['lua', 'dl', 'm']
prefix = '/mingw'
#build_dev=1
tolua_bin = 'tolua++5.1'
tolua_lib = 'tolua++5.1'
TOLUAPP = 'tolua++5.1'

