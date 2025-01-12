#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x9931d80, "module_layout" },
	{ 0x474dc328, "param_ops_int" },
	{ 0x7489e048, "cdev_del" },
	{ 0xd12cfed3, "device_destroy" },
	{ 0x4d25408c, "device_create" },
	{ 0xaed86284, "cdev_add" },
	{ 0x1cfbaee8, "cdev_init" },
	{ 0x2f904e10, "class_destroy" },
	{ 0x986e99b, "kmem_cache_alloc_trace" },
	{ 0x6404c07a, "kmalloc_caches" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x85d7b16f, "__class_create" },
	{ 0x92997ed8, "_printk" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xcefb0c9f, "__mutex_init" },
	{ 0x6cbbfc54, "__arch_copy_to_user" },
	{ 0xdcb764ad, "memset" },
	{ 0x37a0cba, "kfree" },
	{ 0x1e49e99f, "cpu_hwcaps" },
	{ 0xcf2a6966, "up" },
	{ 0x12a4e128, "__arch_copy_from_user" },
	{ 0xb788fb30, "gic_pmr_sync" },
	{ 0x31909811, "cpu_hwcap_keys" },
	{ 0x14b89635, "arm64_const_caps_ready" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0xeb233a45, "__kmalloc" },
	{ 0x6bd0e573, "down_interruptible" },
	{ 0x4b0a3f52, "gic_nonsecure_priorities" },
	{ 0x89940875, "mutex_lock_interruptible" },
	{ 0x3213f038, "mutex_unlock" },
	{ 0x4dfa8d4b, "mutex_lock" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "2E4261B4B6D4E609B27E2D6");
