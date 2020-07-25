#! /bin/bash

file_name="test_big.file"
echo "$file_name">> filelist
dd if=/dev/zero of=$file_name count=80960 bs=1024

# end of file
