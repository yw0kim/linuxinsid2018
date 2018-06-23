#include <linux/module.h>
#include "proc.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Youngwoo Kim");

static int __init foo_init(void)
{
	return foo_proc_init();
}

static void __exit foo_exit(void)
{
	foo_proc_exit();
	return;
}

module_init(foo_init);
module_exit(foo_exit);
