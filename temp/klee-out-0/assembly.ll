; ModuleID = 'usbtv_core.bc'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.usb_driver = type { i8*, i32 (%struct.usb_interface*, %struct.usb_device_id*)*, void (%struct.usb_interface*)*, i32 (%struct.usb_interface*, i32, i8*)*, i32 (%struct.usb_interface*, i32)*, i32 (%struct.usb_interface*)*, i32 (%struct.usb_interface*)*, i32 (%struct.usb_interface*)*, i32 (%struct.usb_interface*)*, %struct.usb_device_id*, %struct.usb_dynids, %struct.usbdrv_wrap, i8 }
%struct.usb_interface = type { %struct.usb_host_interface*, %struct.usb_host_interface*, i32, %struct.usb_interface_assoc_descriptor*, i32, i32, i8, %struct.device, %struct.device*, %struct.atomic_t, %struct.work_struct }
%struct.usb_host_interface = type { %struct.usb_interface_descriptor, i32, i8*, %struct.usb_host_endpoint*, i8* }
%struct.usb_interface_descriptor = type { i8, i8, i8, i8, i8, i8, i8, i8, i8 }
%struct.usb_host_endpoint = type <{ %struct.usb_endpoint_descriptor, %struct.usb_ss_ep_comp_descriptor, %struct.usb_ssp_isoc_ep_comp_descriptor, i8, %struct.list_head, i8*, %struct.ep_device*, i8*, i32, i32, i32, [4 x i8] }>
%struct.usb_endpoint_descriptor = type <{ i8, i8, i8, i8, i16, i8, i8, i8 }>
%struct.usb_ss_ep_comp_descriptor = type { i8, i8, i8, i8, i16 }
%struct.usb_ssp_isoc_ep_comp_descriptor = type { i8, i8, i16, i32 }
%struct.list_head = type { %struct.list_head*, %struct.list_head* }
%struct.ep_device = type opaque
%struct.usb_interface_assoc_descriptor = type { i8, i8, i8, i8, i8, i8, i8, i8 }
%struct.device = type { %struct.device*, %struct.device_private*, %struct.kobject, i8*, %struct.device_type*, %struct.mutex, %struct.bus_type*, %struct.device_driver*, i8*, i8*, %struct.dev_links_info, %struct.dev_pm_info, %struct.dev_pm_domain*, %struct.irq_domain*, %struct.list_head, i32, %struct.dma_map_ops*, i64*, i64, i64, %struct.device_dma_parameters*, %struct.list_head, %struct.dma_coherent_mem*, %struct.dev_archdata, %struct.device_node*, %struct.fwnode_handle*, i32, i32, %struct.spinlock, %struct.list_head, %struct.klist_node, %struct.class*, %struct.attribute_group**, void (%struct.device*)*, %struct.iommu_group*, %struct.iommu_fwspec*, i8 }
%struct.device_private = type opaque
%struct.kobject = type { i8*, %struct.list_head, %struct.kobject*, %struct.kset*, %struct.kobj_type*, %struct.kernfs_node*, %struct.kref, i8 }
%struct.kset = type { %struct.list_head, %struct.spinlock, %struct.kobject, %struct.kset_uevent_ops* }
%struct.kset_uevent_ops = type { i32 (%struct.kset*, %struct.kobject*)*, i8* (%struct.kset*, %struct.kobject*)*, i32 (%struct.kset*, %struct.kobject*, %struct.kobj_uevent_env*)* }
%struct.kobj_uevent_env = type { [3 x i8*], [32 x i8*], i32, [2048 x i8], i32 }
%struct.kobj_type = type { void (%struct.kobject*)*, %struct.sysfs_ops*, %struct.attribute**, %struct.kobj_ns_type_operations* (%struct.kobject*)*, i8* (%struct.kobject*)* }
%struct.sysfs_ops = type { i64 (%struct.kobject*, %struct.attribute*, i8*)*, i64 (%struct.kobject*, %struct.attribute*, i8*, i64)* }
%struct.attribute = type { i8*, i16 }
%struct.kobj_ns_type_operations = type { i32, i1 ()*, i8* ()*, i8* (%struct.sock*)*, i8* ()*, void (i8*)* }
%struct.sock = type opaque
%struct.kernfs_node = type { %struct.atomic_t, %struct.atomic_t, %struct.kernfs_node*, i8*, %struct.rb_node, i8*, i32, %union.anon, i8*, i16, i16, i32, %struct.kernfs_iattrs* }
%struct.rb_node = type { i64, %struct.rb_node*, %struct.rb_node* }
%union.anon = type { %struct.kernfs_elem_attr }
%struct.kernfs_elem_attr = type { %struct.kernfs_ops*, %struct.kernfs_open_node*, i64, %struct.kernfs_node* }
%struct.kernfs_ops = type { i32 (%struct.kernfs_open_file*)*, void (%struct.kernfs_open_file*)*, i32 (%struct.seq_file*, i8*)*, i8* (%struct.seq_file*, i64*)*, i8* (%struct.seq_file*, i8*, i64*)*, void (%struct.seq_file*, i8*)*, i64 (%struct.kernfs_open_file*, i8*, i64, i64)*, i64, i8, i64 (%struct.kernfs_open_file*, i8*, i64, i64)*, i32 (%struct.kernfs_open_file*, %struct.vm_area_struct*)* }
%struct.kernfs_open_file = type { %struct.kernfs_node*, %struct.file*, %struct.seq_file*, i8*, %struct.mutex, %struct.mutex, i32, %struct.list_head, i8*, i64, i8, %struct.vm_operations_struct* }
%struct.file = type { %union.anon.2, %struct.path, %struct.inode*, %struct.file_operations*, %struct.spinlock, %struct.atomic64_t, i32, i32, %struct.mutex, i64, %struct.fown_struct, %struct.cred*, %struct.file_ra_state, i64, i8*, i8*, %struct.list_head, %struct.list_head, %struct.address_space* }
%union.anon.2 = type { %struct.callback_head }
%struct.callback_head = type { %struct.callback_head*, void (%struct.callback_head*)* }
%struct.path = type { %struct.vfsmount*, %struct.dentry* }
%struct.vfsmount = type opaque
%struct.dentry = type { i32, %struct.seqcount, %struct.hlist_bl_node, %struct.dentry*, %struct.qstr, %struct.inode*, [32 x i8], %struct.lockref, %struct.dentry_operations*, %struct.super_block*, i64, i8*, %union.anon.44, %struct.list_head, %struct.list_head, %union.anon.45 }
%struct.seqcount = type { i32 }
%struct.hlist_bl_node = type { %struct.hlist_bl_node*, %struct.hlist_bl_node** }
%struct.qstr = type { %union.anon.3, i8* }
%union.anon.3 = type { i64 }
%struct.lockref = type { %union.anon.42 }
%union.anon.42 = type { i64 }
%struct.dentry_operations = type { i32 (%struct.dentry*, i32)*, i32 (%struct.dentry*, i32)*, i32 (%struct.dentry*, %struct.qstr*)*, i32 (%struct.dentry*, i32, i8*, %struct.qstr*)*, i32 (%struct.dentry*)*, i32 (%struct.dentry*)*, void (%struct.dentry*)*, void (%struct.dentry*)*, void (%struct.dentry*, %struct.inode*)*, i8* (%struct.dentry*, i8*, i32)*, %struct.vfsmount* (%struct.path*)*, i32 (%struct.path*, i1)*, %struct.dentry* (%struct.dentry*, %struct.inode*, i32)*, [24 x i8] }
%struct.super_block = type { %struct.list_head, i32, i8, i64, i64, %struct.file_system_type*, %struct.super_operations*, %struct.dquot_operations*, %struct.quotactl_ops*, %struct.export_operations*, i64, i64, i64, %struct.dentry*, %struct.rw_semaphore, i32, %struct.atomic_t, i8*, %struct.xattr_handler**, %struct.fscrypt_operations*, %struct.hlist_bl_head, %struct.list_head, %struct.block_device*, %struct.backing_dev_info*, %struct.mtd_info*, %struct.hlist_node, i32, %struct.quota_info, %struct.sb_writers, [32 x i8], [16 x i8], i8*, i32, i32, i32, %struct.mutex, i8*, i8*, %struct.dentry_operations*, i32, %struct.shrinker, %struct.atomic64_t, i32, %struct.workqueue_struct*, %struct.hlist_head, %struct.user_namespace*, %struct.list_lru, [56 x i8], %struct.list_lru, %struct.callback_head, %struct.work_struct, %struct.mutex, i32, [36 x i8], %struct.spinlock, %struct.list_head, %struct.spinlock, %struct.list_head, [16 x i8] }
%struct.file_system_type = type { i8*, i32, %struct.dentry* (%struct.file_system_type*, i32, i8*, i8*)*, void (%struct.super_block*)*, %struct.module*, %struct.file_system_type*, %struct.hlist_head, %struct.lock_class_key, %struct.lock_class_key, %struct.lock_class_key, [3 x %struct.lock_class_key], %struct.lock_class_key, %struct.lock_class_key, %struct.lock_class_key }
%struct.module = type { i32, %struct.list_head, [56 x i8], %struct.module_kobject, %struct.module_attribute*, i8*, i8*, %struct.kobject*, %struct.kernel_symbol*, i32*, i32, %struct.mutex, %struct.kernel_param*, i32, i32, %struct.kernel_symbol*, i32*, i8, %struct.kernel_symbol*, i32*, i32, i32, %struct.exception_table_entry*, i32 ()*, [40 x i8], %struct.module_layout, %struct.module_layout, %struct.mod_arch_specific, i64, i32, %struct.list_head, %struct.bug_entry*, %struct.mod_kallsyms*, %struct.mod_kallsyms, %struct.module_sect_attrs*, %struct.module_notes_attrs*, i8*, i8*, i32, i32, %struct.tracepoint**, i32, i8**, %struct.trace_event_call**, i32, %struct.trace_enum_map**, i32, %struct.list_head, %struct.list_head, void ()*, %struct.atomic_t, [12 x i8] }
%struct.module_kobject = type { %struct.kobject, %struct.module*, %struct.kobject*, %struct.module_param_attrs*, %struct.completion* }
%struct.module_param_attrs = type opaque
%struct.completion = type { i32, %struct.__wait_queue_head }
%struct.__wait_queue_head = type { %struct.spinlock, %struct.list_head }
%struct.module_attribute = type { %struct.attribute, i64 (%struct.module_attribute*, %struct.module_kobject*, i8*)*, i64 (%struct.module_attribute*, %struct.module_kobject*, i8*, i64)*, void (%struct.module*, i8*)*, i32 (%struct.module*)*, void (%struct.module*)* }
%struct.kernel_param = type { i8*, %struct.module*, %struct.kernel_param_ops*, i16, i8, i8, %union.anon.64 }
%struct.kernel_param_ops = type { i32, i32 (i8*, %struct.kernel_param*)*, i32 (i8*, %struct.kernel_param*)*, void (i8*)* }
%union.anon.64 = type { i8* }
%struct.kernel_symbol = type { i64, i8* }
%struct.exception_table_entry = type { i32, i32, i32 }
%struct.module_layout = type { i8*, i32, i32, i32, i32, %struct.mod_tree_node }
%struct.mod_tree_node = type { %struct.module*, %struct.latch_tree_node }
%struct.latch_tree_node = type { [2 x %struct.rb_node] }
%struct.mod_arch_specific = type {}
%struct.bug_entry = type { i32, i32, i16, i16 }
%struct.mod_kallsyms = type { %struct.elf64_sym*, i32, i8* }
%struct.elf64_sym = type { i32, i8, i8, i16, i64, i64 }
%struct.module_sect_attrs = type opaque
%struct.module_notes_attrs = type opaque
%struct.tracepoint = type { i8*, %struct.static_key, i32 ()*, void ()*, %struct.tracepoint_func* }
%struct.static_key = type { %struct.atomic_t }
%struct.tracepoint_func = type { i8*, i8*, i32 }
%struct.trace_event_call = type opaque
%struct.trace_enum_map = type opaque
%struct.lock_class_key = type {}
%struct.super_operations = type { %struct.inode* (%struct.super_block*)*, void (%struct.inode*)*, void (%struct.inode*, i32)*, i32 (%struct.inode*, %struct.writeback_control*)*, i32 (%struct.inode*)*, void (%struct.inode*)*, void (%struct.super_block*)*, i32 (%struct.super_block*, i32)*, i32 (%struct.super_block*)*, i32 (%struct.super_block*)*, i32 (%struct.super_block*)*, i32 (%struct.super_block*)*, i32 (%struct.dentry*, %struct.kstatfs*)*, i32 (%struct.super_block*, i32*, i8*)*, void (%struct.super_block*)*, i32 (%struct.seq_file*, %struct.dentry*)*, i32 (%struct.seq_file*, %struct.dentry*)*, i32 (%struct.seq_file*, %struct.dentry*)*, i32 (%struct.seq_file*, %struct.dentry*)*, i64 (%struct.super_block*, i32, i8*, i64, i64)*, i64 (%struct.super_block*, i32, i8*, i64, i64)*, %struct.dquot** (%struct.inode*)*, i32 (%struct.super_block*, %struct.page*, i32)*, i64 (%struct.super_block*, %struct.shrink_control*)*, i64 (%struct.super_block*, %struct.shrink_control*)* }
%struct.writeback_control = type opaque
%struct.kstatfs = type opaque
%struct.dquot = type { %struct.hlist_node, %struct.list_head, %struct.list_head, %struct.list_head, %struct.mutex, %struct.atomic_t, %struct.__wait_queue_head, %struct.super_block*, %struct.kqid, i64, i64, %struct.mem_dqblk }
%struct.kqid = type { %union.anon.4, i32 }
%union.anon.4 = type { %struct.kuid_t }
%struct.kuid_t = type { i32 }
%struct.mem_dqblk = type { i64, i64, i64, i64, i64, i64, i64, i64, i64 }
%struct.page = type { i64, %union.anon.5, %union.anon.28, %union.anon.29, %union.anon.33, %union.anon.36, [8 x i8] }
%union.anon.5 = type { %struct.address_space* }
%union.anon.28 = type { i64 }
%union.anon.29 = type { i64 }
%union.anon.33 = type { %struct.list_head }
%union.anon.36 = type { i64 }
%struct.shrink_control = type { i32, i64, i32, %struct.mem_cgroup* }
%struct.mem_cgroup = type opaque
%struct.dquot_operations = type { i32 (%struct.dquot*)*, %struct.dquot* (%struct.super_block*, i32)*, void (%struct.dquot*)*, i32 (%struct.dquot*)*, i32 (%struct.dquot*)*, i32 (%struct.dquot*)*, i32 (%struct.super_block*, i32)*, i64* (%struct.inode*)*, i32 (%struct.inode*, %struct.kprojid_t*)*, i32 (%struct.super_block*, %struct.kqid*)* }
%struct.kprojid_t = type { i32 }
%struct.quotactl_ops = type { i32 (%struct.super_block*, i32, i32, %struct.path*)*, i32 (%struct.super_block*, i32)*, i32 (%struct.super_block*, i32)*, i32 (%struct.super_block*, i32)*, i32 (%struct.super_block*, i32)*, i32 (%struct.super_block*, i32, %struct.qc_info*)*, i32 (%struct.super_block*, i64, %struct.qc_dqblk*)*, i32 (%struct.super_block*, %struct.kqid*, %struct.qc_dqblk*)*, i32 (%struct.super_block*, i64, %struct.qc_dqblk*)*, i32 (%struct.super_block*, %struct.qc_state*)*, i32 (%struct.super_block*, i32)* }
%struct.qc_info = type { i32, i32, i32, i32, i32, i32, i32, i32 }
%struct.qc_dqblk = type { i32, i64, i64, i64, i64, i64, i64, i64, i64, i32, i32, i64, i64, i64, i64, i32 }
%struct.qc_state = type { i32, [3 x %struct.qc_type_state] }
%struct.qc_type_state = type { i32, i32, i32, i32, i32, i32, i32, i64, i64, i64 }
%struct.export_operations = type opaque
%struct.rw_semaphore = type { %struct.atomic64_t, %struct.list_head, %struct.raw_spinlock, %struct.optimistic_spin_queue, %struct.task_struct* }
%struct.raw_spinlock = type { %struct.qspinlock }
%struct.qspinlock = type { %struct.atomic_t }
%struct.optimistic_spin_queue = type { %struct.atomic_t }
%struct.task_struct = type { %struct.thread_info, i64, i8*, %struct.atomic_t, i32, i32, %struct.llist_node, i32, i32, i32, i64, %struct.task_struct*, i32, i32, i32, i32, i32, i32, %struct.sched_class*, [16 x i8], %struct.sched_entity, %struct.sched_rt_entity, %struct.task_group*, %struct.sched_dl_entity, i32, i32, i32, %struct.cpumask, %struct.sched_info, %struct.list_head, %struct.plist_node, %struct.rb_node, %struct.mm_struct*, %struct.mm_struct*, %struct.vmacache, %struct.task_rss_stat, i32, i32, i32, i32, i64, i32, i8, [3 x i8], i8, i64, %struct.restart_block, i32, i32, %struct.task_struct*, %struct.task_struct*, %struct.list_head, %struct.list_head, %struct.task_struct*, %struct.list_head, %struct.list_head, [3 x %struct.pid_link], %struct.list_head, %struct.list_head, %struct.completion*, i32*, i32*, i64, i64, i64, %struct.prev_cputime, i64, i64, i64, i64, i64, i64, %struct.task_cputime, [3 x %struct.list_head], %struct.cred*, %struct.cred*, %struct.cred*, [16 x i8], %struct.nameidata*, %struct.sysv_sem, %struct.sysv_shm, %struct.fs_struct*, %struct.files_struct*, %struct.nsproxy*, %struct.signal_struct*, %struct.sighand_struct*, %struct.sigset_t, %struct.sigset_t, %struct.sigset_t, %struct.sigpending, i64, i64, i32, %struct.callback_head*, %struct.audit_context*, %struct.kuid_t, i32, %struct.seccomp, i32, i32, %struct.spinlock, %struct.raw_spinlock, %struct.wake_q_node, %struct.rb_root, %struct.rb_node*, %struct.rt_mutex_waiter*, i8*, %struct.bio_list*, %struct.blk_plug*, %struct.reclaim_state*, %struct.backing_dev_info*, %struct.io_context*, i64, %struct.siginfo*, %struct.task_io_accounting, i64, i64, i64, %struct.nodemask_t, %struct.seqcount, i32, i32, %struct.css_set*, %struct.list_head, %struct.robust_list_head*, %struct.compat_robust_list_head*, %struct.list_head, %struct.futex_pi_state*, [2 x %struct.perf_event_context*], %struct.mutex, %struct.list_head, %struct.mempolicy*, i16, i16, %struct.tlbflush_unmap_batch, %struct.callback_head, %struct.pipe_inode_info*, %struct.page_frag, %struct.task_delay_info*, i32, i32, i64, i64, i64, i64, i64, i32, %struct.task_struct*, %struct.vm_struct*, %struct.atomic_t, [28 x i8], %struct.thread_struct }
%struct.thread_info = type { i64 }
%struct.llist_node = type { %struct.llist_node* }
%struct.sched_class = type opaque
%struct.sched_entity = type { %struct.load_weight, %struct.rb_node, %struct.list_head, i32, i64, i64, i64, i64, i64, %struct.sched_statistics, i32, %struct.sched_entity*, %struct.cfs_rq*, %struct.cfs_rq*, [32 x i8], %struct.sched_avg, [24 x i8] }
%struct.load_weight = type { i64, i32 }
%struct.sched_statistics = type { i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64 }
%struct.cfs_rq = type opaque
%struct.sched_avg = type { i64, i64, i32, i32, i64, i64 }
%struct.sched_rt_entity = type { %struct.list_head, i64, i64, i32, i16, i16, %struct.sched_rt_entity* }
%struct.task_group = type opaque
%struct.sched_dl_entity = type { %struct.rb_node, i64, i64, i64, i64, i64, i64, i32, i32, i32, i32, %struct.hrtimer }
%struct.hrtimer = type { %struct.timerqueue_node, i64, i32 (%struct.hrtimer*)*, %struct.hrtimer_clock_base*, i8, i8 }
%struct.timerqueue_node = type { %struct.rb_node, i64 }
%struct.hrtimer_clock_base = type { %struct.hrtimer_cpu_base*, i32, i32, %struct.timerqueue_head, i64 ()*, i64, [16 x i8] }
%struct.hrtimer_cpu_base = type { %struct.raw_spinlock, %struct.seqcount, %struct.hrtimer*, i32, i32, i32, i8, i8, i8, i64, %struct.hrtimer*, i32, i32, i32, i32, [4 x %struct.hrtimer_clock_base] }
%struct.timerqueue_head = type { %struct.rb_root, %struct.timerqueue_node* }
%struct.cpumask = type { [1 x i64] }
%struct.sched_info = type { i64, i64, i64, i64 }
%struct.plist_node = type { i32, %struct.list_head, %struct.list_head }
%struct.mm_struct = type { %struct.vm_area_struct*, %struct.rb_root, i32, i64 (%struct.file*, i64, i64, i64, i64)*, i64, i64, i64, i64, %struct.pgd_t*, %struct.atomic_t, %struct.atomic_t, %struct.atomic64_t, %struct.atomic64_t, i32, %struct.spinlock, %struct.rw_semaphore, %struct.list_head, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, [46 x i64], %struct.mm_rss_stat, %struct.linux_binfmt*, [1 x %struct.cpumask], %struct.mm_context_t, i64, %struct.core_state*, %struct.spinlock, %struct.kioctx_table*, %struct.user_namespace*, %struct.file*, %struct.mmu_notifier_mm*, i8, %struct.uprobes_state, %struct.atomic64_t, %struct.work_struct }
%struct.vm_area_struct = type { i64, i64, %struct.vm_area_struct*, %struct.vm_area_struct*, %struct.rb_node, i64, %struct.mm_struct*, %struct.pgprot, i64, %struct.anon.6, %struct.list_head, %struct.anon_vma*, %struct.vm_operations_struct*, i64, %struct.file*, i8*, %struct.mempolicy*, %struct.vm_userfaultfd_ctx }
%struct.pgprot = type { i64 }
%struct.anon.6 = type { %struct.rb_node, i64 }
%struct.anon_vma = type opaque
%struct.vm_userfaultfd_ctx = type {}
%struct.pgd_t = type { i64 }
%struct.mm_rss_stat = type { [4 x %struct.atomic64_t] }
%struct.linux_binfmt = type opaque
%struct.mm_context_t = type { %struct.ldt_struct*, i16, %struct.mutex, i8*, %struct.vdso_image*, %struct.atomic_t, i16, i16 }
%struct.ldt_struct = type opaque
%struct.vdso_image = type { i8*, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64 }
%struct.core_state = type { %struct.atomic_t, %struct.core_thread, %struct.completion }
%struct.core_thread = type { %struct.task_struct*, %struct.core_thread* }
%struct.kioctx_table = type opaque
%struct.mmu_notifier_mm = type opaque
%struct.uprobes_state = type {}
%struct.vmacache = type { i32, [4 x %struct.vm_area_struct*] }
%struct.task_rss_stat = type { i32, [4 x i32] }
%struct.restart_block = type { i64 (%struct.restart_block*)*, %union.anon.7 }
%union.anon.7 = type { %struct.anon.8 }
%struct.anon.8 = type { i32*, i32, i32, i32, i64, i32* }
%struct.pid_link = type { %struct.hlist_node, %struct.pid* }
%struct.pid = type { %struct.atomic_t, i32, [3 x %struct.hlist_head], %struct.callback_head, [1 x %struct.upid] }
%struct.upid = type { i32, %struct.pid_namespace*, %struct.hlist_node }
%struct.pid_namespace = type opaque
%struct.prev_cputime = type { i64, i64, %struct.raw_spinlock }
%struct.task_cputime = type { i64, i64, i64 }
%struct.nameidata = type opaque
%struct.sysv_sem = type { %struct.sem_undo_list* }
%struct.sem_undo_list = type opaque
%struct.sysv_shm = type { %struct.list_head }
%struct.fs_struct = type opaque
%struct.files_struct = type opaque
%struct.nsproxy = type opaque
%struct.signal_struct = type opaque
%struct.sighand_struct = type opaque
%struct.sigset_t = type { [1 x i64] }
%struct.sigpending = type { %struct.list_head, %struct.sigset_t }
%struct.audit_context = type opaque
%struct.seccomp = type { i32, %struct.seccomp_filter* }
%struct.seccomp_filter = type opaque
%struct.wake_q_node = type { %struct.wake_q_node* }
%struct.rb_root = type { %struct.rb_node* }
%struct.rt_mutex_waiter = type opaque
%struct.bio_list = type opaque
%struct.blk_plug = type opaque
%struct.reclaim_state = type opaque
%struct.io_context = type opaque
%struct.siginfo = type { i32, i32, i32, %union.anon.11 }
%union.anon.11 = type { %struct.anon.15, [80 x i8] }
%struct.anon.15 = type { i32, i32, i32, i64, i64 }
%struct.task_io_accounting = type { i64, i64, i64, i64, i64, i64, i64 }
%struct.nodemask_t = type { [1 x i64] }
%struct.css_set = type opaque
%struct.robust_list_head = type opaque
%struct.compat_robust_list_head = type opaque
%struct.futex_pi_state = type opaque
%struct.perf_event_context = type opaque
%struct.mempolicy = type opaque
%struct.tlbflush_unmap_batch = type { %struct.cpumask, i8, i8 }
%struct.pipe_inode_info = type opaque
%struct.page_frag = type { %struct.page*, i32, i32 }
%struct.task_delay_info = type opaque
%struct.vm_struct = type opaque
%struct.thread_struct = type { [3 x %struct.desc_struct], i64, i64, i16, i16, i16, i16, i32, i64, i64, [4 x %struct.perf_event*], i64, i64, i64, i64, i64, i64*, i64, i32, %struct.mm_segment_t, i8, [15 x i8], %struct.fpu }
%struct.desc_struct = type { %union.anon.21 }
%union.anon.21 = type { %struct.anon.22 }
%struct.anon.22 = type { i32, i32 }
%struct.perf_event = type opaque
%struct.mm_segment_t = type { i64 }
%struct.fpu = type { i32, i8, i8, [58 x i8], %union.fpregs_state }
%union.fpregs_state = type { %struct.xregs_state, [3520 x i8] }
%struct.xregs_state = type { %struct.fxregs_state, %struct.xstate_header, [0 x i8] }
%struct.fxregs_state = type { i16, i16, i16, i16, %union.anon.24, i32, i32, [32 x i32], [64 x i32], [12 x i32], %union.anon.27 }
%union.anon.24 = type { %struct.anon.25 }
%struct.anon.25 = type { i64, i64 }
%union.anon.27 = type { [12 x i32] }
%struct.xstate_header = type { i64, i64, [6 x i64] }
%struct.xattr_handler = type opaque
%struct.fscrypt_operations = type opaque
%struct.hlist_bl_head = type { %struct.hlist_bl_node* }
%struct.block_device = type { i32, i32, %struct.inode*, %struct.super_block*, %struct.mutex, i8*, i8*, i32, i8, %struct.list_head, %struct.block_device*, i32, %struct.hd_struct*, i32, i32, %struct.gendisk*, %struct.request_queue*, %struct.backing_dev_info*, %struct.list_head, i64, i32, %struct.mutex }
%struct.hd_struct = type opaque
%struct.gendisk = type opaque
%struct.request_queue = type opaque
%struct.backing_dev_info = type opaque
%struct.mtd_info = type opaque
%struct.hlist_node = type { %struct.hlist_node*, %struct.hlist_node** }
%struct.quota_info = type { i32, %struct.mutex, [3 x %struct.inode*], [3 x %struct.mem_dqinfo], [3 x %struct.quota_format_ops*] }
%struct.mem_dqinfo = type { %struct.quota_format_type*, i32, %struct.list_head, i64, i32, i32, i64, i64, i8* }
%struct.quota_format_type = type { i32, %struct.quota_format_ops*, %struct.module*, %struct.quota_format_type* }
%struct.quota_format_ops = type { i32 (%struct.super_block*, i32)*, i32 (%struct.super_block*, i32)*, i32 (%struct.super_block*, i32)*, i32 (%struct.super_block*, i32)*, i32 (%struct.dquot*)*, i32 (%struct.dquot*)*, i32 (%struct.dquot*)*, i32 (%struct.super_block*, %struct.kqid*)* }
%struct.sb_writers = type { i32, %struct.__wait_queue_head, [3 x %struct.percpu_rw_semaphore] }
%struct.percpu_rw_semaphore = type { %struct.rcu_sync, i32*, %struct.rw_semaphore, %struct.rcuwait, i32 }
%struct.rcu_sync = type { i32, i32, %struct.__wait_queue_head, i32, %struct.callback_head, i32 }
%struct.rcuwait = type { %struct.task_struct* }
%struct.shrinker = type { i64 (%struct.shrinker*, %struct.shrink_control*)*, i64 (%struct.shrinker*, %struct.shrink_control*)*, i32, i64, i64, %struct.list_head, %struct.atomic64_t* }
%struct.workqueue_struct = type opaque
%struct.hlist_head = type { %struct.hlist_node* }
%struct.user_namespace = type opaque
%struct.list_lru = type { %struct.list_lru_node* }
%struct.list_lru_node = type { %struct.spinlock, %struct.list_lru_one, [32 x i8] }
%struct.list_lru_one = type { %struct.list_head, i64 }
%union.anon.44 = type { %struct.list_head }
%union.anon.45 = type { %struct.hlist_node }
%struct.inode = type { i16, i16, %struct.kuid_t, %struct.kgid_t, i32, %struct.posix_acl*, %struct.posix_acl*, %struct.inode_operations*, %struct.super_block*, %struct.address_space*, i8*, i64, %union.anon.37, i32, i64, %struct.timespec, %struct.timespec, %struct.timespec, %struct.spinlock, i16, i32, i64, i64, %struct.rw_semaphore, i64, i64, %struct.hlist_node, %struct.list_head, %struct.list_head, %struct.list_head, %struct.list_head, %union.anon.38, i64, %struct.atomic_t, %struct.atomic_t, %struct.atomic_t, %struct.file_operations*, %struct.file_lock_context*, %struct.address_space, %struct.list_head, %union.anon.41, i32, i32, %struct.hlist_head, i8* }
%struct.kgid_t = type { i32 }
%struct.posix_acl = type opaque
%struct.inode_operations = type { %struct.dentry* (%struct.inode*, %struct.dentry*, i32)*, i8* (%struct.dentry*, %struct.inode*, %struct.delayed_call*)*, i32 (%struct.inode*, i32)*, %struct.posix_acl* (%struct.inode*, i32)*, i32 (%struct.dentry*, i8*, i32)*, i32 (%struct.inode*, %struct.dentry*, i16, i1)*, i32 (%struct.dentry*, %struct.inode*, %struct.dentry*)*, i32 (%struct.inode*, %struct.dentry*)*, i32 (%struct.inode*, %struct.dentry*, i8*)*, i32 (%struct.inode*, %struct.dentry*, i16)*, i32 (%struct.inode*, %struct.dentry*)*, i32 (%struct.inode*, %struct.dentry*, i16, i32)*, i32 (%struct.inode*, %struct.dentry*, %struct.inode*, %struct.dentry*, i32)*, i32 (%struct.dentry*, %struct.iattr*)*, i32 (%struct.path*, %struct.kstat*, i32, i32)*, i64 (%struct.dentry*, i8*, i64)*, i32 (%struct.inode*, %struct.fiemap_extent_info*, i64, i64)*, i32 (%struct.inode*, %struct.timespec*, i32)*, i32 (%struct.inode*, %struct.dentry*, %struct.file*, i32, i16, i32*)*, i32 (%struct.inode*, %struct.dentry*, i16)*, i32 (%struct.inode*, %struct.posix_acl*, i32)*, [24 x i8] }
%struct.delayed_call = type { void (i8*)*, i8* }
%struct.iattr = type { i32, i16, %struct.kuid_t, %struct.kgid_t, i64, %struct.timespec, %struct.timespec, %struct.timespec, %struct.file* }
%struct.kstat = type { i32, i16, i32, i32, i64, i64, i32, i32, %struct.kuid_t, %struct.kgid_t, i64, %struct.timespec, %struct.timespec, %struct.timespec, %struct.timespec, i64 }
%struct.fiemap_extent_info = type { i32, i32, i32, %struct.fiemap_extent* }
%struct.fiemap_extent = type { i64, i64, i64, [2 x i64], i32, [3 x i32] }
%union.anon.37 = type { i32 }
%struct.timespec = type { i64, i64 }
%union.anon.38 = type { %struct.callback_head }
%struct.file_lock_context = type { %struct.spinlock, %struct.list_head, %struct.list_head, %struct.list_head }
%struct.address_space = type { %struct.inode*, %struct.radix_tree_root, %struct.spinlock, %struct.atomic_t, %struct.rb_root, %struct.rw_semaphore, i64, i64, i64, %struct.address_space_operations*, i64, %struct.spinlock, i32, %struct.list_head, i8* }
%struct.radix_tree_root = type { i32, %struct.radix_tree_node* }
%struct.radix_tree_node = type { i8, i8, i8, i8, %struct.radix_tree_node*, %struct.radix_tree_root*, %union.anon.0, [64 x i8*], [3 x [1 x i64]] }
%union.anon.0 = type { %struct.list_head }
%struct.address_space_operations = type { i32 (%struct.page*, %struct.writeback_control*)*, i32 (%struct.file*, %struct.page*)*, i32 (%struct.address_space*, %struct.writeback_control*)*, i32 (%struct.page*)*, i32 (%struct.file*, %struct.address_space*, %struct.list_head*, i32)*, i32 (%struct.file*, %struct.address_space*, i64, i32, i32, %struct.page**, i8**)*, i32 (%struct.file*, %struct.address_space*, i64, i32, i32, %struct.page*, i8*)*, i64 (%struct.address_space*, i64)*, void (%struct.page*, i32, i32)*, i32 (%struct.page*, i32)*, void (%struct.page*)*, i64 (%struct.kiocb*, %struct.iov_iter*)*, i32 (%struct.address_space*, %struct.page*, %struct.page*, i32)*, i1 (%struct.page*, i32)*, void (%struct.page*)*, i32 (%struct.page*)*, i32 (%struct.page*, i64, i64)*, void (%struct.page*, i8*, i8*)*, i32 (%struct.address_space*, %struct.page*)*, i32 (%struct.swap_info_struct*, %struct.file*, i64*)*, void (%struct.file*)* }
%struct.kiocb = type { %struct.file*, i64, void (%struct.kiocb*, i64, i64)*, i8*, i32 }
%struct.iov_iter = type opaque
%struct.swap_info_struct = type opaque
%union.anon.41 = type { %struct.pipe_inode_info* }
%struct.file_operations = type { %struct.module*, i64 (%struct.file*, i64, i32)*, i64 (%struct.file*, i8*, i64, i64*)*, i64 (%struct.file*, i8*, i64, i64*)*, i64 (%struct.kiocb*, %struct.iov_iter*)*, i64 (%struct.kiocb*, %struct.iov_iter*)*, i32 (%struct.file*, %struct.dir_context*)*, i32 (%struct.file*, %struct.dir_context*)*, i32 (%struct.file*, %struct.poll_table_struct*)*, i64 (%struct.file*, i32, i64)*, i64 (%struct.file*, i32, i64)*, i32 (%struct.file*, %struct.vm_area_struct*)*, i32 (%struct.inode*, %struct.file*)*, i32 (%struct.file*, i8*)*, i32 (%struct.inode*, %struct.file*)*, i32 (%struct.file*, i64, i64, i32)*, i32 (i32, %struct.file*, i32)*, i32 (%struct.file*, i32, %struct.file_lock*)*, i64 (%struct.file*, %struct.page*, i32, i64, i64*, i32)*, i64 (%struct.file*, i64, i64, i64, i64)*, i32 (i32)*, i32 (%struct.file*, i32, %struct.file_lock*)*, i64 (%struct.pipe_inode_info*, %struct.file*, i64*, i64, i32)*, i64 (%struct.file*, i64*, %struct.pipe_inode_info*, i64, i32)*, i32 (%struct.file*, i64, %struct.file_lock**, i8**)*, i64 (%struct.file*, i32, i64, i64)*, void (%struct.seq_file*, %struct.file*)*, i64 (%struct.file*, i64, %struct.file*, i64, i64, i32)*, i32 (%struct.file*, i64, %struct.file*, i64, i64)*, i64 (%struct.file*, i64, i64, %struct.file*, i64)* }
%struct.dir_context = type { i32 (%struct.dir_context*, i8*, i32, i64, i64, i32)*, i64 }
%struct.poll_table_struct = type { void (%struct.file*, %struct.__wait_queue_head*, %struct.poll_table_struct*)*, i64 }
%struct.file_lock = type { %struct.file_lock*, %struct.list_head, %struct.hlist_node, %struct.list_head, i8*, i32, i8, i32, i32, %struct.pid*, %struct.__wait_queue_head, %struct.file*, i64, i64, %struct.fasync_struct*, i64, i64, %struct.file_lock_operations*, %struct.lock_manager_operations*, %union.anon.39 }
%struct.fasync_struct = type { %struct.spinlock, i32, i32, %struct.fasync_struct*, %struct.file*, %struct.callback_head }
%struct.file_lock_operations = type { void (%struct.file_lock*, %struct.file_lock*)*, void (%struct.file_lock*)* }
%struct.lock_manager_operations = type { i32 (%struct.file_lock*, %struct.file_lock*)*, i64 (%struct.file_lock*)*, i8* (i8*)*, void (i8*)*, void (%struct.file_lock*)*, i32 (%struct.file_lock*, i32)*, i1 (%struct.file_lock*)*, i32 (%struct.file_lock*, i32, %struct.list_head*)*, void (%struct.file_lock*, i8**)* }
%union.anon.39 = type { %struct.nfs_lock_info }
%struct.nfs_lock_info = type { i32, %struct.nlm_lockowner*, %struct.list_head }
%struct.nlm_lockowner = type opaque
%struct.atomic64_t = type { i64 }
%struct.fown_struct = type { %struct.rwlock_t, %struct.pid*, i32, %struct.kuid_t, %struct.kuid_t, i32 }
%struct.rwlock_t = type { %struct.qrwlock }
%struct.qrwlock = type { %struct.atomic_t, %struct.qspinlock }
%struct.cred = type opaque
%struct.file_ra_state = type { i64, i32, i32, i32, i32, i64 }
%struct.seq_file = type opaque
%struct.vm_operations_struct = type { void (%struct.vm_area_struct*)*, void (%struct.vm_area_struct*)*, i32 (%struct.vm_area_struct*)*, i32 (%struct.vm_fault*)*, i32 (%struct.vm_fault*, i32)*, void (%struct.vm_fault*, i64, i64)*, i32 (%struct.vm_fault*)*, i32 (%struct.vm_fault*)*, i32 (%struct.vm_area_struct*, i64, i8*, i32, i32)*, i8* (%struct.vm_area_struct*)*, i32 (%struct.vm_area_struct*, %struct.mempolicy*)*, %struct.mempolicy* (%struct.vm_area_struct*, i64)*, %struct.page* (%struct.vm_area_struct*, i64)* }
%struct.vm_fault = type { %struct.vm_area_struct*, i32, i32, i64, i64, %struct.pmd_t*, %struct.pud_t*, %struct.pte_t, %struct.page*, %struct.mem_cgroup*, %struct.page*, %struct.pte_t*, %struct.spinlock*, %struct.page* }
%struct.pmd_t = type { i64 }
%struct.pud_t = type { i64 }
%struct.pte_t = type { i64 }
%struct.kernfs_open_node = type opaque
%struct.kernfs_iattrs = type opaque
%struct.kref = type { %struct.refcount_struct }
%struct.refcount_struct = type { %struct.atomic_t }
%struct.device_type = type { i8*, %struct.attribute_group**, i32 (%struct.device*, %struct.kobj_uevent_env*)*, i8* (%struct.device*, i16*, %struct.kuid_t*, %struct.kgid_t*)*, void (%struct.device*)*, %struct.dev_pm_ops* }
%struct.dev_pm_ops = type { i32 (%struct.device*)*, void (%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*)* }
%struct.mutex = type { %struct.atomic64_t, %struct.spinlock, %struct.optimistic_spin_queue, %struct.list_head }
%struct.bus_type = type { i8*, i8*, %struct.device*, %struct.device_attribute*, %struct.attribute_group**, %struct.attribute_group**, %struct.attribute_group**, i32 (%struct.device*, %struct.device_driver*)*, i32 (%struct.device*, %struct.kobj_uevent_env*)*, i32 (%struct.device*)*, i32 (%struct.device*)*, void (%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*)*, i32 (%struct.device*, i32)*, i32 (%struct.device*)*, i32 (%struct.device*)*, %struct.dev_pm_ops*, %struct.iommu_ops*, %struct.subsys_private*, %struct.lock_class_key }
%struct.device_attribute = type { %struct.attribute, i64 (%struct.device*, %struct.device_attribute*, i8*)*, i64 (%struct.device*, %struct.device_attribute*, i8*, i64)* }
%struct.iommu_ops = type opaque
%struct.subsys_private = type opaque
%struct.device_driver = type { i8*, %struct.bus_type*, %struct.module*, i8*, i8, i32, %struct.of_device_id*, %struct.acpi_device_id*, i32 (%struct.device*)*, i32 (%struct.device*)*, void (%struct.device*)*, i32 (%struct.device*, i32)*, i32 (%struct.device*)*, %struct.attribute_group**, %struct.dev_pm_ops*, %struct.driver_private* }
%struct.of_device_id = type { [32 x i8], [32 x i8], [128 x i8], i8* }
%struct.acpi_device_id = type { [9 x i8], i64, i32, i32 }
%struct.driver_private = type opaque
%struct.dev_links_info = type { %struct.list_head, %struct.list_head, i32 }
%struct.dev_pm_info = type { %struct.pm_message, i16, %struct.spinlock, %struct.list_head, %struct.completion, %struct.wakeup_source*, i8, %struct.timer_list, i64, %struct.work_struct, %struct.__wait_queue_head, %struct.wake_irq*, %struct.atomic_t, %struct.atomic_t, i16, i32, i32, i32, i32, i32, i64, i64, i64, i64, %struct.pm_subsys_data*, void (%struct.device*, i32)*, %struct.dev_pm_qos* }
%struct.pm_message = type { i32 }
%struct.wakeup_source = type { i8*, %struct.list_head, %struct.spinlock, %struct.wake_irq*, %struct.timer_list, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i8 }
%struct.timer_list = type { %struct.hlist_node, i64, void (i64)*, i64, i32 }
%struct.wake_irq = type opaque
%struct.pm_subsys_data = type { %struct.spinlock, i32, %struct.list_head }
%struct.dev_pm_qos = type opaque
%struct.dev_pm_domain = type { %struct.dev_pm_ops, void (%struct.device*, i1)*, i32 (%struct.device*)*, void (%struct.device*)*, void (%struct.device*)* }
%struct.irq_domain = type opaque
%struct.dma_map_ops = type { i8* (%struct.device*, i64, i64*, i32, i64)*, void (%struct.device*, i64, i8*, i64, i64)*, i32 (%struct.device*, %struct.vm_area_struct*, i8*, i64, i64, i64)*, i32 (%struct.device*, %struct.sg_table*, i8*, i64, i64, i64)*, i64 (%struct.device*, %struct.page*, i64, i64, i32, i64)*, void (%struct.device*, i64, i64, i32, i64)*, i32 (%struct.device*, %struct.scatterlist*, i32, i32, i64)*, void (%struct.device*, %struct.scatterlist*, i32, i32, i64)*, i64 (%struct.device*, i64, i64, i32, i64)*, void (%struct.device*, i64, i64, i32, i64)*, void (%struct.device*, i64, i64, i32)*, void (%struct.device*, i64, i64, i32)*, void (%struct.device*, %struct.scatterlist*, i32, i32)*, void (%struct.device*, %struct.scatterlist*, i32, i32)*, i32 (%struct.device*, i64)*, i32 (%struct.device*, i64)*, i32 (%struct.device*, i64)*, i32 }
%struct.sg_table = type { %struct.scatterlist*, i32, i32 }
%struct.scatterlist = type { i64, i32, i32, i64, i32 }
%struct.device_dma_parameters = type { i32, i64 }
%struct.dma_coherent_mem = type opaque
%struct.dev_archdata = type { i8* }
%struct.device_node = type opaque
%struct.fwnode_handle = type opaque
%struct.spinlock = type { %union.anon.1 }
%union.anon.1 = type { %struct.raw_spinlock }
%struct.klist_node = type { i8*, %struct.list_head, %struct.kref }
%struct.class = type { i8*, %struct.module*, %struct.class_attribute*, %struct.attribute_group**, %struct.attribute_group**, %struct.kobject*, i32 (%struct.device*, %struct.kobj_uevent_env*)*, i8* (%struct.device*, i16*)*, void (%struct.class*)*, void (%struct.device*)*, i32 (%struct.device*, i32)*, i32 (%struct.device*)*, %struct.kobj_ns_type_operations*, i8* (%struct.device*)*, %struct.dev_pm_ops*, %struct.subsys_private* }
%struct.class_attribute = type { %struct.attribute, i64 (%struct.class*, %struct.class_attribute*, i8*)*, i64 (%struct.class*, %struct.class_attribute*, i8*, i64)* }
%struct.attribute_group = type { i8*, i16 (%struct.kobject*, %struct.attribute*, i32)*, i16 (%struct.kobject*, %struct.bin_attribute*, i32)*, %struct.attribute**, %struct.bin_attribute** }
%struct.bin_attribute = type { %struct.attribute, i64, i8*, i64 (%struct.file*, %struct.kobject*, %struct.bin_attribute*, i8*, i64, i64)*, i64 (%struct.file*, %struct.kobject*, %struct.bin_attribute*, i8*, i64, i64)*, i32 (%struct.file*, %struct.kobject*, %struct.bin_attribute*, %struct.vm_area_struct*)* }
%struct.iommu_group = type opaque
%struct.iommu_fwspec = type opaque
%struct.atomic_t = type { i32 }
%struct.work_struct = type { %struct.atomic64_t, %struct.list_head, void (%struct.work_struct*)* }
%struct.usb_device_id = type { i16, i16, i16, i16, i16, i8, i8, i8, i8, i8, i8, i8, i64 }
%struct.usb_dynids = type { %struct.spinlock, %struct.list_head }
%struct.usbdrv_wrap = type { %struct.device_driver, i32 }
%struct.usbtv = type { %struct.device*, %struct.usb_device*, %struct.v4l2_device, %struct.v4l2_ctrl_handler, %struct.video_device, %struct.vb2_queue, %struct.mutex, %struct.mutex, %struct.spinlock, %struct.list_head, i32, i32, i32, i64, i32, i32, i32, i32, i32, i32, [16 x %struct.urb*], %struct.snd_card*, %struct.snd_pcm_substream*, %struct.atomic_t, %struct.work_struct, %struct.urb*, i64, i64 }
%struct.usb_device = type { i32, [16 x i8], i32, i32, i32, %struct.usb_tt*, i32, [2 x i32], %struct.usb_device*, %struct.usb_bus*, %struct.usb_host_endpoint, %struct.device, %struct.usb_device_descriptor, %struct.usb_host_bos*, %struct.usb_host_config*, %struct.usb_host_config*, [16 x %struct.usb_host_endpoint*], [16 x %struct.usb_host_endpoint*], i8**, i16, i8, i8, i16, i32, i8*, i8*, i8*, %struct.list_head, i32, i32, %struct.atomic_t, i64, i64, i8, %struct.wusb_dev*, i32, i32, %struct.usb2_lpm_parameters, %struct.usb3_lpm_parameters, %struct.usb3_lpm_parameters, i32 }
%struct.usb_tt = type opaque
%struct.usb_bus = type { %struct.device*, i32, i8*, i8, i8, i8, i8, i32, i32, %struct.mutex, %struct.usb_devmap, %struct.usb_device*, %struct.usb_bus*, i32, i32, i32, i32, %struct.mon_bus*, i32 }
%struct.usb_devmap = type { [2 x i64] }
%struct.mon_bus = type opaque
%struct.usb_device_descriptor = type { i8, i8, i16, i8, i8, i8, i8, i16, i16, i16, i8, i8, i8, i8 }
%struct.usb_host_bos = type { %struct.usb_bos_descriptor*, %struct.usb_ext_cap_descriptor*, %struct.usb_ss_cap_descriptor*, %struct.usb_ssp_cap_descriptor*, %struct.usb_ss_container_id_descriptor*, %struct.usb_ptm_cap_descriptor* }
%struct.usb_bos_descriptor = type <{ i8, i8, i16, i8 }>
%struct.usb_ext_cap_descriptor = type <{ i8, i8, i8, i32 }>
%struct.usb_ss_cap_descriptor = type { i8, i8, i8, i8, i16, i8, i8, i16 }
%struct.usb_ssp_cap_descriptor = type { i8, i8, i8, i8, i32, i16, i16, [1 x i32] }
%struct.usb_ss_container_id_descriptor = type { i8, i8, i8, i8, [16 x i8] }
%struct.usb_ptm_cap_descriptor = type { i8, i8, i8 }
%struct.usb_host_config = type { %struct.usb_config_descriptor, i8*, [16 x %struct.usb_interface_assoc_descriptor*], [32 x %struct.usb_interface*], [32 x %struct.usb_interface_cache*], i8*, i32 }
%struct.usb_config_descriptor = type <{ i8, i8, i16, i8, i8, i8, i8, i8 }>
%struct.usb_interface_cache = type { i32, %struct.kref, [0 x %struct.usb_host_interface] }
%struct.wusb_dev = type opaque
%struct.usb2_lpm_parameters = type { i32, i32 }
%struct.usb3_lpm_parameters = type { i32, i32, i32, i32 }
%struct.v4l2_device = type { %struct.device*, %struct.media_device*, %struct.list_head, %struct.spinlock, [36 x i8], void (%struct.v4l2_subdev*, i32, i8*)*, %struct.v4l2_ctrl_handler*, %struct.v4l2_prio_state, %struct.kref, void (%struct.v4l2_device*)* }
%struct.media_device = type { %struct.device*, %struct.media_devnode*, [32 x i8], [32 x i8], [40 x i8], [32 x i8], i32, i32, i64, i32, %struct.ida, i32, %struct.list_head, %struct.list_head, %struct.list_head, %struct.list_head, %struct.list_head, %struct.mutex, %struct.media_graph, i8*, i32 (%struct.media_entity*, %struct.media_pipeline*)*, void (%struct.media_entity*)*, %struct.media_device_ops* }
%struct.media_devnode = type { %struct.media_device*, %struct.media_file_operations*, %struct.device, %struct.cdev, %struct.device*, i32, i64, void (%struct.media_devnode*)* }
%struct.media_file_operations = type { %struct.module*, i64 (%struct.file*, i8*, i64, i64*)*, i64 (%struct.file*, i8*, i64, i64*)*, i32 (%struct.file*, %struct.poll_table_struct*)*, i64 (%struct.file*, i32, i64)*, i64 (%struct.file*, i32, i64)*, i32 (%struct.file*)*, i32 (%struct.file*)* }
%struct.cdev = type { %struct.kobject, %struct.module*, %struct.file_operations*, %struct.list_head, i32, i32 }
%struct.ida = type { %struct.radix_tree_root }
%struct.media_graph = type { [16 x %struct.anon.46], %struct.media_entity_enum, i32 }
%struct.anon.46 = type { %struct.media_entity*, %struct.list_head* }
%struct.media_entity = type { %struct.media_gobj, i8*, i32, i32, i64, i16, i16, i16, i32, %struct.media_pad*, %struct.list_head, %struct.media_entity_operations*, i32, i32, %struct.media_pipeline*, %union.anon.49 }
%struct.media_gobj = type { %struct.media_device*, i32, %struct.list_head }
%struct.media_pad = type { %struct.media_gobj, %struct.media_entity*, i16, i64 }
%struct.media_entity_operations = type { i32 (%struct.media_entity*, %struct.media_pad*, %struct.media_pad*, i32)*, i32 (%struct.media_link*)* }
%struct.media_link = type { %struct.media_gobj, %struct.list_head, %union.anon.47, %union.anon.48, %struct.media_link*, i64, i8 }
%union.anon.47 = type { %struct.media_gobj* }
%union.anon.48 = type { %struct.media_gobj* }
%struct.media_pipeline = type { i32, %struct.media_graph }
%union.anon.49 = type { %struct.anon.50 }
%struct.anon.50 = type { i32, i32 }
%struct.media_entity_enum = type { i64*, i32 }
%struct.media_device_ops = type { i32 (%struct.media_link*, i32, i32)* }
%struct.v4l2_subdev = type { %struct.media_entity, %struct.list_head, %struct.module*, i8, i32, %struct.v4l2_device*, %struct.v4l2_subdev_ops*, %struct.v4l2_subdev_internal_ops*, %struct.v4l2_ctrl_handler*, [32 x i8], i32, i8*, i8*, %struct.video_device*, %struct.device*, %struct.device_node*, %struct.list_head, %struct.v4l2_async_subdev*, %struct.v4l2_async_notifier*, %struct.v4l2_subdev_platform_data* }
%struct.v4l2_subdev_ops = type { %struct.v4l2_subdev_core_ops*, %struct.v4l2_subdev_tuner_ops*, %struct.v4l2_subdev_audio_ops*, %struct.v4l2_subdev_video_ops*, %struct.v4l2_subdev_vbi_ops*, %struct.v4l2_subdev_ir_ops*, %struct.v4l2_subdev_sensor_ops*, %struct.v4l2_subdev_pad_ops* }
%struct.v4l2_subdev_core_ops = type { i32 (%struct.v4l2_subdev*)*, i32 (%struct.v4l2_subdev*, i64, %struct.v4l2_subdev_io_pin_config*)*, i32 (%struct.v4l2_subdev*, i32)*, i32 (%struct.v4l2_subdev*)*, i32 (%struct.v4l2_subdev*, i32)*, i32 (%struct.v4l2_subdev*, i32)*, i64 (%struct.v4l2_subdev*, i32, i8*)*, i64 (%struct.v4l2_subdev*, i32, i64)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_dbg_register*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_dbg_register*)*, i32 (%struct.v4l2_subdev*, i32)*, i32 (%struct.v4l2_subdev*, i32, i8*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_fh*, %struct.v4l2_event_subscription*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_fh*, %struct.v4l2_event_subscription*)* }
%struct.v4l2_subdev_io_pin_config = type { i32, i8, i8, i8, i8 }
%struct.v4l2_dbg_register = type { %struct.v4l2_dbg_match, i32, i64, i64 }
%struct.v4l2_dbg_match = type { i32, %union.anon.51 }
%union.anon.51 = type { i32, [28 x i8] }
%struct.v4l2_fh = type { %struct.list_head, %struct.video_device*, %struct.v4l2_ctrl_handler*, i32, %struct.__wait_queue_head, %struct.list_head, %struct.list_head, i32, i32, %struct.v4l2_m2m_ctx* }
%struct.v4l2_m2m_ctx = type opaque
%struct.v4l2_event_subscription = type { i32, i32, i32, [5 x i32] }
%struct.v4l2_subdev_tuner_ops = type { i32 (%struct.v4l2_subdev*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_frequency*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_frequency*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_frequency_band*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_tuner*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_tuner*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_modulator*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_modulator*)*, i32 (%struct.v4l2_subdev*, %struct.tuner_setup*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_priv_tun_config*)* }
%struct.v4l2_frequency = type { i32, i32, i32, [8 x i32] }
%struct.v4l2_frequency_band = type { i32, i32, i32, i32, i32, i32, i32, [9 x i32] }
%struct.v4l2_tuner = type { i32, [32 x i8], i32, i32, i32, i32, i32, i32, i32, i32, [4 x i32] }
%struct.v4l2_modulator = type { i32, [32 x i8], i32, i32, i32, i32, i32, [3 x i32] }
%struct.tuner_setup = type opaque
%struct.v4l2_priv_tun_config = type { i32, i8* }
%struct.v4l2_subdev_audio_ops = type { i32 (%struct.v4l2_subdev*, i32)*, i32 (%struct.v4l2_subdev*, i32)*, i32 (%struct.v4l2_subdev*, i32, i32, i32)*, i32 (%struct.v4l2_subdev*, i32)* }
%struct.v4l2_subdev_video_ops = type { i32 (%struct.v4l2_subdev*, i32, i32, i32)*, i32 (%struct.v4l2_subdev*, i32, i32)*, i32 (%struct.v4l2_subdev*, i64*)*, i32 (%struct.v4l2_subdev*, i64)*, i32 (%struct.v4l2_subdev*, i64)*, i32 (%struct.v4l2_subdev*, i64*)*, i32 (%struct.v4l2_subdev*, i64*)*, i32 (%struct.v4l2_subdev*, i64*)*, i32 (%struct.v4l2_subdev*, i64*)*, i32 (%struct.v4l2_subdev*, i32*)*, i32 (%struct.v4l2_subdev*, i32)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_fract*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_streamparm*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_streamparm*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_subdev_frame_interval*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_subdev_frame_interval*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_dv_timings*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_dv_timings*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_dv_timings*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_mbus_config*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_mbus_config*)*, i32 (%struct.v4l2_subdev*, i8*, i32*)* }
%struct.v4l2_fract = type { i32, i32 }
%struct.v4l2_streamparm = type { i32, %union.anon.56 }
%union.anon.56 = type { %struct.v4l2_captureparm, [160 x i8] }
%struct.v4l2_captureparm = type { i32, i32, %struct.v4l2_fract, i32, i32, [4 x i32] }
%struct.v4l2_subdev_frame_interval = type { i32, %struct.v4l2_fract, [9 x i32] }
%struct.v4l2_dv_timings = type { i32, %union.anon.57 }
%union.anon.57 = type { [32 x i32] }
%struct.v4l2_mbus_config = type { i32, i32 }
%struct.v4l2_subdev_vbi_ops = type { i32 (%struct.v4l2_subdev*, %struct.v4l2_decode_vbi_line*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_sliced_vbi_data*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_sliced_vbi_data*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_sliced_vbi_cap*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_vbi_format*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_sliced_vbi_format*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_sliced_vbi_format*)* }
%struct.v4l2_decode_vbi_line = type { i32, i8*, i32, i32 }
%struct.v4l2_sliced_vbi_data = type { i32, i32, i32, i32, [48 x i8] }
%struct.v4l2_sliced_vbi_cap = type { i16, [2 x [24 x i16]], i32, [3 x i32] }
%struct.v4l2_vbi_format = type { i32, i32, i32, i32, [2 x i32], [2 x i32], i32, [2 x i32] }
%struct.v4l2_sliced_vbi_format = type { i16, [2 x [24 x i16]], i32, [2 x i32] }
%struct.v4l2_subdev_ir_ops = type { i32 (%struct.v4l2_subdev*, i8*, i64, i64*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_subdev_ir_parameters*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_subdev_ir_parameters*)*, i32 (%struct.v4l2_subdev*, i8*, i64, i64*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_subdev_ir_parameters*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_subdev_ir_parameters*)* }
%struct.v4l2_subdev_ir_parameters = type { i32, i32, i8, i8, i8, i8, i32, i32, i32, i8, i8, i32, i32, i32, i32 }
%struct.v4l2_subdev_sensor_ops = type { i32 (%struct.v4l2_subdev*, i32*)*, i32 (%struct.v4l2_subdev*, i32*)* }
%struct.v4l2_subdev_pad_ops = type { i32 (%struct.v4l2_subdev*, %struct.v4l2_subdev_pad_config*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_subdev_pad_config*, %struct.v4l2_subdev_mbus_code_enum*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_subdev_pad_config*, %struct.v4l2_subdev_frame_size_enum*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_subdev_pad_config*, %struct.v4l2_subdev_frame_interval_enum*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_subdev_pad_config*, %struct.v4l2_subdev_format*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_subdev_pad_config*, %struct.v4l2_subdev_format*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_subdev_pad_config*, %struct.v4l2_subdev_selection*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_subdev_pad_config*, %struct.v4l2_subdev_selection*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_edid*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_edid*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_dv_timings_cap*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_enum_dv_timings*)*, i32 (%struct.v4l2_subdev*, %struct.media_link*, %struct.v4l2_subdev_format*, %struct.v4l2_subdev_format*)*, i32 (%struct.v4l2_subdev*, i32, %struct.v4l2_mbus_frame_desc*)*, i32 (%struct.v4l2_subdev*, i32, %struct.v4l2_mbus_frame_desc*)* }
%struct.v4l2_subdev_pad_config = type { %struct.v4l2_mbus_framefmt, %struct.v4l2_rect, %struct.v4l2_rect }
%struct.v4l2_mbus_framefmt = type { i32, i32, i32, i32, i32, i16, i16, i16, [11 x i16] }
%struct.v4l2_rect = type { i32, i32, i32, i32 }
%struct.v4l2_subdev_mbus_code_enum = type { i32, i32, i32, i32, [8 x i32] }
%struct.v4l2_subdev_frame_size_enum = type { i32, i32, i32, i32, i32, i32, i32, i32, [8 x i32] }
%struct.v4l2_subdev_frame_interval_enum = type { i32, i32, i32, i32, i32, %struct.v4l2_fract, i32, [8 x i32] }
%struct.v4l2_subdev_format = type { i32, i32, %struct.v4l2_mbus_framefmt, [8 x i32] }
%struct.v4l2_subdev_selection = type { i32, i32, i32, i32, %struct.v4l2_rect, [8 x i32] }
%struct.v4l2_edid = type { i32, i32, i32, [5 x i32], i8* }
%struct.v4l2_dv_timings_cap = type { i32, i32, [2 x i32], %union.anon.58 }
%union.anon.58 = type { %struct.v4l2_bt_timings_cap, [24 x i8] }
%struct.v4l2_bt_timings_cap = type { i32, i32, i32, i32, i64, i64, i32, i32, [16 x i32] }
%struct.v4l2_enum_dv_timings = type { i32, i32, [2 x i32], %struct.v4l2_dv_timings }
%struct.v4l2_mbus_frame_desc = type { [4 x %struct.v4l2_mbus_frame_desc_entry], i16 }
%struct.v4l2_mbus_frame_desc_entry = type { i16, i32, i32 }
%struct.v4l2_subdev_internal_ops = type { i32 (%struct.v4l2_subdev*)*, void (%struct.v4l2_subdev*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_subdev_fh*)*, i32 (%struct.v4l2_subdev*, %struct.v4l2_subdev_fh*)* }
%struct.v4l2_subdev_fh = type { %struct.v4l2_fh, %struct.v4l2_subdev_pad_config* }
%struct.v4l2_async_subdev = type { i32, %union.anon.59, %struct.list_head }
%union.anon.59 = type { %struct.anon.63 }
%struct.anon.63 = type { i1 (%struct.device*, %struct.v4l2_async_subdev*)*, i8* }
%struct.v4l2_async_notifier = type { i32, %struct.v4l2_async_subdev**, %struct.v4l2_device*, %struct.list_head, %struct.list_head, %struct.list_head, i32 (%struct.v4l2_async_notifier*, %struct.v4l2_subdev*, %struct.v4l2_async_subdev*)*, i32 (%struct.v4l2_async_notifier*)*, void (%struct.v4l2_async_notifier*, %struct.v4l2_subdev*, %struct.v4l2_async_subdev*)* }
%struct.v4l2_subdev_platform_data = type { %struct.regulator_bulk_data*, i32, i8* }
%struct.regulator_bulk_data = type opaque
%struct.v4l2_prio_state = type { [4 x %struct.atomic_t] }
%struct.v4l2_ctrl_handler = type { %struct.mutex, %struct.mutex*, %struct.list_head, %struct.list_head, %struct.v4l2_ctrl_ref*, %struct.v4l2_ctrl_ref**, void (%struct.v4l2_ctrl*, i8*)*, i8*, i16, i32 }
%struct.v4l2_ctrl_ref = type { %struct.list_head, %struct.v4l2_ctrl_ref*, %struct.v4l2_ctrl*, %struct.v4l2_ctrl_helper* }
%struct.v4l2_ctrl = type { %struct.list_head, %struct.list_head, %struct.v4l2_ctrl_handler*, %struct.v4l2_ctrl**, i32, i24, %struct.v4l2_ctrl_ops*, %struct.v4l2_ctrl_type_ops*, i32, i8*, i32, i64, i64, i64, i32, i32, [4 x i32], i32, %union.anon.52, %union.anon.53, i64, i8*, i32, %struct.anon.54, %union.v4l2_ctrl_ptr, %union.v4l2_ctrl_ptr }
%struct.v4l2_ctrl_ops = type { i32 (%struct.v4l2_ctrl*)*, i32 (%struct.v4l2_ctrl*)*, i32 (%struct.v4l2_ctrl*)* }
%struct.v4l2_ctrl_type_ops = type { i1 (%struct.v4l2_ctrl*, i32, i32*, i32*)*, void (%struct.v4l2_ctrl*, i32, i32*)*, void (%struct.v4l2_ctrl*)*, i32 (%struct.v4l2_ctrl*, i32, i32*)* }
%union.anon.52 = type { i64 }
%union.anon.53 = type { i8** }
%struct.anon.54 = type { i32 }
%union.v4l2_ctrl_ptr = type { i32* }
%struct.v4l2_ctrl_helper = type opaque
%struct.video_device = type { %struct.media_entity, %struct.media_intf_devnode*, %struct.media_pipeline, %struct.v4l2_file_operations*, i32, %struct.device, %struct.cdev*, %struct.v4l2_device*, %struct.device*, %struct.v4l2_ctrl_handler*, %struct.vb2_queue*, %struct.v4l2_prio_state*, [32 x i8], i32, i32, i32, i16, i64, i32, %struct.spinlock, %struct.list_head, i32, i64, void (%struct.video_device*)*, %struct.v4l2_ioctl_ops*, [3 x i64], [3 x i64], %struct.mutex* }
%struct.media_intf_devnode = type { %struct.media_interface, i32, i32 }
%struct.media_interface = type { %struct.media_gobj, %struct.list_head, i32, i32 }
%struct.v4l2_file_operations = type { %struct.module*, i64 (%struct.file*, i8*, i64, i64*)*, i64 (%struct.file*, i8*, i64, i64*)*, i32 (%struct.file*, %struct.poll_table_struct*)*, i64 (%struct.file*, i32, i64)*, i64 (%struct.file*, i32, i64)*, i64 (%struct.file*, i64, i64, i64, i64)*, i32 (%struct.file*, %struct.vm_area_struct*)*, i32 (%struct.file*)*, i32 (%struct.file*)* }
%struct.v4l2_ioctl_ops = type opaque
%struct.vb2_queue = type { i32, i32, %struct.device*, i64, i8, %struct.mutex*, i8*, %struct.vb2_ops*, %struct.vb2_mem_ops*, %struct.vb2_buf_ops*, i8*, i32, i32, i32, i32, %struct.mutex, i32, [32 x %struct.vb2_buffer*], i32, %struct.list_head, i32, %struct.atomic_t, %struct.list_head, %struct.spinlock, %struct.__wait_queue_head, [8 x %struct.device*], i8, %struct.vb2_fileio_data*, %struct.vb2_threadio_data*, i32, i32, i32, i32, i32 }
%struct.vb2_ops = type { i32 (%struct.vb2_queue*, i32*, i32*, i32*, %struct.device**)*, void (%struct.vb2_queue*)*, void (%struct.vb2_queue*)*, i32 (%struct.vb2_buffer*)*, i32 (%struct.vb2_buffer*)*, void (%struct.vb2_buffer*)*, void (%struct.vb2_buffer*)*, i32 (%struct.vb2_queue*, i32)*, void (%struct.vb2_queue*)*, void (%struct.vb2_buffer*)* }
%struct.vb2_buffer = type { %struct.vb2_queue*, i32, i32, i32, i32, [8 x %struct.vb2_plane], i64, i32, %struct.list_head, %struct.list_head, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32 }
%struct.vb2_plane = type { i8*, %struct.dma_buf*, i32, i32, i32, i32, %union.anon.55, i32 }
%struct.dma_buf = type { i64, %struct.file*, %struct.list_head, %struct.dma_buf_ops*, %struct.mutex, i32, i8*, i8*, %struct.module*, %struct.list_head, i8*, %struct.reservation_object*, %struct.__wait_queue_head, %struct.dma_buf_poll_cb_t, %struct.dma_buf_poll_cb_t }
%struct.dma_buf_ops = type { i32 (%struct.dma_buf*, %struct.device*, %struct.dma_buf_attachment*)*, void (%struct.dma_buf*, %struct.dma_buf_attachment*)*, %struct.sg_table* (%struct.dma_buf_attachment*, i32)*, void (%struct.dma_buf_attachment*, %struct.sg_table*, i32)*, void (%struct.dma_buf*)*, i32 (%struct.dma_buf*, i32)*, i32 (%struct.dma_buf*, i32)*, i8* (%struct.dma_buf*, i64)*, void (%struct.dma_buf*, i64, i8*)*, i8* (%struct.dma_buf*, i64)*, void (%struct.dma_buf*, i64, i8*)*, i32 (%struct.dma_buf*, %struct.vm_area_struct*)*, i8* (%struct.dma_buf*)*, void (%struct.dma_buf*, i8*)* }
%struct.dma_buf_attachment = type { %struct.dma_buf*, %struct.device*, %struct.list_head, i8* }
%struct.reservation_object = type opaque
%struct.dma_buf_poll_cb_t = type { %struct.dma_fence_cb, %struct.__wait_queue_head*, i64 }
%struct.dma_fence_cb = type { %struct.list_head, void (%struct.dma_fence*, %struct.dma_fence_cb*)* }
%struct.dma_fence = type { %struct.kref, %struct.dma_fence_ops*, %struct.callback_head, %struct.list_head, %struct.spinlock*, i64, i32, i64, i64, i32 }
%struct.dma_fence_ops = type { i8* (%struct.dma_fence*)*, i8* (%struct.dma_fence*)*, i1 (%struct.dma_fence*)*, i1 (%struct.dma_fence*)*, i64 (%struct.dma_fence*, i1, i64)*, void (%struct.dma_fence*)*, i32 (%struct.dma_fence*, i8*, i32)*, void (%struct.dma_fence*, i8*, i32)*, void (%struct.dma_fence*, i8*, i32)* }
%union.anon.55 = type { i64 }
%struct.vb2_mem_ops = type { i8* (%struct.device*, i64, i64, i32, i32)*, void (i8*)*, %struct.dma_buf* (i8*, i64)*, i8* (%struct.device*, i64, i64, i32)*, void (i8*)*, void (i8*)*, void (i8*)*, i8* (%struct.device*, %struct.dma_buf*, i64, i32)*, void (i8*)*, i32 (i8*)*, void (i8*)*, i8* (i8*)*, i8* (i8*)*, i32 (i8*)*, i32 (i8*, %struct.vm_area_struct*)* }
%struct.vb2_buf_ops = type { i32 (%struct.vb2_buffer*, i8*)*, void (%struct.vb2_buffer*, i8*)*, i32 (%struct.vb2_buffer*, i8*, %struct.vb2_plane*)*, void (%struct.vb2_buffer*, i8*)* }
%struct.vb2_fileio_data = type opaque
%struct.vb2_threadio_data = type opaque
%struct.snd_card = type opaque
%struct.snd_pcm_substream = type opaque
%struct.urb = type { %struct.kref, i8*, %struct.atomic_t, %struct.atomic_t, i32, %struct.list_head, %struct.list_head, %struct.usb_anchor*, %struct.usb_device*, %struct.usb_host_endpoint*, i32, i32, i32, i32, i8*, i64, %struct.scatterlist*, i32, i32, i32, i32, i8*, i64, i32, i32, i32, i32, i8*, void (%struct.urb*)*, [0 x %struct.usb_iso_packet_descriptor] }
%struct.usb_anchor = type { %struct.list_head, %struct.__wait_queue_head, %struct.spinlock, %struct.atomic_t, i8 }
%struct.usb_iso_packet_descriptor = type { i32, i32, i32, i32 }

@__initcall_usbtv_usb_driver_init6 = internal global i32 ()* @usbtv_usb_driver_init, section ".initcall6.init", align 8
@usbtv_usb_driver = internal global %struct.usb_driver { i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str, i32 0, i32 0), i32 (%struct.usb_interface*, %struct.usb_device_id*)* @usbtv_probe, void (%struct.usb_interface*)* @usbtv_disconnect, i32 (%struct.usb_interface*, i32, i8*)* null, i32 (%struct.usb_interface*, i32)* null, i32 (%struct.usb_interface*)* null, i32 (%struct.usb_interface*)* null, i32 (%struct.usb_interface*)* null, i32 (%struct.usb_interface*)* null, %struct.usb_device_id* getelementptr inbounds ([2 x %struct.usb_device_id], [2 x %struct.usb_device_id]* @usbtv_id_table, i32 0, i32 0), %struct.usb_dynids zeroinitializer, %struct.usbdrv_wrap zeroinitializer, i8 0 }, align 8
@__exitcall_usbtv_usb_driver_exit = internal global void ()* @usbtv_usb_driver_exit, section ".exitcall.exit", align 8
@.str = private unnamed_addr constant [6 x i8] c"usbtv\00", align 1
@usbtv_id_table = internal global [2 x %struct.usb_device_id] [%struct.usb_device_id { i16 3, i16 7025, i16 12290, i16 0, i16 0, i8 0, i8 0, i8 0, i8 0, i8 0, i8 0, i8 0, i64 0 }, %struct.usb_device_id zeroinitializer], align 16
@.str1 = private unnamed_addr constant [39 x i8] c"Fushicai USBTV007 Audio-Video Grabber\0A\00", align 1
@llvm.used = appending global [3 x i8*] [i8* bitcast (i32 ()** @__initcall_usbtv_usb_driver_init6 to i8*), i8* bitcast (void ()* @usbtv_usb_driver_exit to i8*), i8* bitcast (void ()** @__exitcall_usbtv_usb_driver_exit to i8*)], section "llvm.metadata"
@.str.1 = private unnamed_addr constant [12 x i8] c"klee_choose\00", align 1
@.str.1.2 = private unnamed_addr constant [81 x i8] c"/home/tuba/Documents/tools/klee-3.8/klee/runtime/Intrinsic/klee_div_zero_check.c\00", align 1
@.str.1.2.3 = private unnamed_addr constant [15 x i8] c"divide by zero\00", align 1
@.str.2 = private unnamed_addr constant [8 x i8] c"div.err\00", align 1
@.str.3 = private unnamed_addr constant [8 x i8] c"IGNORED\00", align 1
@.str.1.4 = private unnamed_addr constant [16 x i8] c"overshift error\00", align 1
@.str.2.5 = private unnamed_addr constant [14 x i8] c"overshift.err\00", align 1
@.str.6 = private unnamed_addr constant [72 x i8] c"/home/tuba/Documents/tools/klee-3.8/klee/runtime/Intrinsic/klee_range.c\00", align 1
@.str.1.7 = private unnamed_addr constant [14 x i8] c"invalid range\00", align 1
@.str.2.8 = private unnamed_addr constant [5 x i8] c"user\00", align 1

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
  %call = call i32 @__create_pipe(%struct.usb_device* %1, i32 0) #10
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
  %call7 = call i32 @usb_control_msg(%struct.usb_device* %11, i32 %12, i8 zeroext 12, i8 zeroext 64, i16 zeroext %13, i16 zeroext %14, i8* null, i16 zeroext 0, i32 0) #10
  store i32 %call7, i32* %ret, align 4
  %15 = load i32, i32* %ret, align 4
  %cmp8 = icmp slt i32 %15, 0
  br i1 %cmp8, label %if.then, label %for.inc

if.then:                                          ; preds = %for.body
  %16 = load i32, i32* %ret, align 4
  store i32 %16, i32* %retval
  br label %return

for.inc:                                          ; preds = %for.body
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
  %int_cast_to_i64 = zext i32 8 to i64
  call void @klee_overshift_check(i64 32, i64 %int_cast_to_i64)
  %shl = shl i32 %1, 8
  %2 = load i32, i32* %endpoint.addr, align 4
  %int_cast_to_i641 = zext i32 15 to i64
  call void @klee_overshift_check(i64 32, i64 %int_cast_to_i641)
  %shl1 = shl i32 %2, 15
  %or = or i32 %shl, %shl1
  ret i32 %or
}

; Function Attrs: noredzone
declare i32 @usb_control_msg(%struct.usb_device*, i32, i8 zeroext, i8 zeroext, i16 zeroext, i16 zeroext, i8*, i16 zeroext, i32) #2

; Function Attrs: noredzone nounwind
define internal i32 @usbtv_usb_driver_init() #0 section ".init.text" {
entry:
  %call = call i32 @usb_register_driver(%struct.usb_driver* @usbtv_usb_driver, %struct.module* null, i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str, i32 0, i32 0)) #10
  ret i32 %call
}

; Function Attrs: noredzone nounwind
define internal void @usbtv_usb_driver_exit() #0 section ".exit.text" {
entry:
  call void @usb_deregister(%struct.usb_driver* @usbtv_usb_driver) #10
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
  %call = call i32 @usb_endpoint_maxp(%struct.usb_endpoint_descriptor* %desc9) #10
  store i32 %call, i32* %size, align 4
  %10 = load i32, i32* %size, align 4
  %and = and i32 %10, 2047
  %11 = load %struct.usb_host_endpoint*, %struct.usb_host_endpoint** %ep, align 8
  %desc10 = getelementptr inbounds %struct.usb_host_endpoint, %struct.usb_host_endpoint* %11, i32 0, i32 0
  %call11 = call i32 @usb_endpoint_maxp_mult(%struct.usb_endpoint_descriptor* %desc10) #10
  %mul = mul i32 %and, %call11
  store i32 %mul, i32* %size, align 4
  %call12 = call i8* @kzalloc(i64 2560, i32 20971712) #10
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
  %call18 = call %struct.usb_device* @interface_to_usbdev(%struct.usb_interface* %16) #10
  %call19 = call %struct.usb_device* @usb_get_dev(%struct.usb_device* %call18) #10
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
  call void @usb_set_intfdata(%struct.usb_interface* %20, i8* %22) #10
  %23 = load %struct.usbtv*, %struct.usbtv** %usbtv, align 8
  %call20 = call i32 @usbtv_video_init(%struct.usbtv* %23) #10
  store i32 %call20, i32* %ret, align 4
  %24 = load i32, i32* %ret, align 4
  %cmp21 = icmp slt i32 %24, 0
  br i1 %cmp21, label %usbtv_video_fail, label %if.end24

