#include <list>
#include <iterator>
#include "page_table.hpp"
#include "utils.hpp"

using std::forward_list;

/* Page Table Entry functions -------------------------------------------------*/

void PageTableEntry::set(int page, int frame, bool modified, bool referenced)
{
    this->page_num = page;
    this->frame_num = frame;
    this->modified = modified;
    this->referenced = referenced;
}

/*
inline void PageTableEntry::markAsModified() { this->modified = true; }
inline bool PageTableEntry::isModified() { return this->modified; }
inline bool PageTableEntry::isReferenced() { return this->; }

inline int PageTableEntry::pageNumber() { return page_num; }
*/
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

void PrintTableEntries(PageTableBucket *table, int buckets)
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

void PageTableBucket::insertEntry(PageTableEntry entry)
{
    if (this->elements == NULL)
    // Bucket is not initialized (elements list has not been created)
    {
        // Create the list
        this->elements = new forward_list<PageTableEntry>;
    }
    // Having ensured the elements list exists, we insert the entry
    forward_list<PageTableEntry>::iterator itr;
    forward_list<PageTableEntry>::iterator end = elements->end();
    forward_list<PageTableEntry>::iterator prev = itr;

    // Iterating over the entries list, and storing the last element
    for (itr = elements->before_begin(); itr != end; itr++) { prev = itr; }
    // Inserting after the last element
    elements->insert_after(prev, entry);
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
        // If an entry for the specified page number is found,
        {
            // Remove it from the list
            elements->erase_after(prev);
            return;
        }
        prev = itr;
    }
}

/* Hash Page Table functions --------------------------------------------------*/

void InitializePageTable(PageTableBucket **table, int num_buckets)
{
    *table = new PageTableBucket[num_buckets];
}

void DeletePageTable(PageTableBucket *table, int size)
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

void InsertEntryToPageTable(PageTableBucket *table, int page, int frame, bool modified, bool referenced, int buckets)
{
    int hashcode = PageHashcode(page, buckets);

    PageTableEntry new_entry;
    new_entry.set(page, frame, modified, referenced);

    table[hashcode].insertEntry(new_entry);
}

PageTableEntry* GetPageTableEntry(PageTableBucket *table, int page, int buckets)
{
    int hashcode = PageHashcode(page, buckets);
    return table[hashcode].getPageEntry(page);
}

void DeletePageTableEntry(PageTableBucket *table, int page, int buckets)
{
    int hashcode = PageHashcode(page, buckets);
    table[hashcode].deletePageEntry(page);
}