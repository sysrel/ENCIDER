; ModuleID = '/home/tuba/Documents/tools/clang-kernel-build/linux-stable/drivers/media/usb/usbtv/usbtv-core.bc'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.usb_driver = type { i8*, i32 (%struct.usb_interface*, %struct.usb_device_id*)*, void (%struct.usb_interface*)*, 
i32 (%struct.usb_interface*, i32, i8*)*, i32 (%struct.usb_interface*, i32)*, i32 (%struct.usb_interface*)*, i32 
(%struct.usb_interface*)*, i32 (%struct.usb_interface*)*, i32 (%struct.usb_interface*)*, %struct.usb_device_id*, 
%struct.usb_dynids, %struct.usbdrv_wrap, i8 } %struct.usb_interface = type { %struct.usb_host_interface*, 
%struct.usb_host_interface*, i32, %struct.usb_interface_assoc_descriptor*, i32, i32, i8, %struct.device, 
%struct.device*, %struct.atomic_t, %struct.work_struct } %struct.usb_host_interface = type { 
%struct.usb_interface_descriptor, i32, i8*, %struct.usb_host_endpoint*, i8* } %struct.usb_interface_descriptor = type { 
i8, i8, i8, i8, i8, i8, i8, i8, i8 } %struct.usb_host_endpoint = type <{ %struct.usb_endpoint_descriptor, 
%struct.usb_ss_ep_comp_descriptor, %struct.usb_ssp_isoc_ep_comp_descriptor, i8, %struct.list_head, i8*, 
%struct.ep_device*, i8*, i32, i32, i32, [4 x i8] }> %struct.usb_endpoint_descriptor = type <{ i8, i8, i8, i8, i16, i8, 
i8, i8 }> %struct.usb_ss_ep_comp_descriptor = type { i8, i8, i8, i8, i16 } %struct.usb_ssp_isoc_ep_comp_descriptor = 
type { i8, i8, i16, i32 } %struct.list_head = type { %struct.list_head*, %struct.list_head* } %struct.ep_device = type 
opaque %struct.usb_interface_assoc_descriptor = type { i8, i8, i8, i8, i8, i8, i8, i8 } %struct.device = type { 
%struct.device*, %struct.device_private*, %struct.kobject, i8*, %struct.device_type*, %struct.mutex, %struct.bus_type*, 
%struct.device_driver*, i8*, i8*, %struct.dev_links_info, %struct.dev_pm_info, %struct.dev_pm_domain*, 
%struct.irq_domain*, %struct.list_head, i32, %struct.dma_map_ops*, i64*, i64, i64, %struct.device_dma_parameters*, 
%struct.list_head, %struct.dma_coherent_mem*, %struct.dev_archdata, %struct.device_node*, %struct.fwnode_handle*, i32, 
i32, %struct.spinlock, %struct.list_head, %struct.klist_node, %struct.class*, %struct.attribute_group**, void 
(%struct.device*)*, %struct.iommu_group*, %struct.iommu_fwspec*, i8 } %struct.device_private = type opaque 
%struct.kobject = type { i8*, %struct.list_head, %struct.kobject*, %struct.kset*, %struct.kobj_type*, 
%struct.kernfs_node*, %struct.kref, i8 } %struct.kset = type { %struct.list_head, %struct.spinlock, %struct.kobject, 
%struct.kset_uevent_ops* } %struct.kset_uevent_ops = type { i32 (%struct.kset*, %struct.kobject*)*, i8* (%struct.kset*, 
%struct.kobject*)*, i32 (%struct.kset*, %struct.kobject*, %struct.kobj_uevent_env*)* } %struct.kobj_uevent_env = type { 
[3 x i8*], [32 x i8*], i32, [2048 x i8], i32 } %struct.kobj_type = type { void (%struct.kobject*)*, %struct.sysfs_ops*, 
%struct.attribute**, %struct.kobj_ns_type_operations* (%struct.kobject*)*, i8* (%struct.kobject*)* } %struct.sysfs_ops 
= type { i64 (%struct.kobject*, %struct.attribute*, i8*)*, i64 (%struct.kobject*, %struct.attribute*, i8*, i64)* } 
%struct.attribute = type { i8*, i16 } %struct.kobj_ns_type_operations = type { i32, i1 ()*, i8* ()*, i8* 
(%struct.sock*)*, i8* ()*, void (i8*)* } %struct.sock = type opaque %struct.kernfs_node = type { %struct.atomic_t, 
%struct.atomic_t, %struct.kernfs_node*, i8*, %struct.rb_node, i8*, i32, %union.anon, i8*, i16, i16, i32, 
%struct.kernfs_iattrs* } %struct.rb_node = type { i64, %struct.rb_node*, %struct.rb_node* } %union.anon = type { 
%struct.kernfs_elem_attr } %struct.kernfs_elem_attr = type { %struct.kernfs_ops*, %struct.kernfs_open_node*, i64, 
%struct.kernfs_node* } %struct.kernfs_ops = type { i32 (%struct.kernfs_open_file*)*, void (%struct.kernfs_open_file*)*, 
i32 (%struct.seq_file*, i8*)*, i8* (%struct.seq_file*, i64*)*, i8* (%struct.seq_file*, i8*, i64*)*, void 
(%struct.seq_file*, i8*)*, i64 (%struct.kernfs_open_file*, i8*, i64, i64)*, i64, i8, i64 (%struct.kernfs_open_file*, 
i8*, i64, i64)*, i32 (%struct.kernfs_open_file*, %struct.vm_area_struct*)* } %struct.kernfs_open_file = type { 
%struct.kernfs_node*, %struct.file*, %struct.seq_file*, i8*, %struct.mutex, %struct.mutex, i32, %struct.list_head, i8*, 
i64, i8, %struct.vm_operations_struct* } %struct.file = type { %union.anon.2, %struct.path, %struct.inode*, 
%struct.file_operations*, %struct.spinlock, %struct.atomic64_t, i32, i32, %struct.mutex, i64, %struct.fown_struct, 
%struct.cred*, %struct.file_ra_state, i64, i8*, i8*, %struct.list_head, %struct.list_head, %struct.address_space* } 
%union.anon.2 = type { %struct.callback_head } %struct.callback_head = type { %struct.callback_head*, void 
(%struct.callback_head*)* } %struct.path = type { %struct.vfsmount*, %struct.dentry* } %struct.vfsmount = type opaque 
%struct.dentry = type { i32, %struct.seqcount, %struct.hlist_bl_node, %struct.dentry*, %struct.qstr, %struct.inode*, 
[32 x i8], %struct.lockref, %struct.dentry_operations*, %struct.super_block*, i64, i8*, %union.anon.44, 
%struct.list_head, %struct.list_head, %union.anon.45 } %struct.seqcount = type { i32 } %struct.hlist_bl_node = type { 
%struct.hlist_bl_node*, %struct.hlist_bl_node** } %struct.qstr = type { %union.anon.3, i8* } %union.anon.3 = type { i64 
} %struct.lockref = type { %union.anon.42 } %union.anon.42 = type { i64 } %struct.dentry_operations = type { i32 
(%struct.dentry*, i32)*, i32 (%struct.dentry*, i32)*, i32 (%struct.dentry*, %struct.qstr*)*, i32 (%struct.dentry*, i32, 
i8*, %struct.qstr*)*, i32 (%struct.dentry*)*, i32 (%struct.dentry*)*, void (%struct.dentry*)*, void (%struct.dentry*)*, 
void (%struct.dentry*, %struct.inode*)*, i8* (%struct.dentry*, i8*, i32)*, %struct.vfsmount* (%struct.path*)*, i32 
(%struct.path*, i1)*, %struct.dentry* (%struct.dentry*, %struct.inode*, i32)*, [24 x i8] } %struct.super_block = type { 
%struct.list_head, i32, i8, i64, i64, %struct.file_system_type*, %struct.super_operations*, %struct.dquot_operations*, 
%struct.quotactl_ops*, %struct.export_operations*, i64, i64, i64, %struct.dentry*, %struct.rw_semaphore, i32, 
%struct.atomic_t, i8*, %struct.xattr_handler**, %struct.fscrypt_operations*, %struct.hlist_bl_head, %struct.list_head, 
%struct.block_device*, %struct.backing_dev_info*, %struct.mtd_info*, %struct.hlist_node, i32, %struct.quota_info, 
%struct.sb_writers, [32 x i8], [16 x i8], i8*, i32, i32, i32, %struct.mutex, i8*, i8*, %struct.dentry_operations*, i32, 
%struct.shrinker, %struct.atomic64_t, i32, %struct.workqueue_struct*, %struct.hlist_head, %struct.user_namespace*, 
%struct.list_lru, [56 x i8], %struct.list_lru, %struct.callback_head, %struct.work_struct, %struct.mutex, i32, [36 x 
i8], %struct.spinlock, %struct.list_head, %struct.spinlock, %struct.list_head, [16 x i8] } %struct.file_system_type = 
type { i8*, i32, %struct.dentry* (%struct.file_system_type*, i32, i8*, i8*)*, void (%struct.super_block*)*, 
%struct.module*, %struct.file_system_type*, %struct.hlist_head, %struct.lock_class_key, %struct.lock_class_key, 
%struct.lock_class_key, [3 x %struct.lock_class_key], %struct.lock_class_key, %struct.lock_class_key, 
%struct.lock_class_key } %struct.module = type { i32, %struct.list_head, [56 x i8], %struct.module_kobject, 
%struct.module_attribute*, i8*, i8*, %struct.kobject*, %struct.kernel_symbol*, i32*, i32, %struct.mutex, 
%struct.kernel_param*, i32, i32, %struct.kernel_symbol*, i32*, i8, %struct.kernel_symbol*, i32*, i32, i32, 
%struct.exception_table_entry*, i32 ()*, [40 x i8], %struct.module_layout, %struct.module_layout, 
%struct.mod_arch_specific, i64, i32, %struct.list_head, %struct.bug_entry*, %struct.mod_kallsyms*, 
%struct.mod_kallsyms, %struct.module_sect_attrs*, %struct.module_notes_attrs*, i8*, i8*, i32, i32, 
%struct.tracepoint**, i32, i8**, %struct.trace_event_call**, i32, %struct.trace_enum_map**, i32, %struct.list_head, 
%struct.list_head, void ()*, %struct.atomic_t, [12 x i8] } %struct.module_kobject = type { %struct.kobject, 
%struct.module*, %struct.kobject*, %struct.module_param_attrs*, %struct.completion* } %struct.module_param_attrs = type 
opaque %struct.completion = type { i32, %struct.__wait_queue_head } %struct.__wait_queue_head = type { 
%struct.spinlock, %struct.list_head } %struct.module_attribute = type { %struct.attribute, i64 
(%struct.module_attribute*, %struct.module_kobject*, i8*)*, i64 (%struct.module_attribute*, %struct.module_kobject*, 
i8*, i64)*, void (%struct.module*, i8*)*, i32 (%struct.module*)*, void (%struct.module*)* } %struct.kernel_param = type 
{ i8*, %struct.module*, %struct.kernel_param_ops*, i16, i8, i8, %union.anon.64 } %struct.kernel_param_ops = type { i32, 
i32 (i8*, %struct.kernel_param*)*, i32 (i8*, %struct.kernel_param*)*, void (i8*)* } %union.anon.64 = type { i8* } 
%struct.kernel_symbol = type { i64, i8* } %struct.exception_table_entry = type { i32, i32, i32 } %struct.module_layout 
= type { i8*, i32, i32, i32, i32, %struct.mod_tree_node } %struct.mod_tree_node = type { %struct.module*, 
%struct.latch_tree_node } %struct.latch_tree_node = type { [2 x %struct.rb_node] } %struct.mod_arch_specific = type {} 
%struct.bug_entry = type { i32, i32, i16, i16 } %struct.mod_kallsyms = type { %struct.elf64_sym*, i32, i8* } 
%struct.elf64_sym = type { i32, i8, i8, i16, i64, i64 } %struct.module_sect_attrs = type opaque 
%struct.module_notes_attrs = type opaque %struct.tracepoint = type { i8*, %struct.static_key, i32 ()*, void ()*, 
%struct.tracepoint_func* } %struct.static_key = type { %struct.atomic_t } %struct.tracepoint_func = type { i8*, i8*, 
i32 } %struct.trace_event_call = type opaque %struct.trace_enum_map = type opaque %struct.lock_class_key = type {} 
%struct.super_operations = type { %struct.inode* (%struct.super_block*)*, void (%struct.inode*)*, void (%struct.inode*, 
i32)*, i32 (%struct.inode*, %struct.writeback_control*)*, i32 (%struct.inode*)*, void (%struct.inode*)*, void 
(%struct.super_block*)*, i32 (%struct.super_block*, i32)*, i32 (%struct.super_block*)*, i32 (%struct.super_block*)*, 
i32 (%struct.super_block*)*, i32 (%struct.super_block*)*, i32 (%struct.dentry*, %struct.kstatfs*)*, i32 
(%struct.super_block*, i32*, i8*)*, void (%struct.super_block*)*, i32 (%struct.seq_file*, %struct.dentry*)*, i32 
(%struct.seq_file*, %struct.dentry*)*, i32 (%struct.seq_file*, %struct.dentry*)*, i32 (%struct.seq_file*, 
%struct.dentry*)*, i64 (%struct.super_block*, i32, i8*, i64, i64)*, i64 (%struct.super_block*, i32, i8*, i64, i64)*, 
%struct.dquot** (%struct.inode*)*, i32 (%struct.super_block*, %struct.page*, i32)*, i64 (%struct.super_block*, 
%struct.shrink_control*)*, i64 (%struct.super_block*, %struct.shrink_control*)* } %struct.writeback_control = type 
opaque %struct.kstatfs = type opaque %struct.dquot = type { %struct.hlist_node, %struct.list_head, %struct.list_head, 
%struct.list_head, %struct.mutex, %struct.atomic_t, %struct.__wait_queue_head, %struct.super_block*, %struct.kqid, i64, 
i64, %struct.mem_dqblk } %struct.kqid = type { %union.anon.4, i32 } %union.anon.4 = type { %struct.kuid_t } 
%struct.kuid_t = type { i32 } %struct.mem_dqblk = type { i64, i64, i64, i64, i64, i64, i64, i64, i64 } %struct.page = 
type { i64, %union.anon.5, %union.anon.28, %union.anon.29, %union.anon.33, %union.anon.36, [8 x i8] } %union.anon.5 = 
type { %struct.address_space* } %union.anon.28 = type { i64 } %union.anon.29 = type { i64 } %union.anon.33 = type { 
%struct.list_head } %union.anon.36 = type { i64 } %struct.shrink_control = type { i32, i64, i32, %struct.mem_cgroup* } 
%struct.mem_cgroup = type opaque %struct.dquot_operations = type { i32 (%struct.dquot*)*, %struct.dquot* 
(%struct.super_block*, i32)*, void (%struct.dquot*)*, i32 (%struct.dquot*)*, i32 (%struct.dquot*)*, i32 
(%struct.dquot*)*, i32 (%struct.super_block*, i32)*, i64* (%struct.inode*)*, i32 (%struct.inode*, %struct.kprojid_t*)*, 
i32 (%struct.super_block*, %struct.kqid*)* } %struct.kprojid_t = type { i32 } %struct.quotactl_ops = type { i32 
(%struct.super_block*, i32, i32, %struct.path*)*, i32 (%struct.super_block*, i32)*, i32 (%struct.super_block*, i32)*, 
i32 (%struct.super_block*, i32)*, i32 (%struct.super_block*, i32)*, i32 (%struct.super_block*, i32, %struct.qc_info*)*, 
i32 (%struct.super_block*, i64, %struct.qc_dqblk*)*, i32 (%struct.super_block*, %struct.kqid*, %struct.qc_dqblk*)*, i32 
(%struct.super_block*, i64, %struct.qc_dqblk*)*, i32 (%struct.super_block*, %struct.qc_state*)*, i32 
(%struct.super_block*, i32)* } %struct.qc_info = type { i32, i32, i32, i32, i32, i32, i32, i32 } %struct.qc_dqblk = 
type { i32, i64, i64, i64, i64, i64, i64, i64, i64, i32, i32, i64, i64, i64, i64, i32 } %struct.qc_state = type { i32, 
[3 x %struct.qc_type_state] } %struct.qc_type_state = type { i32, i32, i32, i32, i32, i32, i32, i64, i64, i64 } 
%struct.export_operations = type opaque %struct.rw_semaphore = type { %struct.atomic64_t, %struct.list_head, 
%struct.raw_spinlock, %struct.optimistic_spin_queue, %struct.task_struct* } %struct.raw_spinlock = type { 
%struct.qspinlock } %struct.qspinlock = type { %struct.atomic_t } %struct.optimistic_spin_queue = type { 
%struct.atomic_t } %struct.task_struct = type { %struct.thread_info, i64, i8*, %struct.atomic_t, i32, i32, 
%struct.llist_node, i32, i32, i32, i64, %struct.task_struct*, i32, i32, i32, i32, i32, i32, %struct.sched_class*, [16 x 
i8], %struct.sched_entity, %struct.sched_rt_entity, %struct.task_group*, %struct.sched_dl_entity, i32, i32, i32, 
%struct.cpumask, %struct.sched_info, %struct.list_head, %struct.plist_node, %struct.rb_node, %struct.mm_struct*, 
%struct.mm_struct*, %struct.vmacache, %struct.task_rss_stat, i32, i32, i32, i32, i64, i32, i8, [3 x i8], i8, i64, 
%struct.restart_block, i32, i32, %struct.task_struct*, %struct.task_struct*, %struct.list_head, %struct.list_head, 
%struct.task_struct*, %struct.list_head, %struct.list_head, [3 x %struct.pid_link], %struct.list_head, 
%struct.list_head, %struct.completion*, i32*, i32*, i64, i64, i64, %struct.prev_cputime, i64, i64, i64, i64, i64, i64, 
%struct.task_cputime, [3 x %struct.list_head], %struct.cred*, %struct.cred*, %struct.cred*, [16 x i8], 
%struct.nameidata*, %struct.sysv_sem, %struct.sysv_shm, %struct.fs_struct*, %struct.files_struct*, %struct.nsproxy*, 
%struct.signal_struct*, %struct.sighand_struct*, %struct.sigset_t, %struct.sigset_t, %struct.sigset_t, 
%struct.sigpending, i64, i64, i32, %struct.callback_head*, %struct.audit_context*, %struct.kuid_t, i32, 
%struct.seccomp, i32, i32, %struct.spinlock, %struct.raw_spinlock, %struct.wake_q_node, %struct.rb_root, 
%struct.rb_node*, %struct.rt_mutex_waiter*, i8*, %struct.bio_list*, %struct.blk_plug*, %struct.reclaim_state*, 
%struct.backing_dev_info*, %struct.io_context*, i64, %struct.siginfo*, %struct.task_io_accounting, i64, i64, i64, 
%struct.nodemask_t, %struct.seqcount, i32, i32, %struct.css_set*, %struct.list_head, %struct.robust_list_head*, 
%struct.compat_robust_list_head*, %struct.list_head, %struct.futex_pi_state*, [2 x %struct.perf_event_context*], 
%struct.mutex, %struct.list_head, %struct.mempolicy*, i16, i16, %struct.tlbflush_unmap_batch, %struct.callback_head, 
%struct.pipe_inode_info*, %struct.page_frag, %struct.task_delay_info*, i32, i32, i64, i64, i64, i64, i64, i32, 
%struct.task_struct*, %struct.vm_struct*, %struct.atomic_t, [28 x i8], %struct.thread_struct } %struct.thread_info = 
type { i64 } %struct.llist_node = type { %struct.llist_node* } %struct.sched_class = type opaque %struct.sched_entity = 
type { %struct.load_weight, %struct.rb_node, %struct.list_head, i32, i64, i64, i64, i64, i64, %struct.sched_statistics, 
i32, %struct.sched_entity*, %struct.cfs_rq*, %struct.cfs_rq*, [32 x i8], %struct.sched_avg, [24 x i8] } 
%struct.load_weight = type { i64, i32 } %struct.sched_statistics = type { i64, i64, i64, i64, i64, i64, i64, i64, i64, 
i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64 } %struct.cfs_rq = type opaque 
%struct.sched_avg = type { i64, i64, i32, i32, i64, i64 } %struct.sched_rt_entity = type { %struct.list_head, i64, i64, 
i32, i16, i16, %struct.sched_rt_entity* } %struct.task_group = type opaque %struct.sched_dl_entity = type { 
%struct.rb_node, i64, i64, i64, i64, i64, i64, i32, i32, i32, i32, %struct.hrtimer } %struct.hrtimer = type { 
%struct.timerqueue_node, i64, i32 (%struct.hrtimer*)*, %struct.hrtimer_clock_base*, i8, i8 } %struct.timerqueue_node = 
type { %struct.rb_node, i64 } %struct.hrtimer_clock_base = type { %struct.hrtimer_cpu_base*, i32, i32, 
%struct.timerqueue_head, i64 ()*, i64, [16 x i8] } %struct.hrtimer_cpu_base = type { %struct.raw_spinlock, 
%struct.seqcount, %struct.hrtimer*, i32, i32, i32, i8, i8, i8, i64, %struct.hrtimer*, i32, i32, i32, i32, [4 x 
%struct.hrtimer_clock_base] } %struct.timerqueue_head = type { %struct.rb_root, %struct.timerqueue_node* } 
%struct.cpumask = type { [1 x i64] } %struct.sched_info = type { i64, i64, i64, i64 } %struct.plist_node = type { i32, 
%struct.list_head, %struct.list_head } %struct.mm_struct = type { %struct.vm_area_struct*, %struct.rb_root, i32, i64 
(%struct.file*, i64, i64, i64, i64)*, i64, i64, i64, i64, %struct.pgd_t*, %struct.atomic_t, %struct.atomic_t, 
%struct.atomic64_t, %struct.atomic64_t, i32, %struct.spinlock, %struct.rw_semaphore, %struct.list_head, i64, i64, i64, 
i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, [46 x i64], %struct.mm_rss_stat, 
%struct.linux_binfmt*, [1 x %struct.cpumask], %struct.mm_context_t, i64, %struct.core_state*, %struct.spinlock, 
%struct.kioctx_table*, %struct.user_namespace*, %struct.file*, %struct.mmu_notifier_mm*, i8, %struct.uprobes_state, 
%struct.atomic64_t, %struct.work_struct } %struct.vm_area_struct = type { i64, i64, %struct.vm_area_struct*, 
%struct.vm_area_struct*, %struct.rb_node, i64, %struct.mm_struct*, %struct.pgprot, i64, %struct.anon.6, 
%struct.list_head, %struct.anon_vma*, %struct.vm_operations_struct*, i64, %struct.file*, i8*, %struct.mempolicy*, 
%struct.vm_userfaultfd_ctx } %struct.pgprot = type { i64 } %struct.anon.6 = type { %struct.rb_node, i64 } 
%struct.anon_vma = type opaque %struct.vm_userfaultfd_ctx = type {} %struct.pgd_t = type { i64 } %struct.mm_rss_stat = 
type { [4 x %struct.atomic64_t] } %struct.linux_binfmt = type opaque %struct.mm_context_t = type { %struct.ldt_struct*, 
i16, %struct.mutex, i8*, %struct.vdso_image*, %struct.atomic_t, i16, i16 } %struct.ldt_struct = type opaque 
%struct.vdso_image = type { i8*, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64 } %struct.core_state = type 
{ %struct.atomic_t, %struct.core_thread, %struct.completion } %struct.core_thread = type { %struct.task_struct*, 
%struct.core_thread* } %struct.kioctx_table = type opaque %struct.mmu_notifier_mm = type opaque %struct.uprobes_state = 
type {} %struct.vmacache = type { i32, [4 x %struct.vm_area_struct*] } %struct.task_rss_stat = type { i32, [4 x i32] } 
%struct.restart_block = type { i64 (%struct.restart_block*)*, %union.anon.7 } %union.anon.7 = type { %struct.anon.8 } 
%struct.anon.8 = type { i32*, i32, i32, i32, i64, i32* } %struct.pid_link = type { %struct.hlist_node, %struct.pid* } 
%struct.pid = type { %struct.atomic_t, i32, [3 x %struct.hlist_head], %struct.callback_head, [1 x %struct.upid] } 
%struct.upid = type { i32, %struct.pid_namespace*, %struct.hlist_node } %struct.pid_namespace = type opaque 
%struct.prev_cputime = type { i64, i64, %struct.raw_spinlock } %struct.task_cputime = type { i64, i64, i64 } 
%struct.nameidata = type opaque %struct.sysv_sem = type { %struct.sem_undo_list* } %struct.sem_undo_list = type opaque 
%struct.sysv_shm = type { %struct.list_head } %struct.fs_struct = type opaque %struct.files_struct = type opaque 
%struct.nsproxy = type opaque %struct.signal_struct = type opaque %struct.sighand_struct = type opaque %struct.sigset_t 
= type { [1 x i64] } %struct.sigpending = type { %struct.list_head, %struct.sigset_t } %struct.audit_context = type 
opaque %struct.seccomp = type { i32, %struct.seccomp_filter* } %struct.seccomp_filter = type opaque %struct.wake_q_node 
= type { %struct.wake_q_node* } %struct.rb_root = type { %struct.rb_node* } %struct.rt_mutex_waiter = type opaque 
%struct.bio_list = type opaque %struct.blk_plug = type opaque %struct.reclaim_state = type opaque %struct.io_context = 
type opaque %struct.siginfo = type { i32, i32, i32, %union.anon.11 } %union.anon.11 = type { %struct.anon.15, [80 x i8] 
} %struct.anon.15 = type { i32, i32, i32, i64, i64 } %struct.task_io_accounting = type { i64, i64, i64, i64, i64, i64, 
i64 } %struct.nodemask_t = type { [1 x i64] } %struct.css_set = type opaque %struct.robust_list_head = type opaque 
%struct.compat_robust_list_head = type opaque %struct.futex_pi_state = type opaque %struct.perf_event_context = type 
opaque %struct.mempolicy = type opaque %struct.tlbflush_unmap_batch = type { %struct.cpumask, i8, i8 } 
%struct.pipe_inode_info = type opaque %struct.page_frag = type { %struct.page*, i32, i32 } %struct.task_delay_info = 
type opaque %struct.vm_struct = type opaque %struct.thread_struct = type { [3 x %struct.desc_struct], i64, i64, i16, 
i16, i16, i16, i32, i64, i64, [4 x %struct.perf_event*], i64, i64, i64, i64, i64, i64*, i64, i32, %struct.mm_segment_t, 
i8, [15 x i8], %struct.fpu } %struct.desc_struct = type { %union.anon.21 } %union.anon.21 = type { %struct.anon.22 } 
%struct.anon.22 = type { i32, i32 } %struct.perf_event = type opaque %struct.mm_segment_t = type { i64 } %struct.fpu = 
type { i32, i8, i8, [58 x i8], %union.fpregs_state } %union.fpregs_state = type { %struct.xregs_state, [3520 x i8] } 
%struct.xregs_state = type { %struct.fxregs_state, %struct.xstate_header, [0 x i8] } %struct.fxregs_state = type { i16, 
i16, i16, i16, %union.anon.24, i32, i32, [32 x i32], [64 x i32], [12 x i32], %union.anon.27 } %union.anon.24 = type { 
%struct.anon.25 } %struct.anon.25 = type { i64, i64 } %union.anon.27 = type { [12 x i32] } %struct.xstate_header = type 
{ i64, i64, [6 x i64] } %struct.xattr_handler = type opaque %struct.fscrypt_operations = type opaque 
%struct.hlist_bl_head = type { %struct.hlist_bl_node* } %struct.block_device = type { i32, i32, %struct.inode*, 
%struct.super_block*, %struct.mutex, i8*, i8*, i32, i8, %struct.list_head, %struct.block_device*, i32, 
%struct.hd_struct*, i32, i32, %struct.gendisk*, %struct.request_queue*, %struct.backing_dev_info*, %struct.list_head, 
i64, i32, %struct.mutex } %struct.hd_struct = type opaque %struct.gendisk = type opaque %struct.request_queue = type 
opaque %struct.backing_dev_info = type opaque %struct.mtd_info = type opaque %struct.hlist_node = type { 
%struct.hlist_node*, %struct.hlist_node** } %struct.quota_info = type { i32, %struct.mutex, [3 x %struct.inode*], [3 x 
%struct.mem_dqinfo], [3 x %struct.quota_format_ops*] } %struct.mem_dqinfo = type { %struct.quota_format_type*, i32, 
%struct.list_head, i64, i32, i32, i64, i64, i8* } %struct.quota_format_type = type { i32, %struct.quota_format_ops*, 
%struct.module*, %struct.quota_format_type* } %struct.quota_format_ops = type { i32 (%struct.super_block*, i32)*, i32 
(%struct.super_block*, i32)*, i32 (%struct.super_block*, i32)*, i32 (%struct.super_block*, i32)*, i32 
(%struct.dquot*)*, i32 (%struct.dquot*)*, i32 (%struct.dquot*)*, i32 (%struct.super_block*, %struct.kqid*)* } 
%struct.sb_writers = type { i32, %struct.__wait_queue_head, [3 x %struct.percpu_rw_semaphore] } 
%struct.percpu_rw_semaphore = type { %struct.rcu_sync, i32*, %struct.rw_semaphore, %struct.rcuwait, i32 } 
%struct.rcu_sync = type { i32, i32, %struct.__wait_queue_head, i32, %struct.callback_head, i32 } %struct.rcuwait = type 
{ %struct.task_struct* } %struct.shrinker = type { i64 (%struct.shrinker*, %struct.shrink_control*)*, i64 
(%struct.shrinker*, %struct.shrink_control*)*, i32, i64, i64, %struct.list_head, %struct.atomic64_t* } 
%struct.workqueue_struct = type opaque %struct.hlist_head = type { %struct.hlist_node* } %struct.user_namespace = type 
opaque %struct.list_lru = type { %struct.list_lru_node* } %struct.list_lru_node = type { %struct.spinlock, 
%struct.list_lru_one, [32 x i8] } %struct.list_lru_one = type { %struct.list_head, i64 } %union.anon.44 = type { 
%struct.list_head } %union.anon.45 = type { %struct.hlist_node } %struct.inode = type { i16, i16, %struct.kuid_t, 
%struct.kgid_t, i32, %struct.posix_acl*, %struct.posix_acl*, %struct.inode_operations*, %struct.super_block*, 
%struct.address_space*, i8*, i64, %union.anon.37, i32, i64, %struct.timespec, %struct.timespec, %struct.timespec, 
%struct.spinlock, i16, i32, i64, i64, %struct.rw_semaphore, i64, i64, %struct.hlist_node, %struct.list_head, 
%struct.list_head, %struct.list_head, %struct.list_head, %union.anon.38, i64, %struct.atomic_t, %struct.atomic_t, 
%struct.atomic_t, %struct.file_operations*, %struct.file_lock_context*, %struct.address_space, %struct.list_head, 
%union.anon.41, i32, i32, %struct.hlist_head, i8* } %struct.kgid_t = type { i32 } %struct.posix_acl = type opaque 
%struct.inode_operations = type { %struct.dentry* (%struct.inode*, %struct.dentry*, i32)*, i8* (%struct.dentry*, 
%struct.inode*, %struct.delayed_call*)*, i32 (%struct.inode*, i32)*, %struct.posix_acl* (%struct.inode*, i32)*, i32 
(%struct.dentry*, i8*, i32)*, i32 (%struct.inode*, %struct.dentry*, i16, i1)*, i32 (%struct.dentry*, %struct.inode*, 
%struct.dentry*)*, i32 (%struct.inode*, %struct.dentry*)*, i32 (%struct.inode*, %struct.dentry*, i8*)*, i32 
(%struct.inode*, %struct.dentry*, i16)*, i32 (%struct.inode*, %struct.dentry*)*, i32 (%struct.inode*, %struct.dentry*, 
i16, i32)*, i32 (%struct.inode*, %struct.dentry*, %struct.inode*, %struct.dentry*, i32)*, i32 (%struct.dentry*, 
%struct.iattr*)*, i32 (%struct.path*, %struct.kstat*, i32, i32)*, i64 (%struct.dentry*, i8*, i64)*, i32 
(%struct.inode*, %struct.fiemap_extent_info*, i64, i64)*, i32 (%struct.inode*, %struct.timespec*, i32)*, i32 
(%struct.inode*, %struct.dentry*, %struct.file*, i32, i16, i32*)*, i32 (%struct.inode*, %struct.dentry*, i16)*, i32 
(%struct.inode*, %struct.posix_acl*, i32)*, [24 x i8] } %struct.delayed_call = type { void (i8*)*, i8* } %struct.iattr 
= type { i32, i16, %struct.kuid_t, %struct.kgid_t, i64, %struct.timespec, %struct.timespec, %struct.timespec, 
%struct.file* } %struct.kstat = type { i32, i16, i32, i32, i64, i64, i32, i32, %struct.kuid_t, %struct.kgid_t, i64, 
%struct.timespec, %struct.timespec, %struct.timespec, %struct.timespec, i64 } %struct.fiemap_extent_info = type { i32, 
i32, i32, %struct.fiemap_extent* } %struct.fiemap_extent = type { i64, i64, i64, [2 x i64], i32, [3 x i32] } 
%union.anon.37 = type { i32 } %struct.timespec = type { i64, i64 } %union.anon.38 = type { %struct.callback_head } 
%struct.file_lock_context = type { %struct.spinlock, %struct.list_head, %struct.list_head, %struct.list_head } 
%struct.address_space = type { %struct.inode*, %struct.radix_tree_root, %struct.spinlock, %struct.atomic_t, 
%struct.rb_root, %struct.rw_semaphore, i64, i64, i64, %struct.address_space_operations*, i64, %struct.spinlock, i32, 
%struct.list_head, i8* } %struct.radix_tree_root = type { i32, %struct.radix_tree_node* } %struct.radix_tree_node = 
type { i8, i8, i8, i8, %struct.radix_tree_node*, %struct.radix_tree_root*, %union.anon.0, [64 x i8*], [3 x [1 x i64]] } 
%union.anon.0 = type { %struct.list_head } %struct.address_space_operations = type { i32 (%struct.page*, 
%struct.writeback_control*)*, i32 (%struct.file*, %struct.page*)*, i32 (%struct.address_space*, 
%struct.writeback_control*)*, i32 (%struct.page*)*, i32 (%struct.file*, %struct.address_space*, %struct.list_head*, 
i32)*, i32 (%struct.file*, %struct.address_space*, i64, i32, i32, %struct.page**, i8**)*, i32 (%struct.file*, 
%struct.address_space*, i64, i32, i32, %struct.page*, i8*)*, i64 (%struct.address_space*, i64)*, void (%struct.page*, 
i32, i32)*, i32 (%struct.page*, i32)*, void (%struct.page*)*, i64 (%struct.kiocb*, %struct.iov_iter*)*, i32 
(%struct.address_space*, %struct.page*, %struct.page*, i32)*, i1 (%struct.page*, i32)*, void (%struct.page*)*, i32 
(%struct.page*)*, i32 (%struct.page*, i64, i64)*, void (%struct.page*, i8*, i8*)*, i32 (%struct.address_space*, 
%struct.page*)*, i32 (%struct.swap_info_struct*, %struct.file*, i64*)*, void (%struct.file*)* } %struct.kiocb = type { 
%struct.file*, i64, void (%struct.kiocb*, i64, i64)*, i8*, i32 } %struct.iov_iter = type opaque 
%struct.swap_info_struct = type opaque %union.anon.41 = type { %struct.pipe_inode_info* } %struct.file_operations = 
type { %struct.module*, i64 (%struct.file*, i64, i32)*, i64 (%struct.file*, i8*, i64, i64*)*, i64 (%struct.file*, i8*, 
i64, i64*)*, i64 (%struct.kiocb*, %struct.iov_iter*)*, i64 (%struct.kiocb*, %struct.iov_iter*)*, i32 (%struct.file*, 
%struct.dir_context*)*, i32 (%struct.file*, %struct.dir_context*)*, i32 (%struct.file*, %struct.poll_table_struct*)*, 
i64 (%struct.file*, i32, i64)*, i64 (%struct.file*, i32, i64)*, i32 (%struct.file*, %struct.vm_area_struct*)*, i32 
(%struct.inode*, %struct.file*)*, i32 (%struct.file*, i8*)*, i32 (%struct.inode*, %struct.file*)*, i32 (%struct.file*, 
i64, i64, i32)*, i32 (i32, %struct.file*, i32)*, i32 (%struct.file*, i32, %struct.file_lock*)*, i64 (%struct.file*, 
%struct.page*, i32, i64, i64*, i32)*, i64 (%struct.file*, i64, i64, i64, i64)*, i32 (i32)*, i32 (%struct.file*, i32, 
%struct.file_lock*)*, i64 (%struct.pipe_inode_info*, %struct.file*, i64*, i64, i32)*, i64 (%struct.file*, i64*, 
%struct.pipe_inode_info*, i64, i32)*, i32 (%struct.file*, i64, %struct.file_lock**, i8**)*, i64 (%struct.file*, i32, 
i64, i64)*, void (%struct.seq_file*, %struct.file*)*, i64 (%struct.file*, i64, %struct.file*, i64, i64, i32)*, i32 
(%struct.file*, i64, %struct.file*, i64, i64)*, i64 (%struct.file*, i64, i64, %struct.file*, i64)* } 
%struct.dir_context = type { i32 (%struct.dir_context*, i8*, i32, i64, i64, i32)*, i64 } %struct.poll_table_struct = 
type { void (%struct.file*, %struct.__wait_queue_head*, %struct.poll_table_struct*)*, i64 } %struct.file_lock = type { 
%struct.file_lock*, %struct.list_head, %struct.hlist_node, %struct.list_head, i8*, i32, i8, i32, i32, %struct.pid*, 
%struct.__wait_queue_head, %struct.file*, i64, i64, %struct.fasync_struct*, i64, i64, %struct.file_lock_operations*, 
%struct.lock_manager_operations*, %union.anon.39 } %struct.fasync_struct = type { %struct.spinlock, i32, i32, 
%struct.fasync_struct*, %struct.file*, %struct.callback_head } %struct.file_lock_operations = type { void 
(%struct.file_lock*, %struct.file_lock*)*, void (%struct.file_lock*)* } %struct.lock_manager_operations = type { i32 
(%struct.file_lock*, %struct.file_lock*)*, i64 (%struct.file_lock*)*, i8* (i8*)*, void (i8*)*, void 
(%struct.file_lock*)*, i32 (%struct.file_lock*, i32)*, i1 (%struct.file_lock*)*, i32 (%struct.file_lock*, i32, 
%struct.list_head*)*, void (%struct.file_lock*, i8**)* } %union.anon.39 = type { %struct.nfs_lock_info } 
%struct.nfs_lock_info = type { i32, %struct.nlm_lockowner*, %struct.list_head } %struct.nlm_lockowner = type opaque 
%struct.atomic64_t = type { i64 } %struct.fown_struct = type { %struct.rwlock_t, %struct.pid*, i32, %struct.kuid_t, 
%struct.kuid_t, i32 } %struct.rwlock_t = type { %struct.qrwlock } %struct.qrwlock = type { %struct.atomic_t, 
%struct.qspinlock } %struct.cred = type opaque %struct.file_ra_state = type { i64, i32, i32, i32, i32, i64 } 
%struct.seq_file = type opaque %struct.vm_operations_struct = type { void (%struct.vm_area_struct*)*, void 
(%struct.vm_area_struct*)*, i32 (%struct.vm_area_struct*)*, i32 (%struct.vm_fault*)*, i32 (%struct.vm_fault*, i32)*, 
void (%struct.vm_fault*, i64, i64)*, i32 (%struct.vm_fault*)*, i32 (%struct.vm_fault*)*, i32 (%struct.vm_area_struct*, 
i64, i8*, i32, i32)*, i8* (%struct.vm_area_struct*)*, i32 (%struct.vm_area_struct*, %struct.mempolicy*)*, 
%struct.mempolicy* (%struct.vm_area_struct*, i64)*, %struct.page* (%struct.vm_area_struct*, i64)* } %struct.vm_fault = 
type { %struct.vm_area_struct*, i32, i32, i64, i64, %struct.pmd_t*, %struct.pud_t*, %struct.pte_t, %struct.page*, 
%struct.mem_cgroup*, %struct.page*, %struct.pte_t*, %struct.spinlock*, %struct.page* } %struct.pmd_t = type { i64 } 
%struct.pud_t = type { i64 } %struct.pte_t = type { i64 } %struct.kernfs_open_node = type opaque %struct.kernfs_iattrs 
= type opaque %struct.kref = type { %struct.refcount_struct } %struct.refcount_struct = type { %struct.atomic_t } 
%struct.device_type = type { i8*, %struct.attribute_group**, i32 (%struct.device*, %struct.kobj_uevent_env*)*, i8* 
(%struct.device*, i16*, %struct.kuid_t*, %struct.kgid_t*)*, void (%struct.device*)*, %struct.dev_pm_ops* } 
%struct.dev_pm_ops = type { i32 (%struct.device*)*, void (%struct.device*)*, i32 (%struct.device*)*, i32 
(%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*)*, i32 
(%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*)*, i32 
(%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*)*, i32 
(%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*)* } 
%struct.mutex = type { %struct.atomic64_t, %struct.spinlock, %struct.optimistic_spin_queue, %struct.list_head } 
%struct.bus_type = type { i8*, i8*, %struct.device*, %struct.device_attribute*, %struct.attribute_group**, 
%struct.attribute_group**, %struct.attribute_group**, i32 (%struct.device*, %struct.device_driver*)*, i32 
(%struct.device*, %struct.kobj_uevent_env*)*, i32 (%struct.device*)*, i32 (%struct.device*)*, void (%struct.device*)*, 
i32 (%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*, i32)*, i32 (%struct.device*)*, i32 
(%struct.device*)*, %struct.dev_pm_ops*, %struct.iommu_ops*, %struct.subsys_private*, %struct.lock_class_key } 
%struct.device_attribute = type { %struct.attribute, i64 (%struct.device*, %struct.device_attribute*, i8*)*, i64 
(%struct.device*, %struct.device_attribute*, i8*, i64)* } %struct.iommu_ops = type opaque %struct.subsys_private = type 
opaque %struct.device_driver = type { i8*, %struct.bus_type*, %struct.module*, i8*, i8, i32, %struct.of_device_id*, 
%struct.acpi_device_id*, i32 (%struct.device*)*, i32 (%struct.device*)*, void (%struct.device*)*, i32 (%struct.device*, 
i32)*, i32 (%struct.device*)*, %struct.attribute_group**, %struct.dev_pm_ops*, %struct.driver_private* } 
%struct.of_device_id = type { [32 x i8], [32 x i8], [128 x i8], i8* } %struct.acpi_device_id = type { [9 x i8], i64, 
i32, i32 } %struct.driver_private = type opaque %struct.dev_links_info = type { %struct.list_head, %struct.list_head, 
i32 } %struct.dev_pm_info = type { %struct.pm_message, i16, %struct.spinlock, %struct.list_head, %struct.completion, 
%struct.wakeup_source*, i8, %struct.timer_list, i64, %struct.work_struct, %struct.__wait_queue_head, %struct.wake_irq*, 
%struct.atomic_t, %struct.atomic_t, i16, i32, i32, i32, i32, i32, i64, i64, i64, i64, %struct.pm_subsys_data*, void 
(%struct.device*, i32)*, %struct.dev_pm_qos* } %struct.pm_message = type { i32 } %struct.wakeup_source = type { i8*, 
%struct.list_head, %struct.spinlock, %struct.wake_irq*, %struct.timer_list, i64, i64, i64, i64, i64, i64, i64, i64, 
i64, i64, i64, i8 } %struct.timer_list = type { %struct.hlist_node, i64, void (i64)*, i64, i32 } %struct.wake_irq = 
type opaque %struct.pm_subsys_data = type { %struct.spinlock, i32, %struct.list_head } %struct.dev_pm_qos = type opaque 
%struct.dev_pm_domain = type { %struct.dev_pm_ops, void (%struct.device*, i1)*, i32 (%struct.device*)*, void 
(%struct.device*)*, void (%struct.device*)* } %struct.irq_domain = type opaque %struct.dma_map_ops = type { i8* 
(%struct.device*, i64, i64*, i32, i64)*, void (%struct.device*, i64, i8*, i64, i64)*, i32 (%struct.device*, 
%struct.vm_area_struct*, i8*, i64, i64, i64)*, i32 (%struct.device*, %struct.sg_table*, i8*, i64, i64, i64)*, i64 
(%struct.device*, %struct.page*, i64, i64, i32, i64)*, void (%struct.device*, i64, i64, i32, i64)*, i32 
(%struct.device*, %struct.scatterlist*, i32, i32, i64)*, void (%struct.device*, %struct.scatterlist*, i32, i32, i64)*, 
i64 (%struct.device*, i64, i64, i32, i64)*, void (%struct.device*, i64, i64, i32, i64)*, void (%struct.device*, i64, 
i64, i32)*, void (%struct.device*, i64, i64, i32)*, void (%struct.device*, %struct.scatterlist*, i32, i32)*, void 
(%struct.device*, %struct.scatterlist*, i32, i32)*, i32 (%struct.device*, i64)*, i32 (%struct.device*, i64)*, i32 
(%struct.device*, i64)*, i32 } %struct.sg_table = type { %struct.scatterlist*, i32, i32 } %struct.scatterlist = type { 
i64, i32, i32, i64, i32 } %struct.device_dma_parameters = type { i32, i64 } %struct.dma_coherent_mem = type opaque 
%struct.dev_archdata = type { i8* } %struct.device_node = type opaque %struct.fwnode_handle = type opaque 
%struct.spinlock = type { %union.anon.1 } %union.anon.1 = type { %struct.raw_spinlock } %struct.klist_node = type { 
i8*, %struct.list_head, %struct.kref } %struct.class = type { i8*, %struct.module*, %struct.class_attribute*, 
%struct.attribute_group**, %struct.attribute_group**, %struct.kobject*, i32 (%struct.device*, 
%struct.kobj_uevent_env*)*, i8* (%struct.device*, i16*)*, void (%struct.class*)*, void (%struct.device*)*, i32 
(%struct.device*, i32)*, i32 (%struct.device*)*, %struct.kobj_ns_type_operations*, i8* (%struct.device*)*, 
%struct.dev_pm_ops*, %struct.subsys_private* } %struct.class_attribute = type { %struct.attribute, i64 (%struct.class*, 
%struct.class_attribute*, i8*)*, i64 (%struct.class*, %struct.class_attribute*, i8*, i64)* } %struct.attribute_group = 
type { i8*, i16 (%struct.kobject*, %struct.attribute*, i32)*, i16 (%struct.kobject*, %struct.bin_attribute*, i32)*, 
%struct.attribute**, %struct.bin_attribute** } %struct.bin_attribute = type { %struct.attribute, i64, i8*, i64 
(%struct.file*, %struct.kobject*, %struct.bin_attribute*, i8*, i64, i64)*, i64 (%struct.file*, %struct.kobject*, 
%struct.bin_attribute*, i8*, i64, i64)*, i32 (%struct.file*, %struct.kobject*, %struct.bin_attribute*, 
%struct.vm_area_struct*)* } %struct.iommu_group = type opaque %struct.iommu_fwspec = type opaque %struct.atomic_t = 
type { i32 } %struct.work_struct = type { %struct.atomic64_t, %struct.list_head, void (%struct.work_struct*)* } 
%struct.usb_device_id = type { i16, i16, i16, i16, i16, i8, i8, i8, i8, i8, i8, i8, i64 } %struct.usb_dynids = type { 
%struct.spinlock, %struct.list_head } %struct.usbdrv_wrap = type { %struct.device_driver, i32 } 

