#include <iostream>
#include <deque>
#include <fstream>
#include <string.h>
#include <cstdlib>

#include "page_table.hpp"
#include "page_handling.hpp"
#include "utils.hpp"

using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;
using std::ios;
using std::deque;
using std::list;

#define INPUT_FILE_2 "traces/gcc.trace"
#define INPUT_FILE_1 "traces/bzip.trace"
//#define INPUT_FILE "input.txt"
#define PAGE_TABLE_BUCKETS 100
#define LRU_LOOKUP_BUCKETS 150

int disk_writes = 0;
int disk_reads = 0;
int page_faults = 0;

void LRU_Main(ifstream* infiles, PageTableBucket **page_table, char *memory_frames, int num_frames, int total_traces, int traces_per_turn);
void secondChanceMain(ifstream* infiles, PageTableBucket **page_table, char *memory_frames, int num_frames, int total_traces, int traces_per_turn);

int main(int argc, char const *argv[])
{
    // Argument Checking
    checkArgs(argc, argv);

    ifstream* input_files = new ifstream[2];
    checkInputFiles(input_files, INPUT_FILE_1, INPUT_FILE_2);
    
    int frames = atoi(argv[2]);                            // To be extracted from argv
    int traces_per_turn = atoi(argv[3]);
    int total_traces;
    if (argc > 4)
    {
        total_traces = atoi(argv[4]);
    }
    else
    {
        // This indicates that there is no limit for traces
        total_traces = -1;
    }    

    PageTableBucket **page_table = new PageTableBucket*[2];                    // The page tables are stored here

    /* This represents the frames in memory.
    Each element is either in FRAME_NOT_USED or FRAME_USED state.
    Using char type to limit each element size to 1 byte. */
    char* memory_frames = new char[frames];
    for (int i = 0; i < FRAMES; i++) { memory_frames[i] = FRAME_NOT_USED; }

    initializePageTables(page_table, PAGE_TABLE_BUCKETS);

    // Call specified algorithm
    if (strcmp(argv[1], "lru") == 0)
    {
        LRU_Main(input_files, page_table, memory_frames, frames, total_traces, traces_per_turn);
    }
    else
    {
        secondChanceMain(input_files, page_table, memory_frames, frames, total_traces, traces_per_turn);
    }

    cout << "Total Page Faults: " << page_faults <<endl;
    cout << "Total Disk Reads: " << disk_reads <<endl;
    cout << "Total Disk Write-Backs: " << disk_writes <<endl;

    // Releasing Resouces & Memory
    input_files[0].close();
    input_files[1].close();
    delete [] input_files;
    delete [] memory_frames;
    deletePageTable(page_table[0], PAGE_TABLE_BUCKETS);
    deletePageTable(page_table[1], PAGE_TABLE_BUCKETS);
    delete [] page_table;

    return 0;
}

void LRU_Main(ifstream* infiles, PageTableBucket **page_table, char *memory_frames, int num_frames,
              int total_traces, int traces_per_turn)
{
    int occupied_frames = 0;                        // This counts the occupied frames. It is meant to be modified by
                                                    // the internal functions of the algorithm, not by Main.
    char buffer[LINE_SIZE];                         // Buffer to initially place every trace text line
    // Extracted information from every trace is stored in these
    unsigned int page_num, offset;
    unsigned int read_traces = 0, current_turn_traces = 0;
    char action;
    PageTableEntry *current_page_entry;
    int available_frame;
    short pid = 0;

    // All pages in memory will be stored here
    list<QueueEntry> page_queue;
    LRU_LookupBucket* lookup_table = new LRU_LookupBucket[LRU_LOOKUP_BUCKETS];
    list<QueueEntry>::iterator queue_entry;
    QueueIteratorList::iterator lookup_entry;

    while (infiles[pid].getline(buffer, LINE_SIZE) && (read_traces != total_traces))
    {
        // Checking if a line was shorter than expected
        if (strlen(buffer) + 1 < LINE_SIZE) { break; }

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
            LRU_MoveFront(page_queue, lookup_entry);
        }
        else
        {
            page_faults++;
            disk_reads++;
            available_frame = LRU_GetAvailableFrame(page_table, PAGE_TABLE_BUCKETS, page_queue, lookup_table, 
                                                    LRU_LOOKUP_BUCKETS, memory_frames, occupied_frames, num_frames, disk_writes);

            current_page_entry = insertEntryToPageTable(page_table[pid], page_num, available_frame, (action == 'W'),
                                                        true, PAGE_TABLE_BUCKETS);
            memory_frames[available_frame] = FRAME_USED;

            queue_entry = insertPageToLRUQueue(page_queue, current_page_entry, pid);
            insertPageToLookupTable(lookup_table, LRU_LOOKUP_BUCKETS, queue_entry);
        }
        current_turn_traces++;
        if ( current_turn_traces == traces_per_turn )
        {
            current_turn_traces = 0;
            pid = (pid + 1) % 2;
        }
        read_traces++;
    }

    delete [] lookup_table;
}

void secondChanceMain(ifstream* infiles, PageTableBucket **page_table, char *memory_frames, const int num_frames,
                      int total_traces, int traces_per_turn)
{
    int occupied_frames = 0;                        // This counts the occupied frames. It is meant to be modified by
                                                    // the internal functions of the algorithm, not by Main.
    char buffer[LINE_SIZE];                         // Buffer to initially place every trace text line
    // Extracted information from every trace is stored in these
    unsigned int page_num, offset;
    unsigned int read_traces = 0, current_turn_traces = 0;
    char action;
    PageTableEntry *current_page_entry;
    int available_frame;
    short pid = 0;

    // All pages in memory will be stored here
    deque<QueueEntry> page_queue;

    while ( infiles[pid].getline(buffer, LINE_SIZE) && (read_traces != total_traces) )
    {
        // Checking if a line was shorter than expected
        if (strlen(buffer) + 1 < LINE_SIZE) { break; }

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
        {
            current_turn_traces = 0;
            pid = (pid + 1) % 2;
        }
        read_traces++;
    }    
}
