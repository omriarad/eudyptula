#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/wait.h>
#include <linux/kthread.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/mutex.h>

#define MAX_ID_LEN (20)
#define EUDYPTULA ("eudyptula")

struct identity {
	char  name[MAX_ID_LEN];
	int   id;
	bool  busy;
	struct list_head list;
};

static LIST_HEAD(identities);
static DECLARE_WAIT_QUEUE_HEAD(wee_wait);
static DEFINE_MUTEX(identity_lock);
static struct task_struct *keudyptula;
static int counter;

static int identity_create(char *name, int id)
{
	struct identity *new_identity;
	int retval = 0;

	new_identity = kzalloc(sizeof(struct identity), GFP_KERNEL);
	if (!new_identity) {
		retval = -ENOMEM;
		goto err;
	}

	strncpy(new_identity->name, name, sizeof(new_identity->name));
	new_identity->id = id;
	new_identity->busy = false;
	retval = mutex_lock_interruptible(&identity_lock);
	if (retval < 0)
		goto err;

	list_add(&new_identity->list, &identities);
	mutex_unlock(&identity_lock);

err:
	return retval;
}

static struct identity *identity_get(void)
{
	struct identity *identity = NULL;
	int err;

	err = mutex_lock_interruptible(&identity_lock);
	if (err < 0) {
		identity = ERR_PTR(err);
		goto err;
	}

	if (list_empty(&identities))
		goto err_mutex;

	identity = list_entry(identities.next, struct identity, list);
	list_del(&identity->list);

err_mutex:
	mutex_unlock(&identity_lock);
err:
	return identity;
}

static ssize_t eudyptula_write(struct file *file, const char __user *buf,
				size_t len, loff_t *offset)
{
	char input[MAX_ID_LEN] = {};

	if (copy_from_user(input, buf, len < MAX_ID_LEN ? len : MAX_ID_LEN))
		return -EINVAL;

	if (identity_create(input, counter))
		return -EINVAL;

	counter++;
	wake_up_interruptible(&wee_wait);
	return len;
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
	int err;
	struct identity *identity;

	while (1) {
		err = wait_event_interruptible(wee_wait, 1);
		if (err) {
			pr_err("wait_event_interruptible failed\n");
			return err;
		}

		if (kthread_should_stop())
			break;

		identity = identity_get();

		if (identity) {
			msleep_interruptible(5000);
			pr_debug("identity: %i %s\n", identity->id,
					identity->name);
			kfree(identity);
		}
	}

	return 0;
}

static int __init enter_module(void)
{
	int err;

	counter = 0;
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
	struct identity *identity, *next;

	misc_deregister(&eudyptula);
	kthread_stop(keudyptula);
	list_for_each_entry_safe(identity, next, &identities, list) {
		list_del(&identity->list);
		kfree(identity);
	}
	pr_debug("Goodbye cruel world!\n");
}

module_init(enter_module);
module_exit(exit_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("[91bc4039c624]");
MODULE_DESCRIPTION("Used to be a simple hello world kernel module");
