/**
 * File: page_handling.cpp
 * Pavlos Spanoudakis (sdi1800184)
 * Contains implementations of replacement algorithm related class methods and functions.
 */

#include "page_table.hpp"
#include "utils.hpp"
#include <deque>
#include <list>

using std::deque;
using std::list;

#include "page_handling.hpp"

/**
 * Constructs a Queue Entry with the specified information.
 * @param page The address of the Page Table Entry to associate this Queue Entry with
 * @param pid The id of the process that the page belongs to.
 */
QueueEntry::QueueEntry(PageTableEntry *page, short pid): table_entry(page), process_id(pid) {}

/**
 * Inserts a new entry is the Queue which contains the specified information
 * @param page The address of the Page Table Entry to associate the new entry with
 * @param pid The id of the process that the page belongs to.
 */
void insertPageToQueue(std::deque<QueueEntry> &queue, PageTableEntry *page, short pid)
{
    queue.push_back(QueueEntry(page, pid));
}

/* Functions used by Second Chance algorithm -------------------------------------------- */

/**
 * @brief Locates an available frame, using the Second Chance algorithm.
 * 
 * @param page_table The page tables of both processes. Both are needed since a page
 * from either of them may be evicted.
 * @param page_table_buckets The number of buckets in the Hashed Page Tables
 * @param queue The Queue used by Second Chance
 * @param memory_frames The "bitmap" with all frames in memory
 * @param first_free_frame The number of the first free frame reported so far
 * @param total_frames The total number of frames
 * @param disk_writes The disk write-back operations counter.
 * If a modified page is evicted, this is increased
 * 
 * @returns The number of the available frame.
 */
int secondChanceGetAvailableFrame(PageTableBucket** page_table, unsigned int page_table_buckets,
                                  std::deque<QueueEntry> &queue, char* memory_frames, 
                                  unsigned int& first_free_frame, const unsigned int total_frames, unsigned int &disk_writes)
{
    if (first_free_frame < total_frames)
    // Free space available
    {
        // Return the first available frame, after updating it.
        first_free_frame++;
        return first_free_frame - 1;
    }
    else
    // No free space available
    {
        // Evict a page and return the number of the frame where it was stored
        return secondChanceEvict(page_table, page_table_buckets, queue, memory_frames, disk_writes);     
    }

}

/**
 * @brief Evicts a Page which is present in Memory, using the Second Chance algorithm.
 * 
 * @param page_table The page tables of both processes. Both are needed since a page
 * from either of them may be evicted.
 * @param page_table_buckets The number of buckets in the Hashed Page Tables
 * @param queue The Queue used by Second Chance
 * @param memory_frames The "bitmap" with all frames in memory
 * @param disk_writes The disk write-back operations counter.
 * If a modified page is evicted, this is increased
 * 
 * @returns The number of the memory frame which contained the evicted Page.
 */
int secondChanceEvict(PageTableBucket** page_table, int page_table_buckets,
                      std::deque<QueueEntry> &queue, char* memory_frames, unsigned int &disk_writes)
{
    int free_frame;
    deque<QueueEntry>::iterator current_entry;
    QueueEntry temp;

    while (true)
    {
        current_entry = queue.begin();
        // Pushing the first element of the queue at the end, until the first element is not flagged as referenced
        if ( current_entry->table_entry->referenced )
        // First element is flagged as recently referenced
        {
            // Set the flag as false
            current_entry->table_entry->referenced = false;
            temp = *current_entry;
            // Remove the element
            queue.pop_front();
            // And push it back at the end of the queue
            queue.push_back(temp);
        }
        else
        {            
            if (current_entry->table_entry->modified)
            // Writing back to the disk at this point
            {
                disk_writes++;
            }
            // Getting the frame number of the victim page
            free_frame = current_entry->table_entry->frame_num;
            
            // Evicting Page
            deletePageTableEntry(page_table[current_entry->process_id], 
                                 current_entry->table_entry->page_num, page_table_buckets);
            queue.pop_front();
            memory_frames[free_frame] = FRAME_NOT_USED;
            return free_frame;
        }        
    }
}

/* Functions used by LRU algorithm ------------------------------------------------------ */

/**
 * @brief Inserts a Queue Entry in the LRU Queue, which contains the specified information.
 * 
 * @param queue The LRU Queue
 * @param page The address of the Page Table Entry to associate the Queue Entry with
 * @param pid The ID of the process that the Page belongs to.
 * 
 * @returns An iterator that points to the Queue node with the new entry.
 */
list<QueueEntry>::iterator insertPageToLRUQueue(std::list<QueueEntry> &queue, PageTableEntry *page, short pid)
{
    queue.push_front( QueueEntry(page, pid) );
    return queue.begin();
}

/**
 * Adds a Lookup Table entry, which is associated with the speicified Queue Entry, using an iterator.
 */
void insertPageToLookupTable(LRU_LookupBucket* lookup_table, int lookup_table_size, std::list<QueueEntry>::iterator &queue_entry)
{
    int hashcode = pageHashcode( (queue_entry->table_entry->page_num), lookup_table_size);
    lookup_table[hashcode].elements.push_back(queue_entry);
}

/**
 * Removes the Lookup Table entry pointed by the specified bucket list iterator.
 */
void removeEntryFromLookupTable(LRU_LookupBucket* lookup_table, int lookup_table_size, QueueIteratorList::iterator &entry)
{
    int hashcode = pageHashcode( (*entry)->table_entry->page_num, lookup_table_size);
    lookup_table[hashcode].elements.erase(entry);
}

