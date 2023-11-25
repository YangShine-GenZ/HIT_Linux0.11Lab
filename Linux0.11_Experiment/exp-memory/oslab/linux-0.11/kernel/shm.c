#define __LIBRARY__
#include <unistd.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <errno.h>
#include <linux/shm.h>


static shm_ds shm_list[SHM_SIZE] = {{0, 0, 0}};/* unsigned int key,size,page */

int sys_shmget(unsigned int key, size_t size)
{
    int i;
    if(size>PAGE_SIZE){
        printk("shmget: size %u cannot be greater than the page size %ud. \n", size, PAGE_SIZE);
        return -ENOMEM;
    }

    if (key == 0)
    {
        printk("shmget: key cannot be 0.\n");
        return -EINVAL;
    }

    /* find the matched key  */
    for(i = 0;i<SHM_SIZE;i++){
        if(shm_list[i].key == key){
            return i;
        }
    }

    unsigned long page = get_free_page();
    if(!page){
        return -ENOMEM;
    }
    printk("shmget get memory's address is 0x%08x\n", page);
    for(i = 0;i<SHM_SIZE;i++){
        if(shm_list[i].key == 0){
            shm_list[i].key = key;
            shm_list[i].size = size;
            shm_list[i].page = page;
            return i;
        }
    }

    return -1;
}

void *sys_shmat(int shmid)
{

    unsigned long data_base,brk;
    if (shmid < 0 || SHM_SIZE <= shmid || shm_list[shmid].page == 0 || shm_list[shmid].key <= 0)
    {
        printk("In shmat: wrong shmid or wrong shmid!");
        return (void *)-EINVAL;
    }
    data_base = get_base(current->ldt[2]);
    printk("current's data_base = 0x%08x,new page = 0x%08x\n", data_base, shm_list[shmid].page);
    brk = current->brk+data_base;
    current->brk += PAGE_SIZE;
    if (put_page(shm_list[shmid].page, brk) == 0)
    {
        return (void *)-ENOMEM;
    }

    return (void*)(current->brk-PAGE_SIZE);
}