%struct.usbtv = type { 
%struct.device*, %struct.usb_device*, %struct.v4l2_device, %struct.v4l2_ctrl_handler, %struct.video_device, 
%struct.vb2_queue, %struct.mutex, %struct.mutex, %struct.spinlock, %struct.list_head, i32, i32, i32, i64, i32, i32, 
i32, i32, i32, i32, [16 x %struct.urb*], %struct.snd_card*, %struct.snd_pcm_substream*, %struct.atomic_t, 
%struct.work_struct, %struct.urb*, i64, i64 } 


%struct.usb_device = type { i32, [16 x i8], i32, i32, i32, 
%struct.usb_tt*, i32, [2 x i32], %struct.usb_device*, %struct.usb_bus*, %struct.usb_host_endpoint, %struct.device, 
%struct.usb_device_descriptor, %struct.usb_host_bos*, %struct.usb_host_config*, %struct.usb_host_config*, [16 x 
%struct.usb_host_endpoint*], [16 x %struct.usb_host_endpoint*], i8**, i16, i8, i8, i16, i32, i8*, i8*, i8*, 
%struct.list_head, i32, i32, %struct.atomic_t, i64, i64, i8, %struct.wusb_dev*, i32, i32, %struct.usb2_lpm_parameters, 
%struct.usb3_lpm_parameters, %struct.usb3_lpm_parameters, i32 } %struct.usb_tt = type opaque %struct.usb_bus = type { 
%struct.device*, i32, i8*, i8, i8, i8, i8, i32, i32, %struct.mutex, %struct.usb_devmap, %struct.usb_device*, 
%struct.usb_bus*, i32, i32, i32, i32, %struct.mon_bus*, i32 } %struct.usb_devmap = type { [2 x i64] } %struct.mon_bus = 
type opaque %struct.usb_device_descriptor = type { i8, i8, i16, i8, i8, i8, i8, i16, i16, i16, i8, i8, i8, i8 } 
%struct.usb_host_bos = type { %struct.usb_bos_descriptor*, %struct.usb_ext_cap_descriptor*, 
%struct.usb_ss_cap_descriptor*, %struct.usb_ssp_cap_descriptor*, %struct.usb_ss_container_id_descriptor*, 
%struct.usb_ptm_cap_descriptor* } %struct.usb_bos_descriptor = type <{ i8, i8, i16, i8 }> 
%struct.usb_ext_cap_descriptor = type <{ i8, i8, i8, i32 }> %struct.usb_ss_cap_descriptor = type { i8, i8, i8, i8, i16, 
i8, i8, i16 } %struct.usb_ssp_cap_descriptor = type { i8, i8, i8, i8, i32, i16, i16, [1 x i32] } 
%struct.usb_ss_container_id_descriptor = type { i8, i8, i8, i8, [16 x i8] } %struct.usb_ptm_cap_descriptor = type { i8, 
i8, i8 } %struct.usb_host_config = type { %struct.usb_config_descriptor, i8*, [16 x 
%struct.usb_interface_assoc_descriptor*], [32 x %struct.usb_interface*], [32 x %struct.usb_interface_cache*], i8*, i32 
} %struct.usb_config_descriptor = type <{ i8, i8, i16, i8, i8, i8, i8, i8 }> %struct.usb_interface_cache = type { i32, 
%struct.kref, [0 x %struct.usb_host_interface] } %struct.wusb_dev = type opaque %struct.usb2_lpm_parameters = type { 
i32, i32 } %struct.usb3_lpm_parameters = type { i32, i32, i32, i32 } %struct.v4l2_device = type { %struct.device*, 
%struct.media_device*, %struct.list_head, %struct.spinlock, [36 x i8], void (%struct.v4l2_subdev*, i32, i8*)*, 
%struct.v4l2_ctrl_handler*, %struct.v4l2_prio_state, %struct.kref, void (%struct.v4l2_device*)* } %struct.media_device 
= type { %struct.device*, %struct.media_devnode*, [32 x i8], [32 x i8], [40 x i8], [32 x i8], i32, i32, i64, i32, 
%struct.ida, i32, %struct.list_head, %struct.list_head, %struct.list_head, %struct.list_head, %struct.list_head, 
%struct.mutex, %struct.media_graph, i8*, i32 (%struct.media_entity*, %struct.media_pipeline*)*, void 
(%struct.media_entity*)*, %struct.media_device_ops* } %struct.media_devnode = type { %struct.media_device*, 
%struct.media_file_operations*, %struct.device, %struct.cdev, %struct.device*, i32, i64, void (%struct.media_devnode*)* 
} %struct.media_file_operations = type { %struct.module*, i64 (%struct.file*, i8*, i64, i64*)*, i64 (%struct.file*, 
i8*, i64, i64*)*, i32 (%struct.file*, %struct.poll_table_struct*)*, i64 (%struct.file*, i32, i64)*, i64 (%struct.file*, 
i32, i64)*, i32 (%struct.file*)*, i32 (%struct.file*)* } %struct.cdev = type { %struct.kobject, %struct.module*, 
%struct.file_operations*, %struct.list_head, i32, i32 } %struct.ida = type { %struct.radix_tree_root } 
%struct.media_graph = type { [16 x %struct.anon.46], %struct.media_entity_enum, i32 } %struct.anon.46 = type { 
%struct.media_entity*, %struct.list_head* } %struct.media_entity = type { %struct.media_gobj, i8*, i32, i32, i64, i16, 
i16, i16, i32, %struct.media_pad*, %struct.list_head, %struct.media_entity_operations*, i32, i32, 
%struct.media_pipeline*, %union.anon.49 } %struct.media_gobj = type { %struct.media_device*, i32, %struct.list_head } 
%struct.media_pad = type { %struct.media_gobj, %struct.media_entity*, i16, i64 } %struct.media_entity_operations = type 
{ i32 (%struct.media_entity*, %struct.media_pad*, %struct.media_pad*, i32)*, i32 (%struct.media_link*)* } 
%struct.media_link = type { %struct.media_gobj, %struct.list_head, %union.anon.47, %union.anon.48, %struct.media_link*, 
i64, i8 } %union.anon.47 = type { %struct.media_gobj* } %union.anon.48 = type { %struct.media_gobj* } 
%struct.media_pipeline = type { i32, %struct.media_graph } %union.anon.49 = type { %struct.anon.50 } %struct.anon.50 = 
type { i32, i32 } %struct.media_entity_enum = type { i64*, i32 } %struct.media_device_ops = type { i32 
(%struct.media_link*, i32, i32)* } %struct.v4l2_subdev = type { %struct.media_entity, %struct.list_head, 
%struct.module*, i8, i32, %struct.v4l2_device*, %struct.v4l2_subdev_ops*, %struct.v4l2_subdev_internal_ops*, 
%struct.v4l2_ctrl_handler*, [32 x i8], i32, i8*, i8*, %struct.video_device*, %struct.device*, %struct.device_node*, 
%struct.list_head, %struct.v4l2_async_subdev*, %struct.v4l2_async_notifier*, %struct.v4l2_subdev_platform_data* } 
%struct.v4l2_subdev_ops = type { %struct.v4l2_subdev_core_ops*, %struct.v4l2_subdev_tuner_ops*, 
%struct.v4l2_subdev_audio_ops*, %struct.v4l2_subdev_video_ops*, %struct.v4l2_subdev_vbi_ops*, 
%struct.v4l2_subdev_ir_ops*, %struct.v4l2_subdev_sensor_ops*, %struct.v4l2_subdev_pad_ops* } 
%struct.v4l2_subdev_core_ops = type { i32 (%struct.v4l2_subdev*)*, i32 (%struct.v4l2_subdev*, i64, 
%struct.v4l2_subdev_io_pin_config*)*, i32 (%struct.v4l2_subdev*, i32)*, i32 (%struct.v4l2_subdev*)*, i32 
(%struct.v4l2_subdev*, i32)*, i32 (%struct.v4l2_subdev*, i32)*, i64 (%struct.v4l2_subdev*, i32, i8*)*, i64 
(%struct.v4l2_subdev*, i32, i64)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_dbg_register*)*, i32 (%struct.v4l2_subdev*, 
%struct.v4l2_dbg_register*)*, i32 (%struct.v4l2_subdev*, i32)*, i32 (%struct.v4l2_subdev*, i32, i8*)*, i32 
(%struct.v4l2_subdev*, %struct.v4l2_fh*, %struct.v4l2_event_subscription*)*, i32 (%struct.v4l2_subdev*, 
%struct.v4l2_fh*, %struct.v4l2_event_subscription*)* } %struct.v4l2_subdev_io_pin_config = type { i32, i8, i8, i8, i8 } 
%struct.v4l2_dbg_register = type { %struct.v4l2_dbg_match, i32, i64, i64 } %struct.v4l2_dbg_match = type { i32, 
%union.anon.51 } %union.anon.51 = type { i32, [28 x i8] } %struct.v4l2_fh = type { %struct.list_head, 
%struct.video_device*, %struct.v4l2_ctrl_handler*, i32, %struct.__wait_queue_head, %struct.list_head, 
%struct.list_head, i32, i32, %struct.v4l2_m2m_ctx* } %struct.v4l2_m2m_ctx = type opaque %struct.v4l2_event_subscription 
= type { i32, i32, i32, [5 x i32] } %struct.v4l2_subdev_tuner_ops = type { i32 (%struct.v4l2_subdev*)*, i32 
(%struct.v4l2_subdev*, %struct.v4l2_frequency*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_frequency*)*, i32 
(%struct.v4l2_subdev*, %struct.v4l2_frequency_band*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_tuner*)*, i32 
(%struct.v4l2_subdev*, %struct.v4l2_tuner*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_modulator*)*, i32 
(%struct.v4l2_subdev*, %struct.v4l2_modulator*)*, i32 (%struct.v4l2_subdev*, %struct.tuner_setup*)*, i32 
(%struct.v4l2_subdev*, %struct.v4l2_priv_tun_config*)* } %struct.v4l2_frequency = type { i32, i32, i32, [8 x i32] } 
%struct.v4l2_frequency_band = type { i32, i32, i32, i32, i32, i32, i32, [9 x i32] } %struct.v4l2_tuner = type { i32, 
[32 x i8], i32, i32, i32, i32, i32, i32, i32, i32, [4 x i32] } %struct.v4l2_modulator = type { i32, [32 x i8], i32, 
i32, i32, i32, i32, [3 x i32] } %struct.tuner_setup = type opaque %struct.v4l2_priv_tun_config = type { i32, i8* } 
%struct.v4l2_subdev_audio_ops = type { i32 (%struct.v4l2_subdev*, i32)*, i32 (%struct.v4l2_subdev*, i32)*, i32 
(%struct.v4l2_subdev*, i32, i32, i32)*, i32 (%struct.v4l2_subdev*, i32)* } %struct.v4l2_subdev_video_ops = type { i32 
(%struct.v4l2_subdev*, i32, i32, i32)*, i32 (%struct.v4l2_subdev*, i32, i32)*, i32 (%struct.v4l2_subdev*, i64*)*, i32 
(%struct.v4l2_subdev*, i64)*, i32 (%struct.v4l2_subdev*, i64)*, i32 (%struct.v4l2_subdev*, i64*)*, i32 
(%struct.v4l2_subdev*, i64*)*, i32 (%struct.v4l2_subdev*, i64*)*, i32 (%struct.v4l2_subdev*, i64*)*, i32 
(%struct.v4l2_subdev*, i32*)*, i32 (%struct.v4l2_subdev*, i32)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_fract*)*, i32 
(%struct.v4l2_subdev*, %struct.v4l2_streamparm*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_streamparm*)*, i32 
(%struct.v4l2_subdev*, %struct.v4l2_subdev_frame_interval*)*, i32 (%struct.v4l2_subdev*, 
%struct.v4l2_subdev_frame_interval*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_dv_timings*)*, i32 
(%struct.v4l2_subdev*, %struct.v4l2_dv_timings*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_dv_timings*)*, i32 
(%struct.v4l2_subdev*, %struct.v4l2_mbus_config*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_mbus_config*)*, i32 
(%struct.v4l2_subdev*, i8*, i32*)* } %struct.v4l2_fract = type { i32, i32 } %struct.v4l2_streamparm = type { i32, 
%union.anon.56 } %union.anon.56 = type { %struct.v4l2_captureparm, [160 x i8] } %struct.v4l2_captureparm = type { i32, 
i32, %struct.v4l2_fract, i32, i32, [4 x i32] } %struct.v4l2_subdev_frame_interval = type { i32, %struct.v4l2_fract, [9 
x i32] } %struct.v4l2_dv_timings = type { i32, %union.anon.57 } %union.anon.57 = type { [32 x i32] } 
%struct.v4l2_mbus_config = type { i32, i32 } %struct.v4l2_subdev_vbi_ops = type { i32 (%struct.v4l2_subdev*, 
%struct.v4l2_decode_vbi_line*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_sliced_vbi_data*)*, i32 (%struct.v4l2_subdev*, 
%struct.v4l2_sliced_vbi_data*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_sliced_vbi_cap*)*, i32 (%struct.v4l2_subdev*, 
%struct.v4l2_vbi_format*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_sliced_vbi_format*)*, i32 (%struct.v4l2_subdev*, 
%struct.v4l2_sliced_vbi_format*)* } %struct.v4l2_decode_vbi_line = type { i32, i8*, i32, i32 } 
%struct.v4l2_sliced_vbi_data = type { i32, i32, i32, i32, [48 x i8] } %struct.v4l2_sliced_vbi_cap = type { i16, [2 x 
[24 x i16]], i32, [3 x i32] } %struct.v4l2_vbi_format = type { i32, i32, i32, i32, [2 x i32], [2 x i32], i32, [2 x i32] 
} %struct.v4l2_sliced_vbi_format = type { i16, [2 x [24 x i16]], i32, [2 x i32] } %struct.v4l2_subdev_ir_ops = type { 
i32 (%struct.v4l2_subdev*, i8*, i64, i64*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_subdev_ir_parameters*)*, i32 
(%struct.v4l2_subdev*, %struct.v4l2_subdev_ir_parameters*)*, i32 (%struct.v4l2_subdev*, i8*, i64, i64*)*, i32 
(%struct.v4l2_subdev*, %struct.v4l2_subdev_ir_parameters*)*, i32 (%struct.v4l2_subdev*, 
%struct.v4l2_subdev_ir_parameters*)* } %struct.v4l2_subdev_ir_parameters = type { i32, i32, i8, i8, i8, i8, i32, i32, 
i32, i8, i8, i32, i32, i32, i32 } %struct.v4l2_subdev_sensor_ops = type { i32 (%struct.v4l2_subdev*, i32*)*, i32 
(%struct.v4l2_subdev*, i32*)* } %struct.v4l2_subdev_pad_ops = type { i32 (%struct.v4l2_subdev*, 
%struct.v4l2_subdev_pad_config*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_subdev_pad_config*, 
%struct.v4l2_subdev_mbus_code_enum*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_subdev_pad_config*, 
%struct.v4l2_subdev_frame_size_enum*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_subdev_pad_config*, 
%struct.v4l2_subdev_frame_interval_enum*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_subdev_pad_config*, 
%struct.v4l2_subdev_format*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_subdev_pad_config*, 
%struct.v4l2_subdev_format*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_subdev_pad_config*, 
%struct.v4l2_subdev_selection*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_subdev_pad_config*, 
%struct.v4l2_subdev_selection*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_edid*)*, i32 (%struct.v4l2_subdev*, 
%struct.v4l2_edid*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_dv_timings_cap*)*, i32 (%struct.v4l2_subdev*, 
%struct.v4l2_enum_dv_timings*)*, i32 (%struct.v4l2_subdev*, %struct.media_link*, %struct.v4l2_subdev_format*, 
%struct.v4l2_subdev_format*)*, i32 (%struct.v4l2_subdev*, i32, %struct.v4l2_mbus_frame_desc*)*, i32 
(%struct.v4l2_subdev*, i32, %struct.v4l2_mbus_frame_desc*)* } %struct.v4l2_subdev_pad_config = type { 
%struct.v4l2_mbus_framefmt, %struct.v4l2_rect, %struct.v4l2_rect } %struct.v4l2_mbus_framefmt = type { i32, i32, i32, 
i32, i32, i16, i16, i16, [11 x i16] } %struct.v4l2_rect = type { i32, i32, i32, i32 } 
%struct.v4l2_subdev_mbus_code_enum = type { i32, i32, i32, i32, [8 x i32] } %struct.v4l2_subdev_frame_size_enum = type 
{ i32, i32, i32, i32, i32, i32, i32, i32, [8 x i32] } %struct.v4l2_subdev_frame_interval_enum = type { i32, i32, i32, 
i32, i32, %struct.v4l2_fract, i32, [8 x i32] } %struct.v4l2_subdev_format = type { i32, i32, 
%struct.v4l2_mbus_framefmt, [8 x i32] } %struct.v4l2_subdev_selection = type { i32, i32, i32, i32, %struct.v4l2_rect, 
[8 x i32] } %struct.v4l2_edid = type { i32, i32, i32, [5 x i32], i8* } %struct.v4l2_dv_timings_cap = type { i32, i32, 
[2 x i32], %union.anon.58 } %union.anon.58 = type { %struct.v4l2_bt_timings_cap, [24 x i8] } 
%struct.v4l2_bt_timings_cap = type { i32, i32, i32, i32, i64, i64, i32, i32, [16 x i32] } %struct.v4l2_enum_dv_timings 
= type { i32, i32, [2 x i32], %struct.v4l2_dv_timings } %struct.v4l2_mbus_frame_desc = type { [4 x 
%struct.v4l2_mbus_frame_desc_entry], i16 } %struct.v4l2_mbus_frame_desc_entry = type { i16, i32, i32 } 
%struct.v4l2_subdev_internal_ops = type { i32 (%struct.v4l2_subdev*)*, void (%struct.v4l2_subdev*)*, i32 
(%struct.v4l2_subdev*, %struct.v4l2_subdev_fh*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_subdev_fh*)* } 
%struct.v4l2_subdev_fh = type { %struct.v4l2_fh, %struct.v4l2_subdev_pad_config* } %struct.v4l2_async_subdev = type { 
i32, %union.anon.59, %struct.list_head } %union.anon.59 = type { %struct.anon.63 } %struct.anon.63 = type { i1 
(%struct.device*, %struct.v4l2_async_subdev*)*, i8* } %struct.v4l2_async_notifier = type { i32, 
%struct.v4l2_async_subdev**, %struct.v4l2_device*, %struct.list_head, %struct.list_head, %struct.list_head, i32 
(%struct.v4l2_async_notifier*, %struct.v4l2_subdev*, %struct.v4l2_async_subdev*)*, i32 (%struct.v4l2_async_notifier*)*, 
void (%struct.v4l2_async_notifier*, %struct.v4l2_subdev*, %struct.v4l2_async_subdev*)* } 
%struct.v4l2_subdev_platform_data = type { %struct.regulator_bulk_data*, i32, i8* } %struct.regulator_bulk_data = type 
opaque %struct.v4l2_prio_state = type { [4 x %struct.atomic_t] } %struct.v4l2_ctrl_handler = type { %struct.mutex, 
%struct.mutex*, %struct.list_head, %struct.list_head, %struct.v4l2_ctrl_ref*, %struct.v4l2_ctrl_ref**, void 
(%struct.v4l2_ctrl*, i8*)*, i8*, i16, i32 } %struct.v4l2_ctrl_ref = type { %struct.list_head, %struct.v4l2_ctrl_ref*, 
%struct.v4l2_ctrl*, %struct.v4l2_ctrl_helper* } %struct.v4l2_ctrl = type { %struct.list_head, %struct.list_head, 
%struct.v4l2_ctrl_handler*, %struct.v4l2_ctrl**, i32, i24, %struct.v4l2_ctrl_ops*, %struct.v4l2_ctrl_type_ops*, i32, 
i8*, i32, i64, i64, i64, i32, i32, [4 x i32], i32, %union.anon.52, %union.anon.53, i64, i8*, i32, %struct.anon.54, 
%union.v4l2_ctrl_ptr, %union.v4l2_ctrl_ptr } %struct.v4l2_ctrl_ops = type { i32 (%struct.v4l2_ctrl*)*, i32 
(%struct.v4l2_ctrl*)*, i32 (%struct.v4l2_ctrl*)* } %struct.v4l2_ctrl_type_ops = type { i1 (%struct.v4l2_ctrl*, i32, 
i32*, i32*)*, void (%struct.v4l2_ctrl*, i32, i32*)*, void (%struct.v4l2_ctrl*)*, i32 (%struct.v4l2_ctrl*, i32, i32*)* } 
%union.anon.52 = type { i64 } %union.anon.53 = type { i8** } %struct.anon.54 = type { i32 } %union.v4l2_ctrl_ptr = type 
{ i32* } %struct.v4l2_ctrl_helper = type opaque %struct.video_device = type { %struct.media_entity, 
%struct.media_intf_devnode*, %struct.media_pipeline, %struct.v4l2_file_operations*, i32, %struct.device, %struct.cdev*, 
%struct.v4l2_device*, %struct.device*, %struct.v4l2_ctrl_handler*, %struct.vb2_queue*, %struct.v4l2_prio_state*, [32 x 
i8], i32, i32, i32, i16, i64, i32, %struct.spinlock, %struct.list_head, i32, i64, void (%struct.video_device*)*, 
%struct.v4l2_ioctl_ops*, [3 x i64], [3 x i64], %struct.mutex* } %struct.media_intf_devnode = type { 
%struct.media_interface, i32, i32 } %struct.media_interface = type { %struct.media_gobj, %struct.list_head, i32, i32 } 
%struct.v4l2_file_operations = type { %struct.module*, i64 (%struct.file*, i8*, i64, i64*)*, i64 (%struct.file*, i8*, 
i64, i64*)*, i32 (%struct.file*, %struct.poll_table_struct*)*, i64 (%struct.file*, i32, i64)*, i64 (%struct.file*, i32, 
i64)*, i64 (%struct.file*, i64, i64, i64, i64)*, i32 (%struct.file*, %struct.vm_area_struct*)*, i32 (%struct.file*)*, 
i32 (%struct.file*)* } %struct.v4l2_ioctl_ops = type opaque %struct.vb2_queue = type { i32, i32, %struct.device*, i64, 
i8, %struct.mutex*, i8*, %struct.vb2_ops*, %struct.vb2_mem_ops*, %struct.vb2_buf_ops*, i8*, i32, i32, i32, i32, 
%struct.mutex, i32, [32 x %struct.vb2_buffer*], i32, %struct.list_head, i32, %struct.atomic_t, %struct.list_head, 
%struct.spinlock, %struct.__wait_queue_head, [8 x %struct.device*], i8, %struct.vb2_fileio_data*, 
%struct.vb2_threadio_data*, i32, i32, i32, i32, i32 } %struct.vb2_ops = type { i32 (%struct.vb2_queue*, i32*, i32*, 
i32*, %struct.device**)*, void (%struct.vb2_queue*)*, void (%struct.vb2_queue*)*, i32 (%struct.vb2_buffer*)*, i32 
(%struct.vb2_buffer*)*, void (%struct.vb2_buffer*)*, void (%struct.vb2_buffer*)*, i32 (%struct.vb2_queue*, i32)*, void 
(%struct.vb2_queue*)*, void (%struct.vb2_buffer*)* } %struct.vb2_buffer = type { %struct.vb2_queue*, i32, i32, i32, 
i32, [8 x %struct.vb2_plane], i64, i32, %struct.list_head, %struct.list_head, i32, i32, i32, i32, i32, i32, i32, i32, 
i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32 } %struct.vb2_plane = type { i8*, %struct.dma_buf*, 
i32, i32, i32, i32, %union.anon.55, i32 } %struct.dma_buf = type { i64, %struct.file*, %struct.list_head, 
%struct.dma_buf_ops*, %struct.mutex, i32, i8*, i8*, %struct.module*, %struct.list_head, i8*, 
%struct.reservation_object*, %struct.__wait_queue_head, %struct.dma_buf_poll_cb_t, %struct.dma_buf_poll_cb_t } 
%struct.dma_buf_ops = type { i32 (%struct.dma_buf*, %struct.device*, %struct.dma_buf_attachment*)*, void 
(%struct.dma_buf*, %struct.dma_buf_attachment*)*, %struct.sg_table* (%struct.dma_buf_attachment*, i32)*, void 
(%struct.dma_buf_attachment*, %struct.sg_table*, i32)*, void (%struct.dma_buf*)*, i32 (%struct.dma_buf*, i32)*, i32 
(%struct.dma_buf*, i32)*, i8* (%struct.dma_buf*, i64)*, void (%struct.dma_buf*, i64, i8*)*, i8* (%struct.dma_buf*, 
i64)*, void (%struct.dma_buf*, i64, i8*)*, i32 (%struct.dma_buf*, %struct.vm_area_struct*)*, i8* (%struct.dma_buf*)*, 
void (%struct.dma_buf*, i8*)* } %struct.dma_buf_attachment = type { %struct.dma_buf*, %struct.device*, 
%struct.list_head, i8* } %struct.reservation_object = type opaque %struct.dma_buf_poll_cb_t = type { 
%struct.dma_fence_cb, %struct.__wait_queue_head*, i64 } %struct.dma_fence_cb = type { %struct.list_head, void 
(%struct.dma_fence*, %struct.dma_fence_cb*)* } %struct.dma_fence = type { %struct.kref, %struct.dma_fence_ops*, 
%struct.callback_head, %struct.list_head, %struct.spinlock*, i64, i32, i64, i64, i32 } %struct.dma_fence_ops = type { 
i8* (%struct.dma_fence*)*, i8* (%struct.dma_fence*)*, i1 (%struct.dma_fence*)*, i1 (%struct.dma_fence*)*, i64 
(%struct.dma_fence*, i1, i64)*, void (%struct.dma_fence*)*, i32 (%struct.dma_fence*, i8*, i32)*, void 
(%struct.dma_fence*, i8*, i32)*, void (%struct.dma_fence*, i8*, i32)* } %union.anon.55 = type { i64 } 
%struct.vb2_mem_ops = type { i8* (%struct.device*, i64, i64, i32, i32)*, void (i8*)*, %struct.dma_buf* (i8*, i64)*, i8* 
(%struct.device*, i64, i64, i32)*, void (i8*)*, void (i8*)*, void (i8*)*, i8* (%struct.device*, %struct.dma_buf*, i64, 
i32)*, void (i8*)*, i32 (i8*)*, void (i8*)*, i8* (i8*)*, i8* (i8*)*, i32 (i8*)*, i32 (i8*, %struct.vm_area_struct*)* } 
%struct.vb2_buf_ops = type { i32 (%struct.vb2_buffer*, i8*)*, void (%struct.vb2_buffer*, i8*)*, i32 
(%struct.vb2_buffer*, i8*, %struct.vb2_plane*)*, void (%struct.vb2_buffer*, i8*)* } %struct.vb2_fileio_data = type 
opaque %struct.vb2_threadio_data = type opaque %struct.snd_card = type opaque %struct.snd_pcm_substream = type opaque 
%struct.urb = type { %struct.kref, i8*, %struct.atomic_t, %struct.atomic_t, i32, %struct.list_head, %struct.list_head, 
%struct.usb_anchor*, %struct.usb_device*, %struct.usb_host_endpoint*, i32, i32, i32, i32, i8*, i64, 
%struct.scatterlist*, i32, i32, i32, i32, i8*, i64, i32, i32, i32, i32, i8*, void (%struct.urb*)*, [0 x 
%struct.usb_iso_packet_descriptor] } %struct.usb_anchor = type { %struct.list_head, %struct.__wait_queue_head, 
%struct.spinlock, %struct.atomic_t, i8 } %struct.usb_iso_packet_descriptor = type { i32, i32, i32, i32 }

