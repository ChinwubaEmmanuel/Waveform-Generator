// WAVEGEN IP
// WAVEGEN Shell Command

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

#include <stdlib.h>          // EXIT_ codes
#include <stdio.h>           // printf
#include <string.h>          // strcmp
#include "wavegen_ip.h"         // WAVEGEN IP library

int main(int argc, char* argv[])
{
    char *out;
    int offset = 0;
    int cycle_n = 0;
    int frequency = 0;
    int amplitude = 0;
    int phase_offset = 0;
    int dc = 0;
    int val1;
    int val2;
    int val3;
    int val4;
    int val5;

    // run, stop
    if (argc == 2)
    {
        wavegenOpen();
        if (strcmp(argv[1], "run") == 0)
        {
            run_stop("c", 1);
            printf("run\n");
        }
        else if (strcmp(argv[1], "stop") == 0)
        {
            run_stop("c", 0);
            printf("stop\n");
        }         
    }
    // cycles continuous
    else if (argc == 3)
    {
        out = argv[2];
        wavegenOpen();
        if (strcmp(argv[1], "cycles") == 0)
        {
            if (strcmp(argv[2], "continuous") == 0)
            {
                cycles(out, 0);
            }
        }
        else if(strcmp(argv[1], "run") == 0)
        {
            run_stop(out, 1);
        }
        else if(strcmp(argv[1], "stop") == 0)
        {
            run_stop(out, 0);
        }
    }
    // cycles, dc
    else if (argc == 4)
    {
        wavegenOpen();
        out = argv[2];
        if (strcmp(argv[1], "dc") == 0)
        {
            offset = atoi(argv[3]);
            wave_type(out, 0, 0, 0, offset, 0); // ???
        }
        else if (strcmp(argv[1], "cycles") == 0)
        {
            cycle_n = atoi(argv[3]);
            cycles(out, cycle_n);
        } 
        val1 = getfreqa();
        val2 = getcycles();
        val3 = getofs();
        val4 = getrun();
        printf("out: %s, %d, %d, %d, %d; freqA: %d, cycles: %d, offset: %d, run: %d  \n",out, frequency, amplitude, offset, dc, val1, val2, val3, val4);
    }
    else if (argc == 5 || argc == 6 || argc == 7)
    {
        wavegenOpen();
        out       = argv[2];
        frequency = atoi(argv[3]);
        amplitude = atoi(argv[4]);
        phase_offset = atoi(argv[4]);
        offset = 0;
        dc = 50;
        if(argc == 6)
        {
            offset = atoi(argv[5]);
        }
        else if(argc == 7)
        {
            offset = atoi(argv[5]);
            dc  = atoi(argv[6]);
        }
        if (strcmp(argv[1], "sine") == 0)
        {
            wave_type(out, 1, frequency, amplitude, offset, 0);
        }
        else if (strcmp(argv[1], "sawtooth") == 0)
        {
            wave_type(out, 2, frequency, amplitude, offset, 0);
        }  
        else if (strcmp(argv[1], "triangle") == 0)
        {
            wave_type(out, 3, frequency, amplitude, offset, 0);
        }
        else if (strcmp(argv[1], "square") == 0)
        {
            wave_type(out, 4, frequency, amplitude, offset, dc);
        }
        else if (strcmp(argv[1], "arb") == 0)
        {
            wave_type(out, 5, frequency, amplitude, offset, 0);
        }
        else if (strcmp(argv[1], "phase") == 0)
        {
            if (strcmp(argv[2], "offset") == 0)
            {
                phaseOffset(argv[3], phase_offset);
            }
        }

        val1 = getfreqa();
        val2 = getcycles();
        val3 = getmode();
        val4 = phaseoffa();
        val5 = phaseoffb();
        printf("out: %s, %d, %d, %d, %d; freqA: %d; cycles: %d; mode: %d; phase offset a: %d; phase offset b: %d\n",out,
                                                        frequency, amplitude, offset, dc, val1, val2, val3, val4, val5);
    }
    else
        printf("  command not understood\n");
    return EXIT_SUCCESS;
}