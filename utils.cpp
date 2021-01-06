#include <openssl/sha.h>
#include <cstdlib>
#include <cstring>

#include "utils.hpp"

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