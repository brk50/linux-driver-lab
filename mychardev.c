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
#include <linux/fs.h>       /* To include for file_operations */
#include<linux/init.h>      /* To include __init, __exit */
#include <linux/module.h>   /* To include module_init, module_exit */
#include <linux/slab.h>     /* To include kmalloc, kfree */
#include <linux/uaccess.h>    /* To include copy_to_user, copy_from_user */ 


#define DEVICE_NAME "mychardev"
#define BUFFER_SIZE 1024

struct mydev_ctx{
    char buf[BUFFER_SIZE];
    size_t size;
};

static ssize_t dev_write(struct file *filep, const char __user *user_buf, size_t len, loff_t *offset){
    struct mydev_ctx *ctx = filep->private_data;
    if(len > BUFFER_SIZE){
        printk(KERN_ALERT "mychardev: Buffer overflow\n");
        return -EINVAL;
    }
    ctx->size = len;
    if(copy_from_user(ctx->buf, user_buf, len)){
        printk(KERN_ALERT "mychardev: Failed to copy data from user\n");
        return -EFAULT;
    }
    printk(KERN_INFO "mychardev: Received %zu bytes from user\n", len);

    return len;
}

bool is_buffer_empty(char* buf){
	if(buf == NULL)
        return true;
    return false;
}

static ssize_t dev_read(struct file *filep, char __user *user_buf, size_t len, loff_t *offset){
	struct mydev_ctx *ctx = filep->private_data;
	char *buffer_to_send = ctx->buf;
	
	//case 1: what is ctx->buf buffer is bigger than BUFFER SIZE
	if(len > BUFFER_SIZE){
        len = BUFFER_SIZE;
	}

	//case 2: what if buffer is empty
	if(ctx->size == 0){
        const char* fallback = "OH HAI MY CHARDEV";
		len = strlen(fallback);
        memcpy(ctx->buf, fallback, len);
        ctx->size = len;
	}
    //case 3: what if buffer is not empty
	if(copy_to_user(user_buf, buffer_to_send, len)){
        printk(KERN_ALERT "mychardev: Failed to copy date to user\n");
        return -EFAULT;
    }
	
    return ctx->size;
}

static int dev_open(struct inode *inodep, struct file *filep)
{
    struct mydev_ctx *ctx;
    ctx = kzalloc(sizeof(struct mydev_ctx), GFP_KERNEL);
    if(!ctx){
        printk(KERN_ALERT "memory not allocated");
        return -ENOMEM;
    }
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
    .write = dev_write,
    .read = dev_read,
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

