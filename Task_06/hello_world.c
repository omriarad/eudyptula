#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/usb.h>
#include <linux/usb/input.h>
#include <linux/hid.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/uaccess.h>

#define INVALID_INPUT (-1)
#define MAX_CHARS (100)
#define STOP_READ (1)
#define MY_ID ("91bc4039c624")
#define ID_LEN (strlen(MY_ID))

static int eudyptula_open(struct inode *inode, struct file *file);
static int eudyptula_close(struct inode *inodep, struct file *filp);
static ssize_t eudyptula_write(struct file *file, const char __user *buf,
				size_t len, loff_t *offset);
static ssize_t eudyptula_read(struct file *file, char __user *buf,
				size_t len, loff_t *offset);

static const struct file_operations eudyptula_fops = {
	.write			= eudyptula_write,
	.read			= eudyptula_read,
	.open			= eudyptula_open,
	.release		= eudyptula_close,
};

static struct miscdevice eudyptula;

static int eudyptula_open(struct inode *inode, struct file *file)
{
	pr_info("Device opened\n");
	return 0;
}

static int eudyptula_close(struct inode *inodep, struct file *filp)
{
	pr_info("Device closed\n");
	return 0;
}

static ssize_t eudyptula_write(struct file *file, const char __user *buf,
				size_t len, loff_t *offset)
{
	if (strncmp(buf, MY_ID, ID_LEN) == 0)
		return strlen(buf);

	return INVALID_INPUT;
}

static ssize_t eudyptula_read(struct file *file, char __user *buf,
				size_t len, loff_t *offset)
{
	int i;

	if (*offset != 0)
		return 0;

	for (i = 0; i < ID_LEN; i++)
		put_user(MY_ID[i], buf++);

	*offset = STOP_READ;
	return ID_LEN;
}

static int __init enter_module(void)
{
	int retval;

	eudyptula.minor = MISC_DYNAMIC_MINOR;
	eudyptula.name = "eudyptula";
	eudyptula.fops = &eudyptula_fops;
	retval = misc_register(&eudyptula);
	if (retval)
		return retval;

	pr_info("got minor %i\n", eudyptula.minor);
	pr_debug("Hello World!\n");
	return 0;
}

static void __exit exit_module(void)
{
	misc_deregister(&eudyptula);
	pr_debug("Goodbye cruel world!\n");
}

module_init(enter_module);
module_exit(exit_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("[91bc4039c624]");
MODULE_DESCRIPTION("Used to be a simple hello world kernel module");
