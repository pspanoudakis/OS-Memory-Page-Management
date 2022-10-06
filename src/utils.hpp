/**
 * File: utils.hpp
 * Pavlos Spanoudakis (sdi1800184)
 * Header file for useful routines used throughout the execution.
 * Also contains several #define'd words used by the program.
 */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include "page_table.hpp"

#define LINE_SIZE 11
#define ADDRESS_LENGTH 32
#define OFFSET_LENGTH 12

#define FRAME_USED '1'
#define FRAME_NOT_USED '0'

int pageHashcode(int page, unsigned long int mod);
void extractTrace(char *buffer, char &action, unsigned int &page_number, unsigned int &offset);
void initInputFiles(std::vector<std::ifstream> &input_files, const std::vector<const char*> &file_paths);
void checkArgs(int argc, const char *argv[]);
void printArgs(const unsigned int& frames, const unsigned int& traces_per_turn, int& total_traces);
void printStats(unsigned int& page_faults, unsigned int& disk_reads, unsigned int& disk_writes, unsigned int& read_traces);
void releaseResources(std::vector<std::ifstream> &infiles, char* memory_frames, std::vector<PageTableBucket*> &page_tables, const unsigned int buckets);
void checkEOF(std::vector<std::ifstream> &input_files);
bool isPositiveNumber(const char* str);

#endif
