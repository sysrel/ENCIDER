	.text
	.file	"test4.bc"
	.globl	loop
	.align	16, 0x90
	.type	loop,@function
loop:                                   # @loop
.Lfunc_begin0:
	.file	1 "test4.c"
	.loc	1 20 0                  # test4.c:20:0
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
	.loc	1 21 18 prologue_end    # test4.c:21:18
.Ltmp3:
	movl	-8(%rbp), %esi
	.loc	1 21 9 is_stmt 0        # test4.c:21:9
	movl	%esi, -12(%rbp)
	.loc	1 22 13 is_stmt 1       # test4.c:22:13
.Ltmp4:
	movl	$0, -16(%rbp)
.LBB0_1:                                # =>This Inner Loop Header: Depth=1
	.loc	1 22 18 is_stmt 0 discriminator 1 # test4.c:22:18
.Ltmp5:
	movl	-16(%rbp), %eax
	.loc	1 22 19 discriminator 1 # test4.c:22:19
	cmpl	-4(%rbp), %eax
	.loc	1 22 5 discriminator 1  # test4.c:22:5
	jge	.LBB0_4
# BB#2:                                 #   in Loop: Header=BB0_1 Depth=1
	.loc	1 23 17 is_stmt 1       # test4.c:23:17
	movl	-12(%rbp), %eax
	.loc	1 23 24 is_stmt 0       # test4.c:23:24
	imull	-8(%rbp), %eax
	.loc	1 23 15                 # test4.c:23:15
	movl	%eax, -12(%rbp)