if.end24:                                         ; preds = %if.end16
  %25 = load %struct.usbtv*, %struct.usbtv** %usbtv, align 8
  %call25 = call i32 @usbtv_audio_init(%struct.usbtv* %25) #10
  store i32 %call25, i32* %ret, align 4
  %26 = load i32, i32* %ret, align 4
  %cmp26 = icmp slt i32 %26, 0
  br i1 %cmp26, label %usbtv_audio_fail, label %if.end29

if.end29:                                         ; preds = %if.end24
  %27 = load %struct.usbtv*, %struct.usbtv** %usbtv, align 8
  %v4l2_dev = getelementptr inbounds %struct.usbtv, %struct.usbtv* %27, i32 0, i32 2
  call void @v4l2_device_get(%struct.v4l2_device* %v4l2_dev) #10
  %28 = load %struct.device*, %struct.device** %dev, align 8
  call void (%struct.device*, i8*, ...) @_dev_info(%struct.device* %28, i8* getelementptr inbounds ([39 x i8], [39 x i8]* @.str1, i32 0, i32 0)) #10
  store i32 0, i32* %retval
  br label %return

usbtv_audio_fail:                                 ; preds = %if.end24
  %29 = load %struct.usb_interface*, %struct.usb_interface** %intf.addr, align 8
  call void @usb_set_intfdata(%struct.usb_interface* %29, i8* null) #10
  %30 = load %struct.usbtv*, %struct.usbtv** %usbtv, align 8
  %udev30 = getelementptr inbounds %struct.usbtv, %struct.usbtv* %30, i32 0, i32 1
  %31 = load %struct.usb_device*, %struct.usb_device** %udev30, align 8
  call void @usb_put_dev(%struct.usb_device* %31) #10
  %32 = load %struct.usbtv*, %struct.usbtv** %usbtv, align 8
  call void @usbtv_video_free(%struct.usbtv* %32) #10
  %33 = load i32, i32* %ret, align 4
  store i32 %33, i32* %retval
  br label %return

usbtv_video_fail:                                 ; preds = %if.end16
  %34 = load %struct.usb_interface*, %struct.usb_interface** %intf.addr, align 8
  call void @usb_set_intfdata(%struct.usb_interface* %34, i8* null) #10
  %35 = load %struct.usbtv*, %struct.usbtv** %usbtv, align 8
  %udev31 = getelementptr inbounds %struct.usbtv, %struct.usbtv* %35, i32 0, i32 1
  %36 = load %struct.usb_device*, %struct.usb_device** %udev31, align 8
  call void @usb_put_dev(%struct.usb_device* %36) #10
  %37 = load %struct.usbtv*, %struct.usbtv** %usbtv, align 8
  %38 = bitcast %struct.usbtv* %37 to i8*
  call void @kfree(i8* %38) #10
  %39 = load i32, i32* %ret, align 4
  store i32 %39, i32* %retval
  br label %return

return:                                           ; preds = %usbtv_video_fail, %usbtv_audio_fail, %if.end29, %if.then15, %if.then4, %if.then
  %40 = load i32, i32* %retval
  ret i32 %40
}

; Function Attrs: noredzone nounwind
define internal void @usbtv_disconnect(%struct.usb_interface* %intf) #0 {
entry:
  %intf.addr = alloca %struct.usb_interface*, align 8
  %usbtv = alloca %struct.usbtv*, align 8
  store %struct.usb_interface* %intf, %struct.usb_interface** %intf.addr, align 8
  %0 = load %struct.usb_interface*, %struct.usb_interface** %intf.addr, align 8
  %call = call i8* @usb_get_intfdata(%struct.usb_interface* %0) #10
  %1 = bitcast i8* %call to %struct.usbtv*
  store %struct.usbtv* %1, %struct.usbtv** %usbtv, align 8
  %2 = load %struct.usb_interface*, %struct.usb_interface** %intf.addr, align 8
  call void @usb_set_intfdata(%struct.usb_interface* %2, i8* null) #10
  %3 = load %struct.usbtv*, %struct.usbtv** %usbtv, align 8
  %tobool = icmp ne %struct.usbtv* %3, null
  br i1 %tobool, label %if.end, label %return

if.end:                                           ; preds = %entry
  %4 = load %struct.usbtv*, %struct.usbtv** %usbtv, align 8
  call void @usbtv_audio_free(%struct.usbtv* %4) #10
  %5 = load %struct.usbtv*, %struct.usbtv** %usbtv, align 8
  call void @usbtv_video_free(%struct.usbtv* %5) #10
  %6 = load %struct.usbtv*, %struct.usbtv** %usbtv, align 8
  %udev = getelementptr inbounds %struct.usbtv, %struct.usbtv* %6, i32 0, i32 1
  %7 = load %struct.usb_device*, %struct.usb_device** %udev, align 8
  call void @usb_put_dev(%struct.usb_device* %7) #10
  %8 = load %struct.usbtv*, %struct.usbtv** %usbtv, align 8
  %udev1 = getelementptr inbounds %struct.usbtv, %struct.usbtv* %8, i32 0, i32 1
  store %struct.usb_device* null, %struct.usb_device** %udev1, align 8
  %9 = load %struct.usbtv*, %struct.usbtv** %usbtv, align 8
  %v4l2_dev = getelementptr inbounds %struct.usbtv, %struct.usbtv* %9, i32 0, i32 2
  %call2 = call i32 @v4l2_device_put(%struct.v4l2_device* %v4l2_dev) #10
  br label %return

return:                                           ; preds = %entry, %if.end
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
  %int_cast_to_i64 = zext i32 11 to i64
  call void @klee_overshift_check(i64 32, i64 %int_cast_to_i64)
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
  %call = call i8* @kmalloc(i64 %0, i32 %or) #10
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
  call void @dev_set_drvdata(%struct.device* %dev, i8* %1) #10
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
  call void @kref_get(%struct.kref* %ref) #10
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
  %call = call noalias i8* @__kmalloc(i64 %0, i32 %1) #10
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
  call void @refcount_inc(%struct.refcount_struct* %refcount) #10
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
  %call = call i8* @dev_get_drvdata(%struct.device* %dev) #10
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

; Function Attrs: nounwind uwtable
define i64 @klee_choose(i64 %n) #4 !dbg !6 {
  %x = alloca i64, align 8
  %1 = bitcast i64* %x to i8*, !dbg !113
  call void @klee_make_symbolic(i8* %1, i64 8, i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.1, i64 0, i64 0)) #11, !dbg !114
  %2 = load i64, i64* %x, align 8, !dbg !115, !tbaa !117
  %3 = icmp ult i64 %2, %n, !dbg !121
  br i1 %3, label %5, label %4, !dbg !122

; <label>:4                                       ; preds = %0
  call void @klee_silent_exit(i32 0) #12, !dbg !123
  unreachable, !dbg !123

; <label>:5                                       ; preds = %0
  ret i64 %2, !dbg !124
}

; Function Attrs: nounwind readnone
declare void @llvm.dbg.value(metadata, i64, metadata, metadata) #5

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.start(i64, i8* nocapture) #6

declare void @klee_make_symbolic(i8*, i64, i8*) #7

; Function Attrs: noreturn
declare void @klee_silent_exit(i32) #8

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.end(i64, i8* nocapture) #6

; Function Attrs: nounwind uwtable
define void @klee_div_zero_check(i64 %z) #4 !dbg !18 {
  %1 = icmp eq i64 %z, 0, !dbg !125
  br i1 %1, label %2, label %3, !dbg !127

; <label>:2                                       ; preds = %0
  tail call void @klee_report_error(i8* nonnull getelementptr inbounds ([81 x i8], [81 x i8]* @.str.1.2, i64 0, i64 0), i32 14, i8* nonnull getelementptr inbounds ([15 x i8], [15 x i8]* @.str.1.2.3, i64 0, i64 0), i8* nonnull getelementptr inbounds ([8 x i8], [8 x i8]* @.str.2, i64 0, i64 0)) #12, !dbg !128
  unreachable, !dbg !128

; <label>:3                                       ; preds = %0
  ret void, !dbg !129
}

; Function Attrs: noreturn
declare void @klee_report_error(i8*, i32, i8*, i8*) #8

; Function Attrs: nounwind uwtable
define i32 @klee_int(i8* %name) #4 !dbg !27 {
  %x = alloca i32, align 4
  %1 = bitcast i32* %x to i8*, !dbg !130
  call void @klee_make_symbolic(i8* %1, i64 4, i8* %name) #11, !dbg !131
  %2 = load i32, i32* %x, align 4, !dbg !132, !tbaa !133
  ret i32 %2, !dbg !135
}

; Function Attrs: nounwind uwtable
define void @klee_overshift_check(i64 %bitWidth, i64 %shift) #4 !dbg !40 {
  %1 = icmp ult i64 %shift, %bitWidth, !dbg !136
  br i1 %1, label %3, label %2, !dbg !138

; <label>:2                                       ; preds = %0
  tail call void @klee_report_error(i8* nonnull getelementptr inbounds ([8 x i8], [8 x i8]* @.str.3, i64 0, i64 0), i32 0, i8* nonnull getelementptr inbounds ([16 x i8], [16 x i8]* @.str.1.4, i64 0, i64 0), i8* nonnull getelementptr inbounds ([14 x i8], [14 x i8]* @.str.2.5, i64 0, i64 0)) #12, !dbg !139
  unreachable, !dbg !139

; <label>:3                                       ; preds = %0
  ret void, !dbg !141
}

; Function Attrs: nounwind uwtable
define i32 @klee_range(i32 %start, i32 %end, i8* %name) #4 !dbg !52 {
  %x = alloca i32, align 4
  %1 = bitcast i32* %x to i8*, !dbg !142
  %2 = icmp slt i32 %start, %end, !dbg !143
  br i1 %2, label %4, label %3, !dbg !145

; <label>:3                                       ; preds = %0
  tail call void @klee_report_error(i8* nonnull getelementptr inbounds ([72 x i8], [72 x i8]* @.str.6, i64 0, i64 0), i32 17, i8* nonnull getelementptr inbounds ([14 x i8], [14 x i8]* @.str.1.7, i64 0, i64 0), i8* nonnull getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2.8, i64 0, i64 0)) #12, !dbg !146
  unreachable, !dbg !146

; <label>:4                                       ; preds = %0
  %5 = add nsw i32 %start, 1, !dbg !147
  %6 = icmp eq i32 %5, %end, !dbg !149
  br i1 %6, label %21, label %7, !dbg !150

; <label>:7                                       ; preds = %4
  call void @klee_make_symbolic(i8* %1, i64 4, i8* %name) #11, !dbg !151
  %8 = icmp eq i32 %start, 0, !dbg !153
  %9 = load i32, i32* %x, align 4, !dbg !155, !tbaa !133
  br i1 %8, label %10, label %13, !dbg !157

; <label>:10                                      ; preds = %7
  %11 = icmp ult i32 %9, %end, !dbg !158
  %12 = zext i1 %11 to i64, !dbg !159
  call void @klee_assume(i64 %12) #11, !dbg !160
  br label %19, !dbg !161

; <label>:13                                      ; preds = %7
  %14 = icmp sge i32 %9, %start, !dbg !162
  %15 = zext i1 %14 to i64, !dbg !164
  call void @klee_assume(i64 %15) #11, !dbg !165
  %16 = load i32, i32* %x, align 4, !dbg !166, !tbaa !133
  %17 = icmp slt i32 %16, %end, !dbg !167
  %18 = zext i1 %17 to i64, !dbg !166
  call void @klee_assume(i64 %18) #11, !dbg !168
  br label %19

; <label>:19                                      ; preds = %13, %10
  %20 = load i32, i32* %x, align 4, !dbg !169, !tbaa !133
  br label %21, !dbg !170

; <label>:21                                      ; preds = %4, %19
  %.0 = phi i32 [ %20, %19 ], [ %start, %4 ]
  ret i32 %.0, !dbg !171
}

declare void @klee_assume(i64) #7

; Function Attrs: norecurse nounwind uwtable
define weak i8* @memcpy(i8* %destaddr, i8* %srcaddr, i64 %len) #9 !dbg !63 {
  %1 = icmp eq i64 %len, 0, !dbg !172
  br i1 %1, label %._crit_edge, label %.lr.ph.preheader, !dbg !174

.lr.ph.preheader:                                 ; preds = %0
  %min.iters.check = icmp ult i64 %len, 16, !dbg !175
  br i1 %min.iters.check, label %.lr.ph.preheader71, label %min.iters.checked, !dbg !175

min.iters.checked:                                ; preds = %.lr.ph.preheader
  %n.vec = and i64 %len, -16, !dbg !175
  %cmp.zero = icmp eq i64 %n.vec, 0, !dbg !175
  br i1 %cmp.zero, label %.lr.ph.preheader71, label %vector.memcheck, !dbg !175

