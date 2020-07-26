#! /bin/bash

file_name="test_big.file"
echo "$file_name">> filelist
dd if=/dev/urandom of=$file_name count=4096000 bs=1024

# end of file
