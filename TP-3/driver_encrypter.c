#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <asm/uaccess.h>

/* Module Information */
/* ------------------ */

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Device Driver Encryptor");
MODULE_AUTHOR("Arian Giles Garcia");

/* Variables */
/* --------- */

/* Major Number of the Driver */
static int major_number = 256;

/* Amount that is added to every char of the data */
static int encryptation_seed = 1;

/* Data to be encripted */
static char data[100] = {0};

/* Encripted data */
static char encrypted_data[100] = {0};


/* Functions Declarations */
/* ---------------------- */
int dev_open(struct inode *pinode, struct file *pfile);
ssize_t dev_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset);
ssize_t dev_write (struct file *pfile, const char __user *buffer, size_t length, loff_t *offset);
int dev_close (struct inode *pinode, struct file *pfile);

void encrypt_data(void);

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
	printk(KERN_ALERT "Device was opened\n");
	return 0;
}

/* Device Read */
ssize_t dev_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset) {
	int k;
	int bytes_read = 0;

	if((*offset) >= strlen(encrypted_data)) {
		return 0;
	}	

	printk(KERN_ALERT "Read Data: %s\n", data);
	printk(KERN_ALERT "Length: %zu\n", length);

	printk(KERN_ALERT "Encrypted Data: %s\n", encrypted_data);

	for(k = 0; k < strlen(encrypted_data); k++) {
		put_user(encrypted_data[k], buffer++);
		bytes_read++;
	}

	printk(KERN_ALERT "bytes_read: %d\n", bytes_read);

	(*offset) += bytes_read;

	return bytes_read;
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
	printk(KERN_ALERT "Device was closed\n");
	return 0;
}

/* Driver Init */
int driver_init(void) {
	printk(KERN_ALERT "Device init\n");

	/* Register with the kernel and indicate 
	that we are registering a char device*/

	/* Parameteres: Major Number, Name of the driver, File operations */ 
	register_chrdev(major_number, "encrypter", &fops); 
	return 0;
}

/* Driver Exit */
void driver_exit(void) {
	printk(KERN_ALERT "Device exit\n");

	/* Unregister the char device driver */
	unregister_chrdev(major_number, "encrypter");
}

/* Encrypt data */
void encrypt_data(void) {
	int k;
	for(k = 0; k < strlen(data); k++) {
		encrypted_data[k] = data[k] + encryptation_seed;
	}

	strcat(encrypted_data, "\n");
}

module_init(driver_init);
module_exit(driver_exit);
