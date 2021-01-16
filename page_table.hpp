/**
 * File: page_table.hpp
 * Pavlos Spanoudakis (sdi1800184)
 * Contains definitions of Page Table related classes and functions.
 */

#ifndef PAGE_TABLE_HPP
#define PAGE_TABLE_HPP

#include <forward_list>

/**
 * Represents a Page Table Entry, with all the required information.
 */
class PageTableEntry {
    public:
        int page_num;                                               // Page Number
        int frame_num;                                              // Number of the frame where the page is stored
        bool modified;                                              // Indicates whether the page has been modified since being retrieved from the disk
        bool referenced;                                            // Indicates whether the page has been recently referenced (used by Second Chance-like algorithms)

        PageTableEntry(int page, int frame, bool mod, bool ref);    // Constructor
};

/**
 * Represents a Hashed Page Table Bucket, implemented using a single-linked list.
 */
class PageTableBucket {
    public:
        std::forward_list<PageTableEntry> *elements;                // The bucket list with all entries of this bucket
        std::forward_list<PageTableEntry>::iterator last;           // Points to the last element of the list. Used for faster insertions.

        PageTableBucket();                                          // Constructor
        PageTableEntry* getPageEntry(int page);                     // Returns the bucket entry for the specified page
        PageTableEntry* insertEntry(PageTableEntry entry);          // Inserts the specified entry in the bucket
        void deletePageEntry(int page);                             // Deletes the entry for the specified page from the bucket
};

/* Hash Page Table functions --------------------------------------------------*/

void initializePageTables(PageTableBucket **table, int num_buckets);

void deletePageTable(PageTableBucket *table, int size);

PageTableEntry* insertEntryToPageTable(PageTableBucket *table, int page, int frame, bool modified, bool referenced, int buckets);

PageTableEntry* getPageTableEntry(PageTableBucket *table, int page, int buckets);

void deletePageTableEntry(PageTableBucket *table, int page, int buckets);

#endif
