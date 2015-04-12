#include <nuttx/compiler.h>
#include <nuttx/binfmt/symtab.h>

extern void *__aeabi_atexit;
extern void *__dso_handle;
extern void *exit;
extern void *fflush;
extern void *fopen;
extern void *fprintf;
extern void *get_errno_ptr;
extern void *getpid;
extern void *kill;
extern void *memset;
extern void *printf;
extern void *pthread_attr_init;
extern void *pthread_create;
extern void *pthread_exit;
extern void *pthread_join;
extern void *pthread_mutex_init;
extern void *pthread_mutex_lock;
extern void *pthread_mutex_unlock;
extern void *puts;
extern void *sched_getstreams;
extern void *sem_destroy;
extern void *sem_init;
extern void *sem_post;
extern void *sem_wait;
extern void *sigaction;
extern void *sigemptyset;
extern void *sigqueue;
extern void *sleep;
extern void *strcmp;
extern void *task_create;
extern void *usleep;
extern void *_ZdlPv;
extern void *_Znwj;
extern void *_ZTVN10__cxxabiv117__class_type_infoE;

const struct symtab_s exports[] = 
{
  {"__aeabi_atexit", &__aeabi_atexit},
  {"__dso_handle", &__dso_handle},
  {"exit", &exit},
  {"fflush", &fflush},
  {"fopen", &fopen},
  {"fprintf", &fprintf},
  {"get_errno_ptr", &get_errno_ptr},
  {"getpid", &getpid},
  {"kill", &kill},
  {"memset", &memset},
  {"printf", &printf},
  {"pthread_attr_init", &pthread_attr_init},
  {"pthread_create", &pthread_create},
  {"pthread_exit", &pthread_exit},
  {"pthread_join", &pthread_join},
  {"pthread_mutex_init", &pthread_mutex_init},
  {"pthread_mutex_lock", &pthread_mutex_lock},
  {"pthread_mutex_unlock", &pthread_mutex_unlock},
  {"puts", &puts},
  {"sched_getstreams", &sched_getstreams},
  {"sem_destroy", &sem_destroy},
  {"sem_init", &sem_init},
  {"sem_post", &sem_post},
  {"sem_wait", &sem_wait},
  {"sigaction", &sigaction},
  {"sigemptyset", &sigemptyset},
  {"sigqueue", &sigqueue},
  {"sleep", &sleep},
  {"strcmp", &strcmp},
  {"task_create", &task_create},
  {"usleep", &usleep},
  {"_ZdlPv", &_ZdlPv},
  {"_Znwj", &_Znwj},
  {"_ZTVN10__cxxabiv117__class_type_infoE", &_ZTVN10__cxxabiv117__class_type_infoE},
};

const int nexports = sizeof(exports) / sizeof(struct symtab_s);
