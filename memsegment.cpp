#include <string>
#include <iostream>
#include "memsegment.h"



using namespace std;
void MemSegment::print() {
    cout << hex << start;
    cout << "\t" << name;
    cout << "\t";
    switch (type) {
        case CODE_SEGMENT:
            cout << "code" ;
            break;
        case DATA_SEGMENT:
            cout << "data" ;
            break;
        case CALL_TABLE_SEGMENT:
            cout << "call_table";
            break;
    }
    cout << "\t" << hex << pointer <<endl;
}