vector.memcheck:                                  ; preds = %min.iters.checked
  %2 = add i64 %len, -1, !dbg !175
  %scevgep = getelementptr i8, i8* %destaddr, i64 %2, !dbg !175
  %scevgep4 = getelementptr i8, i8* %srcaddr, i64 %2, !dbg !175
  %bound0 = icmp uge i8* %scevgep4, %destaddr, !dbg !175
  %bound1 = icmp uge i8* %scevgep, %srcaddr, !dbg !175
  %memcheck.conflict = and i1 %bound0, %bound1, !dbg !175
  %ind.end = getelementptr i8, i8* %srcaddr, i64 %n.vec, !dbg !175
  %ind.end6 = getelementptr i8, i8* %destaddr, i64 %n.vec, !dbg !175
  %ind.end8 = sub i64 %len, %n.vec, !dbg !175
  br i1 %memcheck.conflict, label %.lr.ph.preheader71, label %vector.body.preheader, !dbg !175

vector.body.preheader:                            ; preds = %vector.memcheck
  %3 = add i64 %len, -16
  %4 = lshr i64 %3, 4
  %5 = add nuw nsw i64 %4, 1
  %xtraiter72 = and i64 %5, 7
  %lcmp.mod73 = icmp eq i64 %xtraiter72, 0
  br i1 %lcmp.mod73, label %vector.body.preheader.split, label %vector.body.prol

vector.body.prol:                                 ; preds = %vector.body.preheader, %vector.body.prol
  %index.prol = phi i64 [ %index.next.prol, %vector.body.prol ], [ 0, %vector.body.preheader ]
  %prol.iter74 = phi i64 [ %prol.iter74.sub, %vector.body.prol ], [ %xtraiter72, %vector.body.preheader ]
  %next.gep.prol = getelementptr i8, i8* %srcaddr, i64 %index.prol
  %next.gep39.prol = getelementptr i8, i8* %destaddr, i64 %index.prol
  %6 = bitcast i8* %next.gep.prol to <16 x i8>*, !dbg !176
  %wide.load.prol = load <16 x i8>, <16 x i8>* %6, align 1, !dbg !176, !tbaa !177
  %7 = bitcast i8* %next.gep39.prol to <16 x i8>*, !dbg !178
  store <16 x i8> %wide.load.prol, <16 x i8>* %7, align 1, !dbg !178, !tbaa !177
  %index.next.prol = add i64 %index.prol, 16, !dbg !175
  %prol.iter74.sub = add i64 %prol.iter74, -1, !dbg !175
  %prol.iter74.cmp = icmp eq i64 %prol.iter74.sub, 0, !dbg !175
  br i1 %prol.iter74.cmp, label %vector.body.preheader.split, label %vector.body.prol, !dbg !175, !llvm.loop !179

vector.body.preheader.split:                      ; preds = %vector.body.prol, %vector.body.preheader
  %index.unr = phi i64 [ 0, %vector.body.preheader ], [ %index.next.prol, %vector.body.prol ]
  %8 = icmp ult i64 %3, 112
  br i1 %8, label %middle.block, label %vector.body

vector.body:                                      ; preds = %vector.body.preheader.split, %vector.body
  %index = phi i64 [ %index.next.7, %vector.body ], [ %index.unr, %vector.body.preheader.split ]
  %next.gep = getelementptr i8, i8* %srcaddr, i64 %index
  %next.gep39 = getelementptr i8, i8* %destaddr, i64 %index
  %9 = bitcast i8* %next.gep to <16 x i8>*, !dbg !176
  %wide.load = load <16 x i8>, <16 x i8>* %9, align 1, !dbg !176, !tbaa !177
  %10 = bitcast i8* %next.gep39 to <16 x i8>*, !dbg !178
  store <16 x i8> %wide.load, <16 x i8>* %10, align 1, !dbg !178, !tbaa !177
  %index.next = add i64 %index, 16, !dbg !175
  %next.gep.1 = getelementptr i8, i8* %srcaddr, i64 %index.next
  %next.gep39.1 = getelementptr i8, i8* %destaddr, i64 %index.next
  %11 = bitcast i8* %next.gep.1 to <16 x i8>*, !dbg !176
  %wide.load.1 = load <16 x i8>, <16 x i8>* %11, align 1, !dbg !176, !tbaa !177
  %12 = bitcast i8* %next.gep39.1 to <16 x i8>*, !dbg !178
  store <16 x i8> %wide.load.1, <16 x i8>* %12, align 1, !dbg !178, !tbaa !177
  %index.next.1 = add i64 %index, 32, !dbg !175
  %next.gep.2 = getelementptr i8, i8* %srcaddr, i64 %index.next.1
  %next.gep39.2 = getelementptr i8, i8* %destaddr, i64 %index.next.1
  %13 = bitcast i8* %next.gep.2 to <16 x i8>*, !dbg !176
  %wide.load.2 = load <16 x i8>, <16 x i8>* %13, align 1, !dbg !176, !tbaa !177
  %14 = bitcast i8* %next.gep39.2 to <16 x i8>*, !dbg !178
  store <16 x i8> %wide.load.2, <16 x i8>* %14, align 1, !dbg !178, !tbaa !177
  %index.next.2 = add i64 %index, 48, !dbg !175
  %next.gep.3 = getelementptr i8, i8* %srcaddr, i64 %index.next.2
  %next.gep39.3 = getelementptr i8, i8* %destaddr, i64 %index.next.2
  %15 = bitcast i8* %next.gep.3 to <16 x i8>*, !dbg !176
  %wide.load.3 = load <16 x i8>, <16 x i8>* %15, align 1, !dbg !176, !tbaa !177
  %16 = bitcast i8* %next.gep39.3 to <16 x i8>*, !dbg !178
  store <16 x i8> %wide.load.3, <16 x i8>* %16, align 1, !dbg !178, !tbaa !177
  %index.next.3 = add i64 %index, 64, !dbg !175
  %next.gep.4 = getelementptr i8, i8* %srcaddr, i64 %index.next.3
  %next.gep39.4 = getelementptr i8, i8* %destaddr, i64 %index.next.3
  %17 = bitcast i8* %next.gep.4 to <16 x i8>*, !dbg !176
  %wide.load.4 = load <16 x i8>, <16 x i8>* %17, align 1, !dbg !176, !tbaa !177
  %18 = bitcast i8* %next.gep39.4 to <16 x i8>*, !dbg !178
  store <16 x i8> %wide.load.4, <16 x i8>* %18, align 1, !dbg !178, !tbaa !177
  %index.next.4 = add i64 %index, 80, !dbg !175
  %next.gep.5 = getelementptr i8, i8* %srcaddr, i64 %index.next.4
  %next.gep39.5 = getelementptr i8, i8* %destaddr, i64 %index.next.4
  %19 = bitcast i8* %next.gep.5 to <16 x i8>*, !dbg !176
  %wide.load.5 = load <16 x i8>, <16 x i8>* %19, align 1, !dbg !176, !tbaa !177
  %20 = bitcast i8* %next.gep39.5 to <16 x i8>*, !dbg !178
  store <16 x i8> %wide.load.5, <16 x i8>* %20, align 1, !dbg !178, !tbaa !177
  %index.next.5 = add i64 %index, 96, !dbg !175
  %next.gep.6 = getelementptr i8, i8* %srcaddr, i64 %index.next.5
  %next.gep39.6 = getelementptr i8, i8* %destaddr, i64 %index.next.5
  %21 = bitcast i8* %next.gep.6 to <16 x i8>*, !dbg !176
  %wide.load.6 = load <16 x i8>, <16 x i8>* %21, align 1, !dbg !176, !tbaa !177
  %22 = bitcast i8* %next.gep39.6 to <16 x i8>*, !dbg !178
  store <16 x i8> %wide.load.6, <16 x i8>* %22, align 1, !dbg !178, !tbaa !177
  %index.next.6 = add i64 %index, 112, !dbg !175
  %next.gep.7 = getelementptr i8, i8* %srcaddr, i64 %index.next.6
  %next.gep39.7 = getelementptr i8, i8* %destaddr, i64 %index.next.6
  %23 = bitcast i8* %next.gep.7 to <16 x i8>*, !dbg !176
  %wide.load.7 = load <16 x i8>, <16 x i8>* %23, align 1, !dbg !176, !tbaa !177
  %24 = bitcast i8* %next.gep39.7 to <16 x i8>*, !dbg !178
  store <16 x i8> %wide.load.7, <16 x i8>* %24, align 1, !dbg !178, !tbaa !177
  %index.next.7 = add i64 %index, 128, !dbg !175
  %25 = icmp eq i64 %index.next.7, %n.vec, !dbg !175
  br i1 %25, label %middle.block, label %vector.body, !dbg !175, !llvm.loop !181

middle.block:                                     ; preds = %vector.body, %vector.body.preheader.split
  %cmp.n = icmp eq i64 %n.vec, %len
  br i1 %cmp.n, label %._crit_edge, label %.lr.ph.preheader71, !dbg !175

.lr.ph.preheader71:                               ; preds = %middle.block, %vector.memcheck, %min.iters.checked, %.lr.ph.preheader
  %src.03.ph = phi i8* [ %srcaddr, %vector.memcheck ], [ %srcaddr, %min.iters.checked ], [ %srcaddr, %.lr.ph.preheader ], [ %ind.end, %middle.block ]
  %dest.02.ph = phi i8* [ %destaddr, %vector.memcheck ], [ %destaddr, %min.iters.checked ], [ %destaddr, %.lr.ph.preheader ], [ %ind.end6, %middle.block ]
  %.01.ph = phi i64 [ %len, %vector.memcheck ], [ %len, %min.iters.checked ], [ %len, %.lr.ph.preheader ], [ %ind.end8, %middle.block ]
  %26 = add i64 %.01.ph, -1, !dbg !175
  %xtraiter = and i64 %.01.ph, 7, !dbg !175
  %lcmp.mod = icmp eq i64 %xtraiter, 0, !dbg !175
  br i1 %lcmp.mod, label %.lr.ph.preheader71.split, label %.lr.ph.prol, !dbg !175

.lr.ph.prol:                                      ; preds = %.lr.ph.preheader71, %.lr.ph.prol
  %src.03.prol = phi i8* [ %28, %.lr.ph.prol ], [ %src.03.ph, %.lr.ph.preheader71 ]
  %dest.02.prol = phi i8* [ %30, %.lr.ph.prol ], [ %dest.02.ph, %.lr.ph.preheader71 ]
  %.01.prol = phi i64 [ %27, %.lr.ph.prol ], [ %.01.ph, %.lr.ph.preheader71 ]
  %prol.iter = phi i64 [ %prol.iter.sub, %.lr.ph.prol ], [ %xtraiter, %.lr.ph.preheader71 ]
  %27 = add i64 %.01.prol, -1, !dbg !175
  %28 = getelementptr inbounds i8, i8* %src.03.prol, i64 1, !dbg !184
  %29 = load i8, i8* %src.03.prol, align 1, !dbg !176, !tbaa !177
  %30 = getelementptr inbounds i8, i8* %dest.02.prol, i64 1, !dbg !185
  store i8 %29, i8* %dest.02.prol, align 1, !dbg !178, !tbaa !177
  %prol.iter.sub = add i64 %prol.iter, -1, !dbg !174
  %prol.iter.cmp = icmp eq i64 %prol.iter.sub, 0, !dbg !174
  br i1 %prol.iter.cmp, label %.lr.ph.preheader71.split, label %.lr.ph.prol, !dbg !174, !llvm.loop !186

.lr.ph.preheader71.split:                         ; preds = %.lr.ph.prol, %.lr.ph.preheader71
  %src.03.unr = phi i8* [ %src.03.ph, %.lr.ph.preheader71 ], [ %28, %.lr.ph.prol ]
  %dest.02.unr = phi i8* [ %dest.02.ph, %.lr.ph.preheader71 ], [ %30, %.lr.ph.prol ]
  %.01.unr = phi i64 [ %.01.ph, %.lr.ph.preheader71 ], [ %27, %.lr.ph.prol ]
  %31 = icmp ult i64 %26, 7, !dbg !175
  br i1 %31, label %._crit_edge, label %.lr.ph, !dbg !175

.lr.ph:                                           ; preds = %.lr.ph.preheader71.split, %.lr.ph
  %src.03 = phi i8* [ %54, %.lr.ph ], [ %src.03.unr, %.lr.ph.preheader71.split ]
  %dest.02 = phi i8* [ %56, %.lr.ph ], [ %dest.02.unr, %.lr.ph.preheader71.split ]
  %.01 = phi i64 [ %53, %.lr.ph ], [ %.01.unr, %.lr.ph.preheader71.split ]
  %32 = getelementptr inbounds i8, i8* %src.03, i64 1, !dbg !184
  %33 = load i8, i8* %src.03, align 1, !dbg !176, !tbaa !177
  %34 = getelementptr inbounds i8, i8* %dest.02, i64 1, !dbg !185
  store i8 %33, i8* %dest.02, align 1, !dbg !178, !tbaa !177
  %35 = getelementptr inbounds i8, i8* %src.03, i64 2, !dbg !184
  %36 = load i8, i8* %32, align 1, !dbg !176, !tbaa !177
  %37 = getelementptr inbounds i8, i8* %dest.02, i64 2, !dbg !185
  store i8 %36, i8* %34, align 1, !dbg !178, !tbaa !177
  %38 = getelementptr inbounds i8, i8* %src.03, i64 3, !dbg !184
  %39 = load i8, i8* %35, align 1, !dbg !176, !tbaa !177
  %40 = getelementptr inbounds i8, i8* %dest.02, i64 3, !dbg !185
  store i8 %39, i8* %37, align 1, !dbg !178, !tbaa !177
  %41 = getelementptr inbounds i8, i8* %src.03, i64 4, !dbg !184
  %42 = load i8, i8* %38, align 1, !dbg !176, !tbaa !177
  %43 = getelementptr inbounds i8, i8* %dest.02, i64 4, !dbg !185
  store i8 %42, i8* %40, align 1, !dbg !178, !tbaa !177
  %44 = getelementptr inbounds i8, i8* %src.03, i64 5, !dbg !184
  %45 = load i8, i8* %41, align 1, !dbg !176, !tbaa !177
  %46 = getelementptr inbounds i8, i8* %dest.02, i64 5, !dbg !185
  store i8 %45, i8* %43, align 1, !dbg !178, !tbaa !177
  %47 = getelementptr inbounds i8, i8* %src.03, i64 6, !dbg !184
  %48 = load i8, i8* %44, align 1, !dbg !176, !tbaa !177
  %49 = getelementptr inbounds i8, i8* %dest.02, i64 6, !dbg !185
  store i8 %48, i8* %46, align 1, !dbg !178, !tbaa !177
  %50 = getelementptr inbounds i8, i8* %src.03, i64 7, !dbg !184
  %51 = load i8, i8* %47, align 1, !dbg !176, !tbaa !177
  %52 = getelementptr inbounds i8, i8* %dest.02, i64 7, !dbg !185
  store i8 %51, i8* %49, align 1, !dbg !178, !tbaa !177
  %53 = add i64 %.01, -8, !dbg !175
  %54 = getelementptr inbounds i8, i8* %src.03, i64 8, !dbg !184
  %55 = load i8, i8* %50, align 1, !dbg !176, !tbaa !177
  %56 = getelementptr inbounds i8, i8* %dest.02, i64 8, !dbg !185
  store i8 %55, i8* %52, align 1, !dbg !178, !tbaa !177
  %57 = icmp eq i64 %53, 0, !dbg !172
  br i1 %57, label %._crit_edge, label %.lr.ph, !dbg !174, !llvm.loop !187

._crit_edge:                                      ; preds = %.lr.ph.preheader71.split, %.lr.ph, %middle.block, %0
  ret i8* %destaddr, !dbg !188
}

; Function Attrs: norecurse nounwind uwtable
define weak i8* @memmove(i8* %dst, i8* %src, i64 %count) #9 !dbg !81 {
  %1 = icmp eq i8* %src, %dst, !dbg !189
  br i1 %1, label %.loopexit, label %2, !dbg !191

; <label>:2                                       ; preds = %0
  %3 = icmp ugt i8* %src, %dst, !dbg !192
  br i1 %3, label %.preheader, label %61, !dbg !194

.preheader:                                       ; preds = %2
  %4 = icmp eq i64 %count, 0, !dbg !195
  br i1 %4, label %.loopexit, label %.lr.ph.preheader, !dbg !195

.lr.ph.preheader:                                 ; preds = %.preheader
  %min.iters.check152 = icmp ult i64 %count, 16, !dbg !198
  br i1 %min.iters.check152, label %.lr.ph.preheader244, label %min.iters.checked153, !dbg !198

min.iters.checked153:                             ; preds = %.lr.ph.preheader
  %n.vec155 = and i64 %count, -16, !dbg !198
  %cmp.zero156 = icmp eq i64 %n.vec155, 0, !dbg !198
  br i1 %cmp.zero156, label %.lr.ph.preheader244, label %vector.memcheck164, !dbg !198

vector.memcheck164:                               ; preds = %min.iters.checked153
  %5 = add i64 %count, -1, !dbg !198
  %scevgep158 = getelementptr i8, i8* %dst, i64 %5, !dbg !198
  %scevgep159 = getelementptr i8, i8* %src, i64 %5, !dbg !198
  %bound0160 = icmp uge i8* %scevgep159, %dst, !dbg !198
  %bound1161 = icmp uge i8* %scevgep158, %src, !dbg !198
  %memcheck.conflict163 = and i1 %bound0160, %bound1161, !dbg !198
  %ind.end169 = getelementptr i8, i8* %src, i64 %n.vec155, !dbg !198
  %ind.end171 = getelementptr i8, i8* %dst, i64 %n.vec155, !dbg !198
  %ind.end173 = sub i64 %count, %n.vec155, !dbg !198
  br i1 %memcheck.conflict163, label %.lr.ph.preheader244, label %vector.body149.preheader, !dbg !198

vector.body149.preheader:                         ; preds = %vector.memcheck164
  %6 = add i64 %count, -16
  %7 = lshr i64 %6, 4
  %8 = add nuw nsw i64 %7, 1
  %xtraiter247 = and i64 %8, 7
  %lcmp.mod248 = icmp eq i64 %xtraiter247, 0
  br i1 %lcmp.mod248, label %vector.body149.preheader.split, label %vector.body149.prol

vector.body149.prol:                              ; preds = %vector.body149.preheader, %vector.body149.prol
  %index166.prol = phi i64 [ %index.next167.prol, %vector.body149.prol ], [ 0, %vector.body149.preheader ]
  %prol.iter249 = phi i64 [ %prol.iter249.sub, %vector.body149.prol ], [ %xtraiter247, %vector.body149.preheader ]
  %next.gep175.prol = getelementptr i8, i8* %src, i64 %index166.prol
  %next.gep207.prol = getelementptr i8, i8* %dst, i64 %index166.prol
  %9 = bitcast i8* %next.gep175.prol to <16 x i8>*, !dbg !199
  %wide.load243.prol = load <16 x i8>, <16 x i8>* %9, align 1, !dbg !199, !tbaa !177
  %10 = bitcast i8* %next.gep207.prol to <16 x i8>*, !dbg !201
  store <16 x i8> %wide.load243.prol, <16 x i8>* %10, align 1, !dbg !201, !tbaa !177
  %index.next167.prol = add i64 %index166.prol, 16, !dbg !198
  %prol.iter249.sub = add i64 %prol.iter249, -1, !dbg !198
  %prol.iter249.cmp = icmp eq i64 %prol.iter249.sub, 0, !dbg !198
  br i1 %prol.iter249.cmp, label %vector.body149.preheader.split, label %vector.body149.prol, !dbg !198, !llvm.loop !202

vector.body149.preheader.split:                   ; preds = %vector.body149.prol, %vector.body149.preheader
  %index166.unr = phi i64 [ 0, %vector.body149.preheader ], [ %index.next167.prol, %vector.body149.prol ]
  %11 = icmp ult i64 %6, 112
  br i1 %11, label %middle.block150, label %vector.body149

