#define __LIBRARY__
#include <unistd.h>
#include <linux/sem.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/sched.h>



_syscall2(sem_t *,sem_open,const char *,name,unsigned int,value);
_syscall1(int,sem_wait,sem_t *,sem);
_syscall1(int,sem_post,sem_t *,sem);
_syscall1(int,sem_unlink,const char *,name);



int consumerNum = 5;    /* 消费者个数 */
const int maxNum = 500; /* 写入的数据量 */
const int bufSize = 10; /* 缓冲区大小 */
int data = -404;        /* 存放读取的数据*/
pid_t p_pid[5];         /* 进程号数组*/

sem_t *empty, *full, *mutex; /*三个信号量 */

void producer(){
    

    int i;
    int end_count = 0;
    int endpos_produce = 0; /* 记录消费者进程的消费次数*/
    int fo,fx;
    char buf[4];
    printf("Ready to open!\n");
    fo = open("report.txt", O_RDWR|O_CREAT|O_TRUNC, 0666);
    fx = open("out.txt", O_RDWR | O_CREAT | O_APPEND);
    /*printf("the number of fo is:%d\n",fo);*/
	if (fo == -1||fx==-1)
	{
		perror("打开文件失败！\n");
		return;
	}
    lseek(fo, bufSize * sizeof(int), SEEK_SET);
    sprintf(buf, "%d", end_count);
    write(fo, buf, sizeof(int));


    i = 0;

    for(i = 1; i <= maxNum+10; i++){
        printf("print i: %d",i);
        sem_wait(empty);
        sem_wait(mutex);
        lseek(fo, endpos_produce * sizeof(int), SEEK_SET);
        sprintf(buf, "%d", i);
        write(fo, buf, sizeof(int));


        /*将生产信息写到out文件*/
        write(fx, "p", 1);
        write(fx, ":", 1);
        write(fx, buf, sizeof(int));
        write(fx, "\n", 1);
        fflush(stdout);

        endpos_produce = (endpos_produce + 1) % bufSize;
        sem_post(mutex); /* 出了临界区，需要mutex++，以便下一次可以进入 */
        sem_post(full);  /* 看是不是需要唤醒阻塞 */

    }

    close(fo);

}


void consumer(){

    char buf[4];
    char data[4];
    char pid[4];
    int endpos_consumer = 0;
    int n = 0;
    int end_data = 0;
    int id;

    int fi = open("report.txt", O_RDONLY);
    int fx = open("outc.txt", O_RDWR | O_CREAT | O_APPEND);

    if (fi == -1 || fx == -1) {
        perror("创建文件缓冲区失败！\n");
        return;
    }

    
    while(1){
        
        sem_wait(full);
        sem_wait(mutex);
        lseek(fi, bufSize * sizeof(int), SEEK_SET);
        n = read(fi, buf, sizeof(int));
        if(n==0){
            endpos_consumer = 0;
        }
        sscanf(buf, "%d", &endpos_consumer);

        lseek(fi, endpos_consumer * sizeof(int), SEEK_SET);
        read(fi,data,sizeof(int));
        sprintf(data, "%d", end_data);

        if(end_data>=500){
            sem_post(mutex);
            sem_post(empty);
            break;
        }



        write(fx, "c", 1);
        write(fx, "-", 1);
        id = getpid();
        sprintf(pid, "%d", id);
        write(fx, pid, sizeof(int));
        write(fx, ":", 1);
        write(fx, data, sizeof(int));
        write(fx, "\n", 1);

        endpos_consumer = (endpos_consumer + 1) % bufSize;
        sprintf(buf, "%d", endpos_consumer);
        lseek(fi, bufSize * sizeof(int), SEEK_SET);
        write(fi,buf,sizeof(int));


        fflush(stdout);
        sem_post(mutex);
        sem_post(empty);

    }

    close(fi);
    close(fx);
}



int main(){
    
    empty = sem_open("empty", bufSize);
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

    if (!fork())
	{
		printf("producer is running！\n");
		producer();
		exit(0); /* 生产者任务完成后，杀死该子进程 */
	}
    while (consumerNum--)
    {
        p_pid[consumerNum] = fork();
        if (!p_pid[consumerNum])
        {
            printf("consumer %d is running!\n", getpid());
            consumer();
            exit(0);
        }
    }
    wait(NULL);
    /* 关闭信号量 */
    sem_unlink("empty");
    sem_unlink("full");
    sem_unlink("mutex");
    return 0;


}