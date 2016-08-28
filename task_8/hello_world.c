
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/debugfs.h>
#include <linux/jiffies.h>

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
	.write	= eudyptula_write,
	.read	= eudyptula_read,
};

static struct dentry *debugfs_eudyptula_dir;

int __init enter_module(void)
{
	struct dentry *id_file, *jiffies_file;

	pr_debug("Hello World!\n");
	debugfs_eudyptula_dir = debugfs_create_dir("eudyptula", NULL);
	if (debugfs_eudyptula_dir == NULL) {
		pr_err("could not create debugfs dir\n");
		goto err;
	}

	id_file = debugfs_create_file("id", 0755, debugfs_eudyptula_dir,
		NULL, &eudyptula_fops);

	if (id_file == NULL) {
		pr_err("could not create debugfs id file\n");
		goto err_file;
	}

	jiffies_file = debugfs_create_u64("jiffies", 0444,
		debugfs_eudyptula_dir, &jiffies_64);

	if (jiffies_file == NULL) {
		pr_err("could not create debugfs id file\n");
		goto err_file;
	}

	return 0;

err_file:
	debugfs_remove_recursive(debugfs_eudyptula_dir);
err:
	return -ENOMEM;
}

void __exit exit_module(void)
{
	pr_debug("Goodbye cruel world!\n");
	debugfs_remove_recursive(debugfs_eudyptula_dir);
}

module_init(enter_module);
module_exit(exit_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("[91bc4039c624]");
MODULE_DESCRIPTION("Used to be a simple hello world kernel module");
