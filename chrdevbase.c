/*
 * chrdevbase.c
 * Copyright (C) 2020 kalipy <kalipy@debian>
 *
 * Distributed under terms of the MIT license.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init chrdevbase_init(void)
{
    printk("chrdevbase_init\r\n");
    return 0;
}

static void __exit chrdevbase_exit(void)
{
    printk("chrdevbase_exit\r\n");
}


module_init(chrdevbase_init);
module_exit(chrdevbase_exit);
