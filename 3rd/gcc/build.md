##############################################################################################
tar glibc-2.14.tar.gz  
cd glibc-2.14  
mkdir build  
cd build  


../glibc_2.xxx/configure --prefix=/usr/local/glibc-2.14  --with-headers=/usr/include  
make -j4  
make install  

ln -s  /usr/local/glibc-2.14/lib/libc-2.14.so   /lib64/libc.so.6  


##############################################################################################