@__initcall_usbtv_usb_driver_init6 = internal global i32 ()* @usbtv_usb_driver_init, section ".initcall6.init", align 8
@usbtv_usb_driver = internal global %struct.usb_driver { i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str, i32 0, i32 0), i32 (%struct.usb_interface*, %struct.usb_device_id*)* @usbtv_probe, void (%struct.usb_interface*)* @usbtv_disconnect, i32 (%struct.usb_interface*, i32, i8*)* null, i32 (%struct.usb_interface*, i32)* null, i32 (%struct.usb_interface*)* null, i32 (%struct.usb_interface*)* null, i32 (%struct.usb_interface*)* null, i32 (%struct.usb_interface*)* null, %struct.usb_device_id* getelementptr inbounds ([2 x %struct.usb_device_id], [2 x %struct.usb_device_id]* @usbtv_id_table, i32 0, i32 0), %struct.usb_dynids zeroinitializer, %struct.usbdrv_wrap zeroinitializer, i8 0 }, align 8
@__exitcall_usbtv_usb_driver_exit = internal global void ()* @usbtv_usb_driver_exit, section ".exitcall.exit", align 8
@.str = private unnamed_addr constant [6 x i8] c"usbtv\00", align 1
@usbtv_id_table = internal global [2 x %struct.usb_device_id] [%struct.usb_device_id { i16 3, i16 7025, i16 12290, i16 0, i16 0, i8 0, i8 0, i8 0, i8 0, i8 0, i8 0, i8 0, i64 0 }, %struct.usb_device_id zeroinitializer], align 16
@.str1 = private unnamed_addr constant [39 x i8] c"Fushicai USBTV007 Audio-Video Grabber\0A\00", align 1
@llvm.used = appending global [3 x i8*] [i8* bitcast (i32 ()** @__initcall_usbtv_usb_driver_init6 to i8*), i8* bitcast (void ()* @usbtv_usb_driver_exit to i8*), i8* bitcast (void ()** @__exitcall_usbtv_usb_driver_exit to i8*)], section "llvm.metadata"

