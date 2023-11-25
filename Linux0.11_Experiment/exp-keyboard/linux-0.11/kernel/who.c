#include <asm/segment.h>
#include <linux/kernel.h>
#include <string.h>
#include <errno.h>


char who_name[24];


int sys_whoami(char *name, unsigned int size){
    //put_fs_byte(val,addr)
    //custom_address = fs:addr
    int i = 0;
    int len = strlen(who_name);
    printk("The word of the iam is: %s\n",who_name);
    printk("The len of the word is: %d\n",len);
    if(len>size){
        errno = EINVAL;
        return -1;
    }
    else{
        for(i = 0;i<len;i++){
            put_fs_byte(who_name[i],name+i);
        }
        return len;
    }


    //strcpy(name,str);
}


int sys_iam(const char * name){
    char c;
    int i = 0;
    printk("Welcome iam function!\n");
    char str[25];
    while((c=get_fs_byte(name+i))&&(i<=25)){
        str[i] = c;
        i++;
    }
    str[i] = '\0';
    if(i>24){
        errno = EINVAL;
        printk("Unsuccessfully reserve your name!\n");
        i = -1;
    }
    else{
        strcpy(who_name,str);
        printk("Successfully reserve your name!\n");
    }
    printk("The len of name is: %d\n",i);
    printk("The word of the iam is: %s\n",who_name);
    return i;
    
}



/*
int sys_iam(const char *name)
{
    char str[25];
    int i = 0;

    do
    {
        // get char from user input
        str[i] = get_fs_byte(name + i);
    } while (i <= 25 && str[i++] != '\0');


    if (i > 24)
    {
        errno = EINVAL;
        i = -1;
    }
    else
    {
        // copy from user mode to kernel mode
        strcpy(who_name, str);
    }


    printk("The word of the iam is: %s\n",who_name);
    return i;


}
*/

