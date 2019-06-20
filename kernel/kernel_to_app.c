// 运行下面用户态的脚本后，路由器的工作模式就被写入msg中，内核空间直接使用msg的值即可
#include <linux/init.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/stat.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

MODULE_AUTHOR("yahai.zhang");
MODULE_DESCRIPTION("procfs get_operation_mode module.");
MODULE_LICENSE("GPL");

#define PROCNAME "operation_mode"
struct proc_dir_entry *proc_entry = NULL;
char msg[16]={0};
static int operation_file_read(struct file * file, char *data, size_t len, loff_t *off)
{
    if(*off > 0)
        return 0;
    if(copy_to_user(data, msg, strlen(msg)))
        return -EFAULT;
    *off += strlen(msg);
    return strlen(msg);
}

static int operation_file_write(struct file *file, const char *data, size_t len, loff_t *off)
{
    memset(msg, 0, sizeof(char)*16);
    if(copy_from_user(msg, (void*)data, len))
        return -EFAULT;
    return len;
}

static struct file_operations operationmode_ops = {
    .read = operation_file_read,
    .write = operation_file_write,
};

static int __init procMode_init(void)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
    if(proc_entry = create_proc_entry(PROCNAME, 0666, NULL))
        proc_entry->proc_fops = &operationmode_ops;
    else
#else
    if(!(proc_entry = proc_create(PROCNAME, 0666, NULL, &operationmode_ops)))
#endif
        printk("!! FAIL to create %s PROC !!\n", PROCNAME);

    return 0;
}

static void __exit procMode_exit(void)
{
    remove_proc_entry(PROCNAME,NULL);
}

module_init(procMode_init);
module_exit(procMode_exit);