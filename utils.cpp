#include <string>
#include <iostream>
#include <vector>
#include "utils.h"

using namespace std;

vector<string>* split_string(string input, string delim, int max)
{
    size_t pos;
    size_t strt = 0;
    int counter = 0;
    vector<string>* result = new vector<string>();
    while(!((pos = input.find(delim, strt)) == input.npos)) {
        result->push_back(input.substr(strt, pos - strt));
        strt = pos + delim.length();
        counter++;
        if(counter == max)
            break;
    }
    result->push_back(input.substr(strt));
    return result;
}

vector<string>* split_string(string input, string delim) {
    return split_string(input, delim, 0);
}
string triml_string(string input) {
    char c;
    size_t i = 0;

    while(i < input.length()) {
        c = input.at(i);
        if(c != ' ' && c != '\t')
            break;
        i++;
    }
    return (i < input.length())?input.substr(i):"";
}
string trimr_string(string input) {
    char c;
    int i = input.length();

    while(--i != -1) {
        c = input.at(i);
        if(c != ' ' && c != '\t')
            break;
    }
    if(i == -1)
        i = input.length();
    return input.substr(0,i+1);
}
string trim_string(string input) {
    return triml_string(trimr_string(input));
}


uint8_t shiftMask(uint8_t mask, uint8_t byte)
{
	int i, shifts;
	uint8_t retval = 0;
	shifts = 0;
	for(i = 0; i < 8; i++)
	{
		if(mask & (1 << i))
		{
			retval |= (byte & (1 << i)) >> shifts;
		} else
			shifts++;
	}
	return retval;
	
}

uint16_t shiftMask(uint16_t mask, uint16_t byte)
{
	int i, shifts;
	uint16_t retval = 0;
	shifts = 0;
	for(i = 0; i < 16; i++)
	{
		if(mask & (1 << i))
		{
			retval |= (byte & (1 << i)) >> shifts;
		} else
			shifts++;
	}
	return retval;
	
}


int8_t shiftMaskSigned(uint8_t mask, uint8_t byte)
{
	int i, shifts;
	int8_t retval = 0;
	shifts = 0;
	for(i = 0; i < 8; i++)
	{
		if(mask & (1 << i))
		{
			retval |= (byte & (1 << i)) >> shifts;
		} else
			shifts++;  
	}
	
	if(retval & (1 << (7-shifts)))
		for(i = 7-shifts; i < 8; i++)
			retval |=  (1 << i);	
	
	return retval;	
}
int16_t shiftMaskSigned(uint16_t mask, uint16_t byte)
{
	int i, shifts;
	int16_t retval = 0;
	shifts = 0;
	for(i = 0; i < 16; i++)
	{
		if(mask & (1 << i))
		{
			retval |= (byte & (1 << i)) >> shifts;
		} else
			shifts++;
	}
	
	if(retval & (1 << (15-shifts)))
		for(i = 15-shifts; i < 16; i++)
			retval |=  (1 << i);	
	
	return retval;	
}
