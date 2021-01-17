/**
 * File: utils.cpp
 * Pavlos Spanoudakis (sdi1800184)
 * Contains implementations of useful routines used throughout the execution.
 */

#include <iostream>
#include <fstream>
#include <openssl/sha.h>
#include <cstdlib>
#include <cstring>
#include <cerrno>

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
    long logical_address;

    // Storing action type ('R' for read or 'W' for write)
    action = buffer[LINE_SIZE - 2];
    // Getting the complete logical address
    logical_address = strtol(buffer, nullptr, 16);
    // Getting the Page Number by shifting
    page_number = logical_address >> OFFSET_LENGTH;
    // Getting the offset with a similar technique
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
    else if (!isPositiveNumber(argv[2]))
    {
        cerr << "Invalid Number of frames. Make sure it is a positive integer." << endl;
    }
    else if (!isPositiveNumber(argv[3]))
    {
        cerr << "Invalid Number of traces to read per file turn. Make sure it is a positive integer." << endl;
    }
    else if (argc > 4)
    {
        if (isPositiveNumber(argv[4])) { return; }
        cerr << "Invalid number of maximum traces to read in total. Make sure it is a positive integer." << endl;
    }
    else { return; }
    
    cout << "Execution example:" << endl;
    cout << "./main <lru>/<2ch> <Number of Frames> <References per Process>" << endl;
    cout << "Optional args: <max total traces> (to be placed last)" << endl;

    exit(EXIT_FAILURE);
}

/**
 * Prints the execution options that have been selected from the user.
 */
void printArgs(const unsigned int& frames, const unsigned int& traces_per_turn, int& total_traces)
{
    cout << "Number of Frames in Memory: " << frames << endl;
    cout << "Traces to interchangeably read by every process: " << traces_per_turn << endl;
    if (total_traces == -1)
    {
        cout << "No limit for total traces selected." << endl;
    }
    else
    {
        cout << "Max traces to read: " << total_traces << endl;
    }    
}

/**
 * Prints the total stats at the end of the execution.
 */
void printStats(unsigned int& page_faults, unsigned int& disk_reads, unsigned int& disk_writes, unsigned int& read_traces)
{
    cout << "------------------------------------" << endl;
    cout << "Total Read Traces: " << read_traces << endl;
    cout << "Total Page Faults: " << page_faults << endl;
    cout << "Total Disk Reads: " << disk_reads << endl;
    cout << "Total Disk Write-Backs: " << disk_writes << endl;
}

/**
 * @brief Releases resources and memory used by main.
 * 
 * @param input_files An array with the ifstreams used by the program
 * @param memory_frames The "bitmap" with all the frames in memory
 * @param page_table An array with the hashed page tables used
 * @param buckets The number of buckets in the hash tables
 */
void releaseResources(std::ifstream* input_files, char* memory_frames, PageTableBucket** page_table, const unsigned int buckets)
{
    input_files[0].close();
    input_files[1].close();
    delete [] input_files;
    delete [] memory_frames;
    deletePageTable(page_table[0], buckets);
    deletePageTable(page_table[1], buckets);
    delete [] page_table;
}

/**
 * Checks if EOF has been reached in one of the files.
 * If this is not the case, an error message is displayed.
 */
void checkEOF(ifstream* inputFiles)
{
    if ( (inputFiles[0].eof()) || (inputFiles[1].eof()) ) { return; }
    cerr << "------------------------------------" << endl;
    cerr << "Unexpected line syntax found. The simulation will stop at this point." << endl;
    cerr << "Each line must have an 8-digit hexadecimal number, a white space and a 'W'/'R' character," << endl;
    cerr << "plus a newline character at the end (11 characters in total)." << endl;
}

/**
 * Determines whether the given c-string represents a positive decimal number.
 */
bool isPositiveNumber(const char* str)
{
    char* endptr;    
    return (strtol(str, &endptr, 10) > 0) && (*endptr == '\0');
}