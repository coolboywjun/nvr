

#ifndef _OSA_MUTEX_H_
#define _OSA_MUTEX_H_

#include <pthread.h>

typedef struct {

  pthread_mutex_t lock;
  
} OSA_MutexHndl;


#include <osa.h>


int OSA_mutexCreate(OSA_MutexHndl *hndl);
int OSA_mutexDelete(OSA_MutexHndl *hndl);
int OSA_mutexLock(OSA_MutexHndl *hndl);
int OSA_mutexUnlock(OSA_MutexHndl *hndl);

#endif /* _OSA_MUTEX_H_ */



