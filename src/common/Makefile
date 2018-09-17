CC = g++
C = gcc
AR = ar

EVENT_SOURCE_DIR = ../../3rd/libevent
LOG4CPP_SOURCE_DIR = ../../3rd/log4cpp

#LFLAGS = -m64 -pie -fPIC

CFLAGS = -c -fPIC
CFLAGS += -I.
CFLAGS += -I/usr/local/include
CFLAGS += -I/usr/local/include/hiredis



CPPFLAGS = -std=c++11

EVENT_LIB = /usr/local/lib/libevent.a 
EVENT_LIB_CORE = /usr/local/lib/libevent_core.a 
EVENT_LIB_EXT = /usr/local/lib/libevent_extra.a 
EVENT_LIB_PTHREAD = /usr/local/lib/libevent_pthreads.a
LOG_LIB = /usr/local/lib/liblog4cpp.a



EVENT_LIB_PREFIX = --prefix=/usr/local
LOG_LIB_PREFIX = --prefix=/usr/local

TARGET = ../lib/libcommon.a
SOURCES = $(wildcard *.cpp) $(wildcard LuaBridge/*.cpp)
OBJS += $(patsubst %.cpp,objs/%.o,$(SOURCES)) 

LUA_SOURCES = $(wildcard lua/*.c)
LUA = $(patsubst %.c,objs/%.o,$(LUA_SOURCES))
LUA_JSON_SOURCES = luacjson/fpconv.c luacjson/lua_cjson.c luacjson/strbuf.c
LUA_JSON = $(patsubst %.c,objs/%.o,$(LUA_JSON_SOURCES))

PF = LINUX
DEPS_LIB = $(LOG_LIB) $(EVENT_LIB)

CFLAGS += -D$(PF)

ifdef RELEASE
	CFLAGS += -Wall -Werror -g -O2 
else
	CFLAGS += -Wall -Werror -g -D_DEBUG 
endif





default : $(TARGET)

objs/luacjson/%.o: luacjson/%.c
	$(C) $(CFLAGS) -o $@ $<

objs/lua/%.o: lua/%.c
	$(C) $(CFLAGS) -o $@ $<

objs/%.o: %.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS)  -o $@ $<

clean :
	rm -f $(TARGET) objs/*.o *.o objs/lua/*.o objs/LuaBridge/*.o objs/luacjson/*.o
	

$(TARGET) : $(OBJS) $(LUA) $(LUA_JSON)
	
	$(AR) -x $(EVENT_LIB)
	$(AR) -x $(EVENT_LIB_CORE)
	$(AR) -x $(EVENT_LIB_EXT)
	$(AR) -x $(EVENT_LIB_PTHREAD)
	$(AR) -x $(LOG_LIB)

	$(AR) $(LFLAGS) rcs $@ $(OBJS) $(LUA) $(LUA_JSON) *.o
	rm -f *.o
	