vector.body149:                                   ; preds = %vector.body149.preheader.split, %vector.body149
  %index166 = phi i64 [ %index.next167.7, %vector.body149 ], [ %index166.unr, %vector.body149.preheader.split ]
  %next.gep175 = getelementptr i8, i8* %src, i64 %index166
  %next.gep207 = getelementptr i8, i8* %dst, i64 %index166
  %12 = bitcast i8* %next.gep175 to <16 x i8>*, !dbg !199
  %wide.load243 = load <16 x i8>, <16 x i8>* %12, align 1, !dbg !199, !tbaa !177
  %13 = bitcast i8* %next.gep207 to <16 x i8>*, !dbg !201
  store <16 x i8> %wide.load243, <16 x i8>* %13, align 1, !dbg !201, !tbaa !177
  %index.next167 = add i64 %index166, 16, !dbg !198
  %next.gep175.1 = getelementptr i8, i8* %src, i64 %index.next167
  %next.gep207.1 = getelementptr i8, i8* %dst, i64 %index.next167
  %14 = bitcast i8* %next.gep175.1 to <16 x i8>*, !dbg !199
  %wide.load243.1 = load <16 x i8>, <16 x i8>* %14, align 1, !dbg !199, !tbaa !177
  %15 = bitcast i8* %next.gep207.1 to <16 x i8>*, !dbg !201
  store <16 x i8> %wide.load243.1, <16 x i8>* %15, align 1, !dbg !201, !tbaa !177
  %index.next167.1 = add i64 %index166, 32, !dbg !198
  %next.gep175.2 = getelementptr i8, i8* %src, i64 %index.next167.1
  %next.gep207.2 = getelementptr i8, i8* %dst, i64 %index.next167.1
  %16 = bitcast i8* %next.gep175.2 to <16 x i8>*, !dbg !199
  %wide.load243.2 = load <16 x i8>, <16 x i8>* %16, align 1, !dbg !199, !tbaa !177
  %17 = bitcast i8* %next.gep207.2 to <16 x i8>*, !dbg !201
  store <16 x i8> %wide.load243.2, <16 x i8>* %17, align 1, !dbg !201, !tbaa !177
  %index.next167.2 = add i64 %index166, 48, !dbg !198
  %next.gep175.3 = getelementptr i8, i8* %src, i64 %index.next167.2
  %next.gep207.3 = getelementptr i8, i8* %dst, i64 %index.next167.2
  %18 = bitcast i8* %next.gep175.3 to <16 x i8>*, !dbg !199
  %wide.load243.3 = load <16 x i8>, <16 x i8>* %18, align 1, !dbg !199, !tbaa !177
  %19 = bitcast i8* %next.gep207.3 to <16 x i8>*, !dbg !201
  store <16 x i8> %wide.load243.3, <16 x i8>* %19, align 1, !dbg !201, !tbaa !177
  %index.next167.3 = add i64 %index166, 64, !dbg !198
  %next.gep175.4 = getelementptr i8, i8* %src, i64 %index.next167.3
  %next.gep207.4 = getelementptr i8, i8* %dst, i64 %index.next167.3
  %20 = bitcast i8* %next.gep175.4 to <16 x i8>*, !dbg !199
  %wide.load243.4 = load <16 x i8>, <16 x i8>* %20, align 1, !dbg !199, !tbaa !177
  %21 = bitcast i8* %next.gep207.4 to <16 x i8>*, !dbg !201
  store <16 x i8> %wide.load243.4, <16 x i8>* %21, align 1, !dbg !201, !tbaa !177
  %index.next167.4 = add i64 %index166, 80, !dbg !198
  %next.gep175.5 = getelementptr i8, i8* %src, i64 %index.next167.4
  %next.gep207.5 = getelementptr i8, i8* %dst, i64 %index.next167.4
  %22 = bitcast i8* %next.gep175.5 to <16 x i8>*, !dbg !199
  %wide.load243.5 = load <16 x i8>, <16 x i8>* %22, align 1, !dbg !199, !tbaa !177
  %23 = bitcast i8* %next.gep207.5 to <16 x i8>*, !dbg !201
  store <16 x i8> %wide.load243.5, <16 x i8>* %23, align 1, !dbg !201, !tbaa !177
  %index.next167.5 = add i64 %index166, 96, !dbg !198
  %next.gep175.6 = getelementptr i8, i8* %src, i64 %index.next167.5
  %next.gep207.6 = getelementptr i8, i8* %dst, i64 %index.next167.5
  %24 = bitcast i8* %next.gep175.6 to <16 x i8>*, !dbg !199
  %wide.load243.6 = load <16 x i8>, <16 x i8>* %24, align 1, !dbg !199, !tbaa !177
  %25 = bitcast i8* %next.gep207.6 to <16 x i8>*, !dbg !201
  store <16 x i8> %wide.load243.6, <16 x i8>* %25, align 1, !dbg !201, !tbaa !177
  %index.next167.6 = add i64 %index166, 112, !dbg !198
  %next.gep175.7 = getelementptr i8, i8* %src, i64 %index.next167.6
  %next.gep207.7 = getelementptr i8, i8* %dst, i64 %index.next167.6
  %26 = bitcast i8* %next.gep175.7 to <16 x i8>*, !dbg !199
  %wide.load243.7 = load <16 x i8>, <16 x i8>* %26, align 1, !dbg !199, !tbaa !177
  %27 = bitcast i8* %next.gep207.7 to <16 x i8>*, !dbg !201
  store <16 x i8> %wide.load243.7, <16 x i8>* %27, align 1, !dbg !201, !tbaa !177
  %index.next167.7 = add i64 %index166, 128, !dbg !198
  %28 = icmp eq i64 %index.next167.7, %n.vec155, !dbg !198
  br i1 %28, label %middle.block150, label %vector.body149, !dbg !198, !llvm.loop !203

middle.block150:                                  ; preds = %vector.body149, %vector.body149.preheader.split
  %cmp.n174 = icmp eq i64 %n.vec155, %count
  br i1 %cmp.n174, label %.loopexit, label %.lr.ph.preheader244, !dbg !198

.lr.ph.preheader244:                              ; preds = %middle.block150, %vector.memcheck164, %min.iters.checked153, %.lr.ph.preheader
  %b.05.ph = phi i8* [ %src, %vector.memcheck164 ], [ %src, %min.iters.checked153 ], [ %src, %.lr.ph.preheader ], [ %ind.end169, %middle.block150 ]
  %a.04.ph = phi i8* [ %dst, %vector.memcheck164 ], [ %dst, %min.iters.checked153 ], [ %dst, %.lr.ph.preheader ], [ %ind.end171, %middle.block150 ]
  %.03.ph = phi i64 [ %count, %vector.memcheck164 ], [ %count, %min.iters.checked153 ], [ %count, %.lr.ph.preheader ], [ %ind.end173, %middle.block150 ]
  %29 = add i64 %.03.ph, -1, !dbg !198
  %xtraiter = and i64 %.03.ph, 7, !dbg !198
  %lcmp.mod = icmp eq i64 %xtraiter, 0, !dbg !198
  br i1 %lcmp.mod, label %.lr.ph.preheader244.split, label %.lr.ph.prol, !dbg !198

.lr.ph.prol:                                      ; preds = %.lr.ph.preheader244, %.lr.ph.prol
  %b.05.prol = phi i8* [ %31, %.lr.ph.prol ], [ %b.05.ph, %.lr.ph.preheader244 ]
  %a.04.prol = phi i8* [ %33, %.lr.ph.prol ], [ %a.04.ph, %.lr.ph.preheader244 ]
  %.03.prol = phi i64 [ %30, %.lr.ph.prol ], [ %.03.ph, %.lr.ph.preheader244 ]
  %prol.iter = phi i64 [ %prol.iter.sub, %.lr.ph.prol ], [ %xtraiter, %.lr.ph.preheader244 ]
  %30 = add i64 %.03.prol, -1, !dbg !198
  %31 = getelementptr inbounds i8, i8* %b.05.prol, i64 1, !dbg !204
  %32 = load i8, i8* %b.05.prol, align 1, !dbg !199, !tbaa !177
  %33 = getelementptr inbounds i8, i8* %a.04.prol, i64 1, !dbg !205
  store i8 %32, i8* %a.04.prol, align 1, !dbg !201, !tbaa !177
  %prol.iter.sub = add i64 %prol.iter, -1, !dbg !195
  %prol.iter.cmp = icmp eq i64 %prol.iter.sub, 0, !dbg !195
  br i1 %prol.iter.cmp, label %.lr.ph.preheader244.split, label %.lr.ph.prol, !dbg !195, !llvm.loop !206

.lr.ph.preheader244.split:                        ; preds = %.lr.ph.prol, %.lr.ph.preheader244
  %b.05.unr = phi i8* [ %b.05.ph, %.lr.ph.preheader244 ], [ %31, %.lr.ph.prol ]
  %a.04.unr = phi i8* [ %a.04.ph, %.lr.ph.preheader244 ], [ %33, %.lr.ph.prol ]
  %.03.unr = phi i64 [ %.03.ph, %.lr.ph.preheader244 ], [ %30, %.lr.ph.prol ]
  %34 = icmp ult i64 %29, 7, !dbg !198
  br i1 %34, label %.loopexit, label %.lr.ph, !dbg !198

.lr.ph:                                           ; preds = %.lr.ph.preheader244.split, %.lr.ph
  %b.05 = phi i8* [ %57, %.lr.ph ], [ %b.05.unr, %.lr.ph.preheader244.split ]
  %a.04 = phi i8* [ %59, %.lr.ph ], [ %a.04.unr, %.lr.ph.preheader244.split ]
  %.03 = phi i64 [ %56, %.lr.ph ], [ %.03.unr, %.lr.ph.preheader244.split ]
  %35 = getelementptr inbounds i8, i8* %b.05, i64 1, !dbg !204
  %36 = load i8, i8* %b.05, align 1, !dbg !199, !tbaa !177
  %37 = getelementptr inbounds i8, i8* %a.04, i64 1, !dbg !205
  store i8 %36, i8* %a.04, align 1, !dbg !201, !tbaa !177
  %38 = getelementptr inbounds i8, i8* %b.05, i64 2, !dbg !204
  %39 = load i8, i8* %35, align 1, !dbg !199, !tbaa !177
  %40 = getelementptr inbounds i8, i8* %a.04, i64 2, !dbg !205
  store i8 %39, i8* %37, align 1, !dbg !201, !tbaa !177
  %41 = getelementptr inbounds i8, i8* %b.05, i64 3, !dbg !204
  %42 = load i8, i8* %38, align 1, !dbg !199, !tbaa !177
  %43 = getelementptr inbounds i8, i8* %a.04, i64 3, !dbg !205
  store i8 %42, i8* %40, align 1, !dbg !201, !tbaa !177
  %44 = getelementptr inbounds i8, i8* %b.05, i64 4, !dbg !204
  %45 = load i8, i8* %41, align 1, !dbg !199, !tbaa !177
  %46 = getelementptr inbounds i8, i8* %a.04, i64 4, !dbg !205
  store i8 %45, i8* %43, align 1, !dbg !201, !tbaa !177
  %47 = getelementptr inbounds i8, i8* %b.05, i64 5, !dbg !204
  %48 = load i8, i8* %44, align 1, !dbg !199, !tbaa !177
  %49 = getelementptr inbounds i8, i8* %a.04, i64 5, !dbg !205
  store i8 %48, i8* %46, align 1, !dbg !201, !tbaa !177
  %50 = getelementptr inbounds i8, i8* %b.05, i64 6, !dbg !204
  %51 = load i8, i8* %47, align 1, !dbg !199, !tbaa !177
  %52 = getelementptr inbounds i8, i8* %a.04, i64 6, !dbg !205
  store i8 %51, i8* %49, align 1, !dbg !201, !tbaa !177
  %53 = getelementptr inbounds i8, i8* %b.05, i64 7, !dbg !204
  %54 = load i8, i8* %50, align 1, !dbg !199, !tbaa !177
  %55 = getelementptr inbounds i8, i8* %a.04, i64 7, !dbg !205
  store i8 %54, i8* %52, align 1, !dbg !201, !tbaa !177
  %56 = add i64 %.03, -8, !dbg !198
  %57 = getelementptr inbounds i8, i8* %b.05, i64 8, !dbg !204
  %58 = load i8, i8* %53, align 1, !dbg !199, !tbaa !177
  %59 = getelementptr inbounds i8, i8* %a.04, i64 8, !dbg !205
  store i8 %58, i8* %55, align 1, !dbg !201, !tbaa !177
  %60 = icmp eq i64 %56, 0, !dbg !195
  br i1 %60, label %.loopexit, label %.lr.ph, !dbg !195, !llvm.loop !207

; <label>:61                                      ; preds = %2
  %62 = add i64 %count, -1, !dbg !208
  %63 = icmp eq i64 %count, 0, !dbg !210
  br i1 %63, label %.loopexit, label %.lr.ph9.preheader, !dbg !210

.lr.ph9.preheader:                                ; preds = %61
  %64 = getelementptr inbounds i8, i8* %src, i64 %62, !dbg !212
  %65 = getelementptr inbounds i8, i8* %dst, i64 %62, !dbg !213
  %min.iters.check = icmp ult i64 %count, 32, !dbg !214
  br i1 %min.iters.check, label %.lr.ph9.preheader245, label %min.iters.checked, !dbg !214

min.iters.checked:                                ; preds = %.lr.ph9.preheader
  %n.vec = and i64 %count, -32, !dbg !214
  %cmp.zero = icmp eq i64 %n.vec, 0, !dbg !214
  br i1 %cmp.zero, label %.lr.ph9.preheader245, label %vector.memcheck, !dbg !214

vector.memcheck:                                  ; preds = %min.iters.checked
  %66 = add i64 %count, -1, !dbg !214
  %scevgep = getelementptr i8, i8* %dst, i64 %66, !dbg !214
  %scevgep13 = getelementptr i8, i8* %src, i64 %66, !dbg !214
  %bound0 = icmp uge i8* %scevgep13, %dst, !dbg !214
  %bound1 = icmp uge i8* %scevgep, %src, !dbg !214
  %memcheck.conflict = and i1 %bound0, %bound1, !dbg !214
  %67 = sub i64 0, %n.vec, !dbg !214
  %ind.end = getelementptr i8, i8* %64, i64 %67, !dbg !214
  %68 = sub i64 0, %n.vec, !dbg !214
  %ind.end15 = getelementptr i8, i8* %65, i64 %68, !dbg !214
  %ind.end17 = sub i64 %count, %n.vec, !dbg !214
  br i1 %memcheck.conflict, label %.lr.ph9.preheader245, label %vector.body.preheader, !dbg !214

vector.body.preheader:                            ; preds = %vector.memcheck
  %69 = add i64 %count, -32
  %70 = lshr i64 %69, 5
  %71 = and i64 %70, 1
  %lcmp.mod254 = icmp eq i64 %71, 0
  br i1 %lcmp.mod254, label %vector.body.prol, label %vector.body.preheader.split

vector.body.prol:                                 ; preds = %vector.body.preheader
  %72 = getelementptr i8, i8* %64, i64 -15, !dbg !215
  %73 = bitcast i8* %72 to <16 x i8>*, !dbg !215
  %wide.load.prol = load <16 x i8>, <16 x i8>* %73, align 1, !dbg !215, !tbaa !177
  %74 = getelementptr i8, i8* %64, i64 -16, !dbg !215
  %75 = getelementptr i8, i8* %74, i64 -15, !dbg !215
  %76 = bitcast i8* %75 to <16 x i8>*, !dbg !215
  %wide.load145.prol = load <16 x i8>, <16 x i8>* %76, align 1, !dbg !215, !tbaa !177
  %77 = getelementptr i8, i8* %65, i64 -15, !dbg !217
  %78 = bitcast i8* %77 to <16 x i8>*, !dbg !217
  store <16 x i8> %wide.load.prol, <16 x i8>* %78, align 1, !dbg !217, !tbaa !177
  %79 = getelementptr i8, i8* %65, i64 -16, !dbg !217
  %80 = getelementptr i8, i8* %79, i64 -15, !dbg !217
  %81 = bitcast i8* %80 to <16 x i8>*, !dbg !217
  store <16 x i8> %wide.load145.prol, <16 x i8>* %81, align 1, !dbg !217, !tbaa !177
  br label %vector.body.preheader.split, !dbg !214

vector.body.preheader.split:                      ; preds = %vector.body.prol, %vector.body.preheader
  %index.unr = phi i64 [ 0, %vector.body.preheader ], [ 32, %vector.body.prol ]
  %82 = icmp eq i64 %70, 0
  br i1 %82, label %middle.block, label %vector.body

vector.body:                                      ; preds = %vector.body.preheader.split, %vector.body
  %index = phi i64 [ %index.next.1, %vector.body ], [ %index.unr, %vector.body.preheader.split ]
  %83 = sub i64 0, %index
  %next.gep = getelementptr i8, i8* %64, i64 %83
  %84 = sub i64 0, %index
  %next.gep80 = getelementptr i8, i8* %65, i64 %84
  %85 = getelementptr i8, i8* %next.gep, i64 -15, !dbg !215
  %86 = bitcast i8* %85 to <16 x i8>*, !dbg !215
  %wide.load = load <16 x i8>, <16 x i8>* %86, align 1, !dbg !215, !tbaa !177
  %87 = getelementptr i8, i8* %next.gep, i64 -16, !dbg !215
  %88 = getelementptr i8, i8* %87, i64 -15, !dbg !215
  %89 = bitcast i8* %88 to <16 x i8>*, !dbg !215
  %wide.load145 = load <16 x i8>, <16 x i8>* %89, align 1, !dbg !215, !tbaa !177
  %90 = getelementptr i8, i8* %next.gep80, i64 -15, !dbg !217
  %91 = bitcast i8* %90 to <16 x i8>*, !dbg !217
  store <16 x i8> %wide.load, <16 x i8>* %91, align 1, !dbg !217, !tbaa !177
  %92 = getelementptr i8, i8* %next.gep80, i64 -16, !dbg !217
  %93 = getelementptr i8, i8* %92, i64 -15, !dbg !217
  %94 = bitcast i8* %93 to <16 x i8>*, !dbg !217
  store <16 x i8> %wide.load145, <16 x i8>* %94, align 1, !dbg !217, !tbaa !177
  %95 = sub i64 -32, %index
  %next.gep.1 = getelementptr i8, i8* %64, i64 %95
  %96 = sub i64 -32, %index
  %next.gep80.1 = getelementptr i8, i8* %65, i64 %96
  %97 = getelementptr i8, i8* %next.gep.1, i64 -15, !dbg !215
  %98 = bitcast i8* %97 to <16 x i8>*, !dbg !215
  %wide.load.1 = load <16 x i8>, <16 x i8>* %98, align 1, !dbg !215, !tbaa !177
  %99 = getelementptr i8, i8* %next.gep.1, i64 -16, !dbg !215
  %100 = getelementptr i8, i8* %99, i64 -15, !dbg !215
  %101 = bitcast i8* %100 to <16 x i8>*, !dbg !215
  %wide.load145.1 = load <16 x i8>, <16 x i8>* %101, align 1, !dbg !215, !tbaa !177
  %102 = getelementptr i8, i8* %next.gep80.1, i64 -15, !dbg !217
  %103 = bitcast i8* %102 to <16 x i8>*, !dbg !217
  store <16 x i8> %wide.load.1, <16 x i8>* %103, align 1, !dbg !217, !tbaa !177
  %104 = getelementptr i8, i8* %next.gep80.1, i64 -16, !dbg !217
  %105 = getelementptr i8, i8* %104, i64 -15, !dbg !217
  %106 = bitcast i8* %105 to <16 x i8>*, !dbg !217
  store <16 x i8> %wide.load145.1, <16 x i8>* %106, align 1, !dbg !217, !tbaa !177
  %index.next.1 = add i64 %index, 64, !dbg !214
  %107 = icmp eq i64 %index.next.1, %n.vec, !dbg !214
  br i1 %107, label %middle.block, label %vector.body, !dbg !214, !llvm.loop !218

middle.block:                                     ; preds = %vector.body, %vector.body.preheader.split
  %cmp.n = icmp eq i64 %n.vec, %count
  br i1 %cmp.n, label %.loopexit, label %.lr.ph9.preheader245, !dbg !214

.lr.ph9.preheader245:                             ; preds = %middle.block, %vector.memcheck, %min.iters.checked, %.lr.ph9.preheader
  %b.18.ph = phi i8* [ %64, %vector.memcheck ], [ %64, %min.iters.checked ], [ %64, %.lr.ph9.preheader ], [ %ind.end, %middle.block ]
  %a.17.ph = phi i8* [ %65, %vector.memcheck ], [ %65, %min.iters.checked ], [ %65, %.lr.ph9.preheader ], [ %ind.end15, %middle.block ]
  %.16.ph = phi i64 [ %count, %vector.memcheck ], [ %count, %min.iters.checked ], [ %count, %.lr.ph9.preheader ], [ %ind.end17, %middle.block ]
  %108 = add i64 %.16.ph, -1, !dbg !214
  %xtraiter250 = and i64 %.16.ph, 7, !dbg !214
  %lcmp.mod251 = icmp eq i64 %xtraiter250, 0, !dbg !214
  br i1 %lcmp.mod251, label %.lr.ph9.preheader245.split, label %.lr.ph9.prol, !dbg !214

.lr.ph9.prol:                                     ; preds = %.lr.ph9.preheader245, %.lr.ph9.prol
  %b.18.prol = phi i8* [ %110, %.lr.ph9.prol ], [ %b.18.ph, %.lr.ph9.preheader245 ]
  %a.17.prol = phi i8* [ %112, %.lr.ph9.prol ], [ %a.17.ph, %.lr.ph9.preheader245 ]
  %.16.prol = phi i64 [ %109, %.lr.ph9.prol ], [ %.16.ph, %.lr.ph9.preheader245 ]
  %prol.iter252 = phi i64 [ %prol.iter252.sub, %.lr.ph9.prol ], [ %xtraiter250, %.lr.ph9.preheader245 ]
  %109 = add i64 %.16.prol, -1, !dbg !214
  %110 = getelementptr inbounds i8, i8* %b.18.prol, i64 -1, !dbg !219
  %111 = load i8, i8* %b.18.prol, align 1, !dbg !215, !tbaa !177
  %112 = getelementptr inbounds i8, i8* %a.17.prol, i64 -1, !dbg !220
  store i8 %111, i8* %a.17.prol, align 1, !dbg !217, !tbaa !177
  %prol.iter252.sub = add i64 %prol.iter252, -1, !dbg !210
  %prol.iter252.cmp = icmp eq i64 %prol.iter252.sub, 0, !dbg !210
  br i1 %prol.iter252.cmp, label %.lr.ph9.preheader245.split, label %.lr.ph9.prol, !dbg !210, !llvm.loop !221

.lr.ph9.preheader245.split:                       ; preds = %.lr.ph9.prol, %.lr.ph9.preheader245
  %b.18.unr = phi i8* [ %b.18.ph, %.lr.ph9.preheader245 ], [ %110, %.lr.ph9.prol ]
  %a.17.unr = phi i8* [ %a.17.ph, %.lr.ph9.preheader245 ], [ %112, %.lr.ph9.prol ]
  %.16.unr = phi i64 [ %.16.ph, %.lr.ph9.preheader245 ], [ %109, %.lr.ph9.prol ]
  %113 = icmp ult i64 %108, 7, !dbg !214
  br i1 %113, label %.loopexit, label %.lr.ph9, !dbg !214

.lr.ph9:                                          ; preds = %.lr.ph9.preheader245.split, %.lr.ph9
  %b.18 = phi i8* [ %136, %.lr.ph9 ], [ %b.18.unr, %.lr.ph9.preheader245.split ]
  %a.17 = phi i8* [ %138, %.lr.ph9 ], [ %a.17.unr, %.lr.ph9.preheader245.split ]
  %.16 = phi i64 [ %135, %.lr.ph9 ], [ %.16.unr, %.lr.ph9.preheader245.split ]
  %114 = getelementptr inbounds i8, i8* %b.18, i64 -1, !dbg !219
  %115 = load i8, i8* %b.18, align 1, !dbg !215, !tbaa !177
  %116 = getelementptr inbounds i8, i8* %a.17, i64 -1, !dbg !220
  store i8 %115, i8* %a.17, align 1, !dbg !217, !tbaa !177
  %117 = getelementptr inbounds i8, i8* %b.18, i64 -2, !dbg !219
  %118 = load i8, i8* %114, align 1, !dbg !215, !tbaa !177
  %119 = getelementptr inbounds i8, i8* %a.17, i64 -2, !dbg !220
  store i8 %118, i8* %116, align 1, !dbg !217, !tbaa !177
  %120 = getelementptr inbounds i8, i8* %b.18, i64 -3, !dbg !219
  %121 = load i8, i8* %117, align 1, !dbg !215, !tbaa !177
  %122 = getelementptr inbounds i8, i8* %a.17, i64 -3, !dbg !220
  store i8 %121, i8* %119, align 1, !dbg !217, !tbaa !177
  %123 = getelementptr inbounds i8, i8* %b.18, i64 -4, !dbg !219
  %124 = load i8, i8* %120, align 1, !dbg !215, !tbaa !177
  %125 = getelementptr inbounds i8, i8* %a.17, i64 -4, !dbg !220
  store i8 %124, i8* %122, align 1, !dbg !217, !tbaa !177
  %126 = getelementptr inbounds i8, i8* %b.18, i64 -5, !dbg !219
  %127 = load i8, i8* %123, align 1, !dbg !215, !tbaa !177
  %128 = getelementptr inbounds i8, i8* %a.17, i64 -5, !dbg !220
  store i8 %127, i8* %125, align 1, !dbg !217, !tbaa !177
  %129 = getelementptr inbounds i8, i8* %b.18, i64 -6, !dbg !219
  %130 = load i8, i8* %126, align 1, !dbg !215, !tbaa !177
  %131 = getelementptr inbounds i8, i8* %a.17, i64 -6, !dbg !220
  store i8 %130, i8* %128, align 1, !dbg !217, !tbaa !177
  %132 = getelementptr inbounds i8, i8* %b.18, i64 -7, !dbg !219
  %133 = load i8, i8* %129, align 1, !dbg !215, !tbaa !177
  %134 = getelementptr inbounds i8, i8* %a.17, i64 -7, !dbg !220
  store i8 %133, i8* %131, align 1, !dbg !217, !tbaa !177
  %135 = add i64 %.16, -8, !dbg !214
  %136 = getelementptr inbounds i8, i8* %b.18, i64 -8, !dbg !219
  %137 = load i8, i8* %132, align 1, !dbg !215, !tbaa !177
  %138 = getelementptr inbounds i8, i8* %a.17, i64 -8, !dbg !220
  store i8 %137, i8* %134, align 1, !dbg !217, !tbaa !177
  %139 = icmp eq i64 %135, 0, !dbg !210
  br i1 %139, label %.loopexit, label %.lr.ph9, !dbg !210, !llvm.loop !222

