#include<linux/module.h>
#include<linux/init.h>
#include<linux/miscdevice.h>
#include<linux/fs.h>
#include<linux/wait.h>
#include<asm/gpio.h>
#include<asm/io.h>
#include<linux/ioport.h>
#include<asm/uaccess.h>
#include<linux/sched.h>

#define DEVICENAME	"s3c6410_led"

#define GPMCON		0x7F008820
#define GPMDAT		0x7F008824
#define GPMPUD		0x7F008828

static volatile long int *gpmcon;
static volatile long int *gpmdat;
static volatile long int *gpmpud;


int s3c6410_open (struct inode *inode, struct file *file)
{
	iowrite32((ioread32(gpmcon) & 0xFFFF0000),gpmcon);
	iowrite32((ioread32(gpmcon) | 0x1111),gpmcon);
	return 0;
}

int s3c6410_ioctl (struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	if(cmd>1)
	{
		return -1;
	}
	if(arg>3)
	{
		return -1;
	}
	switch(cmd)
	{
		case 0:
			{
				iowrite16((ioread16(gpmdat)|(1<<arg)),gpmdat);		
			}
			break;
		case 1:
			{
				iowrite16((ioread16(gpmdat)&(~(1<<arg))),gpmdat);
			}
			break;
		default:
			break;
	}

	return 0;
}

int s3c6410_close(struct inode *inode, struct file *file)
{
	return 0;
}

struct file_operations s3c6410_fops = 
{
	.owner = THIS_MODULE,
	.open = &s3c6410_open,
	.ioctl = &s3c6410_ioctl,
	.release = &s3c6410_close,
};

struct miscdevice misc =
{
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICENAME,
	.fops = &s3c6410_fops,
};

static int __init led_init(void)
{
	int ret=0;
	if(!request_mem_region(GPMCON,12,"s3c6410_led"))
	{
		ret = -EBUSY;
		printk("s3c6410_led request_mem_failed\n");
		goto request_mem_failed;
	}
	gpmcon = ioremap(GPMCON,4);
	if(gpmcon==NULL)
	{
		ret = -EBUSY;
		printk("s3c6410_led gpmcon failed\n");
		goto gpmcon_failed;
	}
	gpmdat = ioremap(GPMDAT,4);
	if(gpmdat==NULL)
	{
		ret = -EBUSY;
		printk("s3c6410_led gpmdat failed\n");
		goto gpmdat_failed;
	}
	gpmpud = ioremap(GPMPUD,4);
	if(gpmpud==NULL)
	{
		ret = -EBUSY;
		printk("s3c6410_led gpmpub failed\n");
		goto gpmpud_failed;
	}
	ret = misc_register(&misc);
	if(ret<0)
	{
		return -EBUSY;
		goto misc_regster_failed;
	}
	printk("s3c6410 led initialize\n");
	return ret;
misc_regster_failed:
	misc_deregister(&misc);
gpmpud_failed:
	iounmap(gpmpud);
gpmdat_failed:
	iounmap(gpmdat);
gpmcon_failed:
	iounmap(gpmcon);
request_mem_failed:
	release_mem_region(GPMCON,12);
	return ret;
}

static void __exit led_exit(void)
{
	iounmap(gpmpud);
	iounmap(gpmdat);
	iounmap(gpmcon);
	release_mem_region(GPMCON,12);
	misc_deregister(&misc);
	printk("s3c6410 led exit\n");
}

module_init(led_init);
module_exit(led_exit);

MODULE_AUTHOR("Xu Xianyin");
MODULE_LICENSE("GPL");
