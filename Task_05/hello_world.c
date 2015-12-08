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

static int keyboard_probe(struct usb_interface *interface,
	const struct usb_device_id *id)
{
	pr_err("Hello World!\n");
	return 0;
}

static void keyboard_disconnect(struct usb_interface *interface)
{
	pr_err("Goodbye cruel world!\n");
}

static struct usb_driver keyboard_driver = {
	.name  =	"hello_world",
	.probe =	keyboard_probe,
	.disconnect =	keyboard_disconnect,
	.id_table =	keyboard_id_table,
};

static int __init enter_module(void)
{
	int retval = 0;

	pr_debug("<%s:%s:%d>\n", __FILE__, __func__, __LINE__);
	retval = usb_register(&keyboard_driver);
	if (retval)
		pr_err("usb_register failed with error %d", retval);
	return retval;
}

static void __exit exit_module(void)
{
	pr_debug("<%s:%s:%d>\n", __FILE__, __func__, __LINE__);
	usb_deregister(&keyboard_driver);
}

module_init(enter_module);
module_exit(exit_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("[91bc4039c624]");
MODULE_DESCRIPTION("A simple hello world kernel module");
