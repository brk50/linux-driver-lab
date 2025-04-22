/*
 * mychardev.c - A simple character device driver
 *
 * Copyright (C) 2025 Bhaviripudi Rakesh Naidu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include<linux/init.h>
#include <linux/fs.h>        // for file_operations 
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>  // for copy_to_user, copy_from_user


#define DEVICE_NAME "mychardev"
#define BUFFER_SIZE 1024

struct mydev_ctx{
    char buf[BUFFER_SIZE];
    size_t size;
};

static int dev_open(struct inode *inodep, struct file *filep)
{
    struct mydev_ctx *ctx;
    ctx = kmalloc(sizeof(struct mydev_ctx), GFP_KERNEL);
    if(!ctx){
        printk(KERN_ALERT "memory not allocated");
        return -ENOMEM;
    }
    memset(ctx, 0, sizeof(struct mydev_ctx));
    filep->private_data = ctx;

    printk(KERN_INFO "mychardev: Device opened\n");
    return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
    kfree(filep->private_data);
    printk(KERN_INFO "mychardev: Device closed\n");
    return 0;
}

static int major; 

static struct file_operations fops ={
    .owner = THIS_MODULE,
    .open = dev_open,
    .release = dev_release,
};

static int mychardev_init(void)
{
    major = register_chrdev(0 , DEVICE_NAME, &fops );

    if(major < 0){
        printk(KERN_ALERT "mychardev: Failed to register character device\n");
        return major;
    }
    
    printk(KERN_INFO "mychardev: Registered successfully with major number %d\n", major);
    return 0;
}

static int __init myinit(void)
{
    int ret = 0;
    ret = mychardev_init();
    if(ret < 0){
        printk(KERN_ALERT "mychardev: Module failed to load\n");
    }
    printk(KERN_INFO "MyCharDev: Module loaded successfully!\n");
    return 0;
}

static void __exit myexit(void)
{  
    if (major >= 0) {
        unregister_chrdev(major, DEVICE_NAME);
    }

    printk(KERN_INFO "MyCharDev: Module unloaded.\n");
    return;
}

module_init(myinit);
module_exit(myexit);
MODULE_LICENSE("GPL");