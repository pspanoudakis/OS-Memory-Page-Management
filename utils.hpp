#ifndef UTILS_HPP
#define UTILS_HPP

#include "page_table.hpp"

#define LINE_SIZE 11
#define ADDRESS_LENGTH 32
#define OFFSET_LENGTH 12

#define FRAME_USED '1'
#define FRAME_NOT_USED '0'

int pageHashcode(int page, unsigned long int mod);
void extractTrace(char *buffer, char &action, unsigned int &page_number, unsigned int &offset);
void checkArgs(int argc, const char *argv[]);

// To be deleted
#define FRAMES 100
#define TOTAL_TRACES 20000
#define TURN_TRCES 250

#endif
