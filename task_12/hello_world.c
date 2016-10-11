#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/string.h>

struct identity {
	char  name[20];
	int   id;
	bool  busy;
	struct list_head list;
};

static LIST_HEAD(identities);

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
	list_add(&new_identity->list, &identities);

err:
	return retval;
}

static struct identity *identity_find(int id)
{
	struct list_head *pos;
	struct identity *cur;

	list_for_each(pos, &identities) {
		cur = list_entry(pos, struct identity, list);
		if (cur->id == id)
			return cur;
	}

	return NULL;
}

static void identity_destroy(int id)
{
	struct identity *to_remove = identity_find(id);

	if (!to_remove) {
		pr_warn("%s: id %d not found in list\n", __func__, id);
		return;
	}

	list_del(&to_remove->list);
	kfree(to_remove);
}

int __init my_init(void)
{
	struct identity *temp;
	int retval = 0;

	retval |= identity_create("Alice", 1);
	retval |= identity_create("Bob", 2);
	retval |= identity_create("Dave", 3);
	retval |= identity_create("Gena", 10);
	if (retval) {
		pr_err("couldn't create identities\n");
		goto err;
	}

	temp = identity_find(3);
	pr_debug("id 3 = %s\n", temp->name);

	temp = identity_find(42);
	if (temp == NULL)
		pr_debug("id 42 not found\n");

err:
	identity_destroy(2);
	identity_destroy(1);
	identity_destroy(10);
	identity_destroy(42);
	identity_destroy(3);

	return retval;
}

void __exit my_exit(void)
{
	;
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("[91bc4039c624]");
MODULE_DESCRIPTION("Kernel linked list demo module");
