
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

int __init enter_module(void)
{
	pr_debug("Hello World!\n");
	return 0;
}

void __exit exit_module(void)
{
	pr_debug("Goodbye cruel world!\n");
}

module_init(enter_module);
module_exit(exit_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("[91bc4039c624]");
MODULE_DESCRIPTION("A simple hello world kernel module");
