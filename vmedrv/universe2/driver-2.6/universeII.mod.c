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
	{ 0x174b38cd, "module_layout" },
	{ 0x6980fe91, "param_get_int" },
	{ 0xff964b25, "param_set_int" },
	{ 0xadf42bd5, "__request_region" },
	{ 0xb8aa2342, "__check_region" },
	{ 0x37a0cba, "kfree" },
	{ 0x884f145, "kmem_cache_alloc_trace" },
	{ 0xc288f8ce, "malloc_sizes" },
	{ 0xf2a644fb, "copy_from_user" },
	{ 0x973873ab, "_spin_lock" },
	{ 0x1af40e18, "__copy_from_user_ll" },
	{ 0xa1c76e0a, "_cond_resched" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0xd1ac8da2, "remove_proc_entry" },
	{ 0xf20dabd8, "free_irq" },
	{ 0x16305289, "warn_slowpath_null" },
	{ 0x3c061aef, "dma_release_from_coherent" },
	{ 0x8ff4079b, "pv_irq_ops" },
	{ 0xffc7c184, "__init_waitqueue_head" },
	{ 0x6a9f26c9, "init_timer_key" },
	{ 0xc628563f, "create_proc_entry" },
	{ 0xf343a6a9, "__register_chrdev" },
	{ 0x859c6dc7, "request_threaded_irq" },
	{ 0x3af98f9e, "ioremap_nocache" },
	{ 0x5e9929ca, "pci_bus_read_config_dword" },
	{ 0xcb8145d6, "pci_bus_write_config_dword" },
	{ 0x271de510, "pci_find_device" },
	{ 0x59b5a216, "x86_dma_fallback_dev" },
	{ 0x38a02c6e, "dma_alloc_from_coherent" },
	{ 0x6db0168b, "dma_ops" },
	{ 0xdb543a43, "mem_section" },
	{ 0x3c2c5af5, "sprintf" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0x642e54ac, "__wake_up" },
	{ 0x9ccb2622, "finish_wait" },
	{ 0x6307fc98, "del_timer" },
	{ 0x4292364c, "schedule" },
	{ 0x33d92f9a, "prepare_to_wait" },
	{ 0x46085e4f, "add_timer" },
	{ 0xc8b57c27, "autoremove_wake_function" },
	{ 0x7d11c268, "jiffies" },
	{ 0x90cd35f2, "per_cpu__current_task" },
	{ 0x7ba4c424, "remap_pfn_range" },
	{ 0x9bce482f, "__release_region" },
	{ 0x788fe103, "iomem_resource" },
	{ 0xedc03953, "iounmap" },
	{ 0xb72397d5, "printk" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "50BB4544D607CBFE42066D0");

static const struct rheldata _rheldata __used
__attribute__((section(".rheldata"))) = {
	.rhel_major = 6,
	.rhel_minor = 7,
	.rhel_release = 572,
};
