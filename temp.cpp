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

#define INPUT_FILE "traces/gcc.trace"

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
    finput.close();
    return 0;
}
