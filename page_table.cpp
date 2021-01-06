#include <list>
#include <iterator>
#include "page_table.hpp"
#include "utils.hpp"

using std::list;

/* Page Table Entry functions -------------------------------------------------*/

void PageTableEntry::set(int page, int frame, bool modified)
{
    this->page_num = page;
    this->frame_num = frame;
    this->modified = modified;
}

inline void PageTableEntry::markAsModified() { this->modified = true; }

inline int PageTableEntry::pageNumber() { return page_num; }

// To be deleted --------------------------------------------------------------
#include <iostream>

void PageTableEntry::print()
{
    using std::cout;
    using std::endl;
    cout << "Page Number: " << page_num << endl;
    cout << "Frame: " << frame_num << endl;
    cout << "------------------------------------" << endl;
}

void PrintTableEntries(PageTableBucket *table, int buckets)
{
    list<PageTableEntry>::iterator itr;
    list<PageTableEntry>::iterator end;
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

    list<PageTableEntry>::iterator itr;
    list<PageTableEntry>::iterator end = this->elements->end();

    // Iterating over the entries list
    for (itr = this->elements->begin(); itr != end; itr++)
    {
        if (itr->pageNumber() == page)
        // If an entry for the specified page number is found,
        {
            // Return its address (so that iteration is not needed again)
            return &(*itr);
        }
    }
    return NULL;
}

void PageTableBucket::insertEntry(PageTableEntry entry)
{
    if (this->elements == NULL)
    // Bucket is not initialized (elements list has not been created)
    {
        // Create the list
        this->elements = new list<PageTableEntry>;
    }
    // Having ensured the elements list exists, we insert the entry
    this->elements->push_back(entry);
}

inline bool PageTableBucket::empty()
{
    return ( (this->elements == NULL) || (this->elements->size() == 0));
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

void InsertEntryToPageTable(PageTableBucket *table, int page, int frame, bool modified, int buckets)
{
    int hashcode = PageHashcode(page, buckets);

    PageTableEntry new_entry;
    new_entry.set(page, frame, modified);

    table[hashcode].insertEntry(new_entry);
}

PageTableEntry* GetPageTableEntry(PageTableBucket *table, int page, int buckets)
{
    int hashcode = PageHashcode(page, buckets);
    return table[hashcode].getPageEntry(page);
}