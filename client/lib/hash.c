#include "../include/small_file.h"
#include "../etc/config.h"

/*
  SDBMHash function to calculate send to which disk(server)
  @param str  input filepath
  @return num 
*/
unsigned int my_hash(char *str)
{
  unsigned int hash = 0;

  while (*str)
  {
    // equivalent to: hash = 65599*hash + (*str++);
    hash = (*str++) + (hash << 6) + (hash << 16) - hash;
  }

  return (hash & SERVER_DISK_COUNT);
}