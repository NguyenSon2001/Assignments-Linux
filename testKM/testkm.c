#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int __init test_init(void) {
    printk(KERN_INFO "Test module initialized\n");
    return 0; /* success */
}

static void __exit test_exit(void) {
    printk(KERN_INFO "Test module removed\n");
}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple test module");
MODULE_VERSION("1.0");
