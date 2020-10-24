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
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>

//入口
static int /*__init*/ dtsof_init(void)
{
    int ret = 0;
    struct device_node *bl_nd = NULL;//节点
    struct property *comppro = NULL;

    u32 def_value = 0;
    u32 elemsize = 0;
    u32 *brival;
    u8 i = 0;
    
    //1,找到smb节点，路径为：/smb
    bl_nd = of_find_node_by_path("/smb");
    if (bl_nd == NULL) {
        ret = -EINVAL;
        goto fail_findnd;
    }
    //2,获取属性
    comppro = of_find_property(bl_nd, "compatible", NULL);
    if (comppro == NULL) {
        ret = -EINVAL;
        goto fail_findpro;
    } else {
        printk("compatible=%s\r\n", (char *)comppro->value);
    }

    //3,获取数字属性值
    ret = of_property_read_u32(bl_nd, "#interrupt-cells", &def_value);
    if (ret < 0) {
        goto fail_read32;
    } else {
        printk("#interrupt-cells=%d\r\n", def_value);
    }

    //4,获取数组类型的属性
    elemsize = of_property_count_elems_of_size(bl_nd, "interrupt-map-mask", sizeof(u32));
    if (elemsize < 0) {
        ret = -EINVAL;
        goto  fail_readelem;
    } else {
        printk("interrupt-map-mask elems size=%d\r\n", elemsize);
    }

    //申请内存
    brival = kmalloc(elemsize * sizeof(u32), GFP_KERNEL);
    if (!brival) {
        ret = -EINVAL;
        goto fail_mem;
    }

    //获取数组
    ret = of_property_read_u32_array(bl_nd, "interrupt-map-mask", brival, elemsize);
    if (ret < 0) {
        goto fail_read32array;
    } else {
        for (i = 0; i < elemsize; i++)
        {
            printk("interrupt-map-mask[%d]=%d\r\n", i, *(brival + i));
        }
    }
    kfree(brival);

    return 0;

fail_read32array:
    kfree(brival);
fail_mem:
fail_readelem:
fail_read32:
fail_findpro:
fail_findnd:
    return ret;
}

//出口
static void /*__exit*/ dtsof_exit(void)
{
    int val = 0;
}


module_init(dtsof_init);
module_exit(dtsof_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kalipy");
