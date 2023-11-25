#include <linux/kernel.h>
#include <linux/sched.h>
#include <asm/segment.h>
#include <linux/fs.h>
#include <stdarg.h>
#include <unistd.h>

#define set_bit(bitnr,addr) ({ \
register int __res ; \
__asm__("bt %2,%3;setb %%al":"=a" (__res):"a" (0),"r" (bitnr),"m" (*(addr))); \
__res; })

char proc_buf[4096] ={'\0'};

extern int vsprintf(char * buf, const char * fmt, va_list args);

//Linux0.11没有sprintf()，该函数是用于输出结果到字符串中的，所以就实现一个，这里是通过vsprintf()实现的。
int sprintf(char *buf, const char *fmt, ...)
{
	va_list args; int i;
	va_start(args, fmt);
	i=vsprintf(buf, fmt, args);
	va_end(args);
	return i;
}



int get_psinfo()
{
	int read = 0;
    read += sprintf(proc_buf+read,"%s","pid\tstate\tfather\tcounter\tstart_time\n");
    struct task_struct **p;
	for(p = &FIRST_TASK ; p <= &LAST_TASK ; ++p)
 	if (*p != NULL)
 	{
 		read += sprintf(proc_buf+read,"%d\t",(*p)->pid);
 		read += sprintf(proc_buf+read,"%d\t",(*p)->state);
 		read += sprintf(proc_buf+read,"%d\t",(*p)->father);
 		read += sprintf(proc_buf+read,"%d\t",(*p)->counter);
 		read += sprintf(proc_buf+read,"%d\n",(*p)->start_time);
 	}
 	return read;

}

/*
*  参考fs/super.c mount_root()函数
*/
int get_hdinfo()
{
	int read = 0;
	int i,used;
	struct super_block * sb;
	sb=get_super(current->root->i_dev);  /*磁盘设备号 3*256+1*/
	/*Blocks信息*/
	read += sprintf(proc_buf+read,"Total blocks:%d\n",sb->s_nzones);
	used = 0;
	i=sb->s_nzones;
	while(--i >= 0)
	{
		if(set_bit(i&8191,sb->s_zmap[i>>13]->b_data))
			used++;
	}
	read += sprintf(proc_buf+read,"Used blocks:%d\n",used);
	read += sprintf(proc_buf+read,"Free blocks:%d\n",sb->s_nzones-used);
	/*Inodes 信息*/
	read += sprintf(proc_buf+read,"Total inodes:%d\n",sb->s_ninodes);
	used = 0;
	i=sb->s_ninodes+1;
	while(--i >= 0)
	{
		if(set_bit(i&8191,sb->s_imap[i>>13]->b_data))
			used++;
	}
	read += sprintf(proc_buf+read,"Used inodes:%d\n",used);
	read += sprintf(proc_buf+read,"Free inodes:%d\n",sb->s_ninodes-used);
 	return read;

    
}

int get_inodeinfo()
{
	return -1;
}

int proc_read(int dev, unsigned long * pos, char * buf, int count)
{
	int i;
	if(dev == 0)
		get_psinfo();
	if(dev == 1)
		get_hdinfo();
	if(dev == 2)
		get_inodeinfo();
	
 	for(i=0;i<count;i++)
 	{
 		if(proc_buf[i+ *pos ] == '\0')  
          break; 
 		put_fs_byte(proc_buf[i],buf + i+ *pos);
 	}
 	*pos += i;
 	return i;

 	return -1;
}
