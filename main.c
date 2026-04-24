/** main.c
 * ===========================================================
 * Name: C1C John Rust III, 2026
 * Section: CS483 / M4
 * Project: PEX3 - Page Replacement Simulator
 * Purpose: Reads a BYU binary memory trace file and simulates
 *          LRU page replacement to measure fault rates across
 *          varying frame allocations.
 * Documentation: 
 * 
 * I compared my output graph (like the one in the report) with C1C Watson and they matched. If
 * they did not match, I would have continued  to refine my code. No copying occurred  and this was
 * within the help policy. I used this website for how to use calloc: https://www.geeksforgeeks.
 * org/c/dynamic-memory-allocation-in-c-using-malloc-calloc-free-and-realloc/. I also  utilized 
 * Claude to make a python script to make my graphs since it was within the boundaries of AI use
 *  per the help policy. I did not use it to analyze  the graphs but since the instructions for 
 * making a graph were already outlined, I felt that taking that and having Claude make a python
 *  script would be the same. The chat I had with Claude (after giving it the help policy) is 
 * linked here: https://claude.ai/share/97aac2fd-acd5-46e2-b7d0-7566883b15ce.
 * 
 * No AI was used in the actual coding of main.c or in pagequeue.c. No copying of code from anyone
 * but Jack's previous self-occurred  (and it was his Euchre final project in which he made a 
 * double linked list prior to even learning about it).
 * =========================================================== */
#include <stdio.h>
#include <stdlib.h>
#include "byutr.h"
#include "pagequeue.h"

#define PROGRESS_INTERVAL 100000  // print status every N accesses

int main(int argc, char **argv) {
    FILE *ifp = NULL;
    unsigned long numAccesses = 0;
    p2AddrTr traceRecord;

    // Validate command-line arguments: trace_file frame_size
    if (argc != 3) {
        fprintf(stderr, "usage: %s input_byutr_file frame_size\n", argv[0]);
        fprintf(stderr, "\nframe_size:\n\t1: 512 bytes\n\t2: 1KB\n\t3: 2KB\n\t4: 4KB\n");
        exit(1);
    }

    // Open the binary trace file
    ifp = fopen(argv[1], "rb");
    if (ifp == NULL) {
        fprintf(stderr, "cannot open %s for reading\n", argv[1]);
        exit(1);
    }

    // Parse and validate frame size selection
    int menuOption = atoi(argv[2]);
    if (menuOption < 1 || menuOption > 4) {
        fprintf(stderr, "invalid frame size option: %s (must be 1-4)\n", argv[2]);
        fclose(ifp);
        exit(1);
    }

    // Map menu option to page geometry
    int offsetBits = 0;
    int maxFrames = 0;
    switch (menuOption) {
        case 1:
            offsetBits = 9;   // 512-byte pages
            maxFrames = 8192;
            break;
        case 2:
            offsetBits = 10;  // 1KB pages
            maxFrames = 4096;
            break;
        case 3:
            offsetBits = 11;  // 2KB pages
            maxFrames = 2048;
            break;
        case 4:
            offsetBits = 12;  // 4KB pages
            maxFrames = 1024;
            break;
    }

    fprintf(stderr, "Frame size option %d: %d offset bits, %d max frames, algorithm=LRU\n",
            menuOption, offsetBits, maxFrames);

    // TODO: Create your PageQueue (call pqInit, which returns a pointer)
    //       and allocate the faults[] array.  faults[f] will hold the
    //       total number of page faults that occur when f frames are
    //       available.  Use calloc so all entries start at zero.
    PageQueue* pq = pqInit(maxFrames);
    long* faults = (long*)calloc(maxFrames+1, sizeof(long));
    
    // Process each memory access from the trace file
    while (!feof(ifp)) {
        fread(&traceRecord, sizeof(p2AddrTr), 1, ifp);

        // Extract page number by shifting off the offset bits
        unsigned long pageNum = traceRecord.addr >> offsetBits;
        numAccesses++;

        // Print progress indicator to stderr every PROGRESS_INTERVAL accesses
        // (also prints the last page number seen — useful for early debugging)
        if ((numAccesses % PROGRESS_INTERVAL) == 0) {
            fprintf(stderr, "%lu samples read, last page: %lu\r", numAccesses, pageNum);
        }

        // TODO: Call pqAccess() to simulate this memory reference.
        //       It returns:
        //         -1      -> page was NOT in the queue (fault for ALL frame counts)
        //         d >= 0  -> page was at depth d from the MRU end
        //                    (fault for any allocation with fewer than d+1 frames)
        //
        //       Update faults[] accordingly.
        long result = pqAccess(pq, pageNum);
        
        if (result == -1) {
            for (int f = 1; f < maxFrames + 1; f++) {
                faults[f]++;
            }
        }
        else {
            for (long f = 1; f < result + 1; f++) {
                faults[(int)f]++;
            }
        }

    }

    fprintf(stderr, "\n%lu total accesses processed\n", numAccesses);

    // Output CSV results to stdout (redirect with > to create a .csv file)
    printf("Total Accesses:,%lu\n", numAccesses);
    printf("Frames,Missees,Miss Rate\n");

    // TODO: Loop from frame count 1 to maxFrames and print each row:
    //       printf("%d,%lu,%f\n", frameCount, faults[frameCount],
    //              (double)faults[frameCount] / (double)numAccesses);
    for (int f = 1; f < maxFrames + 1; f++) {
        printf("%d,%lu,%f\n", f, faults[f],(double)faults[f] / (double)numAccesses);
    }

    // TODO: Free your PageQueue and the faults[] array,
    //       then close the file.
    pqFree(pq);
    free(faults);
    fclose(ifp);

    return 0;
}
