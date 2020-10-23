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
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/io.h>

#define LED_MAJOR   200 //主设备号
#define LED_NAME    "led" //名字

static int led_open(struct inode *inode, struct file *filp)
{
    printk("chrdevbase_open\r\n");
    return 0;
}

static int led_release(struct inode *inode, struct file *filp)
{
    printk("chrdevbase_release\r\n");
    return 0;
}

static ssize_t led_write(struct file *filp, __user char *buf, 
        size_t count, loff_t *ppos)
{
    printk("chrdevbase_read\r\n");
    return 0;
}


//字符设备 操作集合
static struct file_operations led_fops = {
    .owner = THIS_MODULE,
    .open = led_open,
    .release = led_release,
    .write = led_write,
};

//入口
static int __init led_init(void)
{
    int ret = 0;

    //注册字符设备
    ret = register_chrdev(LED_MAJOR, LED_NAME, &led_fops);
    if (ret < 0) {
        printk("register chardev failed!\r\n");
        return -EIO;
    }

    printk("led_init\r\n");
    return 0;
}

//出口
static void __exit led_exit(void)
{
    //注销字符设备
    unregister_chrdev(LED_MAJOR, LED_NAME);
    printk("led_exit\r\n");
}


module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kalipy");
