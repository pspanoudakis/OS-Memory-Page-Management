/**
 * File: main.cpp
 * Pavlos Spanoudakis (sdi1800184)
 * Contains the main client function, as well as the main functions
 * for LRU and Second Chance algorithms.
 */

#include <iostream>
#include <deque>
#include <fstream>
#include <string.h>
#include <cstdlib>

#include "page_table.hpp"
#include "page_handling.hpp"
#include "utils.hpp"

using std::cout;
using std::endl;
using std::ifstream;

#define INPUT_FILE_1 "samples/bzip.trace"
#define INPUT_FILE_2 "samples/gcc.trace"
#define PAGE_TABLE_BUCKETS 50
#define LRU_LOOKUP_BUCKETS 100

// Counters for total statistics
unsigned int disk_writes = 0, disk_reads = 0, page_faults = 0;

unsigned int LRU_Main(ifstream* infiles, PageTableBucket **page_table, char *memory_frames,
                      const unsigned int num_frames, const int total_traces, const unsigned int traces_per_turn);
unsigned int secondChanceMain(ifstream* infiles, PageTableBucket **page_table, char *memory_frames,
                              const unsigned int num_frames, const int total_traces, const unsigned int traces_per_turn);

int main(int argc, char const *argv[])
{
    // Argument Checking
    checkArgs(argc, argv);

    // Opening input files
    ifstream* input_files = new ifstream[2];
    initInputFiles(input_files, INPUT_FILE_1, INPUT_FILE_2);
    
    // Getting number of memory frames
    const unsigned int frames = atoi(argv[2]);
    // Getting number of traces to interchangeably read by every process
    const unsigned int traces_per_turn = atoi(argv[3]);
    int total_traces;
    // Getting number of traces to read in total
    if (argc > 4)
    {
        total_traces = atoi(argv[4]);
    }
    else
    {
        // This indicates that there is no limit for traces
        total_traces = -1;
    }
    // Number of read traces (in case no limit was specified)
    unsigned int read_traces;

    PageTableBucket **page_table = new PageTableBucket*[2];                    // The page tables are stored here

    /* This represents the frames in memory.
    Each element is either in FRAME_NOT_USED or FRAME_USED state.
    Using char type to limit each element size to 1 byte. */
    char* memory_frames = new char[frames];
    for (unsigned int i = 0; i < frames; i++) { memory_frames[i] = FRAME_NOT_USED; }

    initializePageTables(page_table, PAGE_TABLE_BUCKETS);
    printArgs(frames, traces_per_turn, total_traces);

    // Calling specified algorithm main
    if (strcmp(argv[1], "lru") == 0)
    {
        cout << "Using LRU algorithm" << endl;
        read_traces = LRU_Main(input_files, page_table, memory_frames, frames, total_traces, traces_per_turn);
    }
    else
    {
        cout << "Using 2nd chance algorithm" << endl;
        read_traces = secondChanceMain(input_files, page_table, memory_frames, frames, total_traces, traces_per_turn);
    }

    printStats(page_faults, disk_reads, disk_writes, read_traces);

    // Releasing Resouces & Memory
    releaseResources(input_files, memory_frames, page_table, PAGE_TABLE_BUCKETS);

    return 0;
}

/**
 * Main function for LRU algorithm. The skeleton is the same as the Second Chance main, 
 * but different structures and page replacement functions are used.
 */
