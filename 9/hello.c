#include <linux/init.h>
#include <linux/module.h>
#include <linux/jiffies.h>
#include <linux/mutex.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lorenzo Stoakes <lstoakes@gmail.com>");
MODULE_DESCRIPTION("A test module that interfaces with sysfs.");

/*
 * References Used:-
 *
 * http://geekwentfreak-raviteja.rhcloud.com/blog/2010/06/28/
 * https://stuff.mit.edu/afs/sipb/contrib/linux/samples/kobject/kobject-example.c
 * https://www.kernel.org/doc/Documentation/filesystems/sysfs.txt
 */

static DEFINE_MUTEX(foo_lock);
static unsigned long foo_page;

static const char hello_id[] = "f8cdc7cef553";
static const size_t hello_id_len = 12;

static struct kobject *eudyptula_kobj;

/* ID Read/Write */

static ssize_t
id_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%s\n", hello_id);
}

static ssize_t
id_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf,
	 size_t count)
{
	if (count < hello_id_len || strncmp(buf, hello_id, hello_id_len))
		return -EINVAL;

	return count;
}

/* Jiffies Read */

static ssize_t
jiffies_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%lu\n", jiffies);
}

/* Foo Read/Write */

static ssize_t
foo_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	ssize_t ret;

	mutex_lock(&foo_lock);
	ret = sprintf(buf, "%s", (char *)foo_page);
	mutex_unlock(&foo_lock);

	return ret;
}

static ssize_t
foo_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf,
	 size_t count)
{
	/* buf has a maximum size of PAGE_SIZE so no limit checks
	   required. */

	mutex_lock(&foo_lock);
	memcpy((char *)foo_page, buf, count);
	mutex_unlock(&foo_lock);

	return count;
}

static struct kobj_attribute id_attr = __ATTR(id, 0644, id_show, id_store),
	jiffies_attr = __ATTR(jiffies, 0444, jiffies_show, NULL),
	foo_attr = __ATTR(foo, 0644, foo_show, foo_store);

static struct attribute *attrs[] = {
	&id_attr.attr,
	&jiffies_attr.attr,
	&foo_attr.attr,
	NULL
};

static struct attribute_group attr_group = {
	.attrs = attrs
};

static int __init
hello_init(void)
{
	int ret;

	eudyptula_kobj = kobject_create_and_add("eudyptula", kernel_kobj);
	if (!eudyptula_kobj)
		return -ENOMEM;

	foo_page = get_zeroed_page(GFP_KERNEL);
	if (!foo_page) {
		ret = -ENOMEM;
		goto error;
	}

	ret = sysfs_create_group(eudyptula_kobj, &attr_group);
	if (ret) {
		free_page(foo_page);
		goto error;
	}

	return 0;

 error:
	kobject_put(eudyptula_kobj);

	return ret;
}

static void __exit
hello_exit(void)
{
	free_page(foo_page);
	kobject_put(eudyptula_kobj);
}

module_init(hello_init);
module_exit(hello_exit);