; Function Attrs: noredzone nounwind
define i32 @usbtv_set_regs(%struct.usbtv* %usbtv, [2 x i16]* %regs, i32 %size) #0 {
entry:
  %retval = alloca i32, align 4
  %usbtv.addr = alloca %struct.usbtv*, align 8
  %regs.addr = alloca [2 x i16]*, align 8
  %size.addr = alloca i32, align 4
  %ret = alloca i32, align 4
  %pipe = alloca i32, align 4
  %i = alloca i32, align 4
  %index = alloca i16, align 2
  %value = alloca i16, align 2
  store %struct.usbtv* %usbtv, %struct.usbtv** %usbtv.addr, align 8
  store [2 x i16]* %regs, [2 x i16]** %regs.addr, align 8
  store i32 %size, i32* %size.addr, align 4
  %0 = load %struct.usbtv*, %struct.usbtv** %usbtv.addr, align 8
  %udev = getelementptr inbounds %struct.usbtv, %struct.usbtv* %0, i32 0, i32 1
  %1 = load %struct.usb_device*, %struct.usb_device** %udev, align 8
  %call = call i32 @__create_pipe(%struct.usb_device* %1, i32 0) #4
  %or = or i32 -2147483648, %call
  %or1 = or i32 %or, 128
  store i32 %or1, i32* %pipe, align 4
  store i32 0, i32* %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %2 = load i32, i32* %i, align 4
  %3 = load i32, i32* %size.addr, align 4
  %cmp = icmp slt i32 %2, %3
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %4 = load i32, i32* %i, align 4
  %idxprom = sext i32 %4 to i64
  %5 = load [2 x i16]*, [2 x i16]** %regs.addr, align 8
  %arrayidx = getelementptr [2 x i16], [2 x i16]* %5, i64 %idxprom
  %arrayidx2 = getelementptr [2 x i16], [2 x i16]* %arrayidx, i32 0, i64 0
  %6 = load i16, i16* %arrayidx2, align 2
  store i16 %6, i16* %index, align 2
  %7 = load i32, i32* %i, align 4
  %idxprom3 = sext i32 %7 to i64
  %8 = load [2 x i16]*, [2 x i16]** %regs.addr, align 8
  %arrayidx4 = getelementptr [2 x i16], [2 x i16]* %8, i64 %idxprom3
  %arrayidx5 = getelementptr [2 x i16], [2 x i16]* %arrayidx4, i32 0, i64 1
  %9 = load i16, i16* %arrayidx5, align 2
  store i16 %9, i16* %value, align 2
  %10 = load %struct.usbtv*, %struct.usbtv** %usbtv.addr, align 8
  %udev6 = getelementptr inbounds %struct.usbtv, %struct.usbtv* %10, i32 0, i32 1
  %11 = load %struct.usb_device*, %struct.usb_device** %udev6, align 8
  %12 = load i32, i32* %pipe, align 4
  %13 = load i16, i16* %value, align 2
  %14 = load i16, i16* %index, align 2
  %call7 = call i32 @usb_control_msg(%struct.usb_device* %11, i32 %12, i8 zeroext 12, i8 zeroext 64, i16 zeroext %13, i16 zeroext %14, i8* null, i16 zeroext 0, i32 0) #4
  store i32 %call7, i32* %ret, align 4
  %15 = load i32, i32* %ret, align 4
  %cmp8 = icmp slt i32 %15, 0
  br i1 %cmp8, label %if.then, label %if.end

