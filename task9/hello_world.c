#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/jiffies.h>
#include <linux/gfp.h>
#include <linux/mutex.h>

#define MY_ID ("91bc4039c624\n")
#define ID_LEN (13)

static ssize_t id_store(struct kobject *kobj, struct kobj_attribute *attr,
				 const char *buf, size_t count)
{
	if (strncmp(buf, MY_ID, ID_LEN - 1) == 0)
		return ID_LEN;

	return -EINVAL;
}

static ssize_t id_show(struct kobject *kobj, struct kobj_attribute *attr,
				char *buf)
{
	return sprintf(buf, MY_ID);
}

static struct kobj_attribute id_attribute =
	__ATTR_RW(id);

static ssize_t jiffes_show(struct kobject *kobj, struct kobj_attribute *attr,
				char *buf)
{
	return sprintf(buf, "%llu\n", jiffies_64);
}

static struct kobj_attribute jiffies_attribute =
	__ATTR_RO(jiffes);

static void *foo_page;
static DEFINE_MUTEX(foo_lock);

static ssize_t foo_store(struct kobject *kobj, struct kobj_attribute *attr,
				 const char *buf, size_t count)
{
	ssize_t retval;

	retval = mutex_lock_interruptible(&foo_lock);
	if (retval < 0)
		goto err;

	retval = snprintf(foo_page, count, buf);
	mutex_unlock(&foo_lock);
err:
	return retval;
}

static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr,
				char *buf)
{
	ssize_t retval;

	retval = mutex_lock_interruptible(&foo_lock);
	if (retval < 0)
		goto err;

	retval = snprintf(buf, PAGE_SIZE, foo_page);
	mutex_unlock(&foo_lock);
err:
	return retval;
}

static struct kobj_attribute foo_attribute =
	__ATTR_RW(foo);

static struct attribute *attrs[] = {
	&id_attribute.attr,
	&jiffies_attribute.attr,
	&foo_attribute.attr,
	NULL,
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

static struct kobject *eudyptula_kobj;

int __init enter_module(void)
{
	int retval;

	pr_debug("Hello World!\n");

	foo_page = (void *)get_zeroed_page(GFP_KERNEL);
	if (foo_page == NULL) {
		pr_err("could not allocate page for foo\n");
		goto err;
	}

	eudyptula_kobj = kobject_create_and_add("eudyptula", kernel_kobj);
	if (!eudyptula_kobj) {
		pr_err("could not create kobject eudyptula\n");
		goto err_page;
	}

	retval = sysfs_create_group(eudyptula_kobj, &attr_group);
	if (retval) {
		pr_err("could not create sysfs group\n");
		goto err_kobject;
	}

	return 0;

err_kobject:
	kobject_put(eudyptula_kobj);
err_page:
	free_page((unsigned long)foo_page);
err:
	return -ENOMEM;
}

void __exit exit_module(void)
{
	pr_debug("Goodbye cruel world!\n");
	kobject_put(eudyptula_kobj);
	free_page((unsigned long)foo_page);
}

module_init(enter_module);
module_exit(exit_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("[91bc4039c624]");
MODULE_DESCRIPTION("Used to be a simple hello world kernel module");
