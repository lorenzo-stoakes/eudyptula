#include <linux/init.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lorenzo Stoakes <lstoakes@gmail.com>");
MODULE_DESCRIPTION("A module that explores Linux Kernel style linked lists.");

struct identity {
	char  name[20];
	int   id;
	bool  busy;

	struct list_head list;
};

static struct kmem_cache *identity_cache;

static LIST_HEAD(identities);

static int identity_create(char *name, int id)
{
	struct identity *new;
	int name_len = strlen(name);

	if (name_len > 19)
		return -EINVAL;

	new = kmem_cache_alloc(identity_cache, GFP_KERNEL);
	if (!new)
		return -ENOMEM;

	strncpy(new->name, name, name_len + 1);
	new->id = id;
	new->busy = false;

	list_add(&new->list, &identities);

	return 0;
}

static struct identity *identity_find(int id)
{
	struct identity *el;

	list_for_each_entry(el, &identities, list)
		if (el->id == id)
			return el;

	return NULL;
}

static void identity_destroy(int id)
{
	struct identity *el = identity_find(id);

	if (el) {
		list_del(&el->list);
		kmem_cache_free(identity_cache, el);
	}
}

static void identity_create_and_check(char *name, int id)
{
	int num = identity_create(name, id);

	if (!num)
		return;

	switch (num) {
	case -EINVAL:
		pr_debug("list_test: couldn't create %s, invalid argument.",
			name);
		break;
	case -ENOMEM:
		pr_debug("list_test: couldn't create %s, insufficient memory.",
			name);
		break;
	default:
		pr_debug("list_test: couldn't create %s, unknown error %d.",
			name, num);
	}
}

static void print_identity(int id)
{
	struct identity *found = identity_find(id);

	if (found == NULL)
		pr_debug("list_test: %d: not found", id);
	else
		pr_debug("list_test: %d: found: %s", id, found->name);
}

static int __init list_test_init(void)
{
	int i;
	int ids[] = { 1, 1337, 907, 123, 456 };

	identity_cache = KMEM_CACHE(identity, 0);
	if (!identity_cache)
		return -ENOMEM;

	identity_create_and_check("Lorenzo", ids[0]);
	identity_create_and_check("Little", ids[1]);
	identity_create_and_check("Linus", ids[2]);

	for (i = 0; i < ARRAY_SIZE(ids); i++)
		print_identity(ids[i]);

	/* Delete in arbitrary order, because we can. */
	identity_destroy(ids[3]);
	identity_destroy(ids[1]);
	identity_destroy(ids[2]);
	identity_destroy(ids[0]);
	identity_destroy(ids[4]);

	/* Try again, this time we expect to see ARRAY_SIZE(ids) 'not
	 * found's. */
	for (i = 0; i < ARRAY_SIZE(ids); i++)
		print_identity(ids[i]);

	return 0;
}

static void __exit list_test_exit(void)
{
	kmem_cache_destroy(identity_cache);
}

module_init(list_test_init);
module_exit(list_test_exit);
