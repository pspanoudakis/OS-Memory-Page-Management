#include <list>
#include <iterator>
#include <openssl/sha.h>
#include <cstdlib>
#include <cstring>
#include "utils.hpp"

using std::list;

struct PageTableEntry {
    int page_num;
    int frame_num;
    bool modified = 0;
};

struct PageTableBucket {
    list<PageTableEntry> *elements = NULL;
};

//TODO
int PageHashcode(int page, unsigned long int mod)
{
    int result;
    const unsigned char *data = (const unsigned char*)&page;

    // Initial hashcode is stored here
    unsigned char *hash = (unsigned char*)malloc(SHA_DIGEST_LENGTH);
    unsigned long int hash_num;

    // Getting the hashcode
    SHA1(data, 4, hash);

    // Copying part of it and storing it
    memcpy(&hash_num, hash, sizeof(unsigned long int));

    free(hash);

    // Calculating modulo and returning the result in a typical int
    hash_num = hash_num % mod;
    result = hash_num;
    return result;
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

// To be deleted --------------------------------------------------------------
#include <iostream>

void PrintEntry(PageTableEntry &entry)
{
    using std::cout;
    using std::endl;
    cout << "Page Number: " << entry.page_num << endl;
    cout << "Frame: " << entry.frame_num << endl;
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
            PrintEntry(*itr);
        }
    }
}
//-----------------------------------------------------------------

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
            table->elements->clear();
        }
    }
    delete [] table;
}

void InsertEntryToPageTable(PageTableBucket *table, int page, int frame, bool modified, int buckets)
{
    int hashcode = PageHashcode(page, buckets);

    PageTableEntry new_entry;
    new_entry.page_num = page;
    new_entry.frame_num = frame;
    new_entry.modified = modified;

    InsertEntryToBucket(table[hashcode], new_entry);
}

PageTableEntry* GetPageTableEntry(PageTableBucket *table, int page, int buckets)
{
    int hashcode = PageHashcode(page, buckets);
    return GetBucketPageEntry(table[hashcode], page);
}