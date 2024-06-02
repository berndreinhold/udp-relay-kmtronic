#ifndef UDP_COMM_VALIDATION
#define UDP_COMM_VALIDATION


#include <string>

std::string validate_command(const std::string& command);
std::string trim_command(std::string& command);
bool _validate_FF(const std::string& buffer);
bool _validate_last_4_bits(const std::string& buffer);

#endif