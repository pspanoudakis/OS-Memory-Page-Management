#ifndef UTILS_HPP
#define UTILS_HPP

#define LINE_SIZE 11
#define ADDRESS_LENGTH 32
#define OFFSET_LENGTH 12

#include "page_table.hpp"

class QueueEntry
{
    public:
    PageTableEntry* table_entry = NULL;
    short process_id;
};

int pageHashcode(int page, unsigned long int mod);
void extractTrace(char *buffer, char &action, unsigned int &page_number, unsigned int &offset);
void checkArgs(int argc, const char *argv[]);

// To be deleted
#define FRAMES 4

#endif