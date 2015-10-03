#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <asm/uaccess.h>

/* Module Information */
/* ------------------ */

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Device Driver Decrypter");
MODULE_AUTHOR("Arian Giles Garcia");

/* Variables */
/* --------- */

/* Major Number of the Driver */
static int major_number = 128;

/* Device Name */
static char device_name[] = "decrypter";


/* Amount that is substracted from every char of the data */
static int encryption_seed = 1;

/* Encripted data */
static char encrypted_data[100] = {0};

/* Original data */
static char data[100] = {0};

/* Functions Declarations */
/* ---------------------- */
int dev_open(struct inode *pinode, struct file *pfile);
ssize_t dev_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset);
ssize_t dev_write (struct file *pfile, const char __user *buffer, size_t length, loff_t *offset);
int dev_close (struct inode *pinode, struct file *pfile);

void decrypt_data(void);

/* File Operations Structure */
/* ------------------------- */

struct file_operations fops = {
	.open = dev_open,
	.read = dev_read,
	.write = dev_write,
	.release = dev_close
};

/* File Operations Functions */
/* ------------------------- */

/* Device Open */
int dev_open(struct inode *pinode, struct file *pfile) {

	return 0;
}

/* Device Read */
ssize_t dev_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset) {
	
	return 0;
}

/* Device Write */
ssize_t dev_write (struct file *pfile, const char __user *buffer, size_t length, loff_t *offset) {

	return 0;
}

/* Device Close */
int dev_close (struct inode *pinode, struct file *pfile) {

	return 0;
}

/* Driver Init */
int driver_init(void) {
	printk(KERN_ALERT "Device decrypter init\n");

	/* Register with the kernel and indicate 
	that we are registering a char device*/

	/* Parameteres: Major Number, Name of the driver, File operations */ 
	register_chrdev(major_number, device_name, &fops); 
	return 0;
}

/* Driver Exit */
void driver_exit(void) {
	printk(KERN_ALERT "Device decrypter exit\n");

	/* Unregister the char device driver */
	unregister_chrdev(major_number, device_name);
}

/* Decrypt data */
void decrypt_data(void) {
	int k;
	for(k = 0; k < strlen(encrypted_data) - 1; k++) {
		data[k] = encrypted_data[k] - encryption_seed;
	}

	//strcat(data, "\n");
}

module_init(driver_init);
module_exit(driver_exit);
