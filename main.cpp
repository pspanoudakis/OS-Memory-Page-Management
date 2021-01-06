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
#define LINE_SIZE 11
#define ADDRESS_LENGTH 32
#define OFFSET_LENGTH 12
#define PAGE_TABLE_BUCKETS 10

int main(int argc, char const *argv[])
{
    /*
    if ( argc < 4 )
    {
        cout << "Insufficient arguments." << endl;
        cout << "Execution example:" << endl;
        cout << "./main <LRU>/<2CH> <Number of Frames> <References per Process>" << endl;
        cout << "Optional args: <max total traces> (to be placed last)" << endl;

        return 1;
    }
    */
    ifstream finput;
    long logical_address;
    unsigned int page_num;
    unsigned int offset;
    finput.open(INPUT_FILE, ios::in);
    if (!finput) {
        cerr << "Unable to open specified file. Abort." << endl;
        return 1; 
    }
    char buffer[LINE_SIZE];
    char action;

    PageTableBucket *page_table;
    InitializePageTable(&page_table, PAGE_TABLE_BUCKETS);

    while (finput.getline(buffer, LINE_SIZE))
    {
        action = buffer[9];
        cout << buffer << endl;
        logical_address = strtol(buffer, NULL, 16);
        page_num = logical_address >> OFFSET_LENGTH;
        offset = logical_address << (ADDRESS_LENGTH - OFFSET_LENGTH); 
        offset = offset >> (ADDRESS_LENGTH - OFFSET_LENGTH);

        InsertEntryToPageTable(page_table, page_num, 666, false, PAGE_TABLE_BUCKETS);
    }
    if (!finput.eof())
    {
        cerr << "Unexpected line syntax found." << endl;
        cerr << "Each line must have an 8-digit hexadecimal number, a white space and a 'W'/'R' character," << endl;
        cerr << "plus a newline character at the end (11 characters in total)." << endl;
        finput.close();
        cerr << "Aborting." << endl;
        return 1;
    }
    finput.close();
    PrintTableEntries(page_table, PAGE_TABLE_BUCKETS);
    DeletePageTable(page_table, PAGE_TABLE_BUCKETS);

    return 0;
}
