#include <list>
#include <iterator>
#include "page_table.hpp"
#include "utils.hpp"

using std::forward_list;

/* Page Table Entry functions -------------------------------------------------*/

/**
 * Constructs a Page Table Entry with the specified values & flags.
 */
PageTableEntry::PageTableEntry(int page, int frame, bool mod, bool ref, bool val)
: page_num(page), frame_num(frame), modified(mod), referenced(ref), valid(val) {}

/* Hash Page Table Bucket functions -------------------------------------------*/

/**
 * Constructs a Page Table Bucket, with an uninitiallized entry list.
 */
PageTableBucket::PageTableBucket(): elements(nullptr) {}

/**
 * Returns the address of the bucket entry which is associated
 * with the specified Page Number.
 */
PageTableEntry* PageTableBucket::getPageEntry(int page)
{
    if (this->elements == nullptr) { return nullptr; }

    forward_list<PageTableEntry>::iterator itr;
    forward_list<PageTableEntry>::iterator end = elements->end();

    // Iterating over the entries list
    for (itr = elements->begin(); itr != end; itr++)
    {
        if (itr->page_num == page)
        // If an entry for the specified page number is found,
        {
            // Return its address (so that iteration is not needed again)
            return &(*itr);
        }
    }
    // No Entry for the specified page number was found
    return nullptr;
}

/**
 * Inserts the specified entry to the bucket list.
 * Note that no checking for duplicate entry takes place.
 */
PageTableEntry* PageTableBucket::insertEntry(PageTableEntry entry)
{
    if (this->elements == nullptr)
    // Bucket is not initialized (elements list has not been created)
    {
        // Create the list
        this->elements = new forward_list<PageTableEntry>;
        this->last = elements->before_begin();
    }
    this->last = elements->insert_after(this->last, entry);
    return &(*last);
}

/**
 * Deletes the entry associated with the specified page number.
 * If there is no such entry, no changes are made.
 */
void PageTableBucket::deletePageEntry(int page)
{
    if (this->elements == nullptr) { return; }

    forward_list<PageTableEntry>::iterator itr;
    forward_list<PageTableEntry>::iterator end = elements->end();
    forward_list<PageTableEntry>::iterator prev = elements->before_begin();

    // Iterating over the entries list
    for (itr = elements->begin(); itr != end; itr++)
    {
        if (itr->page_num == page)
        // If an entry for the specified page number is found, it will be removed
        {
            if (itr == this->last)
            // If the entry was the last one, update the last entry iterator
            {
                this->last = prev;
            }
            // And remove the entry from the list
            elements->erase_after(prev);
            return;
        }
        prev = itr;
    }
}

/* Hash Page Table functions --------------------------------------------------*/

/**
 * Initializes the Page Tables by creating their respective buckets.
 */
void initializePageTables(PageTableBucket **table, int num_buckets)
{
    table[0] = new PageTableBucket[num_buckets];
    table[1] = new PageTableBucket[num_buckets];
}

/**
 * Deletes the specified Hashed Page Table properly.
 */
void deletePageTable(PageTableBucket *table, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (table[i].elements != nullptr)
        {
            table[i].elements->clear();
            delete table[i].elements;
        }
    }
    delete [] table;
}

/**
 * Inserts a new entry in the Page Table, with all the specified values and flags.
 */
PageTableEntry* insertEntryToPageTable(PageTableBucket *table, int page, int frame, bool modified, bool referenced, int buckets)
{
    int hashcode = pageHashcode(page, buckets);

    return table[hashcode].insertEntry(PageTableEntry(page, frame, modified, referenced));
}

/**
 * Returns the address of the Page Table entry which is associated with the specified Page Number.
 */
PageTableEntry* getPageTableEntry(PageTableBucket *table, int page, int buckets)
{
    int hashcode = pageHashcode(page, buckets);
    return table[hashcode].getPageEntry(page);
}

/**
 * Removes the Page Table Entry which is associated with the specified Page Number.
 * If such entry is not present, no changes are made.
 */
void deletePageTableEntry(PageTableBucket *table, int page, int buckets)
{
    int hashcode = pageHashcode(page, buckets);
    table[hashcode].deletePageEntry(page);
}