unsigned int LRU_Main(ifstream* infiles, PageTableBucket **page_table, char *memory_frames,
                      const unsigned int num_frames, const int total_traces, const unsigned int traces_per_turn)
{
    unsigned int occupied_frames = 0;                           // This counts the occupied frames. It is meant to be modified by
                                                                // the internal functions of the algorithm, not by Main.
    char buffer[LINE_SIZE];                                     // Buffer to initially place every trace text line
    
    unsigned int page_num, offset;                              // Extracted information from every trace is stored in these
    char action;
    unsigned int read_traces = 0, current_turn_traces = 0;      // Counters for read traces
    
    PageTableEntry *current_page_entry;                         // Used for storing the Page Table Entry for the current page
    unsigned int available_frame;                               // Keeps track of the first available frame, until all frames are used
    short pid = 0;                                              // Keeps track of the current process

    using std::list;

    list<QueueEntry> page_queue;                                                // The LRU queue-like structure (implemented with a double-linked list)
    LRU_LookupBucket* lookup_table = new LRU_LookupBucket[LRU_LOOKUP_BUCKETS];  // A hash table for quicker access to the queue entry for every page
    list<QueueEntry>::iterator queue_entry;                                     // These are used for operations on the above structures
    QueueIteratorList::iterator lookup_entry;

    // Reading traces until max traces reached or EOF found
    while (infiles[pid].getline(buffer, LINE_SIZE) && (read_traces != total_traces))
    {
        // If a line was shorter than expected, stop
        if (strlen(buffer) + 1 < LINE_SIZE) { break; }

        // Extract the trace from the text line
        extractTrace(buffer, action, page_num, offset);
        
        current_page_entry = getPageTableEntry(page_table[pid], page_num, PAGE_TABLE_BUCKETS);
        if (current_page_entry != nullptr)
        // There is an entry for this page in the Page Table
        {
            // Update flags if needed
            current_page_entry->referenced = true;
            if ( !current_page_entry->modified )
            {
                // If the page is not marked as modified, and this is a write operation, set the flag to true
                current_page_entry->modified = (action == 'W');
            }
            lookup_entry = getPageEntryInLookupTable(lookup_table, LRU_LOOKUP_BUCKETS, *current_page_entry, pid);
            // Move the LRU queue entry for this page back in front of the queue
            LRU_MoveFront(page_queue, lookup_entry);
        }
        else
        // There is no entry for this page, so this is a page fault
        {
            page_faults++;
            disk_reads++;
            
            available_frame = LRU_GetAvailableFrame(page_table, PAGE_TABLE_BUCKETS, page_queue, lookup_table, 
                                                    LRU_LOOKUP_BUCKETS, memory_frames, occupied_frames, num_frames, disk_writes);

            current_page_entry = insertEntryToPageTable(page_table[pid], page_num, available_frame, (action == 'W'),
                                                        true, PAGE_TABLE_BUCKETS);
            memory_frames[available_frame] = FRAME_USED;

            // Insert an entry for this page in the LRU queue
            queue_entry = insertPageToLRUQueue(page_queue, current_page_entry, pid);
            insertPageToLookupTable(lookup_table, LRU_LOOKUP_BUCKETS, queue_entry);
        }
        current_turn_traces++;
        if ( current_turn_traces == traces_per_turn )
        // Properly changing between the 2 process when needed
        {
            current_turn_traces = 0;
            pid = (pid + 1) % 2;
        }
        read_traces++;
    }
    delete [] lookup_table;
    // If the maximum number of traces has not been reached, then check if
    // we have reached EOF in one of the files. If this is not the case, then 
    // an unexpected line syntax has been detected.
    if (read_traces != total_traces) { checkEOF(infiles); }
    return read_traces;
}

/**
 * Main function for Second chance algorithm. The skeleton is the same as the LRU main, 
 * but different structures and page replacement functions are used.
 */
unsigned int secondChanceMain(ifstream* infiles, PageTableBucket **page_table, char *memory_frames,
                              const unsigned int num_frames, const int total_traces, const unsigned int traces_per_turn)
{
    unsigned int occupied_frames = 0;                           // This counts the occupied frames. It is meant to be modified by
                                                                // the internal functions of the algorithm, not by Main.
    char buffer[LINE_SIZE];                                     // Buffer to initially place every trace text line
    
    unsigned int page_num, offset;                              // Extracted information from every trace is stored in these
    char action;
    unsigned int read_traces = 0, current_turn_traces = 0;      // Counters for read traces
    
    PageTableEntry *current_page_entry;                         // Used for storing the Page Table Entry for the current page
    int available_frame;                                        // Keeps track of the first available frame, until all frames are used
    short pid = 0;                                              // Keeps track of the current process

    using std::deque;
    deque<QueueEntry> page_queue;                               // The Second Chance queue (implemented with a double-ended queue)

    // Reading traces until max traces reached or EOF found
    while ( infiles[pid].getline(buffer, LINE_SIZE) && (read_traces != total_traces) )
    {
        // If a line was shorter than expected, stop
        if (strlen(buffer) + 1 < LINE_SIZE) { break; }

        // Extract the trace from the text line
        extractTrace(buffer, action, page_num, offset);
        
        current_page_entry = getPageTableEntry(page_table[pid], page_num, PAGE_TABLE_BUCKETS);
        if (current_page_entry != nullptr)
        // There is an entry for this page in the Page Table
        {
            // Update flags if needed
            current_page_entry->referenced = true;
            if ( !current_page_entry->modified )
            {
                // If the page is not marked as modified, and this is a write operation, set the flag to true
                current_page_entry->modified = (action == 'W');
            }
        }
        else
        // There is no entry for this page, so this is a page fault
        {
            page_faults++;
            disk_reads++;
            available_frame = secondChanceGetAvailableFrame(page_table, PAGE_TABLE_BUCKETS, page_queue, 
                                                            memory_frames, occupied_frames, num_frames, disk_writes);
            current_page_entry = insertEntryToPageTable(page_table[pid], page_num, available_frame, (action == 'W'),
                                                        true, PAGE_TABLE_BUCKETS);
            memory_frames[available_frame] = FRAME_USED;

            insertPageToQueue(page_queue, current_page_entry, pid);
        }
        current_turn_traces++;
        if ( current_turn_traces == traces_per_turn )
        // Properly changing between the 2 process when needed
        {
            current_turn_traces = 0;
            pid = (pid + 1) % 2;
        }
        read_traces++;
    }
    // If the maximum number of traces has not been reached, then check if
    // we have reached EOF in one of the files. If this is not the case, then 
    // an unexpected line syntax has been detected.
    if (read_traces != total_traces) { checkEOF(infiles); }
    return read_traces;
}
