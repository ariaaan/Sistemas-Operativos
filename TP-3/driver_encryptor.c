#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Device Driver Encryptor");
MODULE_AUTHOR("Arian Giles Garcia");

static int major_number = 256;

static int encryptation_seed = 1;

static char data[100] = {0};
static char encrypted_data[100] = {0};

void encrypt_data(void) {
	int k;
	for(k = 0; k < strlen(data); k++) {
		encrypted_data[k] = data[k] + encryptation_seed;
	}
}

/* File Operations Functions */
/* ------------------------- */

/* Device Open */
int dev_open(struct inode *pinode, struct file *pfile) {
	printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);
	return 0;
}

/* Device Read */
ssize_t dev_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset) {
	printk(KERN_ALERT "Inside the %s function.\n", __FUNCTION__);
	printk(KERN_ALERT "Data: %s\n", data);
	printk(KERN_ALERT "Encrypted Data: %s\n", encrypted_data);
	return 0;
}

/* Device Write */
ssize_t dev_write (struct file *pfile, const char __user *buffer, size_t length, loff_t *offset) {
	printk(KERN_ALERT "Inside the %s function.\n", __FUNCTION__);
	strncpy(data, buffer, length - 1);
	printk("Data was coppied\n");

	encrypt_data();

	printk("Data was encrypted\n");

	return length;
}

/* Device Close */
int dev_close (struct inode *pinode, struct file *pfile) {
	printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);
	return 0;
}



/* File Operations Structure */
/* ------------------------- */

struct file_operations fops = {
	.open = dev_open,
	.read = dev_read,
	.write = dev_write,
	.release = dev_close
};

/* Driver Init */
int driver_init(void) {
	printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);

	/* Register with the kernel and indicate 
	that we are registering a char device*/

	/* Parameteres: Major Number, Name of the driver, File operations */ 
	register_chrdev(major_number, "encryptor", &fops); 
	return 0;
}

/* Driver Exit */
void driver_exit(void) {
	printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);

	/* Unregister the char device driver */
	unregister_chrdev(major_number, "encryptor");
}

module_init(driver_init);
module_exit(driver_exit);
