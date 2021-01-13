#include <iostream>
#include <fstream>
#include <openssl/sha.h>
#include <cstdlib>
#include <cstring>

#include "utils.hpp"

using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;
using std::ios;

/**
 * Returns the hashcode for the specified page number.
 * The hashcode is created using SHA. It is in range [0, mod)
 */
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

/**
 * Extracts the logical address from the specified string.
 * 
 * @param buffer The string to extract the logical address from
 * @param action Action type is stored here: either 'R' for reading or 'W' for writing.
 * @param page_number Page number will be stored here.
 * @param offset Where the logical address offset will be stored to.
 */
void extractTrace(char *buffer, char &action, unsigned int &page_number, unsigned int &offset)
{
    //cout << buffer << endl;
    long logical_address;

    action = buffer[LINE_SIZE - 2];
    logical_address = strtol(buffer, nullptr, 16);
    page_number = logical_address >> OFFSET_LENGTH;
    offset = logical_address << (ADDRESS_LENGTH - OFFSET_LENGTH); 
    offset = offset >> (ADDRESS_LENGTH - OFFSET_LENGTH);
}

/**
 * Opens the specified files using the given input file streams. 
 * In case of an error, the execution will be terminated.
 * @param input_files The array with the 2 input file streams.
 * @param path1 The path of the file to open using the first ifstream.
 * @param path2 The path of the file to open using the second ifstream.
 */
void initInputFiles(ifstream *input_files, const char* path1, const char* path2)
{
    input_files[0].open(path1, ios::in);
    input_files[1].open(path2, ios::in);
    if ( (!input_files[0]) || (!input_files[1]) ) {
        cerr << "Unable to open specified files. Abort." << endl;
        exit(EXIT_FAILURE); 
    }
}

/**
 * Checks if the program has been called with valid parameters. In case of
 * an invalid parameter, the execution will stop.
 */
void checkArgs(int argc, const char *argv[])
{
    if ( argc < 4 )
    {
        cerr << "Insufficient arguments." << endl;
    }
    else if ( strcmp(argv[1], "lru") && strcmp(argv[1], "2ch") )
    {
        cerr << "Invalid Algorithm argument." << endl;
    }
    else if (atoi(argv[2]) <= 0)
    {
        cerr << "Invalid Number of frames. Make sure it is a positive integer." << endl;
    }
    else if (atoi(argv[3]) <= 0)
    {
        cerr << "Invalid Number of traces to read per file turn. Make sure it is a positive integer." << endl;
    }
    else if (argc > 4)
    {
        if ( atoi(argv[4]) > 0) { return; }
        cerr << "Invalid number of maximum traces to read in total. Make sure it is a positive integer." << endl;
    }
    else { return; }
    
    cout << "Execution example:" << endl;
    cout << "./main <lru>/<2ch> <Number of Frames> <References per Process>" << endl;
    cout << "Optional args: <max total traces> (to be placed last)" << endl;

    exit(EXIT_FAILURE);
}
