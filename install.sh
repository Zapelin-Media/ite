#!/bin/sh

cd it950x_driver
make 
make install
cd ..


cd it951x_driver
make
make install
cd ..

depmod -a

cd it95xx_player
make
cp zmi /usr/bin/zapelinItePlay
cd ..
