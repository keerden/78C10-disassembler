#ifndef UTILS_H
#define UTILS_H
 
#include <string>
#include <vector>
#include <cstdint>

std::vector<std::string>* split_string(std::string  input, std::string delim);
std::vector<std::string>* split_string(std::string  input, std::string delim, int max);
std::string trim_string(std::string input);
std::string triml_string(std::string input);
std::string trimr_string(std::string input);

uint8_t shiftMask(uint8_t mask, uint8_t byte);
uint16_t shiftMask(uint16_t mask, uint16_t byte);
int8_t shiftMaskSigned(uint8_t mask, uint8_t byte);
int16_t shiftMaskSigned(uint16_t mask, uint16_t byte);

#endif