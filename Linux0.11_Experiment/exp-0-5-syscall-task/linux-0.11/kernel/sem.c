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




sem_t *sys_sem_open(const char *name, unsigned int value){

    //buffer get the input name 

    if(name==NULL){
        printk("name is NULL!\n");
        return NULL;
    }

    char nbuf[20];
    sem_t *result = NULL;
    int i = 0;
    for(i = 0;i<20;i++){
        nbuf[i] = get_fs_byte(name+i);
    }
    //for each sem in sem_list:compare sem's name with input name 
    int j=0;
    for(j = 0;j<SEM_LIST_LENGTH;j++){

        if(sem_list[j].name[0]=='\0'){
            break;
        }

        //if find return the sem   
        if(!strcmp(nbuf,sem_list[j].name)){
            result = &sem_list[j];
            printk("Find the sem : %s",result->name);
            return result;
        }
        
    } 
    //if not: using i make a sem(i means sem_list[i].name = '\0')
    if(j==SEM_LIST_LENGTH){
        printk("Sem list is full,make the sem from the start!\n");
        j == 0;
    }
    strcpy(sem_list[j].name,nbuf);
    sem_list[j].value = value;
    sem_list[j].queue = NULL;
    result = &sem_list[j];
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
    if(sem->value<=0){
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


