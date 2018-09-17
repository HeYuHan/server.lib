#openssl
#./config --prefix=/usr/local
#curl 
#export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH 

#./configure --prefix=/usr/local --with-ssl=/usr/local --disable-ldap --disable-ldaps

CC = g++

LFLAGS = -m64 -pthread -lz -ldl -lssl
CFLAGS = -c -fPIC -DBUILDING_LIBCURL

PF = LINUX
COMMON_LIB = ../lib/libcommon.a
CURL_LIB = /usr/local/lib/libcurl.a
HIREDIS = /usr/local/lib/libhiredis.a
#OPENSSL = /usr/local/lib64/libcrypto.a /usr/local/lib64/*.a
TARGET = ../bin/luaserver
SOURCES = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp,objs/%.o,$(SOURCES)) $(COMMON_LIB) $(CURL_LIB) $(OPENSSL) $(HIREDIS)


ifneq ($(shell uname), Linux) 
	PF = MACOS
else
	LFLAGS += -lrt
endif

CFLAGS += -D$(PF)

ifdef RELEASE
	CFLAGS += -Wall  -g -O2
	LFLAGS += -Wall  -g -O2
else
	CFLAGS += -g -D_DEBUG
	LFLAGS += -g
endif
CFLAGS += -I.
CFLAGS += -I./../common
CFLAGS += -I/usr/local
CFLAGS += -I/usr/local/hiredis
CFLAGS += -std=c++0x


#DEPS = $(patsubst %.cpp,objs/%.d,$(SOURCES))


default : $(TARGET)

objs/%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ $<

#-include $(DEPS)

clean :
	rm -f $(TARGET) objs/*.o

$(TARGET) : $(TEMP) $(OBJS)
	$(CC) $(LFLAGS) -o $@ $(OBJS) 
	