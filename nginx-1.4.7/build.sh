#!/bin/sh
INSTALL_PATH="./webserver"
INCLUDE_PATH="-I./src/aret/ngx_aret_ctl/ -I../libs/jsonc/ -I../libs/tera/ -I./src/aret/service/src/"
LD_PATH="-ltera_easy -ljson -L./src/aret/ -L../libs/jsonc/ -L../libs/tera/"
rm -rf ${INSTALL_PATH}
./configure --prefix=${INSTALL_PATH} --with-cc-opt="${INCLUDE_PATH}" --with-ld-opt="${LD_PATH}" --add-module=./src/aret/ngx_aret_ctl

# update g++
sed -i '1095s/\$(CC)/g++/' objs/Makefile;
sed -i '1102s/\$(CC)/g++/' objs/Makefile;
sed -i '1109s/\$(CC)/g++/' objs/Makefile;
sed -i '1116s/\$(CC)/g++/' objs/Makefile;
sed -i '1123s/\$(CC)/g++/' objs/Makefile;
sed -i '219s/\$(LINK)/g++/' objs/Makefile; 

#cp objs_Makefile objs/Makefile
make -j8 && make install
