#include <unistd.h> /* NULL */
#include <string.h> /* strcmp strcpy */
#include <linux/sem.h> /* sem_t */
#include <asm/segment.h> /* get_fs_byte */
#include <asm/system.h> /* cli, sti */
#include <linux/kernel.h> /* printk */

//信号量最大数量
#define SEM_LIST_LENGTH 5

//信号量数组(都初始化为没有的状态)
sem_t sem_list[SEM_LIST_LENGTH] = {
	{"\0",0,NULL}, {"\0",0,NULL},{"\0",0,NULL},{"\0",0,NULL},{"\0",0,NULL}
};




sem_t *sys_sem_open(const char *name,unsigned int value)
{
    if (name == NULL)
    {
        printk("name == NULL\n");
        return NULL;
    }
    char nbuf[20];
    int i = 0;
    for(; i< 20; i++)
    {
        nbuf[i] = get_fs_byte(name+i);
    }
    nbuf[i]='\0';
    sem_t *result;
    i = 0;
    int k=0;
    int tmp=-1;
    for(i = 0; i < SEM_LIST_LENGTH; i++)
    {
        if(sem_list[i].name[0] == '\0'){
            tmp=i;
            break;   
        }   
        if(!strcmp(sem_list[i].name, nbuf))
        {
            result = &sem_list[i];
            printk("sem %s is found\n",result->name);
            return result;
        }
            

    }
    if(tmp==-1)
        printk("sem_list is full");
    for(k=0;nbuf[k]!='\0';k++)
        sem_list[tmp].name[k]=nbuf[k];
    sem_list[tmp].name[k]='\0';
    sem_list[tmp].value = value;
    sem_list[tmp].queue = NULL;
    result = &sem_list[tmp];
    printk("sem %s is created , value = %d\n",sem_list[tmp].name,sem_list[tmp].value);
    printk("sem %s is created , value = %d\n",result->name,result->value);
    return result;
}



int sys_sem_wait(sem_t *sem){

    //judge
    if(sem==NULL || sem<sem_list || sem>(sem_list+SEM_LIST_LENGTH)){
        printk("P(sem) error!\n");
        return -1;
    }

    //while(condition){sleep_on}
    //sem->value--
    cli();
    while(sem->value == 0){
        sleep_on(&(sem->queue));
    }
    sem->value--;
    sti();
    return 0;

}




int sys_sem_post(sem_t *sem){
    //judge
    if(sem==NULL || sem<sem_list || sem>(sem_list+SEM_LIST_LENGTH)){
        printk("V(sem) error!\n");
        return -1;
    }
    cli();
    sem->value++;
    if(sem->value<=1){
        wake_up(&(sem->queue));
    }
    sti();
    return 0;
}



int sys_sem_unlink(const char *name){
    if(name==NULL){
        printk("name is NULL!\n");
        return -1;
    }
    char nbuf[20];
    sem_t *result = NULL;
    int i = 0;
    for(i = 0;i<20;i++){
        nbuf[i] = get_fs_byte(name+i);
        if (nbuf[i] == '\0')
            break;
    }
    int j = 0;
    for(j = 0;j<SEM_LIST_LENGTH;j++){
        if(!(strcmp(nbuf,sem_list[j].name))){
            printk("Delete the sem,find the name %s",sem_list[j].name);
            strcpy(sem_list[j].name,"\0");
            sem_list[j].value = 0;
            sem_list[j].queue = NULL;
            break;
        }
    }
    if(j==SEM_LIST_LENGTH){
        return -1;
    }
    return  0;

}