# BB#3:                                 #   in Loop: Header=BB0_1 Depth=1
	.loc	1 22 25 is_stmt 1 discriminator 2 # test4.c:22:25
	movl	-16(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -16(%rbp)
	.loc	1 22 5 is_stmt 0 discriminator 2 # test4.c:22:5
	jmp	.LBB0_1
.Ltmp6:
.LBB0_4:
	.loc	1 24 13 is_stmt 1       # test4.c:24:13
	movl	-12(%rbp), %eax
	.loc	1 24 6 is_stmt 0        # test4.c:24:6
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
	.loc	1 27 0 is_stmt 1        # test4.c:27:0
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
	.loc	1 28 13 prologue_end    # test4.c:28:13
.Ltmp11:
	movq	-24(%rbp), %rsi
	.loc	1 28 19 is_stmt 0       # test4.c:28:19
	cmpl	$0, (%rsi)
.Ltmp12:
	.loc	1 28 9                  # test4.c:28:9
	jle	.LBB1_6
# BB#1:
	.loc	1 29 17 is_stmt 1       # test4.c:29:17
.Ltmp13:
	movq	-16(%rbp), %rax
	.loc	1 29 22 is_stmt 0       # test4.c:29:22
	cmpl	$1, (%rax)
.Ltmp14:
	.loc	1 29 13                 # test4.c:29:13
	jge	.LBB1_3
# BB#2:
	movl	$3, %edi
	movl	$2, %esi
	.loc	1 30 27 is_stmt 1       # test4.c:30:27
.Ltmp15:
	callq	loop
	movabsq	$.L.str, %rdi
	.loc	1 30 12 is_stmt 0 discriminator 1 # test4.c:30:12
	movl	%eax, %esi
	movb	$0, %al
	callq	printf
	.loc	1 31 12 is_stmt 1       # test4.c:31:12
	movl	$1, -4(%rbp)
	movl	%eax, -28(%rbp)         # 4-byte Spill
	jmp	.LBB1_15
.Ltmp16:
.LBB1_3:
	.loc	1 33 22                 # test4.c:33:22
	movq	-16(%rbp), %rax
	.loc	1 33 27 is_stmt 0       # test4.c:33:27
	cmpl	$5, (%rax)
.Ltmp17:
	.loc	1 33 18                 # test4.c:33:18
	jge	.LBB1_5
# BB#4:
	movl	$4, %edi
	movl	$2, %esi
	.loc	1 34 27 is_stmt 1       # test4.c:34:27
.Ltmp18:
	callq	loop
	movabsq	$.L.str, %rdi
	.loc	1 34 12 is_stmt 0 discriminator 1 # test4.c:34:12
	movl	%eax, %esi
	movb	$0, %al
	callq	printf
	.loc	1 35 12 is_stmt 1       # test4.c:35:12
	movl	$1, -4(%rbp)
	movl	%eax, -32(%rbp)         # 4-byte Spill
	jmp	.LBB1_15
.Ltmp19:
.LBB1_5:
	movl	$5, %edi
	movl	$2, %esi
	.loc	1 38 27                 # test4.c:38:27
.Ltmp20:
	callq	loop
	movabsq	$.L.str, %rdi
	.loc	1 38 12 is_stmt 0 discriminator 1 # test4.c:38:12
	movl	%eax, %esi
	movb	$0, %al
	callq	printf
	.loc	1 39 12 is_stmt 1       # test4.c:39:12
	movl	$1, -4(%rbp)
	movl	%eax, -36(%rbp)         # 4-byte Spill
	jmp	.LBB1_15
.Ltmp21:
.LBB1_6:
	.loc	1 43 17                 # test4.c:43:17
	movq	-16(%rbp), %rax
	.loc	1 43 22 is_stmt 0       # test4.c:43:22
	cmpl	$0, (%rax)
.Ltmp22:
	.loc	1 43 13                 # test4.c:43:13
	jge	.LBB1_14
# BB#7:
	.loc	1 44 20 is_stmt 1       # test4.c:44:20
.Ltmp23:
	movq	-24(%rbp), %rax
	.loc	1 44 26 is_stmt 0       # test4.c:44:26
	cmpl	$-10, (%rax)
.Ltmp24:
	.loc	1 44 16                 # test4.c:44:16
	jle	.LBB1_11
# BB#8:
	.loc	1 45 23 is_stmt 1       # test4.c:45:23
.Ltmp25:
	movq	-16(%rbp), %rax
	.loc	1 45 29 is_stmt 0       # test4.c:45:29
	cmpl	$-1, (%rax)
.Ltmp26:
	.loc	1 45 19                 # test4.c:45:19
	jne	.LBB1_10
# BB#9:
	movl	$6, %edi
	movl	$2, %esi
	.loc	1 46 33 is_stmt 1       # test4.c:46:33
.Ltmp27:
	callq	loop
	movabsq	$.L.str, %rdi
	.loc	1 46 18 is_stmt 0 discriminator 1 # test4.c:46:18
	movl	%eax, %esi
	movb	$0, %al
	callq	printf
	.loc	1 47 18 is_stmt 1       # test4.c:47:18
	movl	$1, -4(%rbp)
	movl	%eax, -40(%rbp)         # 4-byte Spill
	jmp	.LBB1_15
.Ltmp28:
.LBB1_10:
	movl	$7, %edi
	movl	$2, %esi
	.loc	1 50 33                 # test4.c:50:33
.Ltmp29:
	callq	loop
	movabsq	$.L.str, %rdi
	.loc	1 50 18 is_stmt 0 discriminator 1 # test4.c:50:18
	movl	%eax, %esi
	movb	$0, %al
	callq	printf
	.loc	1 51 18 is_stmt 1       # test4.c:51:18
	movl	$1, -4(%rbp)
	movl	%eax, -44(%rbp)         # 4-byte Spill
	jmp	.LBB1_15
.Ltmp30:
.LBB1_11:
	.loc	1 55 23                 # test4.c:55:23
	movq	-16(%rbp), %rax
	.loc	1 55 29 is_stmt 0       # test4.c:55:29
	cmpl	$-1, (%rax)
.Ltmp31:
	.loc	1 55 19                 # test4.c:55:19
	jne	.LBB1_13
# BB#12:
	movl	$8, %edi
	movl	$2, %esi
	.loc	1 56 33 is_stmt 1       # test4.c:56:33
.Ltmp32:
	callq	loop
	movabsq	$.L.str, %rdi
	.loc	1 56 18 is_stmt 0 discriminator 1 # test4.c:56:18
	movl	%eax, %esi
	movb	$0, %al
	callq	printf
	.loc	1 57 18 is_stmt 1       # test4.c:57:18
	movl	$1, -4(%rbp)
	movl	%eax, -48(%rbp)         # 4-byte Spill
	jmp	.LBB1_15
.Ltmp33:
.LBB1_13:
	movl	$9, %edi
	movl	$2, %esi
	.loc	1 60 33                 # test4.c:60:33
.Ltmp34:
	callq	loop
	movabsq	$.L.str, %rdi
	.loc	1 60 18 is_stmt 0 discriminator 1 # test4.c:60:18
	movl	%eax, %esi
	movb	$0, %al
	callq	printf
	.loc	1 61 18 is_stmt 1       # test4.c:61:18
	movl	$1, -4(%rbp)
	movl	%eax, -52(%rbp)         # 4-byte Spill
	jmp	.LBB1_15
.Ltmp35:
.LBB1_14:
	movl	$10, %edi
	movl	$2, %esi
	.loc	1 66 27                 # test4.c:66:27
.Ltmp36:
	callq	loop
	movabsq	$.L.str, %rdi
	.loc	1 66 12 is_stmt 0 discriminator 1 # test4.c:66:12
	movl	%eax, %esi
	movb	$0, %al
	callq	printf
	.loc	1 67 12 is_stmt 1       # test4.c:67:12
	movl	$1, -4(%rbp)
	movl	%eax, -56(%rbp)         # 4-byte Spill
.Ltmp37:
.LBB1_15:
	.loc	1 70 1                  # test4.c:70:1
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
	.asciz	"test4.c"               # string offset=54
.Linfo_string2:
	.asciz	"/home/tuba/Documents/tools/pmguidedsymex/test/test4" # string offset=62
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
	.asciz	"public_t"              # string offset=151
.Linfo_string13:
	.asciz	"H"                     # string offset=160
.Linfo_string14:
	.asciz	"secret_t"              # string offset=162
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
	.long	243                     # Length of Unit
	.short	4                       # DWARF version number
	.long	.Lsection_abbrev        # Offset Into Abbrev. Section
	.byte	8                       # Address Size (in bytes)
	.byte	1                       # Abbrev [1] 0xb:0xec DW_TAG_compile_unit
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
	.byte	20                      # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	192                     # DW_AT_type
                                        # DW_AT_external
	.byte	3                       # Abbrev [3] 0x43:0xe DW_TAG_formal_parameter
	.byte	2                       # DW_AT_location
	.byte	145
	.byte	124
	.long	.Linfo_string6          # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	20                      # DW_AT_decl_line
	.long	192                     # DW_AT_type
	.byte	3                       # Abbrev [3] 0x51:0xe DW_TAG_formal_parameter
	.byte	2                       # DW_AT_location
	.byte	145
	.byte	120
	.long	.Linfo_string7          # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	20                      # DW_AT_decl_line
	.long	192                     # DW_AT_type
	.byte	4                       # Abbrev [4] 0x5f:0xe DW_TAG_variable
	.byte	2                       # DW_AT_location
	.byte	145
	.byte	116
	.long	.Linfo_string8          # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	21                      # DW_AT_decl_line
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
	.byte	22                      # DW_AT_decl_line
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
	.byte	27                      # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	192                     # DW_AT_type
                                        # DW_AT_external
	.byte	3                       # Abbrev [3] 0xa3:0xe DW_TAG_formal_parameter
	.byte	2                       # DW_AT_location
	.byte	145
	.byte	112
	.long	.Linfo_string10         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	27                      # DW_AT_decl_line
	.long	199                     # DW_AT_type
	.byte	3                       # Abbrev [3] 0xb1:0xe DW_TAG_formal_parameter
	.byte	2                       # DW_AT_location
	.byte	145
	.byte	104
	.long	.Linfo_string13         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	27                      # DW_AT_decl_line
	.long	225                     # DW_AT_type
	.byte	0                       # End Of Children Mark
	.byte	6                       # Abbrev [6] 0xc0:0x7 DW_TAG_base_type
	.long	.Linfo_string4          # DW_AT_name
	.byte	5                       # DW_AT_encoding
	.byte	4                       # DW_AT_byte_size
	.byte	7                       # Abbrev [7] 0xc7:0x15 DW_TAG_structure_type
	.long	.Linfo_string12         # DW_AT_name
	.byte	8                       # DW_AT_byte_size
	.byte	1                       # DW_AT_decl_file
	.byte	16                      # DW_AT_decl_line
	.byte	8                       # Abbrev [8] 0xcf:0xc DW_TAG_member
	.long	.Linfo_string11         # DW_AT_name
	.long	220                     # DW_AT_type
	.byte	1                       # DW_AT_decl_file
	.byte	17                      # DW_AT_decl_line
	.byte	0                       # DW_AT_data_member_location
	.byte	0                       # End Of Children Mark
	.byte	9                       # Abbrev [9] 0xdc:0x5 DW_TAG_pointer_type
	.long	192                     # DW_AT_type
	.byte	7                       # Abbrev [7] 0xe1:0x15 DW_TAG_structure_type
	.long	.Linfo_string14         # DW_AT_name
	.byte	8                       # DW_AT_byte_size
	.byte	1                       # DW_AT_decl_file
	.byte	12                      # DW_AT_decl_line
	.byte	8                       # Abbrev [8] 0xe9:0xc DW_TAG_member
	.long	.Linfo_string11         # DW_AT_name
	.long	220                     # DW_AT_type
	.byte	1                       # DW_AT_decl_file
	.byte	13                      # DW_AT_decl_line
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
	.long	247                     # Compilation Unit Length
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
	.long	247                     # Compilation Unit Length
	.long	199                     # DIE offset
	.asciz	"public_t"              # External Name
	.long	225                     # DIE offset
	.asciz	"secret_t"              # External Name
	.long	192                     # DIE offset
	.asciz	"int"                   # External Name
	.long	0                       # End Mark
.LpubTypes_end0:

	.ident	"clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)"
	.section	".note.GNU-stack","",@progbits
	.section	.debug_line,"",@progbits
.Lline_table_start0:
