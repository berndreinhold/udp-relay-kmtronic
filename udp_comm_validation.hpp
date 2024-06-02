#ifndef UDP_COMM_VALIDATION
#define UDP_COMM_VALIDATION


#include <string>

char* validate_command(const std::string& command_);
bool _validate_FF(const std::string& buffer);
bool _validate_4(const std::string& buffer);

#endif