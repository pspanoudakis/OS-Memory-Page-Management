#include "page_table.hpp"
#include "utils.hpp"
#include <deque>
#include <list>

using std::deque;
using std::list;

#include "page_handling.hpp"

/**
 * 
 * 
 */
QueueEntry::QueueEntry(PageTableEntry *page, short pid): table_entry(page), process_id(pid) {}

/**
 * 
 * 
 */
void insertPageToQueue(std::deque<QueueEntry> &queue, PageTableEntry *page, short pid)
{
    queue.push_back(QueueEntry(page, pid));
}

/* Functions used by Second Chance algorithm -------------------------------------------- */

/**
 * 
 * 
 */
int secondChanceGetAvailableFrame( deque<QueueEntry> &queue, char* memory_frames, 
                                   int& first_free_frame, const int total_frames, int &disk_writes)
{
    if (first_free_frame < total_frames)
    // Free space available
    {
        first_free_frame++;
        return first_free_frame - 1;
    }
    else
    // No free space available
    {
        return secondChanceEvict(queue, memory_frames, disk_writes);     
    }

}

/**
 * 
 * 
 */
int secondChanceEvict(deque<QueueEntry> &queue, char* memory_frames, int &disk_writes)
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
            // Page evicted
            current_entry->table_entry->valid = false;
            // The page is still in memory, but is about to be overwritten in main
            free_frame = current_entry->table_entry->frame_num;
            queue.pop_front();
            return free_frame;
        }        
    }
}

/* Functions used by LRU algorithm ------------------------------------------------------ */

list<QueueEntry>::iterator insertPageToLRUQueue(std::list<QueueEntry> &queue, PageTableEntry *page, short pid)
{
    queue.push_front( QueueEntry(page, pid) );
    return queue.begin();
}

/**
 * 
 * 
 */
void insertPageToLookupTable(LRU_LookupBucket* lookup_table, int lookup_table_size, std::list<QueueEntry>::iterator &queue_entry)
{
    int hashcode = pageHashcode( (queue_entry->table_entry->page_num), lookup_table_size);
    lookup_table[hashcode].elements.push_back(queue_entry);
}

/**
 * 
 * 
 */
void removeEntryFromLookupTable(LRU_LookupBucket* lookup_table, int lookup_table_size, QueueIteratorList::iterator &entry)
{
    int hashcode = pageHashcode( (*entry)->table_entry->page_num, lookup_table_size);
    lookup_table[hashcode].elements.erase(entry);
}

/**
 * 
 * 
 */
QueueIteratorList::iterator getPageEntryInLookupTable(LRU_LookupBucket* lookup_table, 
                                                       int lookup_table_size, PageTableEntry &page, short pid)
{
    int hashcode = pageHashcode( page.page_num, lookup_table_size);
    QueueIteratorList::iterator itr = lookup_table[hashcode].elements.begin();
    QueueIteratorList::iterator end = lookup_table[hashcode].elements.end();

    // itr is an iterator which points to a bucket list node (an iterator as well)
    // *itr is an iterator, which points to a node of the LRU queue
    // **itr is the node of the LRU queue

    for (; itr != end; itr++)
    {
        if ( (*itr)->table_entry->page_num == page.page_num )
        {
            if ( (*itr)->process_id == pid )
            {
                return itr;
            }
        }
    }

    // To indicate that an entry for the page was not found, 
    // an iterator at the end of the *first bucket* is returned
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
    queue.push_front( QueueEntry( (*lookup_entry)->table_entry, (*lookup_entry)->process_id ) );
    queue.erase( (*lookup_entry));
    (*lookup_entry) = queue.begin();
}

/**
 * 
 */
int LRU_GetAvailableFrame( std::list<QueueEntry> &queue, LRU_LookupBucket* lookup_table, int lookup_table_size, 
                           char* memory_frames, int& first_free_frame, const int total_frames, int &disk_writes)
{
    if (first_free_frame < total_frames)
    // Free space available
    {
        first_free_frame++;
        return first_free_frame - 1;
    }
    else
    // No free space available
    {
        return LRU_Evict(queue, lookup_table, lookup_table_size, memory_frames, disk_writes);     
    }
}

/**
 * 
 */
int LRU_Evict(std::list<QueueEntry> &queue, LRU_LookupBucket* lookup_table, 
              int lookup_table_size, char* memory_frames, int &disk_writes)
{
    QueueEntry& victim = queue.back();
    QueueIteratorList::iterator lookup_entry;
    int free_frame;

    if (victim.table_entry->modified)
    {
        disk_writes++;
    }

    victim.table_entry->valid = false;
    free_frame = victim.table_entry->frame_num;
    lookup_entry = getPageEntryInLookupTable(lookup_table, lookup_table_size, *(victim.table_entry), victim.process_id );
    removeEntryFromLookupTable(lookup_table, lookup_table_size, lookup_entry);
    queue.pop_back();
    return free_frame;
}