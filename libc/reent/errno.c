#include <wchar.h>
#include <errno.h>
#include <reent.h>

int * __errno(){
  return &_REENT->_errno;
}
