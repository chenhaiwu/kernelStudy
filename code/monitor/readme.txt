
1：增加一个内核模块，使用uptime中的一个全局变量，取出全局变量表示1分钟的负载。
2：负载大于3，则打印所有进程的所有的task信息。
3：创建一个hrtimer的定时器，使用到时的方式，每隔10ms检查一下负载信息。

编译时：
由于新旧内核的关系，
旧内核：具有结构体：struct stack_trace trace，把 bttrace的数组放在此结构体中。
新内核：已经不存在这个结构体了，而是通过另外的接口完成，直接使用bttrace的函数接口。

但是新内核调用的接口：save_stack_trace_tsk，发现这个接口头文件中有，但是连接时出错，因为没有export_symbol出来。

所以需要 重新编译内核。
make -j4
make install

不需要 编译make modules，因为这个是入内核bin的。

运行时：
需要使用fio工具：来提高CPU的利用率，可以通过下面命令达到。
dd if=/dev/zero of=/mnt/hgfs/vm/code/kernelStudy/code/monitor/test.big bs=1M count=1000

测试随机读：
fio --filename=/mnt/hgfs/vm/code/kernelStudy/code/monitor/test.big -iodepth=64 -ioengine=libaio -direct=1 -rw=randread -bs=4k -size=1G -numjobs=64 -runtime=20 -group_reporting -name=test-rand-read



