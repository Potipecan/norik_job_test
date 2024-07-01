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

static int custom_module_init(void);
static void custom_module_exit(void);
static long int custom_module_ioctl(struct file *file, unsigned int cmd, long unsigned int arg);
static int custom_module_open(struct inode *inode, struct file *file);
static int custom_module_release(struct inode *inode, struct file *file);

#endif //TEST_TASK_CUSTOMMODULE_H
