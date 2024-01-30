// WAVEGEN IP Library Registers
// Jason Losh

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: Xilinx XUP Blackboard

// Hardware configuration:
// 
// AXI4-Lite interface:
//  Mapped to offset of 0x20000

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#ifndef WAVEGEN_REGS_H_
#define WAVEGEN_REGS_H_

#define OFS_MODE               0
#define OFS_RUN                1
#define OFS_FREQ_A             2
#define OFS_FREQ_B             3
#define OFS_OFFSET             4
#define OFS_AMPLITUDE          5
#define OFS_DTYCYC             6
#define OFS_CYCLES             7
#define OFS_PHASE_OFFSET_A     8 //this
#define OFS_PHASE_OFFSET_B     9 //this

#define SPAN_IN_BYTES 32

#endif