if.then:                                          ; preds = %for.body
  %16 = load i32, i32* %ret, align 4
  store i32 %16, i32* %retval
  br label %return

if.end:                                           ; preds = %for.body
  br label %for.inc

for.inc:                                          ; preds = %if.end
  %17 = load i32, i32* %i, align 4
  %inc = add i32 %17, 1
  store i32 %inc, i32* %i, align 4
  br label %for.cond

for.end:                                          ; preds = %for.cond
  store i32 0, i32* %retval
  br label %return

return:                                           ; preds = %for.end, %if.then
  %18 = load i32, i32* %retval
  ret i32 %18
}

; Function Attrs: inlinehint noredzone nounwind
define internal i32 @__create_pipe(%struct.usb_device* %dev, i32 %endpoint) #1 {
entry:
  %dev.addr = alloca %struct.usb_device*, align 8
  %endpoint.addr = alloca i32, align 4
  store %struct.usb_device* %dev, %struct.usb_device** %dev.addr, align 8
  store i32 %endpoint, i32* %endpoint.addr, align 4
  %0 = load %struct.usb_device*, %struct.usb_device** %dev.addr, align 8
  %devnum = getelementptr inbounds %struct.usb_device, %struct.usb_device* %0, i32 0, i32 0
  %1 = load i32, i32* %devnum, align 4
  %shl = shl i32 %1, 8
  %2 = load i32, i32* %endpoint.addr, align 4
  %shl1 = shl i32 %2, 15
  %or = or i32 %shl, %shl1
  ret i32 %or
}

