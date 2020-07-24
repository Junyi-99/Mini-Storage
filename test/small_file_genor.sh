#! /bin/bash

for((i=1;i<=20;i++));  
do
file_name="test.file${i}"
echo "$file_name\n">> filelist
dd if=/dev/zero of=$file_name count=8096 bs=1024
done  

# end of file