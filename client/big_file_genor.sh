#! /bin/bash

file_name="test_big.file"
echo "$file_name\n">> filelist
dd if=/dev/zero of=$file_name count=8096000 bs=1024

# end of file
