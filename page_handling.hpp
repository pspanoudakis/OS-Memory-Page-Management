#ifndef PAGE_HANDLING_HPP
#define PAGE_HANDLING_HPP

#include "page_table.hpp"
#include "utils.hpp"
#include <deque>

class QueueEntry
{
    public:
    PageTableEntry* table_entry = NULL;
    short process_id;
    QueueEntry(PageTableEntry *page = NULL, short pid = -1);
};

int secondChanceGetAvailableFrame( PageTableBucket* page_table, std::deque<QueueEntry> &queue, char* memory_frames, 
                                   int& first_free_frame, const int total_frames, int &disk_writes);
int secondChanceEvict(PageTableBucket* page_table, std::deque<QueueEntry> &queue, char* memory_frames, int &disk_writes);

void insertPageToQueue(std::deque<QueueEntry> &queue, PageTableEntry *page, short pid);

#endif