; Function Attrs: noredzone
declare i32 @usb_control_msg(%struct.usb_device*, i32, i8 zeroext, i8 zeroext, i16 zeroext, i16 zeroext, i8*, i16 zeroext, i32) #2

; Function Attrs: noredzone nounwind
define internal i32 @usbtv_usb_driver_init() #0 section ".init.text" {
entry:
  %call = call i32 @usb_register_driver(%struct.usb_driver* @usbtv_usb_driver, %struct.module* null, i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str, i32 0, i32 0)) #4
  ret i32 %call
}

; Function Attrs: noredzone nounwind
define internal void @usbtv_usb_driver_exit() #0 section ".exit.text" {
entry:
  call void @usb_deregister(%struct.usb_driver* @usbtv_usb_driver) #4
  ret void
}

; Function Attrs: noredzone
declare void @usb_deregister(%struct.usb_driver*) #2

; Function Attrs: noredzone
declare i32 @usb_register_driver(%struct.usb_driver*, %struct.module*, i8*) #2

; Function Attrs: noredzone nounwind
define internal i32 @usbtv_probe(%struct.usb_interface* %intf, %struct.usb_device_id* %id) #0 {
entry:
  %retval = alloca i32, align 4
  %intf.addr = alloca %struct.usb_interface*, align 8
  %id.addr = alloca %struct.usb_device_id*, align 8
  %ret = alloca i32, align 4
  %size = alloca i32, align 4
  %dev = alloca %struct.device*, align 8
  %usbtv = alloca %struct.usbtv*, align 8
  %ep = alloca %struct.usb_host_endpoint*, align 8
  store %struct.usb_interface* %intf, %struct.usb_interface** %intf.addr, align 8
  store %struct.usb_device_id* %id, %struct.usb_device_id** %id.addr, align 8
  %0 = load %struct.usb_interface*, %struct.usb_interface** %intf.addr, align 8
  %dev1 = getelementptr inbounds %struct.usb_interface, %struct.usb_interface* %0, i32 0, i32 7
  store %struct.device* %dev1, %struct.device** %dev, align 8
  %1 = load %struct.usb_interface*, %struct.usb_interface** %intf.addr, align 8
  %num_altsetting = getelementptr inbounds %struct.usb_interface, %struct.usb_interface* %1, i32 0, i32 2
  %2 = load i32, i32* %num_altsetting, align 4
  %cmp = icmp ne i32 %2, 2
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  store i32 -19, i32* %retval
  br label %return

