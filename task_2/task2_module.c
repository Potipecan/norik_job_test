//
// Created by poti on 01/07/24.
//

#include "task2_module.h"

MODULE_AUTHOR("podpecan.andraz28@gmail.com");
MODULE_DESCRIPTION("Custom character device driver");
MODULE_LICENSE("GPL");

#define SUCCESS 0
#define DEVICE_NAME "customdevice"

module_init(custom_module_init)
module_exit(custom_module_exit)

enum {
    CDEV_NOT_USED = 0,
    CDEV_EXCLUSIVE_OPEN = 1
};


static int major;
static atomic_t already_open = ATOMIC_INIT(CDEV_NOT_USED);
static struct class *cls;


struct file_operations fops = {
    .open = custom_module_open,
    .release = custom_module_release,
    .unlocked_ioctl = custom_module_ioctl
};

// initializer
int custom_module_init(void){
    printk("Custom character device INIT");
    major = register_chrdev(0, DEVICE_NAME, &fops);

    if(major < 0){
        printk("Char device registering failed with %d\n", major);
        return major;
    }
    printk(DEVICE_NAME ": Char device registering succeeded with major %d\n", major);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)

    cls = class_create(DEVICE_NAME);

#else

    cls = class_create(THIS_MODULE, DEVICE_NAME);

#endif

    device_create(cls, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    pr_info(DEVICE_NAME ": Device created on /dev/%s\n", DEVICE_NAME);
    
    return SUCCESS;
}

void custom_module_exit(void){
    printk(DEVICE_NAME ": Custom character device EXIT");
    device_destroy(cls, MKDEV(major, 0));
    class_destroy(cls);
    unregister_chrdev(major, DEVICE_NAME);
}

long int custom_module_ioctl(struct file *file, unsigned int cmd, long unsigned int arg){
    printk(DEVICE_NAME ": IOCTL call. NAME: %s, PID: %d", current->comm, current->pid);
    return SUCCESS;
}

int custom_module_open(struct inode *inode, struct file *file){
    if (atomic_cmpxchg(&already_open, CDEV_NOT_USED, CDEV_EXCLUSIVE_OPEN)){
        pr_err(DEVICE_NAME ": Invalid access");
        return -EBUSY;
    }
    try_module_get(THIS_MODULE);
    pr_info(DEVICE_NAME ": Device opened");
    return SUCCESS;
}

int custom_module_release(struct inode *inode, struct file *file){
    atomic_set(&already_open, CDEV_NOT_USED);
    module_put(THIS_MODULE);
    pr_info(DEVICE_NAME ": Device released");
    return SUCCESS;
}