#include "page_table.hpp"
#include "utils.hpp"
#include <deque>

int secondChanceGetAvailableFrame( PageTableBucket* page_table, std::deque<QueueEntry> &queue, char* memory_frames, 
                                   int& first_free_frame, const int total_frames, int &disk_writes);
int secondChanceEvict(PageTableBucket* page_table, std::deque<QueueEntry> &queue, char* memory_frames, int &disk_writes);