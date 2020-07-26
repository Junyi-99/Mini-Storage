#! /bin/bash

rm -rf ./bin > /dev/null
rm -rf ./build > /dev/null

mkdir build

cd build || exit

echo "Building"

cmake ../

make -j

cd ..

rm -rf ./build

cp client/small_file_genor.sh     ./bin/gen_small_file.sh
cp client/big_file_genor.sh       ./bin/gen_big_file.sh
cp client/upload_20_small_file.sh ./bin/upload_20_small_file.sh

echo "Build Successful! The file has been written to ./bin"