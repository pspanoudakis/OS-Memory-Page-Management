struct PageTableEntry {
    int page_num;
    int frame_num;
    bool modified = 0;
};

struct PageTableBucket {
    list<PageTableEntry> *elements = NULL;
};
