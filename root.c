#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/fs.h>          /* Needed for char device registration */

MODULE_LICENSE("GPL");             
MODULE_AUTHOR("Zharaskhan Aman");
MODULE_DESCRIPTION("A simple module to obtain root access");
MODULE_VERSION("0.1");  

static ssize_t custom_write(struct file *, const char __user *, size_t, loff_t *);
static dev_t first;
struct file_operations fops = {
	.owner = THIS_MODULE,
	.write = custom_write,
};

int init_module(void)
{
	printk(KERN_INFO "Root module starts.\n");

	//allocating device number
	int error = alloc_chrdev_region(&first, 0, 1, "magic");
	
	if (error < 0) {
		printk(KERN_ERR "Couldn't allocate numer for device");
		return error;
	}
	
	printk(KERN_INFO "Allocated %d major number", MAJOR(first));

	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Root module ends.\n");
	
	//deallocating device number
	unregister_chrdev_region(first, 1);
}
