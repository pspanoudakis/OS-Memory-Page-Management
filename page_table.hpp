#ifndef PAGE_TABLE_HPP
#define PAGE_TABLE_HPP

#include <forward_list>

class PageTableEntry {
    public:
        int page_num;
        int frame_num;
        bool modified;
        bool referenced;
        bool valid;
        void print();
        PageTableEntry(int page, int frame, bool mod, bool ref, bool val = true);
};

class PageTableBucket {
    public:
        std::forward_list<PageTableEntry> *elements;
        std::forward_list<PageTableEntry>::iterator last;
        PageTableBucket();
        PageTableEntry* getPageEntry(int page);
        PageTableEntry* insertEntry(PageTableEntry entry);
        void deletePageEntry(int page);
};

/* Hash Page Table functions --------------------------------------------------*/

void initializePageTable(PageTableBucket **table, int num_buckets);

void deletePageTable(PageTableBucket *table, int size);

PageTableEntry* insertEntryToPageTable(PageTableBucket *table, int page, int frame, bool modified, bool referenced, int buckets);

PageTableEntry* getPageTableEntry(PageTableBucket *table, int page, int buckets);

void deletePageTableEntry(PageTableBucket *table, int page, int buckets);

// To be deleted---------------------------------------------------------------
void printTableEntries(PageTableBucket *table, int buckets);

#endif
