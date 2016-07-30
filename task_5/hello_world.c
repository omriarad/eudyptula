#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/usb.h>
#include <linux/usb/input.h>
#include <linux/hid.h>

static struct usb_device_id keyboard_id_table[] = {
		{ USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID,
		USB_INTERFACE_SUBCLASS_BOOT,
			USB_INTERFACE_PROTOCOL_KEYBOARD) },
	{ }
};

MODULE_DEVICE_TABLE(usb, keyboard_id_table);

static int __init enter_module(void)
{
	pr_debug("Hello World!\n");
	return 0;
}

static void __exit exit_module(void)
{
	pr_debug("Goodbye cruel world!\n");
}

module_init(enter_module);
module_exit(exit_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("[91bc4039c624]");
MODULE_DESCRIPTION("A simple hello world kernel module");
