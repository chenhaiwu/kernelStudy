[  875.351692] cr0=0x80050033, cr3 = 0x11fb20000
[  875.351692] pgdir_SHIFT = 39
[  875.351693] P4D_SHIFT = 39
[  875.351693] PUD_SHIFT = 30
[  875.351694] PMD_SHIFT = 21
[  875.351694] PAGE_SHIFT = 12
[  875.351695] PTRS_PER_PGD = 512
[  875.351695] PTRS_PER_P4D = 1
[  875.351696] PTRS_PER_PUD = 512
[  875.351696] PTRS_PER_PMD = 512
[  875.351696] PTRS_PER_PTE = 512
[  875.351697] PAGE_MASK = 0xfffffffffffff000
[  875.351697] vaddr to phy addr entry!
[  875.351700] __get_free_page, alloc the free page vaddr=0xffff93275f9ce000
[  875.351700] pgd_val=0x6602067, pdg_index=0x126
[  875.351701] p4d_val=0x6602067, p4d_index=0x0
[  875.351702] pud_val=0x6606067, pud_index=0x9d
[  875.351702] pmd_val=0x11fbb2063, pmd_index=0xfc
[  875.351703] pte_val=0x800000011f9ce063, pte_index=0x1ce
[  875.351703] page_offset=0x0, page_addr=0x800000011f9ce000
[  875.351704] vaddr=0xffff93275f9ce000, paddr=0x800000011f9ce000

这是打印的字，这里使用一个工具：fileview，可以来查看物理地址对应的内容。

后面12位存放的是内存的属性，页内偏移，这个本来就是不要的。

cr3：指向段基址
例子：
后面12位存放的是内存的属性，页内偏移，这个本来就是不要的。
每个页目录项都是8bit来表示。因为值是512，所以用8位表示。
要求pgd val则：使用cr3 + pdg_index * 8B

pgd的物理地址: 
cr3 + pgd_offset * 8 = 0x11fb20000 + 0x126 * 8 = 11FB20930
不过，这个会触发segement fault。

pud的物理地址：
pgd_val + pud_offset * 8 = 0x6602000 + 0x9d * 8 = 660 24E8  ，确认存的值为 0x6606067

pmd的物理地址：
pud_val + pmd_offset * 8 = 0x6606000 + 0xfc * 8 = 66067E0  ，确认存的值为 0x11fbb2063

pte_val
pmd_val + pte_offset * 8 = 0x11fbb2000 + 0x1ce * 8 = 11FBB2E70

如果还有page_offset的话，直接在此进行偏移，后12位直接加上。

最后一级：
0x800000011f9ce000
首位1表示是个物理地址。
真正地址为：0x11f9ce000 可以看到物理地址存放的内容为：hello phyical memory

所对应的工具操作等，内容放在印象笔记上面。


cr3一作读的动作，就触发segement fault错误：不让访问：
下面比较清楚的说明了slub对象的数据有爆露的风险，所以就触发segment fault.

