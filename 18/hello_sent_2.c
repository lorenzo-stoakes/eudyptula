#include <linux/atomic.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/list.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/wait.h>

MODULE_AUTHOR("Lorenzo Stoakes <lstoakes@gmail.com>");
MODULE_DESCRIPTION("Temporary identity storage");
MODULE_LICENSE("GPL");

struct identity {
	char  name[20];
	int   id;
	bool  busy;

	struct list_head list;
};

static ssize_t hello_write(struct file *file, const char __user *in,
			size_t size, loff_t *off);

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
static DECLARE_WAIT_QUEUE_HEAD(sleep_wait);
static LIST_HEAD(identities);
static DEFINE_MUTEX(identity_lock);
static atomic_t counter = ATOMIC_INIT(0);

static int identity_create(char *name, int id)
{
	struct identity *new;
	int name_len = strlen(name);

	if (name_len > 19)
		return -EINVAL;

	new = kmalloc(sizeof(*new), GFP_KERNEL);
	if (!new)
		return -ENOMEM;

	strncpy(new->name, name, name_len + 1);
	new->id = id;
	new->busy = false;

	mutex_lock(&identity_lock);
	list_add(&new->list, &identities);
	mutex_unlock(&identity_lock);

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
	struct identity *el;

	mutex_lock(&identity_lock);
	el = identity_find(id);
	if (el) {
		list_del(&el->list);
		kfree(el);
	}
	mutex_unlock(&identity_lock);
}

static struct identity *identity_get(void)
{
	struct identity *ret = NULL;

	mutex_lock(&identity_lock);
	if (!list_empty(&identities)) {
		ret = list_last_entry(&identities, struct identity, list);
		list_del(&ret->list);
	}
	mutex_unlock(&identity_lock);

	return ret;
}

static ssize_t hello_write(struct file *file, const char __user *in,
			size_t size, loff_t *off)
{
	int err;
	char buf[20];
	size_t len = min_t(size_t, size, 19);

	simple_write_to_buffer(buf, 19, off, in, len);

	/* Strip newline if present for better log output. */
	if (buf[len - 1] == '\n')
		len--;
	buf[len] = '\0';

	atomic_inc(&counter);
	err = identity_create(buf, atomic_read(&counter));
	if (err)
		return err;

	wake_up(&wee_wait);

	/* We're a sink. */
	*off = size;
	return size;
}

static int hello_thread(void *data)
{
	int err = 0;
	struct identity *id;

	while (!kthread_should_stop()) {
		err = wait_event_interruptible(
			wee_wait, (id = identity_get()) ||
			kthread_should_stop());
		if (err || kthread_should_stop())
			break;

		/* Use a wait queue so we can cancel sleep on module exit. */
		err = wait_event_interruptible_timeout(
			sleep_wait, kthread_should_stop(),
			msecs_to_jiffies(5000));
		if (err)
			break;

		/* Lock to avoid a race with module exit. */
		mutex_lock(&identity_lock);
		if (!kthread_should_stop()) {
			pr_debug("%s: %d\n", id->name, id->id);
			kfree(id);
		}
		mutex_unlock(&identity_lock);
	}

	return IS_ERR_VALUE(err) ? err : 0;
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
	int i;

	misc_deregister(&hello_dev);
	kthread_stop(hello_task);
	wake_up(&sleep_wait);

	/* identity_destroy ignores missing elements so we just consider all
	 * possible ids. O(n^2) but n is small so we can live with it. */
	for (i = 0; i < atomic_read(&counter); i++)
		identity_destroy(i);
}

module_init(hello_init);
module_exit(hello_exit);
