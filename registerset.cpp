#include <string>
#include <regex>
#include <iostream>
#include <fstream>
#include <sstream>
#include "registerset.h"

using namespace std;

RegisterSet::~RegisterSet() {
	 for(auto elem : regset){
		 delete elem.second;
	 }
}

void RegisterSet::add(regMap *r, string name) {
    regset[name] = r;
}

regMap* RegisterSet::get(string name) {
    map<string, regMap*>::iterator it;

    it = regset.find(name);
    if(it != regset.end())
        return it->second;
    else
        return NULL;

}

bool RegisterSet::load(string filename){
    enum status {START, OPEN, REGDEF, REGDEF_EXT, CLOSE} state;
	bool parse_error = false;
    regMap *regmap;
    string regGroup;
    string str;

    smatch m;
	regex tk_id("^/[\\s]*([a-zA-Z][a-zA-Z0-9]*)[\\s]*");	
	regex tk_blk("^/[\\s]*\\{[\\s]*");
	regex tk_reg("^/[\\s]*0x([0-9a-fA-F]+)[\\s]*=[\\s]*\"([^\"]+)\"[\\s]*");
	regex tk_regmore("^/[\\s]*,[\\s]*");
	regex tk_endblk("^/[\\s]*\\}[\\s]*");
	regex tk_comment("^/[\\s]*#");
	regex tk_empty("^/[\\s]*$");
	
    state = START;

	fstream fin(filename, fstream::in);

    if(!fin.is_open())
        return false;

	while (getline(fin, str) && !parse_error) {
		str = "/" + str;
		//check for comment or empty line
		if(regex_match(str, tk_empty)){
			continue;
		}
		if(regex_search(str, tk_comment)){
			continue;
		}

		while(str != "/" && !parse_error){
			switch(state) {
				case START:
					if(regex_search(str, m, tk_id)) {
                        regGroup = m[1];
                        regmap = new regMap();
						str = "/" + str.substr(m[0].length());
						state = OPEN;
					} else {
						parse_error = true;
					}
					break;
				case OPEN:
					if(regex_search(str, m, tk_blk)) {
						str = "/" + str.substr(m[0].length());
						state = REGDEF;
					} else {
						parse_error = true;
					}
					break;
				case REGDEF:
					if(regex_search(str, m, tk_reg)) {
                        (*regmap)[stoi(m[1], 0, 16)] = m[2];
						str = "/" + str.substr(m[0].length());
						state = REGDEF_EXT;
					} else {
						parse_error = true;
					}
					break;
				case REGDEF_EXT:
					if(regex_search(str, m, tk_regmore)) {
						str = "/" + str.substr(m[0].length());
						state = REGDEF;
					} else {
						state = CLOSE;
					}
					break;	
				case CLOSE:
					if(regex_search(str, m, tk_endblk)) {
                        this->add(regmap, regGroup);
						str = "/" + str.substr(m[0].length());
						state = START;
					} else {
						parse_error = true;
					}
					break;
				default:
					parse_error = true;

			}
		}
		if(str == "/") parse_error = false;;
	}
	return (!parse_error && state == START);
}

void RegisterSet::print() {

    regMap* map;
    for(auto elem : regset)
    {
        cout << elem.first << ":" << endl; 
        map = elem.second;
        for(auto reg : *map)
        {
            cout << "\t0x" << hex << (int) reg.first << " = " << reg.second << endl; 
        }
    }
}