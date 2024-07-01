//
// Created by poti on 01/07/24.
//

#ifndef TEST_TASK_CUSTOMMODULE_H
#define TEST_TASK_CUSTOMMODULE_H

#include <asm/current.h>
#include <asm/io.h>
#include <asm/segment.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/ktime.h>
#include <linux/rtc.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/version.h>

#ifndef GPIO_ENABLED
#define GPIO_ENABLED 0
#define GPIO_PIN 0
#endif

#if GPIO_ENABLED
#include <linux/gpio.h>
#endif


static int custom_module_init(void);
static void custom_module_exit(void);
ssize_t custom_module_write(struct file *file, const char *value, size_t size, loff_t *off);
static int custom_module_open(struct inode *inode, struct file *file);
static int custom_module_release(struct inode *inode, struct file *file);

#endif //TEST_TASK_CUSTOMMODULE_H
