#ifndef _SHM_H
#define _SHM_H
#define SHM_SIZE 64

#include <linux/sched.h>


typedef struct shm_ds{
	unsigned int key;
	unsigned int size;
	unsigned long page;

}shm_ds;

int sys_shmget(unsigned int key,size_t size);
void* sys_shmat(int shmid);

#endif