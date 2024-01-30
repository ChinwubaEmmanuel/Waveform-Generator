// WAVEGEN IP
// WAVEGEN IP Library (wavegen_ip.c)

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: Xilinx XUP Blackboard

// AXI4-Lite interface: 
//   Mapped to offset of 0x20000
//
// GPIO interface:
// GPIO[31-0] is used as a general purpose GPIO p

//-----------------------------------------------------------------------------


#include <stdint.h>          // C99 integer types -- uint32_t
#include <stdbool.h>         // bool
#include <string.h>          // strcmp
#include <fcntl.h>           // open
#include <sys/mman.h>        // mmap
#include <unistd.h>          // close
#include "../address_map.h"  // address map
#include "wavegen_ip.h"         // gpio
#include "wavegen_regs.h"       // registers

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

uint32_t *base = NULL;

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

bool wavegenOpen()
{
    // Open /dev/mem
    int file = open("/dev/mem", O_RDWR | O_SYNC);
    bool bOK = (file >= 0);
    if (bOK)
    {
        // Create a map from the physical memory location of
        // /dev/mem at an offset to LW avalon interface
        // with an aperature of SPAN_IN_BYTES bytes
        // to any location in the virtual 32-bit memory space of the process
        base = mmap(NULL, SPAN_IN_BYTES, PROT_READ | PROT_WRITE, MAP_SHARED,
                    file, AXI4_LITE_BASE + WAVEGEN_BASE_OFFSET);
        bOK = (base != MAP_FAILED);

        // Close /dev/mem
        close(file);
    }
    return bOK;
}

//run stop?
void run_stop(char *out, bool play) //this
{
    if((strcmp(out, "a") == 0) || (strcmp(out, "A") == 0))
    {
        int mask = 0x1;
        *(base+OFS_RUN) &= ~mask;
        *(base+OFS_RUN) |= play;
    }
    else if((strcmp(out, "b") == 0) || (strcmp(out, "B") == 0))
    {
        int mask = 0x1 << 1;
        *(base+OFS_RUN) &= ~mask;
        *(base+OFS_RUN) |= play << 1;
    }
    else if((strcmp(out, "c") == 0))
    {
        *(base+OFS_RUN) = 0; 
        *(base+OFS_RUN) |= play | play << 1; 
    }   
}
// wave type
void wave_type(char *out, int mode, int frequency, int amplitude, int offset, int dc)
{
    if((strcmp(out, "a") == 0) || (strcmp(out, "A") == 0))
    {
        int mask = 0xFFFF;
        int mask2 = 0x7;
        *(base+OFS_AMPLITUDE) &= ~mask;
        *(base+OFS_OFFSET) &= ~mask;
        *(base+OFS_DTYCYC) &= ~mask;
        *(base+OFS_FREQ_A)     = 0;
        *(base+OFS_MODE)      &= ~mask2;

        *(base+OFS_MODE)      |= mode;
        *(base+OFS_FREQ_A)     = frequency;
        *(base+OFS_AMPLITUDE) |= amplitude; 
        *(base+OFS_OFFSET)    |= offset;
        *(base+OFS_DTYCYC)    |= dc;
    } 
    else if((strcmp(out, "b") == 0) || (strcmp(out, "B") == 0))
    {
        int mask = 0xFFFF << 16;
        int mask2 = 0x7 << 3;
        *(base+OFS_AMPLITUDE) &= ~mask;
        *(base+OFS_OFFSET) &= ~mask;
        *(base+OFS_DTYCYC) &= ~mask;
        *(base+OFS_FREQ_B)     = 0;
        *(base+OFS_MODE)      &= ~mask2;

        *(base+OFS_MODE)      |= mode << 3;
        *(base+OFS_FREQ_B)     = frequency;
        *(base+OFS_AMPLITUDE) |= amplitude << 16; 
        *(base+OFS_OFFSET)    |= offset << 16;
        *(base+OFS_DTYCYC)    |= dc << 16;
    } 
}
// cycles
void cycles(char *out, uint16_t cycle_n)
{
    int i = 0;
    //if cycle 0
    if((strcmp(out, "continuous") == 0))
    {   
        *(base+OFS_CYCLES) = 0;
        *(base+OFS_CYCLES) |= cycle_n << 16 | cycle_n;
    }
    else if(cycle_n >= 0 && cycle_n <= 65535 && ((strcmp(out, "a") == 0) || (strcmp(out, "A") == 0)))
    {
        uint32_t freq = *(base+OFS_FREQ_A);
        int sampling_freq = 50000;
        uint16_t cycles_count = 0 ;
        cycles_count = (int)((sampling_freq/freq)*cycle_n) ;
        int mask = 0xFFFF;
        *(base+OFS_CYCLES) &= ~mask;
        *(base+OFS_CYCLES) |= cycle_n;
        //*(base+OFS_CYCLES) |= cycles_count;
    }
    else if(cycle_n >= 0 && cycle_n <= 65535 && ((strcmp(out, "b") == 0) || (strcmp(out, "B") == 0)))
    {
        uint32_t freq = *(base+OFS_FREQ_B);
        int sampling_freq = 50000;
        uint16_t cycles_count = 0 ;
        cycles_count = (int)((sampling_freq/freq)*cycle_n) ;
        int mask = 0xFFFF << 16;
        *(base+OFS_CYCLES) &= ~mask;
        *(base+OFS_CYCLES) |= cycle_n << 16;
        //*(base+OFS_CYCLES) |= cycles_count << 16;
    }
    else
    {
        *(base+OFS_CYCLES) = 0;
    }        
}

void phaseOffset(char *out, int phase_offset) //this
{
    if((strcmp(out, "a") == 0) || (strcmp(out, "A") == 0))
    {
        // int mask = 0xFFFF;
        // *(base+OFS_PHASE_OFFSET) &= ~mask;

        // *(base+OFS_PHASE_OFFSET)      |= phase_offset;
        *(base+OFS_PHASE_OFFSET_A)      = 0;
        *(base+OFS_PHASE_OFFSET_A)      = phase_offset;
    } 
    else if((strcmp(out, "b") == 0) || (strcmp(out, "B") == 0))
    {
        // int mask = 0xFFFF << 16;
        // *(base+OFS_PHASE_OFFSET) &= ~mask;

        // *(base+OFS_PHASE_OFFSET)      |= phase_offset << 16;
        *(base+OFS_PHASE_OFFSET_B)      = 0;
        *(base+OFS_PHASE_OFFSET_B)      = phase_offset;
    } 
}

int getfreqa()
{
    int value = *(base+OFS_FREQ_A);
    return value;
}

int getcycles()
{
    int value = *(base+OFS_CYCLES);
    return value;
}

int getmode()
{
    int value = *(base+OFS_MODE);
    return value;
}

int getofs()
{
    int value = *(base+OFS_OFFSET);
    return value;
}

int getrun()
{
    int value = *(base+OFS_RUN);
    return value;
}

int phaseoffa()
{
    int value = *(base+OFS_PHASE_OFFSET_A);
    return value;
}

int phaseoffb()
{
    int value = *(base+OFS_PHASE_OFFSET_B);
    return value;
}