#include <list>
#include <iterator>
#include "utils.hpp"

using std::list;

//TODO
int PageHashcode(int page)
{
    return 1;
}

/* Page Table Entry functions -------------------------------------------------*/
void SetEntry(PageTableEntry &entry, int page, int frame, bool modified)
{
    entry.page_num = page;
    entry.frame_num = frame;
    entry.modified = modified;
}

void MarkEntryAsModified(PageTableEntry &entry)
{
    entry.modified = true;
}

/* Hash Page Table Bucket functions -------------------------------------------*/
PageTableEntry* GetBucketPageEntry(PageTableBucket &bucket, int page)
{
    if (bucket.elements == NULL) { return NULL; }

    list<PageTableEntry>::iterator itr;
    list<PageTableEntry>::iterator end = bucket.elements->end();

    // Iterating over the entries list
    for (itr = bucket.elements->begin(); itr != end; itr++)
    {
        if (itr->page_num == page)
        // If an entry for the specified page number is found,
        {
            // Return its address (so that iteration is not needed again)
            return &(*itr);
        }
    }
    return NULL;
}

void InsertEntryToBucket(PageTableBucket &bucket, PageTableEntry entry)
{
    if (bucket.elements == NULL)
    // Bucket is not initialized (elements list has not been created)
    {
        // Create the list
        bucket.elements = new list<PageTableEntry>;
    }
    // Having ensured the elements list exists, we insert the entry
    bucket.elements->push_back(entry);
}

bool BucketIsEmpty(const PageTableBucket &bucket)
{
    return ( (bucket.elements == NULL) || (bucket.elements->size() == 0));
}

/* Hash Page Table functions --------------------------------------------------*/

void DeletePageTable(PageTableBucket *table, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (table[i].elements != NULL)
        {
            table->elements->clear();
        }
    }
}

void InsertEntryToPageTable(PageTableBucket *table, int page, int frame, bool modified)
{
    int hashcode = PageHashcode(page);

    PageTableEntry new_entry;
    new_entry.page_num = page;
    new_entry.frame_num = frame;
    new_entry.modified = modified;

    InsertEntryToBucket(table[hashcode], new_entry);
}

PageTableEntry* GetPageTableEntry(PageTableBucket *table, int page)
{
    int hashcode = PageHashcode(page);
    return GetBucketPageEntry(table[hashcode], page);
}