/*
 * chrdevbase.c
 * Copyright (C) 2020 kalipy <kalipy@debian>
 *
 * Distributed under terms of the MIT license.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>

#define CHRDEVBASE_MAJOR    200 //主设备号
#define CHRDEVBASE_NAME     "chrdevbase" //名字

static int chrdevbase_open(struct inode *inode, struct file *filp)
{
    printk("chrdevbase_open\r\n");
    return 0;
}

static int chrdevbase_release(struct inode *inode, struct file *filp)
{
    printk("chrdevbase_release\r\n");
    return 0;
}

static ssize_t chrdevbase_read(struct file *filp, __user char *buf, 
        size_t count, loff_t *ppos)
{
    printk("chrdevbase_read\r\n");
    return 0;
}

static ssize_t chrdevbase_write(struct file *filp, __user char *buf, 
        size_t count, loff_t *ppos)
{
    printk("chrdevbase_write\r\n");
    return 0;
}

//字符设备 操作集合
static struct file_operations chrdevbase_fops={
   .owner = THIS_MODULE,
   .open = chrdevbase_open,
   .release = chrdevbase_release,
   .read =  chrdevbase_read,
   .write =  chrdevbase_write,
};

static int __init chrdevbase_init(void)
{
    int ret = 0;
    printk("chrdevbase_init\r\n");

    //注册字符设备
    ret = register_chrdev(CHRDEVBASE_MAJOR, CHRDEVBASE_NAME, &chrdevbase_fops);
    if (ret < 0)
    {
        printk("chrdevbase init failed!\r\n");
    }

    return 0;
}

static void __exit chrdevbase_exit(void)
{
    //注销字符设备
    unregister_chrdev(CHRDEVBASE_MAJOR, CHRDEVBASE_NAME);
    printk("chrdevbase_exit\r\n");
}


module_init(chrdevbase_init);
module_exit(chrdevbase_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kalipy");
