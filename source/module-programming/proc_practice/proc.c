#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>


static int foo_simple_show(struct seq_file *s, void *unused)
{
	seq_printf(s, "Hello proc!\n");
	return 0;
}

static int foo_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, foo_simple_show, NULL);
}

static const struct file_operations foo_proc_ops = {
	.owner		= THIS_MODULE,
	.open		= foo_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= seq_release,
};

#define FOO_DIR	"foo-dir"
#define FOO_FILE	"foo"

static struct proc_dir_entry *foo_proc_dir = NULL;
static struct proc_dir_entry *foo_proc_file = NULL;

int foo_proc_init(void)
{
	foo_proc_dir = proc_mkdir(FOO_DIR, NULL);
	if (foo_proc_dir == NULL)
	{
		printk(KERN_INFO "Unable to create /proc/%s\n", FOO_DIR);
		return -1;
	}

	foo_proc_file = proc_create(FOO_FILE, 0, foo_proc_dir, &foo_proc_ops); /* S_IRUGO */
	if (foo_proc_file == NULL)
	{
		printk("Unable to create /proc/%s/%s\n", FOO_DIR, FOO_FILE);
		remove_proc_entry(FOO_DIR, NULL);
		return -1;
	}

	printk(KERN_INFO "Created /proc/%s/%s\n", FOO_DIR, FOO_FILE);
	return 0;
}

void foo_proc_exit(void)
{
	remove_proc_subtree(FOO_DIR, NULL);
	/* remove proc_dir_entry instance */
	proc_remove(foo_proc_file);
	proc_remove(foo_proc_dir);
	printk(KERN_INFO "Removed /proc/%s/%s\n", FOO_DIR, FOO_FILE);

}
