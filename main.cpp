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

#define INPUT_FILE "traces/gcc.trace"
//#define INPUT_FILE "trace.txt"
#define PAGE_TABLE_BUCKETS 300

int disk_writes;
int disk_reads;
int page_faults;

void LRU_Main(ifstream &infile, PageTableBucket *page_table, char *memory_frames, int num_frames);
void secondChanceMain(ifstream &infile, PageTableBucket *page_table, char *memory_frames, int num_frames);

int main(int argc, char const *argv[])
{
    // Argument Checking
    //checkArgs(argc, argv);

    ifstream finput;                                // The input file
    /* Opening File */
    finput.open(INPUT_FILE, ios::in);
    if (!finput) {
        cerr << "Unable to open specified file. Abort." << endl;
        return 1; 
    }
    int frames = FRAMES;                            // To be extracted from argv
    PageTableBucket *page_table;                    // The page table

    /* This represents the frames in memory.
    Each element is either in FRAME_NOT_USED or FRAME_USED state.
    Using char type to limit each element size to 1 byte. */
    char* memory_frames = new char[frames];
    for (int i = 0; i < FRAMES; i++) { memory_frames[i] = FRAME_NOT_USED; }

    initializePageTable(&page_table, PAGE_TABLE_BUCKETS);

    // Call specified algorithm
    //LRU_Main(finput, page_table, memory_frames, frames);
    secondChanceMain(finput, page_table, memory_frames, frames);

    // Checking if the function exited unexpectedly
    if (!finput.eof())
    {
        cerr << "Unexpected line syntax found." << endl;
        cerr << "Each line must have an 8-digit hexadecimal number, a white space and a 'W'/'R' character," << endl;
        cerr << "plus a newline character at the end (11 characters in total)." << endl;
        finput.close();
        deletePageTable(page_table, PAGE_TABLE_BUCKETS);
        cerr << "Aborting." << endl;
        return 1;
    }

    cout << "Total Page Faults: " << page_faults <<endl;
    cout << "Total Disk Reads: " << disk_reads <<endl;
    cout << "Total Disk Write-Backs: " << disk_writes <<endl;

    // Releasing Resouces & Memory
    finput.close();
    delete [] memory_frames;
    deletePageTable(page_table, PAGE_TABLE_BUCKETS);

    return 0;
}

void LRU_Main(ifstream &infile, PageTableBucket *page_table, char *memory_frames, int num_frames)
{
    int first_free_frame = 0;                       // This will indicate the number of the first available frame
    char buffer[LINE_SIZE];                         // Buffer to initially place every trace text line

    // Extracted information from every trace is stored in these
    unsigned int page_num;
    unsigned int offset;
    char action;

    while (infile.getline(buffer, LINE_SIZE) )
    {
        // Checking if a line was shorter than expected
        if (strlen(buffer) + 1 < LINE_SIZE) { break; }

        extractTrace(buffer, action, page_num, offset);
        
        cout << buffer << endl;

        insertEntryToPageTable(page_table, page_num, 666, (action == 'W'), true, PAGE_TABLE_BUCKETS);
    }
}

void secondChanceMain(ifstream &infile, PageTableBucket *page_table, char *memory_frames, const int num_frames)
{
    int occupied_frames = 0;                        // This counts the occupied frames. It is meant to be modified by
                                                    // the internal functions of the algorithm, not by Main.
    char buffer[LINE_SIZE];                         // Buffer to initially place every trace text line
    // Extracted information from every trace is stored in these
    unsigned int page_num;
    unsigned int offset;
    char action;
    PageTableEntry *current_page_entry;
    int available_frame;
    int pid;

    // All pages in memory will be stored here
    deque<QueueEntry> page_queue;

    while (infile.getline(buffer, LINE_SIZE) )
    {
        // Checking if a line was shorter than expected
        if (strlen(buffer) + 1 < LINE_SIZE) { break; }

        extractTrace(buffer, action, page_num, offset);
        
        current_page_entry = getPageTableEntry(page_table, page_num, PAGE_TABLE_BUCKETS);
        if (current_page_entry != nullptr)
        // There is an entry for this page in the Page Table
        {
            if (current_page_entry->valid)
            // The entry is valid
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
            // Page is stored in Page Table, but frame number is invalid, so the page is not present in memory
            {
                page_faults++;
                // Find Empty frame to place it
                available_frame = secondChanceGetAvailableFrame(page_table, page_queue, memory_frames, 
                                                                occupied_frames, num_frames, disk_writes);

                // Retrieving from disk
                disk_reads++;

                // Updating the correspoding entry in page table
                current_page_entry->frame_num = available_frame;
                current_page_entry->modified = (action == 'W');
                current_page_entry->referenced = true;
                current_page_entry->valid = true;
                memory_frames[available_frame] = FRAME_USED;

                // Maybe an insertPageToQueue function?
                insertPageToQueue(page_queue, current_page_entry, pid);
            }
            continue;
        }
        page_faults++;
        disk_reads++;
        available_frame = secondChanceGetAvailableFrame(page_table, page_queue, memory_frames, 
                                                        occupied_frames, num_frames, disk_writes);
        current_page_entry = insertEntryToPageTable(page_table, page_num, available_frame, (action == 'W'),
                                                    true, PAGE_TABLE_BUCKETS);
        memory_frames[available_frame] = FRAME_USED;

        insertPageToQueue(page_queue, current_page_entry, pid);
    }
}
