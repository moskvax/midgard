#!/bin/bash
set -e

export LD_LIBRARY_PATH=.:`cat /etc/ld.so.conf.d/* | grep -v -E "#" | tr "\\n" ":" | sed -e "s/:$//g"`

#add ppas
sudo add-apt-repository -y $(echo $@ | grep -F "ppa:")

#check whats in them
#TODO: only update the ppas we added
#sudo apt-get update -o Dir::Etc::sourcelist="sources.list.d/ubuntu-toolchain-r-test-$(lsb_release -c -s).list" -o Dir::Etc::sourceparts="-" -o APT::Get::List-Cleanup="0"
sudo apt-get update

#install some stuff
sudo apt-get install -y $(echo $@ | grep -vF "ppa:")

#need newer compilers
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.9 90
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.9 90

#build this package
./autogen.sh
./configure --enable-coverage CPPFLAGS=-DBOOST_SPIRIT_THREADSAFE
make test -j$(nproc)
sudo make install
