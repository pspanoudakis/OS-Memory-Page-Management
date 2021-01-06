#include <iostream>
//#include <deque>
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

//#define INPUT_FILE "traces/gcc.trace"
#define INPUT_FILE "input.txt"
#define PAGE_TABLE_BUCKETS 10

int main(int argc, char const *argv[])
{
    // Argument Checking
    //checkArgs(argc, argv);

    /* Declarations */
    ifstream finput;
    unsigned int page_num;
    unsigned int offset;
    PageTableBucket *page_table;
    char buffer[LINE_SIZE];
    char action;

    /* This represents the frames in memory.
    Each element is either '0' (frame free) or '1' (frame not free).
    Using char type to limit each element size to 1 byte. */
    char* memory_frames = new char[FRAMES];

    /* Opening File */
    finput.open(INPUT_FILE, ios::in);
    if (!finput) {
        cerr << "Unable to open specified file. Abort." << endl;
        return 1; 
    }

    InitializePageTable(&page_table, PAGE_TABLE_BUCKETS);

    while (finput.getline(buffer, LINE_SIZE) )
    {
        // Checking if a line was shorter than expected
        if (strlen(buffer) + 1 < LINE_SIZE) { break; }

        extractTrace(buffer, action, page_num, offset);
        
        cout << buffer << endl;

        InsertEntryToPageTable(page_table, page_num, 666, (action == 'W'), true, PAGE_TABLE_BUCKETS);
    }

    // Checking if the getline loop exited unexpectedly
    if (!finput.eof())
    {
        cerr << "Unexpected line syntax found." << endl;
        cerr << "Each line must have an 8-digit hexadecimal number, a white space and a 'W'/'R' character," << endl;
        cerr << "plus a newline character at the end (11 characters in total)." << endl;
        finput.close();
        DeletePageTable(page_table, PAGE_TABLE_BUCKETS);
        cerr << "Aborting." << endl;
        return 1;
    }

    // Releasing Resouces & Memory
    finput.close();
    PrintTableEntries(page_table, PAGE_TABLE_BUCKETS);
    DeletePageTable(page_table, PAGE_TABLE_BUCKETS);

    return 0;
}
