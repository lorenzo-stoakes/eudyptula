#include <linux/init.h>
#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/jiffies.h>
#include <linux/gfp.h>
#include <linux/mutex.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lorenzo Stoakes <lstoakes@gmail.com>");
MODULE_DESCRIPTION("A test module that interfaces with debugfs.");

DEFINE_MUTEX(foo_lock);

static struct dentry *eudyptula_dir;
static unsigned long foo_page;

static const char hello_id[] = "f8cdc7cef553\n";
static const size_t hello_id_len = 13;

static ssize_t id_read(struct file *file, char __user *out,
			  size_t size, loff_t *off)
{
	return simple_read_from_buffer(out, size, off, hello_id, hello_id_len);
}

static ssize_t id_write(struct file *file, const char __user *in,
			   size_t size, loff_t *off)
{
	size_t id_chars_len = hello_id_len - 1; /* Excluding newline. */
	char buf[id_chars_len];

	if (size < id_chars_len)
		goto invalid;

	simple_write_to_buffer(buf, id_chars_len, off, in, size);
	if (strncmp(buf, hello_id, id_chars_len))
		goto invalid;

	/* We're a sink. */
	*off = size;
	return size;
 invalid:
	return -EINVAL;
}

static const struct file_operations id_fops = {
	.owner = THIS_MODULE,
	.read = id_read,
	.write = id_write
};

static ssize_t foo_read(struct file *file, char __user *out,
			size_t size, loff_t *off)
{
	ssize_t ret;

	mutex_lock(&foo_lock);
	ret = simple_read_from_buffer(out, size, off, (char *)foo_page,
				      PAGE_SIZE);
	mutex_unlock(&foo_lock);

	return ret;
}

static ssize_t foo_write(struct file *file, const char __user *in,
			 size_t size, loff_t *off)
{
	ssize_t ret;

	mutex_lock(&foo_lock);
	ret = simple_write_to_buffer((char *)foo_page, PAGE_SIZE, off, in, size);
	mutex_unlock(&foo_lock);

	return ret;
}

static const struct file_operations foo_fops = {
	.owner = THIS_MODULE,
	.read = foo_read,
	.write = foo_write
};

static int __init
hello_init(void)
{
	struct dentry *ptr;

	eudyptula_dir = debugfs_create_dir("eudyptula", NULL);
	if (IS_ERR_OR_NULL(eudyptula_dir)) {
		ptr = eudyptula_dir;
		goto error_dir;
	}

	ptr = debugfs_create_file("id", 0666, eudyptula_dir, NULL, &id_fops);
	if (IS_ERR_OR_NULL(ptr))
		goto error;

	ptr = debugfs_create_size_t("jiffies", 0444, eudyptula_dir,
				    (size_t *)&jiffies);
	if (IS_ERR_OR_NULL(ptr))
		goto error;

	ptr = debugfs_create_file("foo", 0644, eudyptula_dir, NULL, &foo_fops);
	if (IS_ERR_OR_NULL(ptr))
		goto error;

	foo_page = get_zeroed_page(GFP_KERNEL);
	if (!foo_page) {
		ptr = NULL;
		goto error;
	}

	return 0;

 error:
	debugfs_remove_recursive(eudyptula_dir);
 error_dir:
	return ptr ? PTR_ERR(ptr) : -ENOMEM;
}

static void __exit
hello_exit(void)
{
	free_page(foo_page);
	debugfs_remove_recursive(eudyptula_dir);
}

module_init(hello_init);
module_exit(hello_exit);
