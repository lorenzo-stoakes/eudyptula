#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/kthread.h>
#include <linux/wait.h>

MODULE_AUTHOR("Lorenzo Stoakes <lstoakes@gmail.com>");
MODULE_DESCRIPTION("Lorenzo's Eudyptula ID.");
MODULE_LICENSE("GPL");

static ssize_t hello_write(struct file *file, const char __user *in,
			size_t size, loff_t *off);

static const char hello_id[] = "f8cdc7cef553";
static const size_t hello_len = 12;
static const struct file_operations hello_fops = {
	.owner = THIS_MODULE,
	.write = hello_write
};
static struct miscdevice hello_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "eudyptula",
	.fops = &hello_fops,
	.mode = 0222
};
static struct task_struct *hello_task;
static DECLARE_WAIT_QUEUE_HEAD(wee_wait);

static ssize_t hello_write(struct file *file, const char __user *in,
			size_t size, loff_t *off)
{
	char buf[hello_len];

	if (size < hello_len)
		goto invalid;

	simple_write_to_buffer(buf, hello_len, off, in, size);
	if (strncmp(buf, hello_id, hello_len))
		goto invalid;

	/* We're a sink. */
	*off = size;
	return size;
invalid:
	return -EINVAL;
}

static int hello_thread(void *data)
{
	return wait_event_interruptible(wee_wait, kthread_should_stop());
}

static int __init hello_init(void)
{
	hello_task = kthread_run(hello_thread, NULL, "eudyptula");
	if (IS_ERR(hello_task))
		return PTR_ERR(hello_task);

	return misc_register(&hello_dev);
}

static void __exit hello_exit(void)
{
	kthread_stop(hello_task);
	misc_deregister(&hello_dev);
}

module_init(hello_init);
module_exit(hello_exit);
