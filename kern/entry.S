/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>

# Shift Right Logical 
#define SRL(val, shamt)		(((val) >> (shamt)) & ~(-1 << (32 - (shamt))))


###################################################################
# The kernel (this code) is linked at address ~(KERNBASE + 1 Meg), 
# but the bootloader loads it at address ~1 Meg.
#	
# RELOC(x) maps a symbol x from its link address to its actual
# location in physical memory (its load address).	 
###################################################################

#define	RELOC(x) ((x) - KERNBASE)

#define MULTIBOOT_HEADER_MAGIC (0x1BADB002)
#define MULTIBOOT_HEADER_FLAGS (0)
#define CHECKSUM (-(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS))

###################################################################
# entry point
###################################################################

.text

# The Multiboot header
.align 4
.long MULTIBOOT_HEADER_MAGIC
.long MULTIBOOT_HEADER_FLAGS
.long CHECKSUM

# '_start' specifies the ELF entry point.  Since we haven't set up
# virtual memory when the bootloader enters this code, we need the
# bootloader to jump to the *physical* address of the entry point.
.globl		_start
_start = RELOC(entry)

.globl entry
entry:
	cli #!!!!!!!!!!
	movw	$0x1234,0x472			# warm boot

	# We haven't set up virtual memory yet, so we're running from
	# the physical address the boot loader loaded the kernel at: 1MB
	# (plus a few bytes).  However, the C code is linked to run at
	# KERNBASE+1MB.  Hence, we set up a trivial page directory that
	# translates virtual addresses [KERNBASE, KERNBASE+4MB) to
	# physical addresses [0, 4MB).  This 4MB region will be
	# sufficient until we set up our real page table in mem_init
	# in lab 2.

	# Load the physical address of entry_pgdir into cr3.  entry_pgdir
	# is defined in entrypgdir.c.
	movl	$(RELOC(entry_pgdir)), %eax
	movl	%eax, %cr3
	# Turn on paging.
	movl	%cr0, %eax
	orl	$(CR0_PE|CR0_PG|CR0_WP), %eax
	movl	%eax, %cr0

	# Now paging is enabled, but we're still running at a low EIP
	# (why is this okay?).  Jump up above KERNBASE before entering
	# C code.
	mov	$relocated, %eax
	jmp	*%eax
relocated:
	# Clear the frame pointer register (EBP)
	# so that once we get into debugging C code,
	# stack backtraces will be terminated properly.
	movl	$0x0,%ebp			# nuke frame pointer

	# Set the stack pointer
	movl	$(bootstacktop),%esp

	# now to C code
	call	i386_init

	# Should never get here, but in case we do, just spin.
spin:	jmp	spin


#ifdef CONFIG_KSPACE
.comm ret_eip, 4
.comm ebp_reg, 4
.comm esp_reg, 4
.comm _g_ret,  4

_generall_syscall:
	popl  _g_ret
	popl ret_eip
	movl %ebp, ebp_reg
	movl %esp, esp_reg
	movl $0x0,%ebp
	movl $(bootstacktop),%esp
	pushl $GD_KD
	pushl esp_reg
	pushfl
	# Guard to avoid hard to debug errors due to cli misusage.
	orl $FL_IF, (%esp)
	pushl $GD_KT
	pushl ret_eip
	pushl $0x0
	pushl $0x0
	pushl %ds
	pushl %es
	pushl %eax
	pushl %ecx
	pushl %edx
	pushl %ebx
	pushl esp_reg
	pushl ebp_reg
	pushl %esi
	pushl %edi
	pushl %esp
	pushl _g_ret
	ret

#ifndef __clang__
.stabs "sys_yield:",36,0,0,sys_yield
#endif
.globl sys_yield
.type  sys_yield, @function
sys_yield:
	cli #!!!!!!!!!!
	call _generall_syscall
	call csys_yield
	jmp .

#LAB 3: Your code here.
#ifndef __clang__
.stabs "sys_exit:",36,0,0,sys_exit
#endif
.globl sys_exit
.type  sys_exit, @function
sys_exit:
	cli #!!!!!!!!!!
	xorl %ebp, %ebp
	movl $(bootstacktop), %esp
	call csys_exit
	jmp .
#endif

.data
###################################################################
# boot stack
###################################################################
	.p2align	PGSHIFT		# force page alignment
	.globl		bootstack
bootstack:
	.space		KSTKSIZE
	.globl		bootstacktop   
bootstacktop:

