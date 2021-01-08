#include <list>
#include <iterator>
#include "page_table.hpp"
#include "utils.hpp"

using std::forward_list;

/* Page Table Entry functions -------------------------------------------------*/

PageTableEntry::PageTableEntry(int page, int frame, bool mod, bool ref, bool val)
: page_num(page), frame_num(frame), modified(mod), referenced(ref), valid(val) {}

// To be deleted --------------------------------------------------------------
#include <iostream>

void PageTableEntry::print()
{
    using std::cout;
    using std::endl;
    cout << "------------------------------------" << endl;
    cout << "Page Number: " << page_num << endl;
    cout << "Frame: " << frame_num << endl;
}

void printTableEntries(PageTableBucket *table, int buckets)
{
    forward_list<PageTableEntry>::iterator itr;
    forward_list<PageTableEntry>::iterator end;
    for (int i = 0; i < buckets; i++)
    {
        if (table[i].elements == NULL) { continue; }
        end = table[i].elements->end();
        // Iterating over the entries list
        for (itr = table[i].elements->begin(); itr != end; itr++)
        {
            itr->print();
        }
    }
}
//-----------------------------------------------------------------

/* Hash Page Table Bucket functions -------------------------------------------*/
PageTableBucket::PageTableBucket(): elements(NULL) {}

PageTableEntry* PageTableBucket::getPageEntry(int page)
{
    if (this->elements == NULL) { return NULL; }

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
    return NULL;
}

PageTableEntry* PageTableBucket::insertEntry(PageTableEntry entry)
{
    if (this->elements == NULL)
    // Bucket is not initialized (elements list has not been created)
    {
        // Create the list
        this->elements = new forward_list<PageTableEntry>;
        this->last = elements->before_begin();
    }
    this->last = elements->insert_after(this->last, entry);
    return &(*last);
}

void PageTableBucket::deletePageEntry(int page)
{
    if (this->elements == NULL) { return; }

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

void initializePageTable(PageTableBucket **table, int num_buckets)
{
    *table = new PageTableBucket[num_buckets];
}

void deletePageTable(PageTableBucket *table, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (table[i].elements != NULL)
        {
            table[i].elements->clear();
            delete table[i].elements;
        }
    }
    delete [] table;
}

PageTableEntry* insertEntryToPageTable(PageTableBucket *table, int page, int frame, bool modified, bool referenced, int buckets)
{
    int hashcode = pageHashcode(page, buckets);

    return table[hashcode].insertEntry(PageTableEntry(page, frame, modified, referenced));
}

PageTableEntry* getPageTableEntry(PageTableBucket *table, int page, int buckets)
{
    int hashcode = pageHashcode(page, buckets);
    return table[hashcode].getPageEntry(page);
}

void deletePageTableEntry(PageTableBucket *table, int page, int buckets)
{
    int hashcode = pageHashcode(page, buckets);
    table[hashcode].deletePageEntry(page);
}
