struct PageTableEntry;

struct PageTableBucket;

int PageHashcode(int page, unsigned long int mod);

/* Page Table Entry functions -------------------------------------------------*/

void SetEntry(PageTableEntry &entry, int page, int frame, bool modified);

void MarkEntryAsModified(PageTableEntry &entry);

/* Hash Page Table Bucket functions -------------------------------------------*/

PageTableEntry* GetBucketPageEntry(PageTableBucket &bucket, int page);

void InsertEntryToBucket(PageTableBucket &bucket, PageTableEntry entry);

bool BucketIsEmpty(const PageTableBucket &bucket);

/* Hash Page Table functions --------------------------------------------------*/

void InitializePageTable(PageTableBucket **table, int num_buckets);

void DeletePageTable(PageTableBucket *table, int size);

void InsertEntryToPageTable(PageTableBucket *table, int page, int frame, bool modified, int buckets);

PageTableEntry* GetPageTableEntry(PageTableBucket *table, int page, int buckets);

// To be deleted---------------------------------------------------------------
void PrintEntry(PageTableEntry &entry);
void PrintTableEntries(PageTableBucket *table, int buckets);