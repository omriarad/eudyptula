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

#define MY_ID ("91bc4039c624\n")
#define ID_LEN (13)

static ssize_t eudyptula_write(struct file *file, const char __user *buf,
				size_t len, loff_t *offset)
{
	char input[ID_LEN];
	ssize_t err;

	err = simple_write_to_buffer(input, ID_LEN - 1, offset, buf, len);
	if (err < 0)
		return err;

	if (strncmp(input, MY_ID, ID_LEN - 1) == 0)
		return ID_LEN;

	return -EINVAL;
}

static ssize_t eudyptula_read(struct file *file, char __user *buf,
				size_t len, loff_t *offset)
{
	return simple_read_from_buffer(buf, len, offset, MY_ID, ID_LEN);
}

static const struct file_operations eudyptula_fops = {
	.write			= eudyptula_write,
	.read			= eudyptula_read,
};

static struct miscdevice eudyptula = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "eudyptula",
	.fops = &eudyptula_fops,
};

static int __init enter_module(void)
{
	int retval;

	retval = misc_register(&eudyptula);
	pr_debug("Hello World!\n");
	return retval;
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
