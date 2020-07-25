#! /bin/bash

FILE=filelist
if [ -e $FILE ]
then
    rm -f $FILE
fi

for((i=1;i<=20;i++));  
do
file_name="test.file${i}"
echo "$file_name">> $FILE
dd if=/dev/urandom of=$file_name count=8096 bs=1024
done  

# end of file