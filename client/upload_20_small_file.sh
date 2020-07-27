#! /bin/bash

cat filelist | while read file_path;do time ./upload  $file_path & done

# end of file