.loopexit:                                        ; preds = %.lr.ph9.preheader245.split, %.lr.ph9, %.lr.ph.preheader244.split, %.lr.ph, %middle.block, %middle.block150, %61, %.preheader, %0
  ret i8* %dst, !dbg !223
}

; Function Attrs: norecurse nounwind uwtable
define weak i8* @mempcpy(i8* %destaddr, i8* %srcaddr, i64 %len) #9 !dbg !91 {
  %1 = icmp eq i64 %len, 0, !dbg !224
  br i1 %1, label %._crit_edge, label %.lr.ph.preheader, !dbg !226

.lr.ph.preheader:                                 ; preds = %0
  %min.iters.check = icmp ult i64 %len, 16, !dbg !227
  br i1 %min.iters.check, label %.lr.ph.preheader72, label %min.iters.checked, !dbg !227

min.iters.checked:                                ; preds = %.lr.ph.preheader
  %n.vec = and i64 %len, -16, !dbg !227
  %cmp.zero = icmp eq i64 %n.vec, 0, !dbg !227
  br i1 %cmp.zero, label %.lr.ph.preheader72, label %vector.memcheck, !dbg !227

vector.memcheck:                                  ; preds = %min.iters.checked
  %2 = add i64 %len, -1, !dbg !227
  %scevgep4 = getelementptr i8, i8* %destaddr, i64 %2, !dbg !227
  %scevgep5 = getelementptr i8, i8* %srcaddr, i64 %2, !dbg !227
  %bound0 = icmp uge i8* %scevgep5, %destaddr, !dbg !227
  %bound1 = icmp uge i8* %scevgep4, %srcaddr, !dbg !227
  %memcheck.conflict = and i1 %bound0, %bound1, !dbg !227
  %ind.end = getelementptr i8, i8* %srcaddr, i64 %n.vec, !dbg !227
  %ind.end7 = getelementptr i8, i8* %destaddr, i64 %n.vec, !dbg !227
  %ind.end9 = sub i64 %len, %n.vec, !dbg !227
  br i1 %memcheck.conflict, label %.lr.ph.preheader72, label %vector.body.preheader, !dbg !227

vector.body.preheader:                            ; preds = %vector.memcheck
  %3 = add i64 %len, -16
  %4 = lshr i64 %3, 4
  %5 = add nuw nsw i64 %4, 1
  %xtraiter73 = and i64 %5, 7
  %lcmp.mod74 = icmp eq i64 %xtraiter73, 0
  br i1 %lcmp.mod74, label %vector.body.preheader.split, label %vector.body.prol

vector.body.prol:                                 ; preds = %vector.body.preheader, %vector.body.prol
  %index.prol = phi i64 [ %index.next.prol, %vector.body.prol ], [ 0, %vector.body.preheader ]
  %prol.iter75 = phi i64 [ %prol.iter75.sub, %vector.body.prol ], [ %xtraiter73, %vector.body.preheader ]
  %next.gep.prol = getelementptr i8, i8* %srcaddr, i64 %index.prol
  %next.gep40.prol = getelementptr i8, i8* %destaddr, i64 %index.prol
  %6 = bitcast i8* %next.gep.prol to <16 x i8>*, !dbg !228
  %wide.load.prol = load <16 x i8>, <16 x i8>* %6, align 1, !dbg !228, !tbaa !177
  %7 = bitcast i8* %next.gep40.prol to <16 x i8>*, !dbg !229
  store <16 x i8> %wide.load.prol, <16 x i8>* %7, align 1, !dbg !229, !tbaa !177
  %index.next.prol = add i64 %index.prol, 16, !dbg !227
  %prol.iter75.sub = add i64 %prol.iter75, -1, !dbg !227
  %prol.iter75.cmp = icmp eq i64 %prol.iter75.sub, 0, !dbg !227
  br i1 %prol.iter75.cmp, label %vector.body.preheader.split, label %vector.body.prol, !dbg !227, !llvm.loop !230

vector.body.preheader.split:                      ; preds = %vector.body.prol, %vector.body.preheader
  %index.unr = phi i64 [ 0, %vector.body.preheader ], [ %index.next.prol, %vector.body.prol ]
  %8 = icmp ult i64 %3, 112
  br i1 %8, label %middle.block, label %vector.body

vector.body:                                      ; preds = %vector.body.preheader.split, %vector.body
  %index = phi i64 [ %index.next.7, %vector.body ], [ %index.unr, %vector.body.preheader.split ]
  %next.gep = getelementptr i8, i8* %srcaddr, i64 %index
  %next.gep40 = getelementptr i8, i8* %destaddr, i64 %index
  %9 = bitcast i8* %next.gep to <16 x i8>*, !dbg !228
  %wide.load = load <16 x i8>, <16 x i8>* %9, align 1, !dbg !228, !tbaa !177
  %10 = bitcast i8* %next.gep40 to <16 x i8>*, !dbg !229
  store <16 x i8> %wide.load, <16 x i8>* %10, align 1, !dbg !229, !tbaa !177
  %index.next = add i64 %index, 16, !dbg !227
  %next.gep.1 = getelementptr i8, i8* %srcaddr, i64 %index.next
  %next.gep40.1 = getelementptr i8, i8* %destaddr, i64 %index.next
  %11 = bitcast i8* %next.gep.1 to <16 x i8>*, !dbg !228
  %wide.load.1 = load <16 x i8>, <16 x i8>* %11, align 1, !dbg !228, !tbaa !177
  %12 = bitcast i8* %next.gep40.1 to <16 x i8>*, !dbg !229
  store <16 x i8> %wide.load.1, <16 x i8>* %12, align 1, !dbg !229, !tbaa !177
  %index.next.1 = add i64 %index, 32, !dbg !227
  %next.gep.2 = getelementptr i8, i8* %srcaddr, i64 %index.next.1
  %next.gep40.2 = getelementptr i8, i8* %destaddr, i64 %index.next.1
  %13 = bitcast i8* %next.gep.2 to <16 x i8>*, !dbg !228
  %wide.load.2 = load <16 x i8>, <16 x i8>* %13, align 1, !dbg !228, !tbaa !177
  %14 = bitcast i8* %next.gep40.2 to <16 x i8>*, !dbg !229
  store <16 x i8> %wide.load.2, <16 x i8>* %14, align 1, !dbg !229, !tbaa !177
  %index.next.2 = add i64 %index, 48, !dbg !227
  %next.gep.3 = getelementptr i8, i8* %srcaddr, i64 %index.next.2
  %next.gep40.3 = getelementptr i8, i8* %destaddr, i64 %index.next.2
  %15 = bitcast i8* %next.gep.3 to <16 x i8>*, !dbg !228
  %wide.load.3 = load <16 x i8>, <16 x i8>* %15, align 1, !dbg !228, !tbaa !177
  %16 = bitcast i8* %next.gep40.3 to <16 x i8>*, !dbg !229
  store <16 x i8> %wide.load.3, <16 x i8>* %16, align 1, !dbg !229, !tbaa !177
  %index.next.3 = add i64 %index, 64, !dbg !227
  %next.gep.4 = getelementptr i8, i8* %srcaddr, i64 %index.next.3
  %next.gep40.4 = getelementptr i8, i8* %destaddr, i64 %index.next.3
  %17 = bitcast i8* %next.gep.4 to <16 x i8>*, !dbg !228
  %wide.load.4 = load <16 x i8>, <16 x i8>* %17, align 1, !dbg !228, !tbaa !177
  %18 = bitcast i8* %next.gep40.4 to <16 x i8>*, !dbg !229
  store <16 x i8> %wide.load.4, <16 x i8>* %18, align 1, !dbg !229, !tbaa !177
  %index.next.4 = add i64 %index, 80, !dbg !227
  %next.gep.5 = getelementptr i8, i8* %srcaddr, i64 %index.next.4
  %next.gep40.5 = getelementptr i8, i8* %destaddr, i64 %index.next.4
  %19 = bitcast i8* %next.gep.5 to <16 x i8>*, !dbg !228
  %wide.load.5 = load <16 x i8>, <16 x i8>* %19, align 1, !dbg !228, !tbaa !177
  %20 = bitcast i8* %next.gep40.5 to <16 x i8>*, !dbg !229
  store <16 x i8> %wide.load.5, <16 x i8>* %20, align 1, !dbg !229, !tbaa !177
  %index.next.5 = add i64 %index, 96, !dbg !227
  %next.gep.6 = getelementptr i8, i8* %srcaddr, i64 %index.next.5
  %next.gep40.6 = getelementptr i8, i8* %destaddr, i64 %index.next.5
  %21 = bitcast i8* %next.gep.6 to <16 x i8>*, !dbg !228
  %wide.load.6 = load <16 x i8>, <16 x i8>* %21, align 1, !dbg !228, !tbaa !177
  %22 = bitcast i8* %next.gep40.6 to <16 x i8>*, !dbg !229
  store <16 x i8> %wide.load.6, <16 x i8>* %22, align 1, !dbg !229, !tbaa !177
  %index.next.6 = add i64 %index, 112, !dbg !227
  %next.gep.7 = getelementptr i8, i8* %srcaddr, i64 %index.next.6
  %next.gep40.7 = getelementptr i8, i8* %destaddr, i64 %index.next.6
  %23 = bitcast i8* %next.gep.7 to <16 x i8>*, !dbg !228
  %wide.load.7 = load <16 x i8>, <16 x i8>* %23, align 1, !dbg !228, !tbaa !177
  %24 = bitcast i8* %next.gep40.7 to <16 x i8>*, !dbg !229
  store <16 x i8> %wide.load.7, <16 x i8>* %24, align 1, !dbg !229, !tbaa !177
  %index.next.7 = add i64 %index, 128, !dbg !227
  %25 = icmp eq i64 %index.next.7, %n.vec, !dbg !227
  br i1 %25, label %middle.block, label %vector.body, !dbg !227, !llvm.loop !231

middle.block:                                     ; preds = %vector.body, %vector.body.preheader.split
  %cmp.n = icmp eq i64 %n.vec, %len
  br i1 %cmp.n, label %._crit_edge.loopexit, label %.lr.ph.preheader72, !dbg !227

.lr.ph.preheader72:                               ; preds = %middle.block, %vector.memcheck, %min.iters.checked, %.lr.ph.preheader
  %src.03.ph = phi i8* [ %srcaddr, %vector.memcheck ], [ %srcaddr, %min.iters.checked ], [ %srcaddr, %.lr.ph.preheader ], [ %ind.end, %middle.block ]
  %dest.02.ph = phi i8* [ %destaddr, %vector.memcheck ], [ %destaddr, %min.iters.checked ], [ %destaddr, %.lr.ph.preheader ], [ %ind.end7, %middle.block ]
  %.01.ph = phi i64 [ %len, %vector.memcheck ], [ %len, %min.iters.checked ], [ %len, %.lr.ph.preheader ], [ %ind.end9, %middle.block ]
  %26 = add i64 %.01.ph, -1, !dbg !227
  %xtraiter = and i64 %.01.ph, 7, !dbg !227
  %lcmp.mod = icmp eq i64 %xtraiter, 0, !dbg !227
  br i1 %lcmp.mod, label %.lr.ph.preheader72.split, label %.lr.ph.prol, !dbg !227

.lr.ph.prol:                                      ; preds = %.lr.ph.preheader72, %.lr.ph.prol
  %src.03.prol = phi i8* [ %28, %.lr.ph.prol ], [ %src.03.ph, %.lr.ph.preheader72 ]
  %dest.02.prol = phi i8* [ %30, %.lr.ph.prol ], [ %dest.02.ph, %.lr.ph.preheader72 ]
  %.01.prol = phi i64 [ %27, %.lr.ph.prol ], [ %.01.ph, %.lr.ph.preheader72 ]
  %prol.iter = phi i64 [ %prol.iter.sub, %.lr.ph.prol ], [ %xtraiter, %.lr.ph.preheader72 ]
  %27 = add i64 %.01.prol, -1, !dbg !227
  %28 = getelementptr inbounds i8, i8* %src.03.prol, i64 1, !dbg !232
  %29 = load i8, i8* %src.03.prol, align 1, !dbg !228, !tbaa !177
  %30 = getelementptr inbounds i8, i8* %dest.02.prol, i64 1, !dbg !233
  store i8 %29, i8* %dest.02.prol, align 1, !dbg !229, !tbaa !177
  %prol.iter.sub = add i64 %prol.iter, -1, !dbg !226
  %prol.iter.cmp = icmp eq i64 %prol.iter.sub, 0, !dbg !226
  br i1 %prol.iter.cmp, label %.lr.ph.preheader72.split, label %.lr.ph.prol, !dbg !226, !llvm.loop !234

.lr.ph.preheader72.split:                         ; preds = %.lr.ph.prol, %.lr.ph.preheader72
  %src.03.unr = phi i8* [ %src.03.ph, %.lr.ph.preheader72 ], [ %28, %.lr.ph.prol ]
  %dest.02.unr = phi i8* [ %dest.02.ph, %.lr.ph.preheader72 ], [ %30, %.lr.ph.prol ]
  %.01.unr = phi i64 [ %.01.ph, %.lr.ph.preheader72 ], [ %27, %.lr.ph.prol ]
  %31 = icmp ult i64 %26, 7, !dbg !227
  br i1 %31, label %._crit_edge.loopexit, label %.lr.ph, !dbg !227

.lr.ph:                                           ; preds = %.lr.ph.preheader72.split, %.lr.ph
  %src.03 = phi i8* [ %54, %.lr.ph ], [ %src.03.unr, %.lr.ph.preheader72.split ]
  %dest.02 = phi i8* [ %56, %.lr.ph ], [ %dest.02.unr, %.lr.ph.preheader72.split ]
  %.01 = phi i64 [ %53, %.lr.ph ], [ %.01.unr, %.lr.ph.preheader72.split ]
  %32 = getelementptr inbounds i8, i8* %src.03, i64 1, !dbg !232
  %33 = load i8, i8* %src.03, align 1, !dbg !228, !tbaa !177
  %34 = getelementptr inbounds i8, i8* %dest.02, i64 1, !dbg !233
  store i8 %33, i8* %dest.02, align 1, !dbg !229, !tbaa !177
  %35 = getelementptr inbounds i8, i8* %src.03, i64 2, !dbg !232
  %36 = load i8, i8* %32, align 1, !dbg !228, !tbaa !177
  %37 = getelementptr inbounds i8, i8* %dest.02, i64 2, !dbg !233
  store i8 %36, i8* %34, align 1, !dbg !229, !tbaa !177
  %38 = getelementptr inbounds i8, i8* %src.03, i64 3, !dbg !232
  %39 = load i8, i8* %35, align 1, !dbg !228, !tbaa !177
  %40 = getelementptr inbounds i8, i8* %dest.02, i64 3, !dbg !233
  store i8 %39, i8* %37, align 1, !dbg !229, !tbaa !177
  %41 = getelementptr inbounds i8, i8* %src.03, i64 4, !dbg !232
  %42 = load i8, i8* %38, align 1, !dbg !228, !tbaa !177
  %43 = getelementptr inbounds i8, i8* %dest.02, i64 4, !dbg !233
  store i8 %42, i8* %40, align 1, !dbg !229, !tbaa !177
  %44 = getelementptr inbounds i8, i8* %src.03, i64 5, !dbg !232
  %45 = load i8, i8* %41, align 1, !dbg !228, !tbaa !177
  %46 = getelementptr inbounds i8, i8* %dest.02, i64 5, !dbg !233
  store i8 %45, i8* %43, align 1, !dbg !229, !tbaa !177
  %47 = getelementptr inbounds i8, i8* %src.03, i64 6, !dbg !232
  %48 = load i8, i8* %44, align 1, !dbg !228, !tbaa !177
  %49 = getelementptr inbounds i8, i8* %dest.02, i64 6, !dbg !233
  store i8 %48, i8* %46, align 1, !dbg !229, !tbaa !177
  %50 = getelementptr inbounds i8, i8* %src.03, i64 7, !dbg !232
  %51 = load i8, i8* %47, align 1, !dbg !228, !tbaa !177
  %52 = getelementptr inbounds i8, i8* %dest.02, i64 7, !dbg !233
  store i8 %51, i8* %49, align 1, !dbg !229, !tbaa !177
  %53 = add i64 %.01, -8, !dbg !227
  %54 = getelementptr inbounds i8, i8* %src.03, i64 8, !dbg !232
  %55 = load i8, i8* %50, align 1, !dbg !228, !tbaa !177
  %56 = getelementptr inbounds i8, i8* %dest.02, i64 8, !dbg !233
  store i8 %55, i8* %52, align 1, !dbg !229, !tbaa !177
  %57 = icmp eq i64 %53, 0, !dbg !224
  br i1 %57, label %._crit_edge.loopexit, label %.lr.ph, !dbg !226, !llvm.loop !235

._crit_edge.loopexit:                             ; preds = %.lr.ph.preheader72.split, %.lr.ph, %middle.block
  %scevgep = getelementptr i8, i8* %destaddr, i64 %len, !dbg !227
  br label %._crit_edge, !dbg !236

._crit_edge:                                      ; preds = %._crit_edge.loopexit, %0
  %dest.0.lcssa = phi i8* [ %destaddr, %0 ], [ %scevgep, %._crit_edge.loopexit ]
  ret i8* %dest.0.lcssa, !dbg !236
}

; Function Attrs: norecurse nounwind uwtable
define weak i8* @memset(i8* %dst, i32 %s, i64 %count) #9 !dbg !101 {
  %1 = icmp eq i64 %count, 0, !dbg !237
  br i1 %1, label %._crit_edge, label %.lr.ph, !dbg !239

.lr.ph:                                           ; preds = %0
  %2 = trunc i32 %s to i8, !dbg !240
  %3 = add i64 %count, -1, !dbg !239
  %xtraiter = and i64 %count, 7, !dbg !239
  %lcmp.mod = icmp eq i64 %xtraiter, 0, !dbg !239
  br i1 %lcmp.mod, label %.lr.ph.split, label %.preheader, !dbg !239

.preheader:                                       ; preds = %.lr.ph, %.preheader
  %a.02.prol = phi i8* [ %5, %.preheader ], [ %dst, %.lr.ph ]
  %.01.prol = phi i64 [ %4, %.preheader ], [ %count, %.lr.ph ]
  %prol.iter = phi i64 [ %prol.iter.sub, %.preheader ], [ %xtraiter, %.lr.ph ]
  %4 = add i64 %.01.prol, -1, !dbg !241
  %5 = getelementptr inbounds i8, i8* %a.02.prol, i64 1, !dbg !242
  store volatile i8 %2, i8* %a.02.prol, align 1, !dbg !243, !tbaa !177
  %prol.iter.sub = add i64 %prol.iter, -1, !dbg !239
  %prol.iter.cmp = icmp eq i64 %prol.iter.sub, 0, !dbg !239
  br i1 %prol.iter.cmp, label %.lr.ph.split, label %.preheader, !dbg !239, !llvm.loop !244

.lr.ph.split:                                     ; preds = %.preheader, %.lr.ph
  %a.02.unr = phi i8* [ %dst, %.lr.ph ], [ %5, %.preheader ]
  %.01.unr = phi i64 [ %count, %.lr.ph ], [ %4, %.preheader ]
  %6 = icmp ult i64 %3, 7, !dbg !239
  br i1 %6, label %._crit_edge, label %.lr.ph.split.split, !dbg !239

.lr.ph.split.split:                               ; preds = %.lr.ph.split, %.lr.ph.split.split
  %a.02 = phi i8* [ %15, %.lr.ph.split.split ], [ %a.02.unr, %.lr.ph.split ]
  %.01 = phi i64 [ %14, %.lr.ph.split.split ], [ %.01.unr, %.lr.ph.split ]
  %7 = getelementptr inbounds i8, i8* %a.02, i64 1, !dbg !242
  store volatile i8 %2, i8* %a.02, align 1, !dbg !243, !tbaa !177
  %8 = getelementptr inbounds i8, i8* %a.02, i64 2, !dbg !242
  store volatile i8 %2, i8* %7, align 1, !dbg !243, !tbaa !177
  %9 = getelementptr inbounds i8, i8* %a.02, i64 3, !dbg !242
  store volatile i8 %2, i8* %8, align 1, !dbg !243, !tbaa !177
  %10 = getelementptr inbounds i8, i8* %a.02, i64 4, !dbg !242
  store volatile i8 %2, i8* %9, align 1, !dbg !243, !tbaa !177
  %11 = getelementptr inbounds i8, i8* %a.02, i64 5, !dbg !242
  store volatile i8 %2, i8* %10, align 1, !dbg !243, !tbaa !177
  %12 = getelementptr inbounds i8, i8* %a.02, i64 6, !dbg !242
  store volatile i8 %2, i8* %11, align 1, !dbg !243, !tbaa !177
  %13 = getelementptr inbounds i8, i8* %a.02, i64 7, !dbg !242
  store volatile i8 %2, i8* %12, align 1, !dbg !243, !tbaa !177
  %14 = add i64 %.01, -8, !dbg !241
  %15 = getelementptr inbounds i8, i8* %a.02, i64 8, !dbg !242
  store volatile i8 %2, i8* %13, align 1, !dbg !243, !tbaa !177
  %16 = icmp eq i64 %14, 0, !dbg !237
  br i1 %16, label %._crit_edge, label %.lr.ph.split.split, !dbg !239

._crit_edge:                                      ; preds = %.lr.ph.split, %.lr.ph.split.split, %0
  ret i8* %dst, !dbg !245
}

