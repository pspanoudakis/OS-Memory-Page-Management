#include <iostream>
//#include <deque>

#include <vector>
#include <fstream>
#include <string.h>
#include <stdlib.h>

// For testing
using std::vector;
#include "temp.hpp"


//using namespace std;
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

int main(int argc, char const *argv[])
{
    ifstream finput;
    long logical_address;
    unsigned int page_num;
    unsigned int offset;
    finput.open(INPUT_FILE, ios::in);
    if (!finput) {
        cerr << "Unable to open specified file. Abort." << endl;
        return 1; 
    }
    vector<TestShiftEntry> results;
    TestShiftEntry entry;
    char buffer[LINE_SIZE];
    /*
    for (int i = 0; i < 11; i++)
    {
        buffer[i] = '^';
    }
    */
    while (finput.getline(buffer, LINE_SIZE))
    {
        cout << buffer << endl;
        logical_address = strtol(buffer, NULL, 16);
        page_num = logical_address >> OFFSET_LENGTH;
        offset = logical_address << (ADDRESS_LENGTH - OFFSET_LENGTH); 
        offset = offset >> (ADDRESS_LENGTH - OFFSET_LENGTH);

        // For testing
        entry.offset = offset;
        entry.page_number = page_num;
        entry.address_text.assign(buffer, 8);
        results.push_back(entry);
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

    // For testing
    cout << "------------------------------------" << endl;

    for(int i = 0; i < results.size(); i++)
    {
        cout << "Address: " << results[i].address_text << endl;
        cout << "Page Number: " << results[i].page_number << endl;
        cout << "Offset: " << results[i].offset << endl;
        cout << "------------------------------------" << endl;
    }

    return 0;
}