输出LOG如下：
[ 2396.568182] !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
[ 2396.568185] 	Load: 1.14, 0.37, 0.21
[ 2396.568185] dump all task: balabala
[ 2396.568185] dump running task.
[ 2396.568226] running task, comm: vmhgfs-fuse, pid 8056
[ 2396.568240]  print_all_task_stack+0x11c/0x1e5 [monitor]
[ 2396.568242]  monitor_handler+0x88/0x8a [monitor]
[ 2396.568247]  __hrtimer_run_queues+0xf7/0x270
[ 2396.568249]  hrtimer_interrupt+0x109/0x220
[ 2396.568253]  smp_apic_timer_interrupt+0x71/0x140
[ 2396.568254]  apic_timer_interrupt+0xf/0x20
[ 2396.568270] dump uninterrupted task.
[ 2396.568271] uninterrupted task, comm: rcu_gp, pid 3
[ 2396.568275]  rescuer_thread+0x2f4/0x370
[ 2396.568276]  kthread+0x104/0x140
[ 2396.568278]  ret_from_fork+0x35/0x40
[ 2396.568279] uninterrupted task, comm: rcu_par_gp, pid 4
[ 2396.568281]  rescuer_thread+0x2f4/0x370
[ 2396.568281]  kthread+0x104/0x140
[ 2396.568282]  ret_from_fork+0x35/0x40
[ 2396.568283] uninterrupted task, comm: kworker/0:0H, pid 6
[ 2396.568285]  worker_thread+0xcf/0x400
[ 2396.568286]  kthread+0x104/0x140
[ 2396.568287]  ret_from_fork+0x35/0x40
[ 2396.568287] uninterrupted task, comm: mm_percpu_wq, pid 9
[ 2396.568289]  rescuer_thread+0x2f4/0x370
[ 2396.568290]  kthread+0x104/0x140
[ 2396.568291]  ret_from_fork+0x35/0x40
[ 2396.568292] uninterrupted task, comm: rcu_sched, pid 11
[ 2396.568295]  rcu_gp_kthread+0x48d/0x990
[ 2396.568296]  kthread+0x104/0x140
[ 2396.568297]  ret_from_fork+0x35/0x40
[ 2396.568298] uninterrupted task, comm: kworker/1:0H, pid 20
[ 2396.568300]  worker_thread+0xcf/0x400
[ 2396.568301]  kthread+0x104/0x140
[ 2396.568302]  ret_from_fork+0x35/0x40
[ 2396.568302] uninterrupted task, comm: netns, pid 22
[ 2396.568304]  rescuer_thread+0x2f4/0x370
[ 2396.568305]  kthread+0x104/0x140
[ 2396.568306]  ret_from_fork+0x35/0x40
[ 2396.568307] uninterrupted task, comm: kworker/1:2, pid 27
[ 2396.568309]  worker_thread+0xcf/0x400
[ 2396.568310]  kthread+0x104/0x140
[ 2396.568311]  ret_from_fork+0x35/0x40
[ 2396.568311] uninterrupted task, comm: writeback, pid 30
[ 2396.568313]  rescuer_thread+0x2f4/0x370
[ 2396.568314]  kthread+0x104/0x140
[ 2396.568315]  ret_from_fork+0x35/0x40
[ 2396.568316] uninterrupted task, comm: kintegrityd, pid 125
[ 2396.568318]  rescuer_thread+0x2f4/0x370
[ 2396.568319]  kthread+0x104/0x140
[ 2396.568320]  ret_from_fork+0x35/0x40
[ 2396.568320] uninterrupted task, comm: kblockd, pid 126
[ 2396.568322]  rescuer_thread+0x2f4/0x370
[ 2396.568323]  kthread+0x104/0x140
[ 2396.568324]  ret_from_fork+0x35/0x40
[ 2396.568367] uninterrupted task, comm: blkcg_punt_bio, pid 127
[ 2396.568372]  rescuer_thread+0x2f4/0x370
[ 2396.568372]  kthread+0x104/0x140
[ 2396.568373]  ret_from_fork+0x35/0x40
[ 2396.568374] uninterrupted task, comm: tpm_dev_wq, pid 128
[ 2396.568376]  rescuer_thread+0x2f4/0x370
[ 2396.568377]  kthread+0x104/0x140
[ 2396.568378]  ret_from_fork+0x35/0x40
[ 2396.568378] uninterrupted task, comm: ata_sff, pid 129
[ 2396.568380]  rescuer_thread+0x2f4/0x370
[ 2396.568381]  kthread+0x104/0x140
[ 2396.568382]  ret_from_fork+0x35/0x40
[ 2396.568383] uninterrupted task, comm: md, pid 130
[ 2396.568385]  rescuer_thread+0x2f4/0x370
[ 2396.568386]  kthread+0x104/0x140
[ 2396.568386]  ret_from_fork+0x35/0x40
[ 2396.568387] uninterrupted task, comm: edac-poller, pid 131
[ 2396.568389]  rescuer_thread+0x2f4/0x370
[ 2396.568390]  kthread+0x104/0x140
[ 2396.568391]  ret_from_fork+0x35/0x40
[ 2396.568391] uninterrupted task, comm: devfreq_wq, pid 132
[ 2396.568393]  rescuer_thread+0x2f4/0x370
[ 2396.568394]  kthread+0x104/0x140
[ 2396.568395]  ret_from_fork+0x35/0x40
[ 2396.568395] uninterrupted task, comm: kthrotld, pid 140
[ 2396.568398]  rescuer_thread+0x2f4/0x370
[ 2396.568398]  kthread+0x104/0x140
[ 2396.568399]  ret_from_fork+0x35/0x40
[ 2396.568401] uninterrupted task, comm: acpi_thermal_pm, pid 173
[ 2396.568404]  rescuer_thread+0x2f4/0x370
[ 2396.568404]  kthread+0x104/0x140
[ 2396.568405]  ret_from_fork+0x35/0x40
[ 2396.568406] uninterrupted task, comm: scsi_tmf_0, pid 175
[ 2396.568408]  rescuer_thread+0x2f4/0x370
[ 2396.568409]  kthread+0x104/0x140
[ 2396.568410]  ret_from_fork+0x35/0x40
[ 2396.568410] uninterrupted task, comm: scsi_tmf_1, pid 177
[ 2396.568412]  rescuer_thread+0x2f4/0x370
[ 2396.568413]  kthread+0x104/0x140
[ 2396.568414]  ret_from_fork+0x35/0x40
[ 2396.568415] uninterrupted task, comm: vfio-irqfd-clea, pid 179
[ 2396.568416]  rescuer_thread+0x2f4/0x370
[ 2396.568417]  kthread+0x104/0x140
[ 2396.568418]  ret_from_fork+0x35/0x40
[ 2396.568419] uninterrupted task, comm: ipv6_addrconf, pid 180
[ 2396.568421]  rescuer_thread+0x2f4/0x370
[ 2396.568422]  kthread+0x104/0x140
[ 2396.568423]  ret_from_fork+0x35/0x40
[ 2396.568424] uninterrupted task, comm: kstrp, pid 192
[ 2396.568426]  rescuer_thread+0x2f4/0x370
[ 2396.568426]  kthread+0x104/0x140
[ 2396.568427]  ret_from_fork+0x35/0x40
[ 2396.568428] uninterrupted task, comm: zswap-shrink, pid 196
[ 2396.568430]  rescuer_thread+0x2f4/0x370
[ 2396.568431]  kthread+0x104/0x140
[ 2396.568432]  ret_from_fork+0x35/0x40
[ 2396.568432] uninterrupted task, comm: kworker/u257:0, pid 197
[ 2396.568434]  worker_thread+0xcf/0x400
[ 2396.568435]  kthread+0x104/0x140
[ 2396.568436]  ret_from_fork+0x35/0x40
[ 2396.568436] uninterrupted task, comm: charger_manager, pid 213
[ 2396.568439]  rescuer_thread+0x2f4/0x370
[ 2396.568439]  kthread+0x104/0x140
[ 2396.568440]  ret_from_fork+0x35/0x40
[ 2396.568441] uninterrupted task, comm: mpt_poll_0, pid 258
[ 2396.568443]  rescuer_thread+0x2f4/0x370
[ 2396.568444]  kthread+0x104/0x140
[ 2396.568445]  ret_from_fork+0x35/0x40
[ 2396.568445] uninterrupted task, comm: mpt/0, pid 259
[ 2396.568447]  rescuer_thread+0x2f4/0x370
[ 2396.568448]  kthread+0x104/0x140
[ 2396.568449]  ret_from_fork+0x35/0x40
[ 2396.568450] uninterrupted task, comm: scsi_tmf_2, pid 261
[ 2396.568452]  rescuer_thread+0x2f4/0x370
[ 2396.568452]  kthread+0x104/0x140
[ 2396.568453]  ret_from_fork+0x35/0x40
[ 2396.568454] uninterrupted task, comm: scsi_tmf_3, pid 263
[ 2396.568456]  rescuer_thread+0x2f4/0x370
[ 2396.568456]  kthread+0x104/0x140
[ 2396.568457]  ret_from_fork+0x35/0x40
[ 2396.568458] uninterrupted task, comm: scsi_tmf_4, pid 265
[ 2396.568460]  rescuer_thread+0x2f4/0x370
[ 2396.568461]  kthread+0x104/0x140
[ 2396.568462]  ret_from_fork+0x35/0x40
[ 2396.568462] uninterrupted task, comm: scsi_tmf_5, pid 267
[ 2396.568464]  rescuer_thread+0x2f4/0x370
[ 2396.568465]  kthread+0x104/0x140
[ 2396.568466]  ret_from_fork+0x35/0x40
[ 2396.568467] uninterrupted task, comm: scsi_tmf_6, pid 269
[ 2396.568469]  rescuer_thread+0x2f4/0x370
[ 2396.568470]  kthread+0x104/0x140
[ 2396.568470]  ret_from_fork+0x35/0x40
[ 2396.568471] uninterrupted task, comm: scsi_tmf_7, pid 271
[ 2396.568474]  rescuer_thread+0x2f4/0x370
[ 2396.568474]  kthread+0x104/0x140
[ 2396.568475]  ret_from_fork+0x35/0x40
[ 2396.568476] uninterrupted task, comm: scsi_tmf_8, pid 273
[ 2396.568478]  rescuer_thread+0x2f4/0x370
[ 2396.568479]  kthread+0x104/0x140
[ 2396.568480]  ret_from_fork+0x35/0x40
[ 2396.568480] uninterrupted task, comm: scsi_tmf_9, pid 275
[ 2396.568482]  rescuer_thread+0x2f4/0x370
[ 2396.568483]  kthread+0x104/0x140
[ 2396.568484]  ret_from_fork+0x35/0x40
[ 2396.568484] uninterrupted task, comm: scsi_tmf_10, pid 277
[ 2396.568486]  rescuer_thread+0x2f4/0x370
[ 2396.568487]  kthread+0x104/0x140
[ 2396.568488]  ret_from_fork+0x35/0x40
[ 2396.568489] uninterrupted task, comm: scsi_tmf_11, pid 279
[ 2396.568491]  rescuer_thread+0x2f4/0x370
[ 2396.568491]  kthread+0x104/0x140
[ 2396.568492]  ret_from_fork+0x35/0x40
[ 2396.568493] uninterrupted task, comm: scsi_tmf_12, pid 281
[ 2396.568495]  rescuer_thread+0x2f4/0x370
[ 2396.568496]  kthread+0x104/0x140
[ 2396.568496]  ret_from_fork+0x35/0x40
[ 2396.568497] uninterrupted task, comm: scsi_tmf_13, pid 283
[ 2396.568499]  rescuer_thread+0x2f4/0x370
[ 2396.568500]  kthread+0x104/0x140
[ 2396.568501]  ret_from_fork+0x35/0x40
[ 2396.568502] uninterrupted task, comm: scsi_tmf_14, pid 285
[ 2396.568504]  rescuer_thread+0x2f4/0x370
[ 2396.568505]  kthread+0x104/0x140
[ 2396.568506]  ret_from_fork+0x35/0x40
[ 2396.568506] uninterrupted task, comm: scsi_tmf_15, pid 287
[ 2396.568508]  rescuer_thread+0x2f4/0x370
[ 2396.568509]  kthread+0x104/0x140
[ 2396.568510]  ret_from_fork+0x35/0x40
[ 2396.568511] uninterrupted task, comm: scsi_tmf_16, pid 289
[ 2396.568513]  rescuer_thread+0x2f4/0x370
[ 2396.568513]  kthread+0x104/0x140
[ 2396.568514]  ret_from_fork+0x35/0x40
[ 2396.568515] uninterrupted task, comm: scsi_tmf_17, pid 291
[ 2396.568517]  rescuer_thread+0x2f4/0x370
[ 2396.568518]  kthread+0x104/0x140
[ 2396.568518]  ret_from_fork+0x35/0x40
[ 2396.568519] uninterrupted task, comm: scsi_tmf_18, pid 293
[ 2396.568521]  rescuer_thread+0x2f4/0x370
[ 2396.568522]  kthread+0x104/0x140
[ 2396.568523]  ret_from_fork+0x35/0x40
[ 2396.568523] uninterrupted task, comm: scsi_tmf_19, pid 295
[ 2396.568525]  rescuer_thread+0x2f4/0x370
[ 2396.568526]  kthread+0x104/0x140
[ 2396.568527]  ret_from_fork+0x35/0x40
[ 2396.568528] uninterrupted task, comm: scsi_tmf_20, pid 297
[ 2396.568529]  rescuer_thread+0x2f4/0x370
[ 2396.568530]  kthread+0x104/0x140
[ 2396.568531]  ret_from_fork+0x35/0x40
[ 2396.568532] uninterrupted task, comm: scsi_tmf_21, pid 299
[ 2396.568534]  rescuer_thread+0x2f4/0x370
[ 2396.568535]  kthread+0x104/0x140
[ 2396.568535]  ret_from_fork+0x35/0x40
[ 2396.568536] uninterrupted task, comm: scsi_tmf_22, pid 301
[ 2396.568539]  rescuer_thread+0x2f4/0x370
[ 2396.568539]  kthread+0x104/0x140
[ 2396.568540]  ret_from_fork+0x35/0x40
[ 2396.568541] uninterrupted task, comm: scsi_tmf_23, pid 303
[ 2396.568543]  rescuer_thread+0x2f4/0x370
[ 2396.568544]  kthread+0x104/0x140
[ 2396.568544]  ret_from_fork+0x35/0x40
[ 2396.568545] uninterrupted task, comm: scsi_tmf_24, pid 305
[ 2396.568547]  rescuer_thread+0x2f4/0x370
[ 2396.568548]  kthread+0x104/0x140
[ 2396.568549]  ret_from_fork+0x35/0x40
[ 2396.568549] uninterrupted task, comm: scsi_tmf_25, pid 307
[ 2396.568551]  rescuer_thread+0x2f4/0x370
[ 2396.568552]  kthread+0x104/0x140
[ 2396.568553]  ret_from_fork+0x35/0x40
[ 2396.568554] uninterrupted task, comm: scsi_tmf_26, pid 309
[ 2396.568556]  rescuer_thread+0x2f4/0x370
[ 2396.568556]  kthread+0x104/0x140
[ 2396.568557]  ret_from_fork+0x35/0x40
[ 2396.568558] uninterrupted task, comm: scsi_tmf_27, pid 311
[ 2396.568560]  rescuer_thread+0x2f4/0x370
[ 2396.568560]  kthread+0x104/0x140
[ 2396.568561]  ret_from_fork+0x35/0x40
[ 2396.568562] uninterrupted task, comm: scsi_tmf_28, pid 313
[ 2396.568564]  rescuer_thread+0x2f4/0x370
[ 2396.568565]  kthread+0x104/0x140
[ 2396.568586]  ret_from_fork+0x35/0x40
[ 2396.568588] uninterrupted task, comm: scsi_tmf_29, pid 315
[ 2396.568591]  rescuer_thread+0x2f4/0x370
[ 2396.568592]  kthread+0x104/0x140
[ 2396.568593]  ret_from_fork+0x35/0x40
[ 2396.568594] uninterrupted task, comm: scsi_tmf_30, pid 317
[ 2396.568596]  rescuer_thread+0x2f4/0x370
[ 2396.568597]  kthread+0x104/0x140
[ 2396.568598]  ret_from_fork+0x35/0x40
[ 2396.568598] uninterrupted task, comm: scsi_tmf_31, pid 319
[ 2396.568600]  rescuer_thread+0x2f4/0x370
[ 2396.568601]  kthread+0x104/0x140
[ 2396.568602]  ret_from_fork+0x35/0x40
[ 2396.568603] uninterrupted task, comm: cryptd, pid 347
[ 2396.568605]  rescuer_thread+0x2f4/0x370
[ 2396.568606]  kthread+0x104/0x140
[ 2396.568606]  ret_from_fork+0x35/0x40
[ 2396.568607] uninterrupted task, comm: ttm_swap, pid 362
[ 2396.568609]  rescuer_thread+0x2f4/0x370
[ 2396.568610]  kthread+0x104/0x140
[ 2396.568611]  ret_from_fork+0x35/0x40
[ 2396.568611] uninterrupted task, comm: scsi_tmf_32, pid 375
[ 2396.568613]  rescuer_thread+0x2f4/0x370
[ 2396.568614]  kthread+0x104/0x140
[ 2396.568615]  ret_from_fork+0x35/0x40
[ 2396.568616] uninterrupted task, comm: kworker/1:1H, pid 387
[ 2396.568618]  worker_thread+0xcf/0x400
[ 2396.568619]  kthread+0x104/0x140
[ 2396.568620]  ret_from_fork+0x35/0x40
[ 2396.568620] uninterrupted task, comm: kworker/0:1H, pid 408
[ 2396.568622]  worker_thread+0xcf/0x400
[ 2396.568623]  kthread+0x104/0x140
[ 2396.568624]  ret_from_fork+0x35/0x40
[ 2396.568625] uninterrupted task, comm: raid5wq, pid 430
[ 2396.568626]  rescuer_thread+0x2f4/0x370
[ 2396.568627]  kthread+0x104/0x140
[ 2396.568628]  ret_from_fork+0x35/0x40
[ 2396.568629] uninterrupted task, comm: ext4-rsv-conver, pid 479
[ 2396.568631]  rescuer_thread+0x2f4/0x370
[ 2396.568631]  kthread+0x104/0x140
[ 2396.568632]  ret_from_fork+0x35/0x40
[ 2396.568633] uninterrupted task, comm: kworker/u257:2, pid 681
[ 2396.568635]  worker_thread+0xcf/0x400
[ 2396.568636]  kthread+0x104/0x140
[ 2396.568637]  ret_from_fork+0x35/0x40
[ 2396.568638] uninterrupted task, comm: kworker/0:3, pid 683
[ 2396.568640]  worker_thread+0xcf/0x400
[ 2396.568640]  kthread+0x104/0x140
[ 2396.568641]  ret_from_fork+0x35/0x40
[ 2396.568642] uninterrupted task, comm: kaluad, pid 724
[ 2396.568644]  rescuer_thread+0x2f4/0x370
[ 2396.568645]  kthread+0x104/0x140
[ 2396.568646]  ret_from_fork+0x35/0x40
[ 2396.568647] uninterrupted task, comm: kmpath_rdacd, pid 725
[ 2396.568649]  rescuer_thread+0x2f4/0x370
[ 2396.568649]  kthread+0x104/0x140
[ 2396.568650]  ret_from_fork+0x35/0x40
[ 2396.568651] uninterrupted task, comm: kmpathd, pid 726
[ 2396.568653]  rescuer_thread+0x2f4/0x370
[ 2396.568654]  kthread+0x104/0x140
[ 2396.568655]  ret_from_fork+0x35/0x40
[ 2396.568655] uninterrupted task, comm: kmpath_handlerd, pid 727
[ 2396.568657]  rescuer_thread+0x2f4/0x370
[ 2396.568658]  kthread+0x104/0x140
[ 2396.568659]  ret_from_fork+0x35/0x40
[ 2396.568667] uninterrupted task, comm: kworker/0:1, pid 3035
[ 2396.568670]  worker_thread+0xcf/0x400
[ 2396.568670]  kthread+0x104/0x140
[ 2396.568671]  ret_from_fork+0x35/0x40
[ 2396.568672] uninterrupted task, comm: kworker/u256:0, pid 3160
[ 2396.568674]  worker_thread+0xcf/0x400
[ 2396.568675]  kthread+0x104/0x140
[ 2396.568676]  ret_from_fork+0x35/0x40
[ 2396.568676] uninterrupted task, comm: kworker/u256:1, pid 4531
[ 2396.568678]  worker_thread+0xcf/0x400
[ 2396.568679]  kthread+0x104/0x140
[ 2396.568680]  ret_from_fork+0x35/0x40
[ 2396.568681] uninterrupted task, comm: kworker/u256:3, pid 6425
[ 2396.568684]  worker_thread+0xcf/0x400
[ 2396.568684]  kthread+0x104/0x140
[ 2396.568685]  ret_from_fork+0x35/0x40
[ 2396.568686] uninterrupted task, comm: kworker/1:0, pid 6497
[ 2396.568688]  worker_thread+0xcf/0x400
[ 2396.568689]  kthread+0x104/0x140
[ 2396.568689]  ret_from_fork+0x35/0x40
[ 2396.568690] uninterrupted task, comm: kworker/1:1, pid 7889
[ 2396.568692]  worker_thread+0xcf/0x400
[ 2396.568693]  kthread+0x104/0x140
[ 2396.568694]  ret_from_fork+0x35/0x40