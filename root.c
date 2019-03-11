#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/smp.h>

MODULE_LICENSE("GPL");             
MODULE_AUTHOR("Zharaskhan Aman");
MODULE_DESCRIPTION("A simple module to obtain root access");
MODULE_VERSION("0.1");  

int init_module(void)
{
	printk(KERN_INFO "Root module starts.\n");
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Root module ends.\n");
}
