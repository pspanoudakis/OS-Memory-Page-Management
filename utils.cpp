#include <iostream>
#include <openssl/sha.h>
#include <cstdlib>
#include <cstring>

#include "utils.hpp"

using std::cout;
using std::cerr;
using std::endl;

int pageHashcode(int page, unsigned long int mod)
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

void extractTrace(char *buffer, char &action, unsigned int &page_number, unsigned int &offset)
{
    long logical_address;

    action = buffer[LINE_SIZE - 2];
    logical_address = strtol(buffer, NULL, 16);
    page_number = logical_address >> OFFSET_LENGTH;
    offset = logical_address << (ADDRESS_LENGTH - OFFSET_LENGTH); 
    offset = offset >> (ADDRESS_LENGTH - OFFSET_LENGTH);
}

// NOT DONE
void checkArgs(int argc, const char *argv[])
{
    if ( argc < 4 )
    {
        cerr << "Insufficient arguments." << endl;
        cout << "Execution example:" << endl;
        cout << "./main <LRU>/<2CH> <Number of Frames> <References per Process>" << endl;
        cout << "Optional args: <max total traces> (to be placed last)" << endl;

        exit(EXIT_FAILURE);
    }
}
