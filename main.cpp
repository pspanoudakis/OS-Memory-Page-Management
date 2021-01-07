#include <iostream>
#include <deque>
#include <fstream>
#include <string.h>
#include <cstdlib>

#include "page_table.hpp"
#include "utils.hpp"

using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;
using std::ios;
using std::deque;

//#define INPUT_FILE "traces/gcc.trace"
#define INPUT_FILE "input.txt"
#define PAGE_TABLE_BUCKETS 10

void LRU_Main(ifstream &infile, PageTableBucket *page_table, char *memory_frames, int num_frames);
void secondChanceMain(ifstream &infile, PageTableBucket *page_table, char *memory_frames, int num_frames);

int main(int argc, char const *argv[])
{
    // Argument Checking
    //checkArgs(argc, argv);

    int frames = FRAMES;                            // To be extracted from argv
    ifstream finput;                                // The input file
    PageTableBucket *page_table;                    // The page table

    /* This represents the frames in memory.
    Each element is either '0' (frame free) or '1' (frame not free).
    Using char type to limit each element size to 1 byte. */
    char* memory_frames = new char[frames];
    for (int i = 0; i < FRAMES; i++) { memory_frames[i] = '0'; }    

    /* Opening File */
    finput.open(INPUT_FILE, ios::in);
    if (!finput) {
        cerr << "Unable to open specified file. Abort." << endl;
        return 1; 
    }

    initializePageTable(&page_table, PAGE_TABLE_BUCKETS);

    // Call specified algorithm
    //LRU_Main(finput, page_table, memory_frames, frames);
    //secondChanceMain(finput, page_table, memory_frames, frames);

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

void secondChanceMain(ifstream &infile, PageTableBucket *page_table, char *memory_frames, int num_frames)
{
    int first_free_frame = 0;
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