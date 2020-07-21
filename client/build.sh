#! /bin/bash

flush_option=1
build_type="Debug"

if [[ "$flush_option" -eq 1 ]] ; then
  rm -rf bld
  mkdir bld
  cd bld
  cmake .. \
    -DCMAKE_BUILD_TYPE="$build_type"                         
else
  echo "No Flush"
  cd bld
fi

make -j4

# end of file