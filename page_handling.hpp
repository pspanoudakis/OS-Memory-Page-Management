#ifndef PAGE_HANDLING_HPP
#define PAGE_HANDLING_HPP

#include "page_table.hpp"
#include "utils.hpp"
#include <deque>
#include <list>

class QueueEntry
{
    public:
    PageTableEntry* table_entry = nullptr;
    short process_id;
    QueueEntry(PageTableEntry *page = nullptr, short pid = -1);
};

// A QueueIteratorList is a list of iterators for a QueueEntry list.
typedef std::list< std::list<QueueEntry>::iterator > QueueIteratorList;

class LRU_LookupBucket
{
    public:
    QueueIteratorList elements;
};

void insertPageToQueue(std::deque<QueueEntry> &queue, PageTableEntry *page, short pid);

/* Functions used by Second Chance algorithm -------------------------------------------- */

int secondChanceGetAvailableFrame( PageTableBucket* page_table, std::deque<QueueEntry> &queue, char* memory_frames, 
                                   int& first_free_frame, const int total_frames, int &disk_writes);

int secondChanceEvict(PageTableBucket* page_table, std::deque<QueueEntry> &queue, char* memory_frames, int &disk_writes);

/* Functions used by LRU algorithm ------------------------------------------------------ */

void removeEntryFromLookupTable(LRU_LookupBucket* lookup_table, int lookup_table_size, QueueIteratorList::iterator &entry);

QueueIteratorList::iterator getPageEntryInLookupTable(LRU_LookupBucket* lookup_table, 
                                                       int lookup_table_size, PageTableEntry page, int pid);

void insertPageToLookupTable(LRU_LookupBucket* lookup_table, std::list<QueueEntry>::iterator &queue_entry);

QueueIteratorList::iterator& getPageFromLookupTable(LRU_LookupBucket* lookup_table, int lookup_table_size, PageTableEntry page, int pid);

int LRU_GetAvailableFrame( PageTableBucket* page_table, std::deque<QueueEntry> &queue, 
                           LRU_LookupBucket* lookup_table, int lookup_table_size,
                           char* memory_frames, int& first_free_frame, const int total_frames, int &disk_writes);

int LRU_Evict(PageTableBucket* page_table, std::deque<QueueEntry> &queue, LRU_LookupBucket* lookup_table, 
              int lookup_table_size, char* memory_frames, int &disk_writes);

#endif
