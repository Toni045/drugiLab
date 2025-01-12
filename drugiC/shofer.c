#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include "config.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Autor");
MODULE_DESCRIPTION("Upravljački program za red poruka");
MODULE_VERSION("1.0");

#define DEVICE_NAME "shofer"
#define CLASS_NAME  "msgq"

static int max_msgs = MAX_MSGS;
static int max_msg_size = MAX_MSG_SIZE;
static int max_threads = MAX_THREADS;

module_param(max_msgs, int, S_IRUGO);
MODULE_PARM_DESC(max_msgs, "Maksimalan broj poruka u redu");
module_param(max_msg_size, int, S_IRUGO);
MODULE_PARM_DESC(max_msg_size, "Maksimalna veličina poruke");
module_param(max_threads, int, S_IRUGO);
MODULE_PARM_DESC(max_threads, "Maksimalan broj dretvi u redu");

struct shofer_dev {
    struct cdev cdev;
    struct msg_queue msg_queue;
};

static dev_t dev_no;
static struct class *shofer_class = NULL;
static struct shofer_dev *shofer = NULL;

void msgq_init(struct msg_queue *msg_queue, size_t max_msgs, size_t max_msg_size, size_t max_threads) {
    msg_queue->max_msgs = max_msgs;
    msg_queue->msg_cnt = 0;
    msg_queue->max_msg_size = max_msg_size;
    msg_queue->max_threads = max_threads;
    msg_queue->thread_cnt = 0;
    INIT_LIST_HEAD(&msg_queue->msgs);
    sema_init(&msg_queue->readers, 0);
    sema_init(&msg_queue->writers, max_msgs);
    mutex_init(&msg_queue->lock);
}

void msgq_delete(struct msg_queue *msg_queue) {
    struct msg *msg, *tmp;
    list_for_each_entry_safe(msg, tmp, &msg_queue->msgs, list) {
        list_del(&msg->list);
        kfree(msg);
    }
}

static int shofer_open(struct inode *inode, struct file *filp) {
    struct shofer_dev *dev = container_of(inode->i_cdev, struct shofer_dev, cdev);
    struct msg_queue *msg_queue = &dev->msg_queue;

    if (mutex_lock_interruptible(&msg_queue->lock))
        return -ERESTARTSYS;

    if (msg_queue->thread_cnt >= msg_queue->max_threads) {
        mutex_unlock(&msg_queue->lock);
        return -EBUSY;
    }

    msg_queue->thread_cnt++;
    mutex_unlock(&msg_queue->lock);
    filp->private_data = dev;
    return 0;
}

static int shofer_release(struct inode *inode, struct file *filp) {
    struct shofer_dev *dev = filp->private_data;
    struct msg_queue *msg_queue = &dev->msg_queue;

    mutex_lock(&msg_queue->lock);
    msg_queue->thread_cnt--;
    mutex_unlock(&msg_queue->lock);

    return 0;
}

static ssize_t shofer_read(struct file *filp, char __user *ubuf, size_t count, loff_t *f_pos) {
    struct shofer_dev *dev = filp->private_data;
    struct msg_queue *msg_queue = &dev->msg_queue;
    struct msg *msg;
    ssize_t retval;

    if ((filp->f_flags & O_ACCMODE) != O_RDONLY && (filp->f_flags & O_ACCMODE) != O_RDWR)
        return -EPERM;

    if (count < msg_queue->max_msg_size)
        return -EINVAL;

    if (down_interruptible(&msg_queue->readers))
        return -ERESTARTSYS;

    if (mutex_lock_interruptible(&msg_queue->lock)) {
        up(&msg_queue->readers);
        return -ERESTARTSYS;
    }

    msg = list_first_entry(&msg_queue->msgs, struct msg, list);
    list_del(&msg->list);
    msg_queue->msg_cnt--;
    mutex_unlock(&msg_queue->lock);

    if (copy_to_user(ubuf, msg->d, msg->size)) {
        retval = -EFAULT;
    } else {
        retval = msg->size;
    }

    kfree(msg);
    up(&msg_queue->writers);

    return retval;
}

static ssize_t shofer_write(struct file *filp, const char __user *ubuf, size_t count, loff_t *f_pos) {
    struct shofer_dev *dev = filp->private_data;
    struct msg_queue *msg_queue = &dev->msg_queue;
    struct msg *msg;

    if ((filp->f_flags & O_ACCMODE) != O_WRONLY && (filp->f_flags & O_ACCMODE) != O_RDWR)
        return -EPERM;

    if (count > msg_queue->max_msg_size)
        return -EINVAL;

    if (down_interruptible(&msg_queue->writers))
        return -ERESTARTSYS;

    msg = kmalloc(sizeof(struct msg) + count - 1, GFP_KERNEL);
    if (!msg)
        return -ENOMEM;

    if (copy_from_user(msg->d, ubuf, count)) {
        kfree(msg);
        return -EFAULT;
    }

    msg->size = count;

    if (mutex_lock_interruptible(&msg_queue->lock)) {
        kfree(msg);
        return -ERESTARTSYS;
    }

    list_add_tail(&msg->list, &msg_queue->msgs);
    msg_queue->msg_cnt++;
    mutex_unlock(&msg_queue->lock);
    up(&msg_queue->readers);

    return count;
}

static const struct file_operations shofer_fops = {
    .owner = THIS_MODULE,
    .open = shofer_open,
    .release = shofer_release,
    .read = shofer_read,
    .write = shofer_write,
};

static int __init shofer_init(void) {
    int ret;

    ret = alloc_chrdev_region(&dev_no, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        printk(KERN_ERR "Failed to allocate major number\n");
        return ret;
    }

    shofer_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(shofer_class)) {
        unregister_chrdev_region(dev_no, 1);
        return PTR_ERR(shofer_class);
    }

    shofer = kzalloc(sizeof(struct shofer_dev), GFP_KERNEL);
    if (!shofer) {
        class_destroy(shofer_class);
        unregister_chrdev_region(dev_no, 1);
        return -ENOMEM;
    }

    cdev_init(&shofer->cdev, &shofer_fops);
    shofer->cdev.owner = THIS_MODULE;

    ret = cdev_add(&shofer->cdev, dev_no, 1);
    if (ret < 0) {
        kfree(shofer);
        class_destroy(shofer_class);
        unregister_chrdev_region(dev_no, 1);
        return ret;
    }

    device_create(shofer_class, NULL, dev_no, NULL, DEVICE_NAME);

    msgq_init(&shofer->msg_queue, max_msgs, max_msg_size, max_threads);

    printk(KERN_INFO "Shofer module loaded\n");
    return 0;
}

static void __exit shofer_exit(void) {
    device_destroy(shofer_class, dev_no);
    cdev_del(&shofer->cdev);
    class_destroy(shofer_class);
    unregister_chrdev_region(dev_no, 1);
    msgq_delete(&shofer->msg_queue);
    kfree(shofer);
    printk(KERN_INFO "Shofer module unloaded\n");
}

module_init(shofer_init);
module_exit(shofer_exit);
