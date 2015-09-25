#include <linux/module.h>	
#include <linux/kernel.h>	
#include <linux/init.h>		

static int __init hello_2_init(void) {
	printk(KERN_INFO "Hola mundo por segunda vez!\n");
	return 0;
}

static void __exit hello_2_exit(void) {
	printk(KERN_INFO "Adios mundo cruel, otra vez!\n");
}

module_init(hello_2_init);
module_exit(hello_2_exit);