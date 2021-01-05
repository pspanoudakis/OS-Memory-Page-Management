#include <iostream>
//#include <deque>
//#include <iterator>
#include <fstream>
#include <string.h>
#include <stdlib.h>

//using namespace std;
using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;
using std::ios;

//#define INPUT_FILE "traces/gcc.trace"
#define INPUT_FILE "wrong.txt"

int main(int argc, char const *argv[])
{
    ifstream finput;
    finput.open(INPUT_FILE, ios::in);
    if (!finput) {
        cerr << "Unable to open specified file. Abort." << endl;
        return 1; 
    }
    
    char buffer[11];
    /*
    for (int i = 0; i < 11; i++)
    {
        buffer[i] = '^';
    }
    */
    while (finput.getline(buffer, 11))
    {
        cout << buffer << endl;
    }
    if (!finput.eof())
    {        
        cerr << "Unexpected line syntax found." << endl;
        cerr << "Each line must have an 8-digit hexadecimal number, a white space and a 'W'/'R' character," << endl;
        cerr << "plus a newline character at the end (11 characters in total)." << endl;
        finput.close();
        cerr << "Aborting." << endl;
        return 1;
    }
    finput.close();
    return 0;
}