if.end:                                           ; preds = %entry
  %3 = load %struct.usb_interface*, %struct.usb_interface** %intf.addr, align 8
  %altsetting = getelementptr inbounds %struct.usb_interface, %struct.usb_interface* %3, i32 0, i32 0
  %4 = load %struct.usb_host_interface*, %struct.usb_host_interface** %altsetting, align 8
  %arrayidx = getelementptr %struct.usb_host_interface, %struct.usb_host_interface* %4, i64 1
  %desc = getelementptr inbounds %struct.usb_host_interface, %struct.usb_host_interface* %arrayidx, i32 0, i32 0
  %bNumEndpoints = getelementptr inbounds %struct.usb_interface_descriptor, %struct.usb_interface_descriptor* %desc, i32 0, i32 4
  %5 = load i8, i8* %bNumEndpoints, align 1
  %conv = zext i8 %5 to i32
  %cmp2 = icmp ne i32 %conv, 4
  br i1 %cmp2, label %if.then4, label %if.end5

if.then4:                                         ; preds = %if.end
  store i32 -19, i32* %retval
  br label %return

if.end5:                                          ; preds = %if.end
  %6 = load %struct.usb_interface*, %struct.usb_interface** %intf.addr, align 8
  %altsetting6 = getelementptr inbounds %struct.usb_interface, %struct.usb_interface* %6, i32 0, i32 0
  %7 = load %struct.usb_host_interface*, %struct.usb_host_interface** %altsetting6, align 8
  %arrayidx7 = getelementptr %struct.usb_host_interface, %struct.usb_host_interface* %7, i64 1
  %endpoint = getelementptr inbounds %struct.usb_host_interface, %struct.usb_host_interface* %arrayidx7, i32 0, i32 3
  %8 = load %struct.usb_host_endpoint*, %struct.usb_host_endpoint** %endpoint, align 8
  %arrayidx8 = getelementptr %struct.usb_host_endpoint, %struct.usb_host_endpoint* %8, i64 0
  store %struct.usb_host_endpoint* %arrayidx8, %struct.usb_host_endpoint** %ep, align 8
  %9 = load %struct.usb_host_endpoint*, %struct.usb_host_endpoint** %ep, align 8
  %desc9 = getelementptr inbounds %struct.usb_host_endpoint, %struct.usb_host_endpoint* %9, i32 0, i32 0
  %call = call i32 @usb_endpoint_maxp(%struct.usb_endpoint_descriptor* %desc9) #4
  store i32 %call, i32* %size, align 4
  %10 = load i32, i32* %size, align 4
  %and = and i32 %10, 2047
  %11 = load %struct.usb_host_endpoint*, %struct.usb_host_endpoint** %ep, align 8
  %desc10 = getelementptr inbounds %struct.usb_host_endpoint, %struct.usb_host_endpoint* %11, i32 0, i32 0
  %call11 = call i32 @usb_endpoint_maxp_mult(%struct.usb_endpoint_descriptor* %desc10) #4
  %mul = mul i32 %and, %call11
  store i32 %mul, i32* %size, align 4
  %call12 = call noalias i8* @malloc(i64 2560) #4
  %12 = bitcast i8* %call12 to %struct.usbtv*
  store %struct.usbtv* %12, %struct.usbtv** %usbtv, align 8
  %13 = load %struct.usbtv*, %struct.usbtv** %usbtv, align 8
  %cmp13 = icmp eq %struct.usbtv* %13, null
  br i1 %cmp13, label %if.then15, label %if.end16

if.then15:                                        ; preds = %if.end5
  store i32 -12, i32* %retval
  br label %return

if.end16:                                         ; preds = %if.end5
  %14 = load %struct.device*, %struct.device** %dev, align 8
  %15 = load %struct.usbtv*, %struct.usbtv** %usbtv, align 8
  %dev17 = getelementptr inbounds %struct.usbtv, %struct.usbtv* %15, i32 0, i32 0
  store %struct.device* %14, %struct.device** %dev17, align 8
  %16 = load %struct.usb_interface*, %struct.usb_interface** %intf.addr, align 8
  %call18 = call %struct.usb_device* @interface_to_usbdev(%struct.usb_interface* %16) #4
  %call19 = call %struct.usb_device* @usb_get_dev(%struct.usb_device* %call18) #4
  %17 = load %struct.usbtv*, %struct.usbtv** %usbtv, align 8
  %udev = getelementptr inbounds %struct.usbtv, %struct.usbtv* %17, i32 0, i32 1
  store %struct.usb_device* %call19, %struct.usb_device** %udev, align 8
  %18 = load i32, i32* %size, align 4
  %19 = load %struct.usbtv*, %struct.usbtv** %usbtv, align 8
  %iso_size = getelementptr inbounds %struct.usbtv, %struct.usbtv* %19, i32 0, i32 17
  store i32 %18, i32* %iso_size, align 4
  %20 = load %struct.usb_interface*, %struct.usb_interface** %intf.addr, align 8
  %21 = load %struct.usbtv*, %struct.usbtv** %usbtv, align 8
  %22 = bitcast %struct.usbtv* %21 to i8*
  call void @usb_set_intfdata(%struct.usb_interface* %20, i8* %22) #4
  %23 = load %struct.usbtv*, %struct.usbtv** %usbtv, align 8
  %call20 = call i32 @usbtv_video_init(%struct.usbtv* %23) #4
  store i32 %call20, i32* %ret, align 4
  %24 = load i32, i32* %ret, align 4
  %cmp21 = icmp slt i32 %24, 0
  br i1 %cmp21, label %if.then23, label %if.end24

if.then23:                                        ; preds = %if.end16
  br label %usbtv_video_fail

if.end24:                                         ; preds = %if.end16
  %25 = load %struct.usbtv*, %struct.usbtv** %usbtv, align 8
  %call25 = call i32 @usbtv_audio_init(%struct.usbtv* %25) #4
  store i32 %call25, i32* %ret, align 4
  %26 = load i32, i32* %ret, align 4
  %cmp26 = icmp slt i32 %26, 0
  br i1 %cmp26, label %if.then28, label %if.end29

if.then28:                                        ; preds = %if.end24
  br label %usbtv_audio_fail

if.end29:                                         ; preds = %if.end24
  %27 = load %struct.usbtv*, %struct.usbtv** %usbtv, align 8
  %v4l2_dev = getelementptr inbounds %struct.usbtv, %struct.usbtv* %27, i32 0, i32 2
  call void @v4l2_device_get(%struct.v4l2_device* %v4l2_dev) #4
  %28 = load %struct.device*, %struct.device** %dev, align 8
  call void (%struct.device*, i8*, ...) @_dev_info(%struct.device* %28, i8* getelementptr inbounds ([39 x i8], [39 x i8]* @.str1, i32 0, i32 0)) #4
  store i32 0, i32* %retval
  br label %return

usbtv_audio_fail:                                 ; preds = %if.then28
  %29 = load %struct.usb_interface*, %struct.usb_interface** %intf.addr, align 8
  call void @usb_set_intfdata(%struct.usb_interface* %29, i8* null) #4
  %30 = load %struct.usbtv*, %struct.usbtv** %usbtv, align 8
  %udev30 = getelementptr inbounds %struct.usbtv, %struct.usbtv* %30, i32 0, i32 1
  %31 = load %struct.usb_device*, %struct.usb_device** %udev30, align 8
  call void @usb_put_dev(%struct.usb_device* %31) #4
  %32 = load %struct.usbtv*, %struct.usbtv** %usbtv, align 8
  call void @usbtv_video_free(%struct.usbtv* %32) #4
  %33 = load i32, i32* %ret, align 4
  store i32 %33, i32* %retval
  br label %return

