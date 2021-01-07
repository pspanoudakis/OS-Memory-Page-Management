//#include <list>
#include <forward_list>

class PageTableEntry {
    /*
    int page_num;
    int frame_num;
    bool modified;
    bool referenced;
    int pageNumber();
    void markAsModified();
    bool isModified();
    bool isReferenced();
    */
    public:
        int page_num;
        int frame_num;
        bool modified;
        bool referenced;
        void set(int page, int frame, bool modified, bool referenced);
        void print();
};

class PageTableBucket {
    public:
        std::forward_list<PageTableEntry> *elements;
        std::forward_list<PageTableEntry>::iterator last;
        PageTableBucket();
        PageTableEntry* getPageEntry(int page);
        void insertEntry(PageTableEntry entry);
        void deletePageEntry(int page);
};

/* Hash Page Table functions --------------------------------------------------*/

void InitializePageTable(PageTableBucket **table, int num_buckets);

void DeletePageTable(PageTableBucket *table, int size);

void InsertEntryToPageTable(PageTableBucket *table, int page, int frame, bool modified, bool referenced, int buckets);

PageTableEntry* GetPageTableEntry(PageTableBucket *table, int page, int buckets);

void DeletePageTableEntry(PageTableBucket *table, int page, int buckets);

// To be deleted---------------------------------------------------------------
void PrintTableEntries(PageTableBucket *table, int buckets);