/**
 * @brief Returns a bucket list iterator, which points to the LRU Queue Entry
 * associated with the specified Page Table Entry.
 * 
 * @returns The described above iterator, if it is found. In any other case,
 * an invalid iterator (which points after the end of the first bucket list) is returned.
 * Normally, this function is called when the specified page is about to be evicted, 
 * so it is quaranteed that the iterator will be found.
 */
QueueIteratorList::iterator getPageEntryInLookupTable(LRU_LookupBucket* lookup_table, 
                                                       int lookup_table_size, PageTableEntry &page, short pid)
{
    // Getting hashcode to locate the bucket where the iterator can be stored
    int hashcode = pageHashcode( page.page_num, lookup_table_size);
    QueueIteratorList::iterator itr = lookup_table[hashcode].elements.begin();
    QueueIteratorList::iterator end = lookup_table[hashcode].elements.end();

    // itr is an iterator which points to a bucket list node (an iterator as well)
    // *itr is an iterator, which points to a node of the LRU queue
    // **itr is the node of the LRU queue

    // Iterating over the bucket list
    for (; itr != end; itr++)
    {
        if ( (*itr)->table_entry->page_num == page.page_num )
        {
            if ( (*itr)->process_id == pid )
            {   // Iterator found
                return itr;
            }
        }
    }

    // To indicate that an entry for the page was not found, 
    // an iterator pointing after the end of the *first bucket* is returned
    return lookup_table[0].elements.end();
}

/**
 * Moves the queue entry pointed by the specified iterator, to the front of the queue
 * and updates the iterator to point to the moved entry properly. 
 * 
 * @param queue The queue to perform the operation to.
 * @param lookup_entry A QueueIteratorList iterator. *lookup_entry is an iterator 
 * which points to a node of the specified queue.
 */
void LRU_MoveFront(std::list<QueueEntry> &queue, QueueIteratorList::iterator &lookup_entry)
{
    // First, place a copy of the entry at the front
    queue.push_front( QueueEntry( (*lookup_entry)->table_entry, (*lookup_entry)->process_id ) );
    // Remove the entry from the original position
    queue.erase( (*lookup_entry));
    // Updating the iterator so that it remains valid
    (*lookup_entry) = queue.begin();
}

/**
 * @brief Locates an available frame, using LRU algorithm.
 * 
 * @param page_table The page tables of both processes. Both are needed since a page
 * from either of them may be evicted.
 * @param page_table_buckets The number of buckets in the Hashed Page Tables
 * @param queue The Queue used by LRU
 * @param lookup_table The lookup table used to speed up LRU Queue operations
 * @param lookup_table_size The size (number of buckets) in the lookup table
 * @param memory_frames The "bitmap" with all frames in memory
 * @param first_free_frame The number of the first free frame reported so far
 * @param total_frames The total number of frames
 * @param disk_writes The disk write-back operations counter.
 * If a modified page is evicted, this is increased
 * 
 * @returns The number of the available frame.
 */
int LRU_GetAvailableFrame(PageTableBucket** page_table, unsigned int page_table_buckets,
                          std::list<QueueEntry> &queue, LRU_LookupBucket* lookup_table, unsigned int lookup_table_size, 
                          char* memory_frames, unsigned int& first_free_frame, const unsigned int total_frames, unsigned int &disk_writes)
{
    if (first_free_frame < total_frames)
    // Free space available
    {
        // Return the first available frame, after updating it.
        first_free_frame++;
        return first_free_frame - 1;
    }
    else
    // No free space available
    {
        // Evict a page and return the number of the frame where it was stored
        return LRU_Evict(page_table, page_table_buckets, queue,
                         lookup_table, lookup_table_size, memory_frames, disk_writes);     
    }
}

/**
 * @brief Evicts a Page which is present in Memory, using LRU algorithm.
 * 
 * @param page_table The page tables of both processes. Both are needed since a page
 * from either of them may be evicted.
 * @param page_table_buckets The number of buckets in the Hashed Page Tables
 * @param queue The Queue used by LRU
 * @param lookup_table The lookup table used to speed up LRU Queue operations
 * @param lookup_table_size The size (number of buckets) in the lookup table
 * @param memory_frames The "bitmap" with all frames in memory
 * @param disk_writes The disk write-back operations counter.
 * If a modified page is evicted, this is increased
 * 
 * @returns The number of the memory frame which contained the evicted Page.
 */
int LRU_Evict(PageTableBucket** page_table, unsigned int page_table_buckets,
              std::list<QueueEntry> &queue, LRU_LookupBucket* lookup_table, 
              unsigned int lookup_table_size, char* memory_frames, unsigned int &disk_writes)
{
    // The victim page is the one at the back of the queue
    QueueEntry& victim = queue.back();
    QueueIteratorList::iterator lookup_entry;
    int free_frame;

    if (victim.table_entry->modified)
    {
        // Writing back to disk if dirty bit is set
        disk_writes++;
    }

    // Getting the frame number of the victim page
    free_frame = victim.table_entry->frame_num;

    // Evicting the page
    lookup_entry = getPageEntryInLookupTable(lookup_table, lookup_table_size, 
                                             *(victim.table_entry), victim.process_id );
    removeEntryFromLookupTable(lookup_table, lookup_table_size, lookup_entry);
    deletePageTableEntry(page_table[victim.process_id], victim.table_entry->page_num, page_table_buckets);
    queue.pop_back();
    memory_frames[free_frame] = FRAME_NOT_USED;
    return free_frame;
}