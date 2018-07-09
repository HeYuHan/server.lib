debug:
	$(MAKE) -C common
	$(MAKE) -C channelserver
	$(MAKE) -C loginserver

release:
	$(MAKE) RELEASE=1 -C common
	$(MAKE) RELEASE=1 -C channelserver
	$(MAKE) RELEASE=1 -C loginserver

clean:
	$(MAKE) clean -C common
	$(MAKE) clean -C channelserver
	$(MAKE) clean -C loginserver
	rm -rf ntcp.server*

deps:
	$(MAKE) deps -C common

clean_deps:
	$(MAKE) clean_deps -C common

package:
	rm -rf ntcp.server*
	mkdir -p ntcp.server.build/v8
	mkdir -p ntcp.server.build/js
	mkdir -p ntcp.server.build/log
	cp bin/channelserver ntcp.server.build/
	cp script/js/* ntcp.server.build/js/
	cp bin/v8/*.bin ntcp.server.build/
	cp bin/ntcp_service ntcp.server.build/
	cp bin/keep_process ntcp.server.build/
	cp ../3rd/v86.4.99/lib/relsease/* ntcp.server.build/v8
	echo -e "export LD_LIBRARY_PATH=./v8:$LD_LIBRARY_PATH\nchannelserver channelserver 0.0.0.0:9500 ./js/main.js" > ntcp.server.build/ntcp_config
	chmod -R 777 ntcp.server.build/
	tar -zcvf ntcp.server.build.$(shell date +%Y%m%d%H%M%S).tar.gz ntcp.server.build
	rm -rf ntcp.server.build

