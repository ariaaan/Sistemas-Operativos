/*
* Sistemas Operativos I.
* ----------------------
*
* Trabajo Práctico Nº 3.
*
* Alumno: Giles García, Arian.
* Matrícula: 36201187.
*/

/*
* Libraries
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <asm/uaccess.h>

/*
* Module Information 
*/

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Device Driver Decrypter");
MODULE_AUTHOR("Arian Giles Garcia");

/*
* Variables 
*/

/* Major Number of the Driver */
static int major_number = 51;

/* Device Name */
static char device_name[] = "decrypter";

/* Amount that is added to every char of the data */
static int encryption_seed = 1;

/* Data to be encripted */
static char data[100] = {0};

/* Encripted data */
static char decrypted_data[100] = {0};

/*
* Functions Declarations 
*/

int dev_open(struct inode *pinode, struct file *pfile);
ssize_t dev_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset);
ssize_t dev_write (struct file *pfile, const char __user *buffer, size_t length, loff_t *offset);
int dev_close (struct inode *pinode, struct file *pfile);

void decrypt_data(void);

/*
* File Operations Structure 
*/

struct file_operations fops = {
	.open = dev_open,
	.read = dev_read,
	.write = dev_write,
	.release = dev_close
};

/*
* File Operations Functions
*/

/* Device Open */
int dev_open(struct inode *pinode, struct file *pfile) {
	printk(KERN_ALERT "Device encrypter open\n");
	return 0;
}

/* Device Read */
ssize_t dev_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset) {
	int k;
	int bytes_read = 0;

	if((*offset) >= strlen(decrypted_data)) {
		return 0;
	}	

	for(k = 0; k < strlen(decrypted_data); k++) {
		put_user(decrypted_data[k], buffer++);
		bytes_read++;
	}

	(*offset) += bytes_read;

	return bytes_read;
}

/* Device Write */
ssize_t dev_write (struct file *pfile, const char __user *buffer, size_t length, loff_t *offset) {
	memset(data, 0, 100);
	memset(decrypted_data, 0, 100);

	strncpy(data, buffer, length);

	decrypt_data();

	return length;
}

/* Device Close */
int dev_close (struct inode *pinode, struct file *pfile) {
	printk(KERN_ALERT "Device decrypter close\n");
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

/* Encrypt data */
void decrypt_data(void) {
	int k;
	for(k = 0; k < strlen(data); k++) {
		decrypted_data[k] = data[k] - encryption_seed;
	}
}

module_init(driver_init);
module_exit(driver_exit);
