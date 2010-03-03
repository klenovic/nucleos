#ifndef __ASM_X86_FPU_H
#define __ASM_X86_FPU_H

#include <nucleos/types.h>

/* x87 FPU state, MMX Technolodgy.
 * 108 bytes.*/
struct fpu_regs_s {
	__u16 fp_control;     /* control */
	__u16 fp_unused_1;
	__u16 fp_status;      /* status */
	__u16 fp_unused_2;
	__u16 fp_tag;         /* register tags */
	__u16 fp_unused_3;
	__u32 fp_eip;         /* eip at failed instruction */
	__u16 fp_cs;          /* cs at failed instruction */
	__u16 fp_opcode;      /* opcode of failed instruction */
	__u32 fp_dp;          /* data address */
	__u16 fp_ds;          /* data segment */
	__u16 fp_unused_4;
	__u16 fp_st_regs[8][5]; /* 8 80-bit FP registers */
};

/* x87 FPU, MMX Technolodgy and SSE state.
 * 512 bytes (if you need size use FPU_XFP_SIZE). */
struct xfp_save {
	__u16 fp_control;       /* control */
	__u16 fp_status;        /* status */
	__u16 fp_tag;           /* register tags */
	__u16 fp_opcode;        /* opcode of failed instruction */
	__u32 fp_eip;           /* eip at failed instruction */
	__u16 fp_cs;            /* cs at failed instruction */
	__u16 fp_unused_1;
	__u32 fp_dp;            /* data address */
	__u16 fp_ds;            /* data segment */
	__u16 fp_unused_2;
	__u32 fp_mxcsr;         /* MXCSR */
	__u32 fp_mxcsr_mask;    /* MXCSR_MASK */
	__u16 fp_st_regs[8][8];   /* 128 bytes for ST/MM regs */
	__u32 fp_xreg_word[32]; /* space for 8 128-bit XMM registers */
	__u32 fp_padding[56];
};

/* Size of xfp_save structure. */
#define FPU_XFP_SIZE		512

union fpu_state_u {
	struct fpu_regs_s fpu_regs;
	struct xfp_save xfp_regs;
};

#endif /*__ASM_X86_FPU_H */
