#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x9877f1d4, "module_layout" },
	{ 0xc5bc715d, "destroy_workqueue" },
	{ 0xe79f6c30, "flush_workqueue" },
	{ 0x49cd35a2, "queue_work" },
	{ 0x4bc5dd74, "__alloc_workqueue_key" },
	{ 0xd16fe105, "sock_sendmsg" },
	{ 0xd5555f43, "sock_recvmsg" },
	{ 0x37a0cba, "kfree" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x64147857, "sock_release" },
	{ 0x50eedeb8, "printk" },
	{ 0xbb6ad7e2, "kmem_cache_alloc_trace" },
	{ 0x34ce69, "kmalloc_caches" },
	{ 0x95225396, "sock_create" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "08B0909E032223CAE0D61D8");
