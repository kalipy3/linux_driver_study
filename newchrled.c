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
#include <linux/cdev.h>
#include <linux/device.h>

//寄存器物理地址
#define CCM_CCGR1_BASE          (0X020C406C)
#define SW_MUX_GPIO1_IO03_BASE  (0X020E0068)
#define SW_PAD_GPIO1_IO03_BASE  (0X020E02F4)
#define GPIO1_DR_BASE           (0X0209C000)
#define GPIO1_GDIR_BASE         (0X0209C004)

//地址映射后的虚拟地址指针
static void __iomem *IMX6U_CCM_CCGR1;
static void __iomem *SW_MUX_GPIO1_IO03;
static void __iomem *SW_PAD_GPIO1_IO03;
static void __iomem *GPIO1_DR;
static void __iomem *GPIO1_GDIR;

#define LEDOFF  0   //关
#define LEDON   1   //开

#define NEWCHRLED_NAME "newchrled"
#define  NEWCHRLED_COUNT    1

//LED设备结构体
struct newchrled_dev
{
    struct cdev cdev;//字符设备
    dev_t devid;    //设备号
    struct class *class;//类
    struct device *device;//设备
    int major;      //主设备号
    int minor;      //次设备号
};

struct newchrled_dev newchrled;//led设备

static void led_switch(u8 sta)
{
    u32 val = 0;

    if (sta == LEDON) {
        val = readl(GPIO1_DR);
        val &= ~(1 << 3);//bit3清零,打开LED灯
        writel(val, GPIO1_DR);
        printk("led on\r\n");
    } else if (sta == LEDOFF) {
        val = readl(GPIO1_DR);
        val |= (1 << 3);//关闭LED灯
        writel(val, GPIO1_DR);
        printk("led off\r\n");
    }
}

static int newchrled_open(struct inode *inode, struct file *filp)
{
    printk("chrdevbase_open\r\n");
    return 0;
}

static int newchrled_release(struct inode *inode, struct file *filp)
{
    printk("chrdevbase_release\r\n");
    return 0;
}

static ssize_t newchrled_write(struct file *filp, __user char *buf, 
        size_t count, loff_t *ppos)
{
    printk("chrdevbase_read\r\n");
    int retvalue;
    unsigned char databuf[1];

    retvalue = copy_from_user(databuf, buf, count);
    if (retvalue < 0) {
        printk("kernel write failed!\r\n");
        return -EFAULT;
    }
    led_switch(databuf[0]);

    return 0;
}


//字符设备 操作集合
static struct file_operations newchrled_fops = {
    .owner = THIS_MODULE,
    .open = newchrled_open,
    .release = newchrled_release,
    .write = newchrled_write,
};

//入口
static int /*__init*/ newchrled_init(void)
{
    printk("led_init\r\n");
    int ret = 0;
    int val = 0;
    //1,初始化LED灯，地址映射
    IMX6U_CCM_CCGR1 = ioremap(CCM_CCGR1_BASE, 4);
    SW_MUX_GPIO1_IO03 = ioremap(SW_MUX_GPIO1_IO03_BASE, 4);
    SW_PAD_GPIO1_IO03 = ioremap(SW_PAD_GPIO1_IO03_BASE, 4);
    GPIO1_DR = ioremap(GPIO1_DR_BASE, 4);
    GPIO1_GDIR = ioremap(GPIO1_GDIR_BASE, 4);

    //2,初始化
    val = readl(IMX6U_CCM_CCGR1);
    val &= -(3 << 26);//清楚以前的配置bit26,27
    val |= 3 << 26;//bit26,27置1
    writel(val, IMX6U_CCM_CCGR1);

    writel(0x5, SW_MUX_GPIO1_IO03);//设置复用
    writel(0x10B0, SW_PAD_GPIO1_IO03);//设置电气属性

    val = readl(GPIO1_GDIR);
    val |= 1 << 3;//bit3置1,设置为输出
    writel(val, GPIO1_GDIR);

    val = readl(GPIO1_DR);
    val &= ~(1 << 3);//bit3清零,打开LED灯
    writel(val, GPIO1_DR);

    //2,注册设备号
    if (newchrled.major) {//给定主设备号
        newchrled.devid = MKDEV(newchrled.major, 0);
        ret = register_chrdev_region(newchrled.devid, 1, NEWCHRLED_NAME);
    } else {//没有给定主设备号
        ret = alloc_chrdev_region(&newchrled.devid, 0, 1, NEWCHRLED_NAME);
        newchrled.major = MAJOR(newchrled.devid);
        newchrled.minor = MINOR(newchrled.devid);
    }

    if (ret < 0) {
        printk("newchrled chardev_region failed!\r\n");
        return -1;
    }
    printk("newchrled major=%d, minor=%d\r\n", newchrled.major, newchrled.minor);

    //3,注册字符设备
    newchrled.cdev.owner = THIS_MODULE;
    cdev_init(&newchrled.cdev, &newchrled_fops);
    ret = cdev_add(&newchrled.cdev, newchrled.devid, NEWCHRLED_COUNT);

    //4,自动创建设备节点
    newchrled.class = class_create(THIS_MODULE, NEWCHRLED_NAME); 
    if (IS_ERR(newchrled.class)) {
        return PTR_ERR(newchrled.class);
    }
    
    newchrled.device = device_create(newchrled.class, NULL, newchrled.devid,
            NULL, NEWCHRLED_NAME);
    if (IS_ERR(newchrled.device)) {
        return PTR_RET(newchrled.device);
    }
    printk("/dev/newchrled device_node create ok\r\n");
    
    return 0;
}

//出口
static void /*__exit*/ newchrled_exit(void)
{
    printk("newchrled_exit\r\n");
    int val = 0;
    val = readl(GPIO1_DR);
    val |= (1 << 3);//关闭LED灯
    writel(val, GPIO1_DR);

    //取消地址映射
    iounmap(IMX6U_CCM_CCGR1);
    iounmap(SW_MUX_GPIO1_IO03);
    iounmap(SW_PAD_GPIO1_IO03);
    iounmap(GPIO1_DR);
    iounmap(GPIO1_GDIR);

    //1,删除字符设备
    cdev_del(&newchrled.cdev);

    //2,注销字符设备号
    unregister_chrdev_region(newchrled.devid, NEWCHRLED_COUNT);

    //3,销毁设备
    device_destroy(newchrled.class, newchrled.devid);

    //4,销毁类
    class_destroy(newchrled.class);
}


module_init(newchrled_init);
module_exit(newchrled_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kalipy");
