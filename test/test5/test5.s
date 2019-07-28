	.text
	.file	"test5.bc"
	.globl	loop
	.align	16, 0x90
	.type	loop,@function
loop:                                   # @loop
.Lfunc_begin0:
	.file	1 "test5.c"
	.loc	1 24 0                  # test5.c:24:0
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp0:
	.cfi_def_cfa_offset 16
.Ltmp1:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp2:
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	.loc	1 25 18 prologue_end    # test5.c:25:18
.Ltmp3:
	movl	-8(%rbp), %esi
	.loc	1 25 9 is_stmt 0        # test5.c:25:9
	movl	%esi, -12(%rbp)
	.loc	1 26 13 is_stmt 1       # test5.c:26:13
.Ltmp4:
	movl	$0, -16(%rbp)
.LBB0_1:                                # =>This Inner Loop Header: Depth=1
	.loc	1 26 18 is_stmt 0 discriminator 1 # test5.c:26:18
.Ltmp5:
	movl	-16(%rbp), %eax
	.loc	1 26 19 discriminator 1 # test5.c:26:19
	cmpl	-4(%rbp), %eax
	.loc	1 26 5 discriminator 1  # test5.c:26:5
	jge	.LBB0_4
# BB#2:                                 #   in Loop: Header=BB0_1 Depth=1
	.loc	1 27 17 is_stmt 1       # test5.c:27:17
	movl	-12(%rbp), %eax
	.loc	1 27 24 is_stmt 0       # test5.c:27:24
	imull	-8(%rbp), %eax
	.loc	1 27 15                 # test5.c:27:15
	movl	%eax, -12(%rbp)