[ 4810.053859] usercopy: Kernel memory exposure attempt detected from SLUB object 'kernfs_node_cache' (offset 48, size 256)!
[ 4810.056514] ------------[ cut here ]------------
[ 4810.057391] kernel BUG at mm/usercopy.c:99!
[ 4810.058416] invalid opcode: 0000 [#6] SMP PTI
[ 4810.059264] CPU: 1 PID: 3096 Comm: fileview Tainted: G      D    OE     5.6.12 #1
[ 4810.060106] Hardware name: VMware, Inc. VMware Virtual Platform/440BX Desktop Reference Platform, BIOS 6.00 07/29/2019
[ 4810.061834] RIP: 0010:usercopy_abort+0x7b/0x7d
[ 4810.062632] Code: 4c 0f 45 de 51 4c 89 d1 48 c7 c2 95 85 7b 82 57 48 c7 c6 a8 40 7a 82 48 c7 c7 60 86 7b 82 48 0f 45 f2 4c 89 da e8 78 2a e3 ff <0f> 0b 4c 89 e1 49 89 d8 44 89 ea 31 f6 48 29 c1 48 c7 c7 d7 85 7b
[ 4810.065264] RSP: 0018:ffffab3b4218bdf8 EFLAGS: 00010246
[ 4810.066169] RAX: 000000000000006d RBX: 0000000000000100 RCX: 0000000000000000
[ 4810.067044] RDX: 0000000000000000 RSI: ffff932779e59c88 RDI: ffff932779e59c88
[ 4810.067872] RBP: ffffab3b4218be10 R08: 00000000000007b3 R09: 0000000000010101
[ 4810.068677] R10: ffff9326fd909b80 R11: 0000000000000001 R12: ffff93275fb20930
[ 4810.069532] R13: 0000000000000001 R14: ffff93275fb20a30 R15: 0000000000000930
[ 4810.070358] FS:  00007f91898df540(0000) GS:ffff932779e40000(0000) knlGS:0000000000000000
[ 4810.071217] CS:  0010 DS: 0000 ES: 0000 CR0: 0000000080050033
[ 4810.071998] CR2: 00007fa6af9b9110 CR3: 000000012322a004 CR4: 00000000001606e0
[ 4810.072821] Call Trace:
[ 4810.073549]  __check_heap_object+0xe6/0x120
[ 4810.074380]  __check_object_size+0x13f/0x150
[ 4810.075459]  my_read+0x92/0xd1 [dram]
[ 4810.076793]  __vfs_read+0x1b/0x40
[ 4810.077478]  vfs_read+0xab/0x160
[ 4810.078225]  ksys_read+0x67/0xe0
[ 4810.078942]  __x64_sys_read+0x1a/0x20
[ 4810.079633]  do_syscall_64+0x57/0x1b0
[ 4810.080812]  entry_SYSCALL_64_after_hwframe+0x44/0xa9
[ 4810.081500] RIP: 0033:0x7f91897fcfb2
[ 4810.082198] Code: c0 e9 c2 fe ff ff 50 48 8d 3d ca cb 0a 00 e8 f5 19 02 00 0f 1f 44 00 00 f3 0f 1e fa 64 8b 04 25 18 00 00 00 85 c0 75 10 0f 05 <48> 3d 00 f0 ff ff 77 56 c3 0f 1f 44 00 00 48 83 ec 28 48 89 54 24
[ 4810.084627] RSP: 002b:00007ffd9e739aa8 EFLAGS: 00000246 ORIG_RAX: 0000000000000000
[ 4810.085885] RAX: ffffffffffffffda RBX: 0000556382ef30e0 RCX: 00007f91897fcfb2
[ 4810.086547] RDX: 0000000000000100 RSI: 0000556382ef60c0 RDI: 0000000000000003
[ 4810.087615] RBP: 00007ffd9e739c60 R08: 0000000000000000 R09: 0000000000000000
[ 4810.088856] R10: 00007f918988aac0 R11: 0000000000000246 R12: 0000556382ef2220
[ 4810.089552] R13: 00007ffd9e739d50 R14: 0000000000000000 R15: 0000000000000000
[ 4810.090680] Modules linked in: v2p_addr(OE) dram(OE) vsock_loopback vmw_vsock_virtio_transport_common vmw_vsock_vmci_transport vsock dm_multipath scsi_dh_rdac scsi_dh_emc scsi_dh_alua snd_ens1371 snd_ac97_codec gameport snd_rawmidi btusb btrtl snd_seq_device ac97_bus btbcm btintel snd_pcm bluetooth intel_rapl_msr snd_timer intel_rapl_common snd ecdh_generic soundcore joydev ecc intel_rapl_perf input_leds vmw_balloon serio_raw vmw_vmci mac_hid sch_fq_codel ip_tables x_tables autofs4 btrfs blake2b_generic zstd_compress raid10 raid456 async_raid6_recov async_memcpy async_pq async_xor async_tx xor raid6_pq libcrc32c raid1 raid0 multipath linear crct10dif_pclmul crc32_pclmul ghash_clmulni_intel vmwgfx hid_generic usbhid hid ttm aesni_intel crypto_simd psmouse cryptd glue_helper drm_kms_helper e1000 syscopyarea sysfillrect sysimgblt fb_sys_fops cec drm ahci mptspi libahci mptscsih mptbase scsi_transport_spi pata_acpi i2c_piix4
[ 4810.098175] ---[ end trace 00ca4ec4697a8342 ]---
[ 4810.098948] RIP: 0010:usercopy_abort+0x7b/0x7d
[ 4810.099646] Code: 4c 0f 45 de 51 4c 89 d1 48 c7 c2 95 85 7b 82 57 48 c7 c6 a8 40 7a 82 48 c7 c7 60 86 7b 82 48 0f 45 f2 4c 89 da e8 78 2a e3 ff <0f> 0b 4c 89 e1 49 89 d8 44 89 ea 31 f6 48 29 c1 48 c7 c7 d7 85 7b
[ 4810.102076] RSP: 0018:ffffab3b412ffdf8 EFLAGS: 00010246
[ 4810.102857] RAX: 000000000000006d RBX: 0000000000000100 RCX: 0000000000000000
[ 4810.103577] RDX: 0000000000000000 RSI: ffff932779e59c88 RDI: ffff932779e59c88
[ 4810.104347] RBP: ffffab3b412ffe10 R08: 00000000000006c3 R09: 0000000000010101
[ 4810.105141] R10: ffff9326fd909c70 R11: 0000000000000001 R12: ffff93275fb20930
[ 4810.106125] R13: 0000000000000001 R14: ffff93275fb20a30 R15: 0000000000000930
[ 4810.107223] FS:  00007f91898df540(0000) GS:ffff932779e40000(0000) knlGS:0000000000000000
[ 4810.108021] CS:  0010 DS: 0000 ES: 0000 CR0: 0000000080050033
[ 4810.108860] CR2: 00007fa6af9b9110 CR3: 000000012322a004 CR4: 00000000001606e0