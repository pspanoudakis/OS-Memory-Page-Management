#include <list>

class PageTableEntry {
    int page_num;
    int frame_num;
    bool modified;
    public:
        void set(int page, int frame, bool modified);
        int pageNumber();
        void markAsModified();
        void print();
};

class PageTableBucket {
    public:
        std::list<PageTableEntry> *elements;
        PageTableBucket();
        PageTableEntry* getPageEntry(int page);
        void insertEntry(PageTableEntry entry);
        bool empty();
};

/* Hash Page Table functions --------------------------------------------------*/

void InitializePageTable(PageTableBucket **table, int num_buckets);

void DeletePageTable(PageTableBucket *table, int size);

void InsertEntryToPageTable(PageTableBucket *table, int page, int frame, bool modified, int buckets);

PageTableEntry* GetPageTableEntry(PageTableBucket *table, int page, int buckets);

// To be deleted---------------------------------------------------------------
void PrintTableEntries(PageTableBucket *table, int buckets);