attributes #0 = { noredzone nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="-avx" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { inlinehint noredzone nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="-avx" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { noredzone "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="-avx" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { alwaysinline inlinehint noredzone nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="-avx" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { nounwind readnone }
attributes #6 = { argmemonly nounwind }
attributes #7 = { "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #8 = { noreturn "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #9 = { norecurse nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #10 = { noredzone }
attributes #11 = { nobuiltin nounwind }
attributes #12 = { nobuiltin noreturn nounwind }

!llvm.ident = !{!0, !1, !1, !1, !1, !1, !1, !1, !1, !1}
!llvm.dbg.cu = !{!2, !15, !24, !37, !47, !60, !78, !88, !98}
!llvm.module.flags = !{!111, !112}

!0 = !{!"clang version 3.7.0 (trunk)"}
!1 = !{!"clang version 3.8.0-2ubuntu3~trusty5 (tags/RELEASE_380/final)"}
!2 = distinct !DICompileUnit(language: DW_LANG_C89, file: !3, producer: "clang version 3.8.0-2ubuntu3~trusty5 (tags/RELEASE_380/final)", isOptimized: true, runtimeVersion: 0, emissionKind: 1, enums: !4, subprograms: !5)
!3 = !DIFile(filename: "/home/tuba/Documents/tools/klee-3.8/klee/runtime/Intrinsic/klee_choose.c", directory: "/home/tuba/Documents/tools/klee-3.8/klee_build_dir/runtime/Intrinsic")
!4 = !{}
!5 = !{!6}
!6 = distinct !DISubprogram(name: "klee_choose", scope: !3, file: !3, line: 12, type: !7, isLocal: false, isDefinition: true, scopeLine: 12, flags: DIFlagPrototyped, isOptimized: true, variables: !12)
!7 = !DISubroutineType(types: !8)
!8 = !{!9, !9}
!9 = !DIDerivedType(tag: DW_TAG_typedef, name: "uintptr_t", file: !10, line: 122, baseType: !11)
!10 = !DIFile(filename: "/usr/include/stdint.h", directory: "/home/tuba/Documents/tools/klee-3.8/klee_build_dir/runtime/Intrinsic")
!11 = !DIBasicType(name: "long unsigned int", size: 64, align: 64, encoding: DW_ATE_unsigned)
!12 = !{!13, !14}
!13 = !DILocalVariable(name: "n", arg: 1, scope: !6, file: !3, line: 12, type: !9)
!14 = !DILocalVariable(name: "x", scope: !6, file: !3, line: 13, type: !9)
!15 = distinct !DICompileUnit(language: DW_LANG_C89, file: !16, producer: "clang version 3.8.0-2ubuntu3~trusty5 (tags/RELEASE_380/final)", isOptimized: true, runtimeVersion: 0, emissionKind: 1, enums: !4, subprograms: !17)
!16 = !DIFile(filename: "/home/tuba/Documents/tools/klee-3.8/klee/runtime/Intrinsic/klee_div_zero_check.c", directory: "/home/tuba/Documents/tools/klee-3.8/klee_build_dir/runtime/Intrinsic")
!17 = !{!18}
!18 = distinct !DISubprogram(name: "klee_div_zero_check", scope: !16, file: !16, line: 12, type: !19, isLocal: false, isDefinition: true, scopeLine: 12, flags: DIFlagPrototyped, isOptimized: true, variables: !22)
!19 = !DISubroutineType(types: !20)
!20 = !{null, !21}
!21 = !DIBasicType(name: "long long int", size: 64, align: 64, encoding: DW_ATE_signed)
!22 = !{!23}
!23 = !DILocalVariable(name: "z", arg: 1, scope: !18, file: !16, line: 12, type: !21)
!24 = distinct !DICompileUnit(language: DW_LANG_C89, file: !25, producer: "clang version 3.8.0-2ubuntu3~trusty5 (tags/RELEASE_380/final)", isOptimized: true, runtimeVersion: 0, emissionKind: 1, enums: !4, subprograms: !26)
!25 = !DIFile(filename: "/home/tuba/Documents/tools/klee-3.8/klee/runtime/Intrinsic/klee_int.c", directory: "/home/tuba/Documents/tools/klee-3.8/klee_build_dir/runtime/Intrinsic")
!26 = !{!27}
!27 = distinct !DISubprogram(name: "klee_int", scope: !25, file: !25, line: 13, type: !28, isLocal: false, isDefinition: true, scopeLine: 13, flags: DIFlagPrototyped, isOptimized: true, variables: !34)
!28 = !DISubroutineType(types: !29)
!29 = !{!30, !31}
!30 = !DIBasicType(name: "int", size: 32, align: 32, encoding: DW_ATE_signed)
!31 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !32, size: 64, align: 64)
!32 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !33)
!33 = !DIBasicType(name: "char", size: 8, align: 8, encoding: DW_ATE_signed_char)
!34 = !{!35, !36}
!35 = !DILocalVariable(name: "name", arg: 1, scope: !27, file: !25, line: 13, type: !31)
!36 = !DILocalVariable(name: "x", scope: !27, file: !25, line: 14, type: !30)
!37 = distinct !DICompileUnit(language: DW_LANG_C89, file: !38, producer: "clang version 3.8.0-2ubuntu3~trusty5 (tags/RELEASE_380/final)", isOptimized: true, runtimeVersion: 0, emissionKind: 1, enums: !4, subprograms: !39)
!38 = !DIFile(filename: "/home/tuba/Documents/tools/klee-3.8/klee/runtime/Intrinsic/klee_overshift_check.c", directory: "/home/tuba/Documents/tools/klee-3.8/klee_build_dir/runtime/Intrinsic")
!39 = !{!40}
!40 = distinct !DISubprogram(name: "klee_overshift_check", scope: !38, file: !38, line: 20, type: !41, isLocal: false, isDefinition: true, scopeLine: 20, flags: DIFlagPrototyped, isOptimized: true, variables: !44)
!41 = !DISubroutineType(types: !42)
!42 = !{null, !43, !43}
!43 = !DIBasicType(name: "long long unsigned int", size: 64, align: 64, encoding: DW_ATE_unsigned)
!44 = !{!45, !46}
!45 = !DILocalVariable(name: "bitWidth", arg: 1, scope: !40, file: !38, line: 20, type: !43)
!46 = !DILocalVariable(name: "shift", arg: 2, scope: !40, file: !38, line: 20, type: !43)
!47 = distinct !DICompileUnit(language: DW_LANG_C89, file: !48, producer: "clang version 3.8.0-2ubuntu3~trusty5 (tags/RELEASE_380/final)", isOptimized: true, runtimeVersion: 0, emissionKind: 1, enums: !4, retainedTypes: !49, subprograms: !51)
!48 = !DIFile(filename: "/home/tuba/Documents/tools/klee-3.8/klee/runtime/Intrinsic/klee_range.c", directory: "/home/tuba/Documents/tools/klee-3.8/klee_build_dir/runtime/Intrinsic")
!49 = !{!50}
!50 = !DIBasicType(name: "unsigned int", size: 32, align: 32, encoding: DW_ATE_unsigned)
!51 = !{!52}
!52 = distinct !DISubprogram(name: "klee_range", scope: !48, file: !48, line: 13, type: !53, isLocal: false, isDefinition: true, scopeLine: 13, flags: DIFlagPrototyped, isOptimized: true, variables: !55)
!53 = !DISubroutineType(types: !54)
!54 = !{!30, !30, !30, !31}
!55 = !{!56, !57, !58, !59}
!56 = !DILocalVariable(name: "start", arg: 1, scope: !52, file: !48, line: 13, type: !30)
!57 = !DILocalVariable(name: "end", arg: 2, scope: !52, file: !48, line: 13, type: !30)
!58 = !DILocalVariable(name: "name", arg: 3, scope: !52, file: !48, line: 13, type: !31)
!59 = !DILocalVariable(name: "x", scope: !52, file: !48, line: 14, type: !30)
!60 = distinct !DICompileUnit(language: DW_LANG_C89, file: !61, producer: "clang version 3.8.0-2ubuntu3~trusty5 (tags/RELEASE_380/final)", isOptimized: true, runtimeVersion: 0, emissionKind: 1, enums: !4, subprograms: !62)
!61 = !DIFile(filename: "/home/tuba/Documents/tools/klee-3.8/klee/runtime/Intrinsic/memcpy.c", directory: "/home/tuba/Documents/tools/klee-3.8/klee_build_dir/runtime/Intrinsic")
!62 = !{!63}
!63 = distinct !DISubprogram(name: "memcpy", scope: !61, file: !61, line: 12, type: !64, isLocal: false, isDefinition: true, scopeLine: 12, flags: DIFlagPrototyped, isOptimized: true, variables: !71)
!64 = !DISubroutineType(types: !65)
!65 = !{!66, !66, !67, !69}
!66 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null, size: 64, align: 64)
!67 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !68, size: 64, align: 64)
!68 = !DIDerivedType(tag: DW_TAG_const_type, baseType: null)
!69 = !DIDerivedType(tag: DW_TAG_typedef, name: "size_t", file: !70, line: 62, baseType: !11)
!70 = !DIFile(filename: "/usr/lib/llvm-3.8/bin/../lib/clang/3.8.0/include/stddef.h", directory: "/home/tuba/Documents/tools/klee-3.8/klee_build_dir/runtime/Intrinsic")
!71 = !{!72, !73, !74, !75, !77}
!72 = !DILocalVariable(name: "destaddr", arg: 1, scope: !63, file: !61, line: 12, type: !66)
!73 = !DILocalVariable(name: "srcaddr", arg: 2, scope: !63, file: !61, line: 12, type: !67)
!74 = !DILocalVariable(name: "len", arg: 3, scope: !63, file: !61, line: 12, type: !69)
!75 = !DILocalVariable(name: "dest", scope: !63, file: !61, line: 13, type: !76)
!76 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !33, size: 64, align: 64)
!77 = !DILocalVariable(name: "src", scope: !63, file: !61, line: 14, type: !31)
!78 = distinct !DICompileUnit(language: DW_LANG_C89, file: !79, producer: "clang version 3.8.0-2ubuntu3~trusty5 (tags/RELEASE_380/final)", isOptimized: true, runtimeVersion: 0, emissionKind: 1, enums: !4, subprograms: !80)
!79 = !DIFile(filename: "/home/tuba/Documents/tools/klee-3.8/klee/runtime/Intrinsic/memmove.c", directory: "/home/tuba/Documents/tools/klee-3.8/klee_build_dir/runtime/Intrinsic")
!80 = !{!81}
!81 = distinct !DISubprogram(name: "memmove", scope: !79, file: !79, line: 12, type: !64, isLocal: false, isDefinition: true, scopeLine: 12, flags: DIFlagPrototyped, isOptimized: true, variables: !82)
!82 = !{!83, !84, !85, !86, !87}
!83 = !DILocalVariable(name: "dst", arg: 1, scope: !81, file: !79, line: 12, type: !66)
!84 = !DILocalVariable(name: "src", arg: 2, scope: !81, file: !79, line: 12, type: !67)
!85 = !DILocalVariable(name: "count", arg: 3, scope: !81, file: !79, line: 12, type: !69)
!86 = !DILocalVariable(name: "a", scope: !81, file: !79, line: 13, type: !76)
!87 = !DILocalVariable(name: "b", scope: !81, file: !79, line: 14, type: !31)
!88 = distinct !DICompileUnit(language: DW_LANG_C89, file: !89, producer: "clang version 3.8.0-2ubuntu3~trusty5 (tags/RELEASE_380/final)", isOptimized: true, runtimeVersion: 0, emissionKind: 1, enums: !4, subprograms: !90)
!89 = !DIFile(filename: "/home/tuba/Documents/tools/klee-3.8/klee/runtime/Intrinsic/mempcpy.c", directory: "/home/tuba/Documents/tools/klee-3.8/klee_build_dir/runtime/Intrinsic")
!90 = !{!91}
!91 = distinct !DISubprogram(name: "mempcpy", scope: !89, file: !89, line: 11, type: !64, isLocal: false, isDefinition: true, scopeLine: 11, flags: DIFlagPrototyped, isOptimized: true, variables: !92)
!92 = !{!93, !94, !95, !96, !97}
!93 = !DILocalVariable(name: "destaddr", arg: 1, scope: !91, file: !89, line: 11, type: !66)
!94 = !DILocalVariable(name: "srcaddr", arg: 2, scope: !91, file: !89, line: 11, type: !67)
!95 = !DILocalVariable(name: "len", arg: 3, scope: !91, file: !89, line: 11, type: !69)
!96 = !DILocalVariable(name: "dest", scope: !91, file: !89, line: 12, type: !76)
!97 = !DILocalVariable(name: "src", scope: !91, file: !89, line: 13, type: !31)
!98 = distinct !DICompileUnit(language: DW_LANG_C89, file: !99, producer: "clang version 3.8.0-2ubuntu3~trusty5 (tags/RELEASE_380/final)", isOptimized: true, runtimeVersion: 0, emissionKind: 1, enums: !4, subprograms: !100)
!99 = !DIFile(filename: "/home/tuba/Documents/tools/klee-3.8/klee/runtime/Intrinsic/memset.c", directory: "/home/tuba/Documents/tools/klee-3.8/klee_build_dir/runtime/Intrinsic")
!100 = !{!101}
!101 = distinct !DISubprogram(name: "memset", scope: !99, file: !99, line: 11, type: !102, isLocal: false, isDefinition: true, scopeLine: 11, flags: DIFlagPrototyped, isOptimized: true, variables: !104)
!102 = !DISubroutineType(types: !103)
!103 = !{!66, !66, !30, !69}
!104 = !{!105, !106, !107, !108}
!105 = !DILocalVariable(name: "dst", arg: 1, scope: !101, file: !99, line: 11, type: !66)
!106 = !DILocalVariable(name: "s", arg: 2, scope: !101, file: !99, line: 11, type: !30)
!107 = !DILocalVariable(name: "count", arg: 3, scope: !101, file: !99, line: 11, type: !69)
!108 = !DILocalVariable(name: "a", scope: !101, file: !99, line: 12, type: !109)
!109 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !110, size: 64, align: 64)
!110 = !DIDerivedType(tag: DW_TAG_volatile_type, baseType: !33)
!111 = !{i32 2, !"Dwarf Version", i32 4}
!112 = !{i32 2, !"Debug Info Version", i32 3}
!113 = !DILocation(line: 13, column: 3, scope: !6)
!114 = !DILocation(line: 14, column: 3, scope: !6)
!115 = !DILocation(line: 17, column: 6, scope: !116)
!116 = distinct !DILexicalBlock(scope: !6, file: !3, line: 17, column: 6)
!117 = !{!118, !118, i64 0}
!118 = !{!"long", !119, i64 0}
!119 = !{!"omnipotent char", !120, i64 0}
!120 = !{!"Simple C/C++ TBAA"}
!121 = !DILocation(line: 17, column: 8, scope: !116)
!122 = !DILocation(line: 17, column: 6, scope: !6)
!123 = !DILocation(line: 18, column: 5, scope: !116)
!124 = !DILocation(line: 19, column: 3, scope: !6)
!125 = !DILocation(line: 13, column: 9, scope: !126)
!126 = distinct !DILexicalBlock(scope: !18, file: !16, line: 13, column: 7)
!127 = !DILocation(line: 13, column: 7, scope: !18)
!128 = !DILocation(line: 14, column: 5, scope: !126)
!129 = !DILocation(line: 15, column: 1, scope: !18)
!130 = !DILocation(line: 14, column: 3, scope: !27)
!131 = !DILocation(line: 15, column: 3, scope: !27)
!132 = !DILocation(line: 16, column: 10, scope: !27)
!133 = !{!134, !134, i64 0}
!134 = !{!"int", !119, i64 0}
!135 = !DILocation(line: 16, column: 3, scope: !27)
!136 = !DILocation(line: 21, column: 13, scope: !137)
!137 = distinct !DILexicalBlock(scope: !40, file: !38, line: 21, column: 7)
!138 = !DILocation(line: 21, column: 7, scope: !40)
!139 = !DILocation(line: 27, column: 5, scope: !140)
!140 = distinct !DILexicalBlock(scope: !137, file: !38, line: 21, column: 26)
!141 = !DILocation(line: 29, column: 1, scope: !40)
!142 = !DILocation(line: 14, column: 3, scope: !52)
!143 = !DILocation(line: 16, column: 13, scope: !144)
!144 = distinct !DILexicalBlock(scope: !52, file: !48, line: 16, column: 7)
!145 = !DILocation(line: 16, column: 7, scope: !52)
!146 = !DILocation(line: 17, column: 5, scope: !144)
!147 = !DILocation(line: 19, column: 12, scope: !148)
!148 = distinct !DILexicalBlock(scope: !52, file: !48, line: 19, column: 7)
!149 = !DILocation(line: 19, column: 14, scope: !148)
!150 = !DILocation(line: 19, column: 7, scope: !52)
!151 = !DILocation(line: 22, column: 5, scope: !152)
!152 = distinct !DILexicalBlock(scope: !148, file: !48, line: 21, column: 10)
!153 = !DILocation(line: 25, column: 14, scope: !154)
!154 = distinct !DILexicalBlock(scope: !152, file: !48, line: 25, column: 9)
!155 = !DILocation(line: 26, column: 30, scope: !156)
!156 = distinct !DILexicalBlock(scope: !154, file: !48, line: 25, column: 19)
!157 = !DILocation(line: 25, column: 9, scope: !152)
!158 = !DILocation(line: 26, column: 32, scope: !156)
!159 = !DILocation(line: 26, column: 19, scope: !156)
!160 = !DILocation(line: 26, column: 7, scope: !156)
!161 = !DILocation(line: 27, column: 5, scope: !156)
!162 = !DILocation(line: 28, column: 25, scope: !163)
!163 = distinct !DILexicalBlock(scope: !154, file: !48, line: 27, column: 12)
!164 = !DILocation(line: 28, column: 19, scope: !163)
!165 = !DILocation(line: 28, column: 7, scope: !163)
!166 = !DILocation(line: 29, column: 19, scope: !163)
!167 = !DILocation(line: 29, column: 21, scope: !163)
!168 = !DILocation(line: 29, column: 7, scope: !163)
!169 = !DILocation(line: 32, column: 12, scope: !152)
!170 = !DILocation(line: 32, column: 5, scope: !152)
!171 = !DILocation(line: 34, column: 1, scope: !52)
!172 = !DILocation(line: 16, column: 16, scope: !173)
!173 = !DILexicalBlockFile(scope: !63, file: !61, discriminator: 1)
!174 = !DILocation(line: 16, column: 3, scope: !173)
!175 = !DILocation(line: 16, column: 13, scope: !173)
!176 = !DILocation(line: 17, column: 15, scope: !63)
!177 = !{!119, !119, i64 0}
!178 = !DILocation(line: 17, column: 13, scope: !63)
!179 = distinct !{!179, !180}
!180 = !{!"llvm.loop.unroll.disable"}
!181 = distinct !{!181, !182, !183}
!182 = !{!"llvm.loop.vectorize.width", i32 1}
!183 = !{!"llvm.loop.interleave.count", i32 1}
!184 = !DILocation(line: 17, column: 19, scope: !63)
!185 = !DILocation(line: 17, column: 10, scope: !63)
!186 = distinct !{!186, !180}
!187 = distinct !{!187, !182, !183}
!188 = !DILocation(line: 18, column: 3, scope: !63)
!189 = !DILocation(line: 16, column: 11, scope: !190)
!190 = distinct !DILexicalBlock(scope: !81, file: !79, line: 16, column: 7)
!191 = !DILocation(line: 16, column: 7, scope: !81)
!192 = !DILocation(line: 19, column: 10, scope: !193)
!193 = distinct !DILexicalBlock(scope: !81, file: !79, line: 19, column: 7)
!194 = !DILocation(line: 19, column: 7, scope: !81)
!195 = !DILocation(line: 20, column: 5, scope: !196)
!196 = !DILexicalBlockFile(scope: !197, file: !79, discriminator: 1)
!197 = distinct !DILexicalBlock(scope: !193, file: !79, line: 19, column: 16)
!198 = !DILocation(line: 20, column: 17, scope: !196)
!199 = !DILocation(line: 20, column: 28, scope: !200)
!200 = !DILexicalBlockFile(scope: !197, file: !79, discriminator: 2)
!201 = !DILocation(line: 20, column: 26, scope: !200)
!202 = distinct !{!202, !180}
!203 = distinct !{!203, !182, !183}
!204 = !DILocation(line: 20, column: 30, scope: !200)
!205 = !DILocation(line: 20, column: 23, scope: !200)
!206 = distinct !{!206, !180}
!207 = distinct !{!207, !182, !183}
!208 = !DILocation(line: 22, column: 13, scope: !209)
!209 = distinct !DILexicalBlock(scope: !193, file: !79, line: 21, column: 10)
!210 = !DILocation(line: 24, column: 5, scope: !211)
!211 = !DILexicalBlockFile(scope: !209, file: !79, discriminator: 1)
!212 = !DILocation(line: 23, column: 6, scope: !209)
!213 = !DILocation(line: 22, column: 6, scope: !209)
!214 = !DILocation(line: 24, column: 17, scope: !211)
!215 = !DILocation(line: 24, column: 28, scope: !216)
!216 = !DILexicalBlockFile(scope: !209, file: !79, discriminator: 2)
!217 = !DILocation(line: 24, column: 26, scope: !216)
!218 = distinct !{!218, !182, !183}
!219 = !DILocation(line: 24, column: 30, scope: !216)
!220 = !DILocation(line: 24, column: 23, scope: !216)
!221 = distinct !{!221, !180}
!222 = distinct !{!222, !182, !183}
!223 = !DILocation(line: 28, column: 1, scope: !81)
!224 = !DILocation(line: 15, column: 16, scope: !225)
!225 = !DILexicalBlockFile(scope: !91, file: !89, discriminator: 1)
!226 = !DILocation(line: 15, column: 3, scope: !225)
!227 = !DILocation(line: 15, column: 13, scope: !225)
!228 = !DILocation(line: 16, column: 15, scope: !91)
!229 = !DILocation(line: 16, column: 13, scope: !91)
!230 = distinct !{!230, !180}
!231 = distinct !{!231, !182, !183}
!232 = !DILocation(line: 16, column: 19, scope: !91)
!233 = !DILocation(line: 16, column: 10, scope: !91)
!234 = distinct !{!234, !180}
!235 = distinct !{!235, !182, !183}
!236 = !DILocation(line: 17, column: 3, scope: !91)
!237 = !DILocation(line: 13, column: 20, scope: !238)
!238 = !DILexicalBlockFile(scope: !101, file: !99, discriminator: 1)
!239 = !DILocation(line: 13, column: 5, scope: !238)
!240 = !DILocation(line: 14, column: 14, scope: !101)
!241 = !DILocation(line: 13, column: 17, scope: !238)
!242 = !DILocation(line: 14, column: 9, scope: !101)
!243 = !DILocation(line: 14, column: 12, scope: !101)
!244 = distinct !{!244, !180}
!245 = !DILocation(line: 15, column: 5, scope: !101)
