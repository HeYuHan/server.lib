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
	rm -rf srs.server*
	mkdir -p srs.server.build/script
	mkdir -p srs.server.build/log
	cp bin/channelserver srs.server.build/
	cp bin/proxyserver srs.server.build/
	cp -r script/* srs.server.build/script/
	cp bin/srs_* srs.server.build/
	cp bin/keep_process srs.server.build/
	chmod -R 777 srs.server.build/
	tar -zcvf srs.server.build.$(shell date +%Y%m%d%H%M%S).tar.gz srs.server.build
	rm -rf srs.server.build

