// WAVEGEN IP
// WAVEGEN IP Library (wavegen_ip.h)

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: Xilinx XUP Blackboard

// Hardware configuration:
//
// AXI4-Lite interface:
//   Mapped to offset of 0x20000
//
// GPIO interface:
// GPIO[31-0] is used as a general purpose GPIO port

//-----------------------------------------------------------------------------

#ifndef WAVEGEN_H_
#define WAVEGEN_H_

#include <stdint.h>
#include <stdbool.h>

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

bool wavegenOpen(void);
void run_stop(char *out, bool play);
void wave_type(char *out, int mode, int frequency, int amplitude, int offset, int dc);
void cycles(char *out, uint16_t cycle_n);
void phaseOffset(char *out, int phase_offset);
int getfreqa();
int getcycles();
int getmode();
int getrun();
int getofs();
int phaseoffa();
int phaseoffb();

#endif