usbtv_video_fail:                                 ; preds = %if.then23
  %34 = load %struct.usb_interface*, %struct.usb_interface** %intf.addr, align 8
  call void @usb_set_intfdata(%struct.usb_interface* %34, i8* null) #4
  %35 = load %struct.usbtv*, %struct.usbtv** %usbtv, align 8
  %udev31 = getelementptr inbounds %struct.usbtv, %struct.usbtv* %35, i32 0, i32 1
  %36 = load %struct.usb_device*, %struct.usb_device** %udev31, align 8
  call void @usb_put_dev(%struct.usb_device* %36) #4
  %37 = load %struct.usbtv*, %struct.usbtv** %usbtv, align 8
  %38 = bitcast %struct.usbtv* %37 to i8*
  call void @kfree(i8* %38) #4
  %39 = load i32, i32* %ret, align 4
  store i32 %39, i32* %retval
  br label %return

return:                                           ; preds = %usbtv_video_fail, %usbtv_audio_fail, %if.end29, %if.then15, %if.then4, %if.then
  %40 = load i32, i32* %retval
  ret i32 %40
}

; Function Attrs: nounwind
declare noalias i8* @malloc(i64) #4

; Function Attrs: noredzone nounwind
define internal void @usbtv_disconnect(%struct.usb_interface* %intf) #0 {
entry:
  %intf.addr = alloca %struct.usb_interface*, align 8
  %usbtv = alloca %struct.usbtv*, align 8
  store %struct.usb_interface* %intf, %struct.usb_interface** %intf.addr, align 8
  %0 = load %struct.usb_interface*, %struct.usb_interface** %intf.addr, align 8
  %call = call i8* @usb_get_intfdata(%struct.usb_interface* %0) #4
  %1 = bitcast i8* %call to %struct.usbtv*
  store %struct.usbtv* %1, %struct.usbtv** %usbtv, align 8
  %2 = load %struct.usb_interface*, %struct.usb_interface** %intf.addr, align 8
  call void @usb_set_intfdata(%struct.usb_interface* %2, i8* null) #4
  %3 = load %struct.usbtv*, %struct.usbtv** %usbtv, align 8
  %tobool = icmp ne %struct.usbtv* %3, null
  br i1 %tobool, label %if.end, label %if.then

if.then:                                          ; preds = %entry
  br label %return

if.end:                                           ; preds = %entry
  %4 = load %struct.usbtv*, %struct.usbtv** %usbtv, align 8
  call void @usbtv_audio_free(%struct.usbtv* %4) #4
  %5 = load %struct.usbtv*, %struct.usbtv** %usbtv, align 8
  call void @usbtv_video_free(%struct.usbtv* %5) #4
  %6 = load %struct.usbtv*, %struct.usbtv** %usbtv, align 8
  %udev = getelementptr inbounds %struct.usbtv, %struct.usbtv* %6, i32 0, i32 1
  %7 = load %struct.usb_device*, %struct.usb_device** %udev, align 8
  call void @usb_put_dev(%struct.usb_device* %7) #4
  %8 = load %struct.usbtv*, %struct.usbtv** %usbtv, align 8
  %udev1 = getelementptr inbounds %struct.usbtv, %struct.usbtv* %8, i32 0, i32 1
  store %struct.usb_device* null, %struct.usb_device** %udev1, align 8
  %9 = load %struct.usbtv*, %struct.usbtv** %usbtv, align 8
  %v4l2_dev = getelementptr inbounds %struct.usbtv, %struct.usbtv* %9, i32 0, i32 2
  %call2 = call i32 @v4l2_device_put(%struct.v4l2_device* %v4l2_dev) #4
  br label %return

return:                                           ; preds = %if.end, %if.then
  ret void
}

; Function Attrs: inlinehint noredzone nounwind
define internal i32 @usb_endpoint_maxp(%struct.usb_endpoint_descriptor* %epd) #1 {
entry:
  %epd.addr = alloca %struct.usb_endpoint_descriptor*, align 8
  store %struct.usb_endpoint_descriptor* %epd, %struct.usb_endpoint_descriptor** %epd.addr, align 8
  %0 = load %struct.usb_endpoint_descriptor*, %struct.usb_endpoint_descriptor** %epd.addr, align 8
  %wMaxPacketSize = getelementptr inbounds %struct.usb_endpoint_descriptor, %struct.usb_endpoint_descriptor* %0, i32 0, i32 4
  %1 = load i16, i16* %wMaxPacketSize, align 1
  %conv = zext i16 %1 to i32
  %and = and i32 %conv, 2047
  ret i32 %and
}

; Function Attrs: inlinehint noredzone nounwind
define internal i32 @usb_endpoint_maxp_mult(%struct.usb_endpoint_descriptor* %epd) #1 {
entry:
  %epd.addr = alloca %struct.usb_endpoint_descriptor*, align 8
  %maxp = alloca i32, align 4
  store %struct.usb_endpoint_descriptor* %epd, %struct.usb_endpoint_descriptor** %epd.addr, align 8
  %0 = load %struct.usb_endpoint_descriptor*, %struct.usb_endpoint_descriptor** %epd.addr, align 8
  %wMaxPacketSize = getelementptr inbounds %struct.usb_endpoint_descriptor, %struct.usb_endpoint_descriptor* %0, i32 0, i32 4
  %1 = load i16, i16* %wMaxPacketSize, align 1
  %conv = zext i16 %1 to i32
  store i32 %conv, i32* %maxp, align 4
  %2 = load i32, i32* %maxp, align 4
  %and = and i32 %2, 6144
  %shr = ashr i32 %and, 11
  %add = add i32 %shr, 1
  ret i32 %add
}

; Function Attrs: inlinehint noredzone nounwind
define internal i8* @kzalloc(i64 %size, i32 %flags) #1 {
entry:
  %size.addr = alloca i64, align 8
  %flags.addr = alloca i32, align 4
  store i64 %size, i64* %size.addr, align 8
  store i32 %flags, i32* %flags.addr, align 4
  %0 = load i64, i64* %size.addr, align 8
  %1 = load i32, i32* %flags.addr, align 4
  %or = or i32 %1, 32768
  %call = call i8* @malloc(i64 %0) #4
  ret i8* %call
}

; Function Attrs: noredzone
declare %struct.usb_device* @usb_get_dev(%struct.usb_device*) #2

; Function Attrs: inlinehint noredzone nounwind
define internal %struct.usb_device* @interface_to_usbdev(%struct.usb_interface* %intf) #1 {
entry:
  %intf.addr = alloca %struct.usb_interface*, align 8
  %__mptr = alloca %struct.device*, align 8
  %tmp = alloca %struct.usb_device*, align 8
  store %struct.usb_interface* %intf, %struct.usb_interface** %intf.addr, align 8
  %0 = load %struct.usb_interface*, %struct.usb_interface** %intf.addr, align 8
  %dev = getelementptr inbounds %struct.usb_interface, %struct.usb_interface* %0, i32 0, i32 7
  %parent = getelementptr inbounds %struct.device, %struct.device* %dev, i32 0, i32 0
  %1 = load %struct.device*, %struct.device** %parent, align 8
  store %struct.device* %1, %struct.device** %__mptr, align 8
  %2 = load %struct.device*, %struct.device** %__mptr, align 8
  %3 = bitcast %struct.device* %2 to i8*
  %add.ptr = getelementptr i8, i8* %3, i64 -152
  %4 = bitcast i8* %add.ptr to %struct.usb_device*
  store %struct.usb_device* %4, %struct.usb_device** %tmp
  %5 = load %struct.usb_device*, %struct.usb_device** %tmp
  ret %struct.usb_device* %5
}

; Function Attrs: inlinehint noredzone nounwind
define internal void @usb_set_intfdata(%struct.usb_interface* %intf, i8* %data) #1 {
entry:
  %intf.addr = alloca %struct.usb_interface*, align 8
  %data.addr = alloca i8*, align 8
  store %struct.usb_interface* %intf, %struct.usb_interface** %intf.addr, align 8
  store i8* %data, i8** %data.addr, align 8
  %0 = load %struct.usb_interface*, %struct.usb_interface** %intf.addr, align 8
  %dev = getelementptr inbounds %struct.usb_interface, %struct.usb_interface* %0, i32 0, i32 7
  %1 = load i8*, i8** %data.addr, align 8
  call void @dev_set_drvdata(%struct.device* %dev, i8* %1) #4
  ret void
}

; Function Attrs: noredzone
declare i32 @usbtv_video_init(%struct.usbtv*) #2

; Function Attrs: noredzone
declare i32 @usbtv_audio_init(%struct.usbtv*) #2

; Function Attrs: inlinehint noredzone nounwind
define internal void @v4l2_device_get(%struct.v4l2_device* %v4l2_dev) #1 {
entry:
  %v4l2_dev.addr = alloca %struct.v4l2_device*, align 8
  store %struct.v4l2_device* %v4l2_dev, %struct.v4l2_device** %v4l2_dev.addr, align 8
  %0 = load %struct.v4l2_device*, %struct.v4l2_device** %v4l2_dev.addr, align 8
  %ref = getelementptr inbounds %struct.v4l2_device, %struct.v4l2_device* %0, i32 0, i32 8
  call void @kref_get(%struct.kref* %ref) #4
  ret void
}

; Function Attrs: noredzone
declare void @_dev_info(%struct.device*, i8*, ...) #2

; Function Attrs: noredzone
declare void @usb_put_dev(%struct.usb_device*) #2

; Function Attrs: noredzone
declare void @usbtv_video_free(%struct.usbtv*) #2

; Function Attrs: noredzone
declare void @kfree(i8*) #2

; Function Attrs: alwaysinline inlinehint noredzone nounwind
define internal i8* @kmalloc(i64 %size, i32 %flags) #3 {
entry:
  %size.addr = alloca i64, align 8
  %flags.addr = alloca i32, align 4
  store i64 %size, i64* %size.addr, align 8
  store i32 %flags, i32* %flags.addr, align 4
  %0 = load i64, i64* %size.addr, align 8
  %1 = load i32, i32* %flags.addr, align 4
  %call = call noalias i8* @__kmalloc(i64 %0, i32 %1) #4
  ret i8* %call
}

; Function Attrs: noredzone
declare noalias i8* @__kmalloc(i64, i32) #2

; Function Attrs: inlinehint noredzone nounwind
define internal void @dev_set_drvdata(%struct.device* %dev, i8* %data) #1 {
entry:
  %dev.addr = alloca %struct.device*, align 8
  %data.addr = alloca i8*, align 8
  store %struct.device* %dev, %struct.device** %dev.addr, align 8
  store i8* %data, i8** %data.addr, align 8
  %0 = load i8*, i8** %data.addr, align 8
  %1 = load %struct.device*, %struct.device** %dev.addr, align 8
  %driver_data = getelementptr inbounds %struct.device, %struct.device* %1, i32 0, i32 9
  store i8* %0, i8** %driver_data, align 8
  ret void
}

; Function Attrs: inlinehint noredzone nounwind
define internal void @kref_get(%struct.kref* %kref) #1 {
entry:
  %kref.addr = alloca %struct.kref*, align 8
  store %struct.kref* %kref, %struct.kref** %kref.addr, align 8
  %0 = load %struct.kref*, %struct.kref** %kref.addr, align 8
  %refcount = getelementptr inbounds %struct.kref, %struct.kref* %0, i32 0, i32 0
  call void @refcount_inc(%struct.refcount_struct* %refcount) #4
  ret void
}

; Function Attrs: noredzone
declare void @refcount_inc(%struct.refcount_struct*) #2

; Function Attrs: inlinehint noredzone nounwind
define internal i8* @usb_get_intfdata(%struct.usb_interface* %intf) #1 {
entry:
  %intf.addr = alloca %struct.usb_interface*, align 8
  store %struct.usb_interface* %intf, %struct.usb_interface** %intf.addr, align 8
  %0 = load %struct.usb_interface*, %struct.usb_interface** %intf.addr, align 8
  %dev = getelementptr inbounds %struct.usb_interface, %struct.usb_interface* %0, i32 0, i32 7
  %call = call i8* @dev_get_drvdata(%struct.device* %dev) #4
  ret i8* %call
}

; Function Attrs: noredzone
declare void @usbtv_audio_free(%struct.usbtv*) #2

; Function Attrs: noredzone
declare i32 @v4l2_device_put(%struct.v4l2_device*) #2

; Function Attrs: inlinehint noredzone nounwind
define internal i8* @dev_get_drvdata(%struct.device* %dev) #1 {
entry:
  %dev.addr = alloca %struct.device*, align 8
  store %struct.device* %dev, %struct.device** %dev.addr, align 8
  %0 = load %struct.device*, %struct.device** %dev.addr, align 8
  %driver_data = getelementptr inbounds %struct.device, %struct.device* %0, i32 0, i32 9
  %1 = load i8*, i8** %driver_data, align 8
  ret i8* %1
}

attributes #0 = { noredzone nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="-avx" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { inlinehint noredzone nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="-avx" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { noredzone "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="-avx" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { alwaysinline inlinehint noredzone nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="-avx" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { noredzone }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.7.0 (trunk)"}
