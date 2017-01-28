#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/wait.h>
#include <linux/kthread.h>

#define MY_ID ("91bc4039c624\n")
#define EUDYPTULA ("eudyptula")
#define ID_LEN (13)

DECLARE_WAIT_QUEUE_HEAD(wee_wait);
static struct task_struct *keudyptula;

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

static const struct file_operations eudyptula_fops = {
	.write			= eudyptula_write,
};

static struct miscdevice eudyptula = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = EUDYPTULA,
	.fops = &eudyptula_fops,
	.mode = 0222,
};

static int eudyptula_kthread(void *data)
{
	pr_debug("eudyptula_kthread going to sleep\n");
	wait_event_interruptible(wee_wait, kthread_should_stop());
	pr_debug("eudyptula_kthread woken\n");
	return 0;
}

static int __init enter_module(void)
{
	int err;

	err = misc_register(&eudyptula);
	if (err)
		goto misc_err;

	keudyptula = kthread_run(eudyptula_kthread, NULL, EUDYPTULA);
	if (keudyptula == ERR_PTR(-ENOMEM)) {
		err = -ENOMEM;
		goto kthread_err;
	}

	pr_debug("Hello World!\n");
	return 0;

kthread_err:
	misc_deregister(&eudyptula);
misc_err:
	return err;
}

static void __exit exit_module(void)
{
	misc_deregister(&eudyptula);
	kthread_stop(keudyptula);
	pr_debug("Goodbye cruel world!\n");
}

module_init(enter_module);
module_exit(exit_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("[91bc4039c624]");
MODULE_DESCRIPTION("Used to be a simple hello world kernel module");
