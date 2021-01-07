#include "page_table.hpp"
#include "utils.hpp"
#include <deque>

using std::deque;

#include "page_handling.hpp"

int secondChanceGetAvailableFrame( PageTableBucket* page_table, deque<QueueEntry> &queue, char* memory_frames, 
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
        return secondChanceEvict(page_table, queue, memory_frames, disk_writes);     
    }

}

int secondChanceEvict(PageTableBucket* page_table, deque<QueueEntry> &queue, char* memory_frames, int &disk_writes)
{
    int free_frame;
    deque<QueueEntry>::iterator current_entry;
    QueueEntry temp;
    QueueEntry *victim_page;

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