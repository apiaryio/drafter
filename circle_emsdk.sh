#!/bin/sh
sudo add-apt-repository -y ppa:git-core/ppa
sudo apt-get update -y
sudo apt-get install git
sudo apt-get install gcc-4.7 g++-4.7
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.7 20
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.7 20
sudo apt-get -y install cmake
curl -sO http://www.cmake.org/files/v3.2/cmake-3.2.1.tar.gz
tar -xzf cmake-3.2.1.tar.gz
cd cmake-3.2.1 && mkdir -p _build && cd _build && cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr && make && sudo make install && sudo ldconfig
cd ../../;
curl -sO https://s3.amazonaws.com/mozilla-games/emscripten/releases/emsdk-portable.tar.gz
mkdir -p emsdk
tar -xzf emsdk-portable.tar.gz -C emsdk --strip-components=1
cd emsdk
./emsdk update
sed -i "s|fastcomp/|fastcomp|g" emsdk_manifest.json
sed -i "s|clang/|clang|g" emsdk_manifest.json
./emsdk install latest -j1
./emsdk activate latest
cp ~/.emscripten .
cd ../
