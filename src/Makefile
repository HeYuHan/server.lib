#c++11
#wget http://people.centos.org/tru/devtools-2/devtools-2.repo -O /etc/yum.repos.d/devtools-2.repo
#yum install devtoolset-2-gcc devtoolset-2-binutils devtoolset-2-gcc-c++
#ln -s /opt/rh/devtoolset-2/root/usr/bin/* /usr/local/bin/
#hash -r
#gcc --version
debug:
	$(MAKE) -C common
	$(MAKE) -C luaserver

release:
	$(MAKE) RELEASE=1 -C common
	$(MAKE) RELEASE=1 -C luaserver

clean:
	$(MAKE) clean -C common
	$(MAKE) clean -C luaserver
	rm -rf ntcp.server*

deps:
	$(MAKE) deps -C common

clean_deps:
	$(MAKE) clean_deps -C common

package:
	rm -rf ntcp.server*
	mkdir -p ntcp.server.build/script
	mkdir -p ntcp.server.build/log
	cp bin/luaserver ntcp.server.build/
	cp -r script/* ntcp.server.build/script/
	cp bin/ntcp_* ntcp.server.build/
	cp bin/keep_process ntcp.server.build/
	chmod -R 777 ntcp.server.build/
	tar -zcvf ntcp.server.build.$(shell date +%Y%m%d%H%M%S).tar.gz ntcp.server.build
	rm -rf ntcp.server.build

