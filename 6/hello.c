#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>

MODULE_AUTHOR("Lorenzo Stoakes <lstoakes@gmail.com>");
MODULE_DESCRIPTION("Lorenzo's Eudyptula ID.");
MODULE_LICENSE("GPL");

static const char hello_id[] = "f8cdc7cef553\n";
static const size_t hello_len = 13;

static ssize_t hello_read(struct file *file, char __user *out,
						size_t size, loff_t *off)
{
	return simple_read_from_buffer(out, size, off, hello_id, hello_len);
}

static ssize_t hello_write(struct file *file, const char __user *in,
						size_t size, loff_t *off)
{
	size_t hello_chars_len = hello_len - 1; /* Excluding newline. */
	char buf[hello_chars_len];

	if (size < hello_chars_len)
		goto invalid;

	simple_write_to_buffer(buf, hello_chars_len, off, in, size);
	if (strncmp(buf, hello_id, hello_chars_len))
		goto invalid;

	/* We're a sink. */
	*off = size;
	return size;
 invalid:
	return -EINVAL;
}

static const struct file_operations hello_fops = {
	.owner = THIS_MODULE,
	.read = hello_read,
	.write = hello_write
};

static struct miscdevice hello_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "eudyptula",
	.fops = &hello_fops
};

static int __init hello_init(void)
{
	return misc_register(&hello_dev);
}

static void __exit hello_exit(void)
{
	misc_deregister(&hello_dev);
}

module_init(hello_init);
module_exit(hello_exit);
