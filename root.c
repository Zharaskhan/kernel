#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/fs.h>          	/* Needed for char device registration */
#include <linux/cdev.h>		/* Needed for char device registration */
MODULE_LICENSE("GPL");             
MODULE_AUTHOR("Zharaskhan Aman");
MODULE_DESCRIPTION("A simple module to obtain root access");
MODULE_VERSION("0.1");  

static ssize_t custom_write(struct file *, const char __user *, size_t, loff_t *);
static dev_t first;

static struct device* custom_dev = NULL;
static struct class*  custom_class = NULL;
static struct cdev cdev;

struct file_operations fops = {
	.owner = THIS_MODULE,
	.write = custom_write,
};

ssize_t custom_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
	//TODO
	printk(KERN_INFO "Write function call\n");
	return len;
}

int init_module(void)
{
	printk(KERN_INFO "Root module starts.\n");

	//Allocating memory
	int error = alloc_chrdev_region(&first, 0, 1, "magic");
	
	if (error < 0) {
		printk(KERN_ERR "Couldn't allocate memory.\n");
		return error;
	}
	
	printk(KERN_INFO "Device number %d:%d\n", MAJOR(first), MINOR(first));

	//register class
	custom_class = class_create(THIS_MODULE, "magic");
	//TODO error handler
	
	//register device
	custom_dev = device_create(custom_class, NULL, first, NULL, "magic");
	//TODO error handler

	//init new device
	cdev_init(&cdev, &fops);

	cdev.owner = THIS_MODULE;
	
	//adding to kernel new device
	cdev_add(&cdev, first, 1);

	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Root module ends.\n");

	device_destroy(custom_class, first);
	class_unregister(custom_class);
	class_destroy(custom_class);
	//Deallocating memory
	unregister_chrdev_region(first, 1);
}
