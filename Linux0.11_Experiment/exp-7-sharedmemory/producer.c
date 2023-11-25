/*producer.c*/
#define   __LIBRARY__
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/sem.h>

_syscall2(sem_t*,sem_open,const char *,name,unsigned int,value);
_syscall1(int,sem_wait,sem_t*,sem);
_syscall1(int,sem_post,sem_t*,sem);
_syscall1(int,sem_unlink,const char *,name);
_syscall1(void*,shmat,int,shmid);
_syscall2(int,shmget,int,key,int,size);



#define NUMBER 520 /*打出数字总数*/
#define BUFSIZE 10 /*缓冲区大小*/

sem_t   *empty, *full, *mutex;

int main(){
    int buf_in = 0;
    int i,shmid;
    int *p;

    empty = sem_open("empty", 10);
    if (empty == NULL)
    {
        perror("empty create falied！\n");
        return -1;
    }
    full = sem_open("full", 0);
    if (full == NULL)
    {
        perror("full create failed！\n");
        return -1;
    }
    mutex = sem_open("mutex", 1);
    if (mutex == NULL)
    {
        perror("mutex create failed！\n");
        return -1;
    }
    if (empty && full && mutex)
    {
        printf("create semphore successed!\n");
    }

    
    shmid = shmget(1,BUFSIZE);
    printf("shmid:%d\n",shmid);
    p = (int*)shmat(shmid);

    if(shmid == -1)
    {
        return -1;
    }

    printf("producer start:\n");

    for(i = 0;i<NUMBER;i++){
        printf("p-debug_i:%d\n",i);
        sem_wait(empty);
        sem_wait(mutex);
        p[buf_in] = i;
        buf_in = (buf_in+1)%BUFSIZE;
        sem_post(mutex);
        sem_post(full);
        
    }
    printf("producer end.\n");
	fflush(stdout);
    /*释放信号量*/
    sem_unlink("full");
    sem_unlink("empty");
    sem_unlink("mutex");
    return 0;

}