/**
 * File: page_handling.hpp
 * Pavlos Spanoudakis (sdi1800184)
 * Contains definitions of replacement algorithm related classes and functions.
 */

#ifndef PAGE_HANDLING_HPP
#define PAGE_HANDLING_HPP

#include "page_table.hpp"
#include "utils.hpp"
#include <deque>
#include <list>
#include <vector>

/**
 * Represents an Entry of the queue-like structure used by LRU and Second Chance algorithms.
 */
class QueueEntry
{
    public:
    PageTableEntry* table_entry;                                            // Pointer to the Page Table entry for the page associated with this queue entry
    short process_id;                                                       // The ID of the process that the page belongs to

    QueueEntry(PageTableEntry *page = nullptr, short pid = -1);             // Constructor
};

/**
 * This is used for the LRU lookup table.
 * A QueueIteratorList is a list of iterators for a QueueEntry list.
 * Mostly used for readability.
 */
typedef std::list< std::list<QueueEntry>::iterator > QueueIteratorList;

/**
 * Represents a bucket of the LRU lookup table.
 * 
 * The LRU lookup table is a hash table, which stores QueueEntry iterators.
 * The number of the bucket where each iterator is stored is the hashcode 
 * of the page number associated with the QueueEntry pointed by the iterator.
 * 
 * Using the lookup table, we can locate the QueueEntry for a page more quickly,
 * and since we are given an iterator that points to it, we can remove it from the queue
 * in constant time.
 */
class LRU_LookupBucket
{
    public:
    QueueIteratorList elements;                                             // Contains just a double-linked list of QueueEntry iterators.
};

void insertPageToQueue(std::deque<QueueEntry> &queue, PageTableEntry *page, short pid);

/* Functions used by Second Chance algorithm -------------------------------------------- */

int secondChanceGetAvailableFrame(std::vector<PageTableBucket*> &page_tables, unsigned int page_table_buckets,
                                  std::deque<QueueEntry> &queue, char* memory_frames, 
                                  unsigned int& first_free_frame, const unsigned int total_frames, unsigned int &disk_writes);

int secondChanceEvict(std::vector<PageTableBucket*> &page_tables, int page_table_buckets,
                      std::deque<QueueEntry> &queue, char* memory_frames, unsigned int &disk_writes);

/* Functions used by LRU algorithm ------------------------------------------------------ */

std::list<QueueEntry>::iterator insertPageToLRUQueue(std::list<QueueEntry> &queue, PageTableEntry *page, short pid);

void removeEntryFromLookupTable(LRU_LookupBucket* lookup_table, int lookup_table_size, QueueIteratorList::iterator &entry);

QueueIteratorList::iterator getPageEntryInLookupTable(LRU_LookupBucket* lookup_table, 
                                                       int lookup_table_size, PageTableEntry &page, short pid);

void insertPageToLookupTable(LRU_LookupBucket* lookup_table, int lookup_table_size, std::list<QueueEntry>::iterator &queue_entry);

QueueIteratorList::iterator& getPageFromLookupTable(LRU_LookupBucket* lookup_table, int lookup_table_size, PageTableEntry page, short pid);

void LRU_MoveFront(std::list<QueueEntry> &queue, QueueIteratorList::iterator &lookup_entry);

int LRU_GetAvailableFrame(std::vector<PageTableBucket*> &, unsigned int page_table_buckets,
                          std::list<QueueEntry> &queue, LRU_LookupBucket* lookup_table, unsigned int lookup_table_size, 
                          char* memory_frames, unsigned int& first_free_frame, const unsigned int total_frames, unsigned int &disk_writes);

int LRU_Evict(std::vector<PageTableBucket*> &, unsigned int page_table_buckets,
              std::list<QueueEntry> &queue, LRU_LookupBucket* lookup_table, 
              unsigned int lookup_table_size, char* memory_frames, unsigned int &disk_writes);

#endif
