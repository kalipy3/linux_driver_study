/*
 * chrdevbaseAPP.c
 * Copyright (C) 2020 kalipy <kalipy@debian>
 *
 * Distributed under terms of the MIT license.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
/*
 * argc:应用程序参数个数
 * argv[]:具体的参数内容，字符串形式
 * ./ledAPP <filename> <0/1> 0表示关灯 1表示开灯
 * ./ledAPP /dev/led 0 关灯
 * ./ledAPP /dev/led 1 开灯
 */

#define LEDOFF  0
#define LEDON   1

int main(int argc, char *argv[])
{
    int fd = 0;
    int retvalue = 0;
    char *filename;
    unsigned char databuf[1];

    if (argc != 3) {
        printf("Error usage!\r\n");
        return -1;
    }
    filename = argv[1];

    fd = open(filename, O_RDWR);
    if (fd < 0)
    {
        printf("Can't open file %s\r\n", filename);
        return -1;
    }

    //write
    databuf[0] = atoi(argv[2]);//将字符串转换为数字

    retvalue = write(fd, databuf, sizeof(databuf));
    if (retvalue < 0)
    {
        printf("LED Control failed!\r\n");
        close(fd);
        return -1;
    }

    //close
    retvalue = close(fd);
    if (retvalue < 0) {
        printf("close file %s failed!\r\n", filename);
    }

    return 0;
}


