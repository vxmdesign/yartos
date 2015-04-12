#include <stdio.h>
#define SYM_EXPORT_STRUCT __attribute__ ((aligned(4),section("symexports")))

typedef struct { 
  const char *cmd;
  void *ref;
}sym_export;

#define SYM_EXPORT_ENTRY(name, func) sym_export __sym_export_##name SYM_EXPORT_STRUCT = {#name, func}

SYM_EXPORT_ENTRY(test1,NULL);
SYM_EXPORT_ENTRY(test2,NULL);
SYM_EXPORT_ENTRY(test3,NULL);



