#include <linux/init.h>
#include <linux/module.h>
#include <linux/usb/input.h>
#include <linux/hid.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lorenzo Stoakes <lstoakes@gmail.com>");
MODULE_DESCRIPTION("A test module that outputs hello world.");

static int __init hello_init(void)
{
	pr_debug("Hello World!\n");

	return 0;
}

static void __exit hello_exit(void)
{
	/* Nothing to do :) */
}

module_init(hello_init);
module_exit(hello_exit);

/* Cribbed from drivers/hid/usbkbd.c. Load module on USB keyboard hotplug. */

static struct usb_device_id usb_kbd_id_table[] = {
	{ USB_INTERFACE_INFO(
		USB_INTERFACE_CLASS_HID,
		USB_INTERFACE_SUBCLASS_BOOT,
		USB_INTERFACE_PROTOCOL_KEYBOARD) },
	{ } /* Terminating entry */
};

MODULE_DEVICE_TABLE(usb, usb_kbd_id_table);
