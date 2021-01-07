#define LINE_SIZE 11
#define ADDRESS_LENGTH 32
#define OFFSET_LENGTH 12

#include "page_table.hpp"

struct QueueEntry
{
    PageTableEntry* table_entry = NULL;
    short process_id;
};

int pageHashcode(int page, unsigned long int mod);
void extractTrace(char *buffer, char &action, unsigned int &page_number, unsigned int &offset);
void checkArgs(int argc, const char *argv[]);

// To be deleted
#define FRAMES 10