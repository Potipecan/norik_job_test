//
// Created by poti on 01/07/24.
//

#include "task1_module.h"

MODULE_AUTHOR("podpecan.andraz28@gmail.com");
MODULE_DESCRIPTION("Custom GPIO device driver");
MODULE_LICENSE("GPL");

#define SUCCESS 0
#define DEVICE_NAME "customled"
#define BUFF_SIZE 2

module_init(custom_module_init)
module_exit(custom_module_exit)

enum {
    CDEV_NOT_USED = 0,
    CDEV_EXCLUSIVE_OPEN = 1
};


static int major;
static atomic_t already_open = ATOMIC_INIT(CDEV_NOT_USED);
static struct class *cls;
static char buff[BUFF_SIZE + 1] = {'\0'};


struct file_operations fops = {
    .open = custom_module_open,
    .release = custom_module_release,
    .write = custom_module_write
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
    
#if GPIO_ENABLED
    if(gpio_request(GPIO_PIN, "LED")){
        pr_err(DEVICE_NAME ": Failed to allocate GPIO pin.");
        goto gpioErr;
    }

    if(gpio_direction_output(GPIO_PIN, 0)){
        pr_err(DEVICE_NAME ": Failed to set GPIO pin " GPIO_PIN " direction.");
        goto pinErr;
    }
#endif
    return SUCCESS;

#if GPIO_ENABLED
pinErr:
    gpio_free(GPIO_PIN);
gpioErr:
    return 100
#endif
}

void custom_module_exit(void){
#if GPIO_ENABLED
    gpio_set_value(GPIO_PIN, 0);
    gpio_free(GPIO_PIN);
#endif
    printk(DEVICE_NAME ": Custom character device EXIT");
    device_destroy(cls, MKDEV(major, 0));
    class_destroy(cls);
    unregister_chrdev(major, DEVICE_NAME);
}

ssize_t custom_module_write(struct file *file, const char *value, size_t size, loff_t *off){
    if(size > BUFF_SIZE) goto invalidValue; // if the data stream is too large for a valid argument, fail.

    if(copy_from_user(buff, value, size)){
        pr_err(DEVICE_NAME ": Failed to to copy value to kernel space.");
        return -EFAULT;
    }

    if(size >= BUFF_SIZE && buff[1]){ // if command doesn't terminate on 2nd character, fail
        goto invalidValue;
    }

    int val = buff[0] - '0'; // extracting value
    // pr_info(DEVICE_NAME ": Received command: %d", val);
    switch(val){
        case 0:
            pr_info(DEVICE_NAME ": Switching LED Off");
        #if GPIO_ENABLED
            gpio_set_value(GPIO_PIN, 0);
        #endif
        break;
        case 1:
            pr_info(DEVICE_NAME ": Switching LED On");
        #if GPIO_ENABLED
            gpio_set_value(GPIO_PIN, 1);
        #endif
        break;
        default:
         // invalid value, fail
        goto invalidValue;
    }

    return size;

invalidValue:
    pr_err(DEVICE_NAME ": Invalid control argument \"%s\" of size %ld", buff, size);
    return -EINVAL;
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