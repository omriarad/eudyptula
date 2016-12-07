#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/string.h>

struct eudyptula_identity {
	char  name[20];
	int   id;
	bool  busy;
	struct list_head list;
};

static struct kmem_cache *eudyptula_cache;

static LIST_HEAD(identities);

static int identity_create(char *name, int id)
{
	struct eudyptula_identity *new_identity;
	int retval = 0;

	new_identity = kmem_cache_alloc(eudyptula_cache, GFP_KERNEL);
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

static struct eudyptula_identity *identity_find(int id)
{
	struct eudyptula_identity *pos;

	list_for_each_entry(pos, &identities, list) {
		if (pos->id == id)
			return pos;
	}

	return NULL;
}

static void identity_destroy(int id)
{
	struct eudyptula_identity *to_remove = identity_find(id);

	if (!to_remove) {
		pr_warn("%s: id %d not found in list\n", __func__, id);
		return;
	}

	list_del(&to_remove->list);
	kmem_cache_free(eudyptula_cache, to_remove);
}

int __init my_init(void)
{
	struct eudyptula_identity *temp;
	int retval = 0;

	eudyptula_cache = KMEM_CACHE(eudyptula_identity, 0);

	if (!eudyptula_cache) {
		pr_err("couldn't allocate kmem_cache\n");
		retval = -ENOMEM;
		goto kmem_cache_err;
	}

	retval |= identity_create("Alice", 1);
	retval |= identity_create("Bob", 2);
	retval |= identity_create("Dave", 3);
	retval |= identity_create("Gena", 10);
	if (retval) {
		pr_err("couldn't create identities\n");
		goto identity_err;
	}

	temp = identity_find(3);
	pr_debug("id 3 = %s\n", temp->name);

	temp = identity_find(42);
	if (temp == NULL)
		pr_debug("id 42 not found\n");

identity_err:
	identity_destroy(2);
	identity_destroy(1);
	identity_destroy(10);
	identity_destroy(42);
	identity_destroy(3);

kmem_cache_err:
	return retval;
}

void __exit my_exit(void)
{
	kmem_cache_destroy(eudyptula_cache);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("[91bc4039c624]");
MODULE_DESCRIPTION("Kernel linked list demo module");
