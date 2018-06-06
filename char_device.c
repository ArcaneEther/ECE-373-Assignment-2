/*
  ECE 373
  Instructors PJ Waskiewicz Jr. and Shannon Nelson
  Higgins, Jeremy
  Assignment #2 - Character Device Driver
  
  The purpose of this program is to write a Character
  Device Driver with minimal File Operations. 
*/



/* Include Required Libraries. */
#include <linux/module.h>  /* Required for ? */
#include <linux/types.h>   /* Required for ? */
#include <linux/kdev_t.h>  /* Required for ? */
#include <linux/fs.h>      /* Required for ? */
#include <linux/cdev.h>    /* Required for ? */
#include <linux/usb.h>     /* Required for ? */
#include <linux/slab.h>    /* Required for ? */
#include <linux/uaccess.h> /* Required for copy_to_user() and copy_from_user(). */



/* Defines. */
#define DEVICE_COUNT 1
#define DEVICE_NAME "char_device"



/* Setting Module Information. */
MODULE_AUTHOR("Jeremy Higgins");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");



/* Declare char_device Struct. */
static struct cdev cdev;
static int syscall_val = 25;



/* Create a dev_t for the Major and Minor Numbers. */
static dev_t mydev_node;



/* Is this how to modify the syscall_val at insmod time? */
module_param(syscall_val, int, S_IRUSR | S_IWUSR);



/* Module Open Function. */
static int char_device_open(struct inode *inode, struct file *file){
  printk(KERN_INFO "char_device module opened.\n");
  /* mydev.syscall_val = exam; */
  return(0);
}



/* Module Read Function. */
static ssize_t char_device_read(struct file *file, char __user *buf, size_t len, loff_t *offset){
  /* "Get a local kernel buffer set aside." */
  /* But this isn't a buffer, it is an integer. */
  int ret;
  
  /* Checking if the "offset" is too big? */
  if (*offset >= sizeof(int)){
    return(0);
  }
  
  /* "Make sure our user wasn't bad." */
  /* What constitutes a "bad user"? */
  if (!buf){
    ret = -EINVAL;
    goto out;
  }
  
  /* Attempt to copy buffered memory from Kernel to User. */
  if (copy_to_user(buf, &syscall_val, sizeof(int))){
    ret = -EFAULT;
    goto out;
  }
  
  /* What is the point of setting the return value manually? */
  ret = sizeof(int);
  *offset += len;
  
  /* "Good to go, so printk the thingy" */
  /* What is a "thingy"? */
  printk(KERN_INFO "User received %d.\n", syscall_val);
  
  /* An error has occured, exit this function. */
  out:
    return(ret);
}



/* Module Write Function. */
static ssize_t char_device_write(struct file *file, const char __user *buf, size_t len, loff_t *offset){
  /* Have local kernel memory ready */
  char *kern_buf;
  int ret;
  
  /* Make sure our user isn't bad... */
  if (!buf){
    ret = -EINVAL;
    goto out;
  }
  
  /* Get some memory to copy into... */
  kern_buf = kmalloc(len, GFP_KERNEL);
  
  /* ...and make sure it's good to go */
  if (!kern_buf){
    ret = -ENOMEM;
    goto out;
  }
  
  /* Copy from the user-provided buffer */
  if (copy_from_user(kern_buf, buf, len)){
    /* uh-oh... */
    ret = -EFAULT;
    goto mem_out;
  }
  
  /* Why am I returning this value? */
  ret = len;
  
  /* Print what Userspace gave us? What does that mean? */
  printk(KERN_INFO "Userspace wrote \"%s\" to us\n", kern_buf);
  
  /* "goto" labels again. */
  mem_out:
    kfree(kern_buf);
  out:
    return(ret);
}



/* File Operations. */
static struct file_operations mydev_file_ops = {
  .owner = THIS_MODULE,
  .open = char_device_open,
  .read = char_device_read,
  .write = char_device_write,
}; /* File Operations Structure ends. */



/* Module Initialization Function. */
static int __init char_device_init(void){  
  /* Dynamically allocate the Character Device. */
  if(alloc_chrdev_region(&mydev_node, 0, DEVICE_COUNT, DEVICE_NAME)){
    /* Echo failure message. */
    printk(KERN_ERR "alloc_chrdev_region() failed!\n");
    
    /* Exit Module Failure. */
    return(-1);
  } /* if ends. */
  
  /* Echo the number of Minor Devices allocated at the Major Number. */
  printk(KERN_INFO "Allocated %d device/s at Major Number %d.\n", DEVICE_COUNT, MAJOR(mydev_node));
  
  /* Initialize the Character Device's File Operations. */
  cdev_init(&cdev, &mydev_file_ops);
  
  /* Set the Owner of the Character Device. */
  cdev.owner = THIS_MODULE;
  
  /* Add the Character to the Kernel. */
  if(cdev_add(&cdev, mydev_node, DEVICE_COUNT)){
    /* Echo failure message. */
    printk(KERN_ERR "cdev_add() failed!\n");
    
    /* Device add failed - unregister the Character Device. */
    unregister_chrdev_region(mydev_node, DEVICE_COUNT);
    
    /* Exit Module Failure. */
    return(-1);
  } /* if ends. */
  
  /* Exit Module Success. */
  return(0);
} /* Module Initialization Function ends. */



/* Module Exit Function. */
static void __exit char_device_exit(void){
  /* Delete the Character Device. */
  cdev_del(&cdev);
  
  /* Unregister the Character Device. */
  unregister_chrdev_region(mydev_node, DEVICE_COUNT);
  
  /* Echo Module Removed message. */
  printk(KERN_INFO "Module Removed.\n");
}/* Module Exit Function ends. */



/* Module Initialization and Exit. */
module_init(char_device_init);
module_exit(char_device_exit);
