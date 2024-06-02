#include "udp_comm_validation.hpp"
#include <cassert>
#include <iostream>
#include <vector>

void test_validate_FF() {
	std::string buffer = "FF";
	assert(_validate_FF(buffer) == true);
	buffer = "00";
	assert(_validate_FF(buffer) == false);
	buffer = "0F";
	assert(_validate_FF(buffer) == false);
	buffer = "F0";
	assert(_validate_FF(buffer) == false);
	buffer = "FF00";
	assert(_validate_FF(buffer) == false);
	buffer = "FF0";
	assert(_validate_FF(buffer) == false);
}


void test_validate_command() {
	std::vector<std::string> pass_commands = {
		"FF0000",
		"0000"
	};
	
	for (auto command : pass_commands) {
		std::string outCommand = validate_command(command);
		assert(outCommand.length() == 6);
		assert(outCommand[0] == 'F');
		assert(outCommand[1] == 'F');
		assert(outCommand[2] == '0');
		assert(outCommand[3] == '0');
		assert(outCommand[4] == '0');
		assert(outCommand[5] == '0');
	}

	std::string command = "0xFF0000";
	std::string outCommand = trim_command(command);
	assert(outCommand.length() == 6);
	assert(outCommand == "FF0000");
	

	std::vector<std::string> commands = {
		"FF00", 
		"FF0a00",
		"FE0000",
		"0xFF00" };
	for(auto command : commands) {
		outCommand = validate_command(command);
		assert(outCommand == "");
	}
	
	// should raise value error: "FF000",

}





int main(int argc, char* argv[]) {
	test_validate_command();
	test_validate_FF();
	return 0;
}