# BB#3:                                 #   in Loop: Header=BB0_1 Depth=1
	.loc	1 26 25 is_stmt 1 discriminator 2 # test5.c:26:25
	movl	-16(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -16(%rbp)
	.loc	1 26 5 is_stmt 0 discriminator 2 # test5.c:26:5
	jmp	.LBB0_1
.Ltmp6:
.LBB0_4:
	.loc	1 28 13 is_stmt 1       # test5.c:28:13
	movl	-12(%rbp), %eax
	.loc	1 28 6 is_stmt 0        # test5.c:28:6
	popq	%rbp
	retq
.Ltmp7:
.Lfunc_end0:
	.size	loop, .Lfunc_end0-loop
	.cfi_endproc

	.globl	foo
	.align	16, 0x90
	.type	foo,@function
foo:                                    # @foo
.Lfunc_begin1:
	.loc	1 31 0 is_stmt 1        # test5.c:31:0
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp8:
	.cfi_def_cfa_offset 16
.Ltmp9:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp10:
	.cfi_def_cfa_register %rbp
	subq	$64, %rsp
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	.loc	1 32 11 prologue_end    # test5.c:32:11
.Ltmp11:
	movq	-24(%rbp), %rsi
	.loc	1 32 23 is_stmt 0       # test5.c:32:23
	cmpl	$0, (%rsi)
.Ltmp12:
	.loc	1 32 9                  # test5.c:32:9
	jle	.LBB1_6
# BB#1:
	.loc	1 33 15 is_stmt 1       # test5.c:33:15
.Ltmp13:
	movq	-16(%rbp), %rax
	.loc	1 33 26 is_stmt 0       # test5.c:33:26
	cmpl	$1, (%rax)
.Ltmp14:
	.loc	1 33 13                 # test5.c:33:13
	jge	.LBB1_3
# BB#2:
	movl	$3, %edi
	movl	$2, %esi
	.loc	1 34 27 is_stmt 1       # test5.c:34:27
.Ltmp15:
	callq	loop
	movabsq	$.L.str, %rdi
	.loc	1 34 12 is_stmt 0 discriminator 1 # test5.c:34:12
	movl	%eax, %esi
	movb	$0, %al
	callq	printf
	.loc	1 35 12 is_stmt 1       # test5.c:35:12
	movl	$1, -4(%rbp)
	movl	%eax, -28(%rbp)         # 4-byte Spill
	jmp	.LBB1_15
.Ltmp16:
.LBB1_3:
	.loc	1 37 20                 # test5.c:37:20
	movq	-16(%rbp), %rax
	.loc	1 37 31 is_stmt 0       # test5.c:37:31
	cmpl	$5, (%rax)
.Ltmp17:
	.loc	1 37 18                 # test5.c:37:18
	jge	.LBB1_5
# BB#4:
	movl	$4, %edi
	movl	$2, %esi
	.loc	1 38 27 is_stmt 1       # test5.c:38:27
.Ltmp18:
	callq	loop
	movabsq	$.L.str, %rdi
	.loc	1 38 12 is_stmt 0 discriminator 1 # test5.c:38:12
	movl	%eax, %esi
	movb	$0, %al
	callq	printf
	.loc	1 39 12 is_stmt 1       # test5.c:39:12
	movl	$1, -4(%rbp)
	movl	%eax, -32(%rbp)         # 4-byte Spill
	jmp	.LBB1_15
.Ltmp19:
.LBB1_5:
	movl	$5, %edi
	movl	$2, %esi
	.loc	1 42 27                 # test5.c:42:27
.Ltmp20:
	callq	loop
	movabsq	$.L.str, %rdi
	.loc	1 42 12 is_stmt 0 discriminator 1 # test5.c:42:12
	movl	%eax, %esi
	movb	$0, %al
	callq	printf
	.loc	1 43 12 is_stmt 1       # test5.c:43:12
	movl	$1, -4(%rbp)
	movl	%eax, -36(%rbp)         # 4-byte Spill
	jmp	.LBB1_15
.Ltmp21:
.LBB1_6:
	.loc	1 47 15                 # test5.c:47:15
	movq	-16(%rbp), %rax
	.loc	1 47 26 is_stmt 0       # test5.c:47:26
	cmpl	$0, (%rax)
.Ltmp22:
	.loc	1 47 13                 # test5.c:47:13
	jge	.LBB1_14
# BB#7:
	.loc	1 48 18 is_stmt 1       # test5.c:48:18
.Ltmp23:
	movq	-24(%rbp), %rax
	.loc	1 48 30 is_stmt 0       # test5.c:48:30
	cmpl	$-10, (%rax)
.Ltmp24:
	.loc	1 48 16                 # test5.c:48:16
	jle	.LBB1_11
# BB#8:
	.loc	1 49 21 is_stmt 1       # test5.c:49:21
.Ltmp25:
	movq	-16(%rbp), %rax
	.loc	1 49 33 is_stmt 0       # test5.c:49:33
	cmpl	$-1, (%rax)
.Ltmp26:
	.loc	1 49 19                 # test5.c:49:19
	jne	.LBB1_10
# BB#9:
	movl	$6, %edi
	movl	$2, %esi
	.loc	1 50 33 is_stmt 1       # test5.c:50:33
.Ltmp27:
	callq	loop
	movabsq	$.L.str, %rdi
	.loc	1 50 18 is_stmt 0 discriminator 1 # test5.c:50:18
	movl	%eax, %esi
	movb	$0, %al
	callq	printf
	.loc	1 51 18 is_stmt 1       # test5.c:51:18
	movl	$1, -4(%rbp)
	movl	%eax, -40(%rbp)         # 4-byte Spill
	jmp	.LBB1_15
.Ltmp28:
.LBB1_10:
	movl	$7, %edi
	movl	$2, %esi
	.loc	1 54 33                 # test5.c:54:33
.Ltmp29:
	callq	loop
	movabsq	$.L.str, %rdi
	.loc	1 54 18 is_stmt 0 discriminator 1 # test5.c:54:18
	movl	%eax, %esi
	movb	$0, %al
	callq	printf
	.loc	1 55 18 is_stmt 1       # test5.c:55:18
	movl	$1, -4(%rbp)
	movl	%eax, -44(%rbp)         # 4-byte Spill
	jmp	.LBB1_15
.Ltmp30:
.LBB1_11:
	.loc	1 59 21                 # test5.c:59:21
	movq	-16(%rbp), %rax
	.loc	1 59 33 is_stmt 0       # test5.c:59:33
	cmpl	$-1, (%rax)
.Ltmp31:
	.loc	1 59 19                 # test5.c:59:19
	jne	.LBB1_13
# BB#12:
	movl	$8, %edi
	movl	$2, %esi
	.loc	1 60 33 is_stmt 1       # test5.c:60:33
.Ltmp32:
	callq	loop
	movabsq	$.L.str, %rdi
	.loc	1 60 18 is_stmt 0 discriminator 1 # test5.c:60:18
	movl	%eax, %esi
	movb	$0, %al
	callq	printf
	.loc	1 61 18 is_stmt 1       # test5.c:61:18
	movl	$1, -4(%rbp)
	movl	%eax, -48(%rbp)         # 4-byte Spill
	jmp	.LBB1_15
.Ltmp33:
.LBB1_13:
	movl	$9, %edi
	movl	$2, %esi
	.loc	1 64 33                 # test5.c:64:33
.Ltmp34:
	callq	loop
	movabsq	$.L.str, %rdi
	.loc	1 64 18 is_stmt 0 discriminator 1 # test5.c:64:18
	movl	%eax, %esi
	movb	$0, %al
	callq	printf
	.loc	1 65 18 is_stmt 1       # test5.c:65:18
	movl	$1, -4(%rbp)
	movl	%eax, -52(%rbp)         # 4-byte Spill
	jmp	.LBB1_15
.Ltmp35:
.LBB1_14:
	movl	$10, %edi
	movl	$2, %esi
	.loc	1 70 27                 # test5.c:70:27
.Ltmp36:
	callq	loop
	movabsq	$.L.str, %rdi
	.loc	1 70 12 is_stmt 0 discriminator 1 # test5.c:70:12
	movl	%eax, %esi
	movb	$0, %al
	callq	printf
	.loc	1 71 12 is_stmt 1       # test5.c:71:12
	movl	$1, -4(%rbp)
	movl	%eax, -56(%rbp)         # 4-byte Spill
.Ltmp37:
.LBB1_15:
	.loc	1 74 1                  # test5.c:74:1
	movl	-4(%rbp), %eax
	addq	$64, %rsp
	popq	%rbp
	retq
.Ltmp38:
.Lfunc_end1:
	.size	foo, .Lfunc_end1-foo
	.cfi_endproc

	.type	.L.str,@object          # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"%d\n"
	.size	.L.str, 4

	.section	.debug_str,"MS",@progbits,1
.Linfo_string0:
	.asciz	"clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)" # string offset=0
.Linfo_string1:
	.asciz	"test5.c"               # string offset=54
.Linfo_string2:
	.asciz	"/home/tuba/Documents/tools/pmguidedsymex/test/test5" # string offset=62
.Linfo_string3:
	.asciz	"loop"                  # string offset=114
.Linfo_string4:
	.asciz	"int"                   # string offset=119
.Linfo_string5:
	.asciz	"foo"                   # string offset=123
.Linfo_string6:
	.asciz	"ru"                    # string offset=127
.Linfo_string7:
	.asciz	"base"                  # string offset=130
.Linfo_string8:
	.asciz	"result"                # string offset=135
.Linfo_string9:
	.asciz	"i"                     # string offset=142
.Linfo_string10:
	.asciz	"L"                     # string offset=144
.Linfo_string11:
	.asciz	"data"                  # string offset=146
.Linfo_string12:
	.asciz	"value"                 # string offset=151
.Linfo_string13:
	.asciz	"data_t"                # string offset=157
.Linfo_string14:
	.asciz	"public_t"              # string offset=164
.Linfo_string15:
	.asciz	"H"                     # string offset=173
.Linfo_string16:
	.asciz	"secret_t"              # string offset=175
	.section	.debug_loc,"",@progbits
	.section	.debug_abbrev,"",@progbits
.Lsection_abbrev:
	.byte	1                       # Abbreviation Code
	.byte	17                      # DW_TAG_compile_unit
	.byte	1                       # DW_CHILDREN_yes
	.byte	37                      # DW_AT_producer
	.byte	14                      # DW_FORM_strp
	.byte	19                      # DW_AT_language
	.byte	5                       # DW_FORM_data2
	.byte	3                       # DW_AT_name
	.byte	14                      # DW_FORM_strp
	.byte	16                      # DW_AT_stmt_list
	.byte	23                      # DW_FORM_sec_offset
	.byte	27                      # DW_AT_comp_dir
	.byte	14                      # DW_FORM_strp
	.byte	17                      # DW_AT_low_pc
	.byte	1                       # DW_FORM_addr
	.byte	18                      # DW_AT_high_pc
	.byte	6                       # DW_FORM_data4
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	2                       # Abbreviation Code
	.byte	46                      # DW_TAG_subprogram
	.byte	1                       # DW_CHILDREN_yes
	.byte	17                      # DW_AT_low_pc
	.byte	1                       # DW_FORM_addr
	.byte	18                      # DW_AT_high_pc
	.byte	6                       # DW_FORM_data4
	.byte	64                      # DW_AT_frame_base
	.byte	24                      # DW_FORM_exprloc
	.byte	3                       # DW_AT_name
	.byte	14                      # DW_FORM_strp
	.byte	58                      # DW_AT_decl_file
	.byte	11                      # DW_FORM_data1
	.byte	59                      # DW_AT_decl_line
	.byte	11                      # DW_FORM_data1
	.byte	39                      # DW_AT_prototyped
	.byte	25                      # DW_FORM_flag_present
	.byte	73                      # DW_AT_type
	.byte	19                      # DW_FORM_ref4
	.byte	63                      # DW_AT_external
	.byte	25                      # DW_FORM_flag_present
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	3                       # Abbreviation Code
	.byte	5                       # DW_TAG_formal_parameter
	.byte	0                       # DW_CHILDREN_no
	.byte	2                       # DW_AT_location
	.byte	24                      # DW_FORM_exprloc
	.byte	3                       # DW_AT_name
	.byte	14                      # DW_FORM_strp
	.byte	58                      # DW_AT_decl_file
	.byte	11                      # DW_FORM_data1
	.byte	59                      # DW_AT_decl_line
	.byte	11                      # DW_FORM_data1
	.byte	73                      # DW_AT_type
	.byte	19                      # DW_FORM_ref4
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	4                       # Abbreviation Code
	.byte	52                      # DW_TAG_variable
	.byte	0                       # DW_CHILDREN_no
	.byte	2                       # DW_AT_location
	.byte	24                      # DW_FORM_exprloc
	.byte	3                       # DW_AT_name
	.byte	14                      # DW_FORM_strp
	.byte	58                      # DW_AT_decl_file
	.byte	11                      # DW_FORM_data1
	.byte	59                      # DW_AT_decl_line
	.byte	11                      # DW_FORM_data1
	.byte	73                      # DW_AT_type
	.byte	19                      # DW_FORM_ref4
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	5                       # Abbreviation Code
	.byte	11                      # DW_TAG_lexical_block
	.byte	1                       # DW_CHILDREN_yes
	.byte	17                      # DW_AT_low_pc
	.byte	1                       # DW_FORM_addr
	.byte	18                      # DW_AT_high_pc
	.byte	6                       # DW_FORM_data4
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	6                       # Abbreviation Code
	.byte	36                      # DW_TAG_base_type
	.byte	0                       # DW_CHILDREN_no
	.byte	3                       # DW_AT_name
	.byte	14                      # DW_FORM_strp
	.byte	62                      # DW_AT_encoding
	.byte	11                      # DW_FORM_data1
	.byte	11                      # DW_AT_byte_size
	.byte	11                      # DW_FORM_data1
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	7                       # Abbreviation Code
	.byte	19                      # DW_TAG_structure_type
	.byte	1                       # DW_CHILDREN_yes
	.byte	3                       # DW_AT_name
	.byte	14                      # DW_FORM_strp
	.byte	11                      # DW_AT_byte_size
	.byte	11                      # DW_FORM_data1
	.byte	58                      # DW_AT_decl_file
	.byte	11                      # DW_FORM_data1
	.byte	59                      # DW_AT_decl_line
	.byte	11                      # DW_FORM_data1
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	8                       # Abbreviation Code
	.byte	13                      # DW_TAG_member
	.byte	0                       # DW_CHILDREN_no
	.byte	3                       # DW_AT_name
	.byte	14                      # DW_FORM_strp
	.byte	73                      # DW_AT_type
	.byte	19                      # DW_FORM_ref4
	.byte	58                      # DW_AT_decl_file
	.byte	11                      # DW_FORM_data1
	.byte	59                      # DW_AT_decl_line
	.byte	11                      # DW_FORM_data1
	.byte	56                      # DW_AT_data_member_location
	.byte	11                      # DW_FORM_data1
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	9                       # Abbreviation Code
	.byte	15                      # DW_TAG_pointer_type
	.byte	0                       # DW_CHILDREN_no
	.byte	73                      # DW_AT_type
	.byte	19                      # DW_FORM_ref4
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	0                       # EOM(3)
	.section	.debug_info,"",@progbits
.Lsection_info:
.Lcu_begin0:
	.long	264                     # Length of Unit
	.short	4                       # DWARF version number
	.long	.Lsection_abbrev        # Offset Into Abbrev. Section
	.byte	8                       # Address Size (in bytes)
	.byte	1                       # Abbrev [1] 0xb:0x101 DW_TAG_compile_unit
	.long	.Linfo_string0          # DW_AT_producer
	.short	12                      # DW_AT_language
	.long	.Linfo_string1          # DW_AT_name
	.long	.Lline_table_start0     # DW_AT_stmt_list
	.long	.Linfo_string2          # DW_AT_comp_dir
	.quad	.Lfunc_begin0           # DW_AT_low_pc
	.long	.Lfunc_end1-.Lfunc_begin0 # DW_AT_high_pc
	.byte	2                       # Abbrev [2] 0x2a:0x60 DW_TAG_subprogram
	.quad	.Lfunc_begin0           # DW_AT_low_pc
	.long	.Lfunc_end0-.Lfunc_begin0 # DW_AT_high_pc
	.byte	1                       # DW_AT_frame_base
	.byte	86
	.long	.Linfo_string3          # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	24                      # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	192                     # DW_AT_type
                                        # DW_AT_external
	.byte	3                       # Abbrev [3] 0x43:0xe DW_TAG_formal_parameter
	.byte	2                       # DW_AT_location
	.byte	145
	.byte	124
	.long	.Linfo_string6          # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	24                      # DW_AT_decl_line
	.long	192                     # DW_AT_type
	.byte	3                       # Abbrev [3] 0x51:0xe DW_TAG_formal_parameter
	.byte	2                       # DW_AT_location
	.byte	145
	.byte	120
	.long	.Linfo_string7          # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	24                      # DW_AT_decl_line
	.long	192                     # DW_AT_type
	.byte	4                       # Abbrev [4] 0x5f:0xe DW_TAG_variable
	.byte	2                       # DW_AT_location
	.byte	145
	.byte	116
	.long	.Linfo_string8          # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	25                      # DW_AT_decl_line
	.long	192                     # DW_AT_type
	.byte	5                       # Abbrev [5] 0x6d:0x1c DW_TAG_lexical_block
	.quad	.Ltmp4                  # DW_AT_low_pc
	.long	.Ltmp6-.Ltmp4           # DW_AT_high_pc
	.byte	4                       # Abbrev [4] 0x7a:0xe DW_TAG_variable
	.byte	2                       # DW_AT_location
	.byte	145
	.byte	112
	.long	.Linfo_string9          # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	26                      # DW_AT_decl_line
	.long	192                     # DW_AT_type
	.byte	0                       # End Of Children Mark
	.byte	0                       # End Of Children Mark
	.byte	2                       # Abbrev [2] 0x8a:0x36 DW_TAG_subprogram
	.quad	.Lfunc_begin1           # DW_AT_low_pc
	.long	.Lfunc_end1-.Lfunc_begin1 # DW_AT_high_pc
	.byte	1                       # DW_AT_frame_base
	.byte	86
	.long	.Linfo_string5          # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	31                      # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	192                     # DW_AT_type
                                        # DW_AT_external
	.byte	3                       # Abbrev [3] 0xa3:0xe DW_TAG_formal_parameter
	.byte	2                       # DW_AT_location
	.byte	145
	.byte	112
	.long	.Linfo_string10         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	31                      # DW_AT_decl_line
	.long	199                     # DW_AT_type
	.byte	3                       # Abbrev [3] 0xb1:0xe DW_TAG_formal_parameter
	.byte	2                       # DW_AT_location
	.byte	145
	.byte	104
	.long	.Linfo_string15         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	31                      # DW_AT_decl_line
	.long	246                     # DW_AT_type
	.byte	0                       # End Of Children Mark
	.byte	6                       # Abbrev [6] 0xc0:0x7 DW_TAG_base_type
	.long	.Linfo_string4          # DW_AT_name
	.byte	5                       # DW_AT_encoding
	.byte	4                       # DW_AT_byte_size
	.byte	7                       # Abbrev [7] 0xc7:0x15 DW_TAG_structure_type
	.long	.Linfo_string14         # DW_AT_name
	.byte	8                       # DW_AT_byte_size
	.byte	1                       # DW_AT_decl_file
	.byte	20                      # DW_AT_decl_line
	.byte	8                       # Abbrev [8] 0xcf:0xc DW_TAG_member
	.long	.Linfo_string11         # DW_AT_name
	.long	220                     # DW_AT_type
	.byte	1                       # DW_AT_decl_file
	.byte	21                      # DW_AT_decl_line
	.byte	0                       # DW_AT_data_member_location
	.byte	0                       # End Of Children Mark
	.byte	9                       # Abbrev [9] 0xdc:0x5 DW_TAG_pointer_type
	.long	225                     # DW_AT_type
	.byte	7                       # Abbrev [7] 0xe1:0x15 DW_TAG_structure_type
	.long	.Linfo_string13         # DW_AT_name
	.byte	4                       # DW_AT_byte_size
	.byte	1                       # DW_AT_decl_file
	.byte	12                      # DW_AT_decl_line
	.byte	8                       # Abbrev [8] 0xe9:0xc DW_TAG_member
	.long	.Linfo_string12         # DW_AT_name
	.long	192                     # DW_AT_type
	.byte	1                       # DW_AT_decl_file
	.byte	13                      # DW_AT_decl_line
	.byte	0                       # DW_AT_data_member_location
	.byte	0                       # End Of Children Mark
	.byte	7                       # Abbrev [7] 0xf6:0x15 DW_TAG_structure_type
	.long	.Linfo_string16         # DW_AT_name
	.byte	8                       # DW_AT_byte_size
	.byte	1                       # DW_AT_decl_file
	.byte	16                      # DW_AT_decl_line
	.byte	8                       # Abbrev [8] 0xfe:0xc DW_TAG_member
	.long	.Linfo_string11         # DW_AT_name
	.long	220                     # DW_AT_type
	.byte	1                       # DW_AT_decl_file
	.byte	17                      # DW_AT_decl_line
	.byte	0                       # DW_AT_data_member_location
	.byte	0                       # End Of Children Mark
	.byte	0                       # End Of Children Mark
	.section	.debug_ranges,"",@progbits
.Ldebug_range:
	.section	.debug_macinfo,"",@progbits
	.byte	0                       # End Of Macro List Mark
	.section	.debug_pubnames,"",@progbits
	.long	.LpubNames_end0-.LpubNames_begin0 # Length of Public Names Info
.LpubNames_begin0:
	.short	2                       # DWARF Version
	.long	.Lcu_begin0             # Offset of Compilation Unit Info
	.long	268                     # Compilation Unit Length
	.long	138                     # DIE offset
	.asciz	"foo"                   # External Name
	.long	42                      # DIE offset
	.asciz	"loop"                  # External Name
	.long	0                       # End Mark
.LpubNames_end0:
	.section	.debug_pubtypes,"",@progbits
	.long	.LpubTypes_end0-.LpubTypes_begin0 # Length of Public Types Info
.LpubTypes_begin0:
	.short	2                       # DWARF Version
	.long	.Lcu_begin0             # Offset of Compilation Unit Info
	.long	268                     # Compilation Unit Length
	.long	199                     # DIE offset
	.asciz	"public_t"              # External Name
	.long	246                     # DIE offset
	.asciz	"secret_t"              # External Name
	.long	192                     # DIE offset
	.asciz	"int"                   # External Name
	.long	225                     # DIE offset
	.asciz	"data_t"                # External Name
	.long	0                       # End Mark
.LpubTypes_end0:

	.ident	"clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)"
	.section	".note.GNU-stack","",@progbits
	.section	.debug_line,"",@progbits
.Lline_table_start0:
