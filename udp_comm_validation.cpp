#include <iostream>
#include <algorithm>
#include <regex>
#include "udp_comm_validation.hpp"
#include <cassert>

std::string trim_command(std::string& command) {
	// check that the command is 4, 6 or 8 characters long
	// if 8, check that the first two are '0x' or '0X'
	size_t len = command.length();
	assert((len == 8) || (len == 6) || (len == 4));

	if (len == 8) {
		// check first 2
		if ((command[0] == '0') && ((command[1] == 'x') || (command[1] == 'X'))) {
			return command.substr(2, 6);
		}
		else {
			std::cerr << "unexpected command begin (expected '0x'): " << command[0] << command[1] << std::endl;
			return "";
		}
	}
	else return command;
}


std::string validate_command(const std::string& command) {
	// https://info.kmtronic.com/lan-ethernet-ip-8-channels-udp-relay-board.html
	// command is assumed to be already in upper case, 
	// and the first two characters are 'FF' or 'ff'
	// the command length is 8, 6 or 4 char.
	// if it is 8, the first two are stripped, leaving a 6 or 4 char long command
	// change for different command lengths and 
	// depending on that make sure that values provided are agreeing to expected values.
	// the return value is six char long.

	std::string out_command = "FF0000";
	size_t len = command.length();
	assert((len == 4) || (len == 6));

	if (len == 6) {
		// check the first two
		std::string buffer2 = "00";
		buffer2[0] = command[0];
		buffer2[1] = command[1];

		if (!_validate_FF(buffer2)) {
			return "";
		}
		std::string buffer = "0000";
		// check for last 4
		for (unsigned i = 2;i < len;i++) {
			buffer[i - 2] = command[i];
		}
		if (_validate_last_4_bits(buffer)) {
			for (unsigned i = 0;i < 4;++i) {
				out_command[2 + i] = buffer[i];
			}
			return out_command;
		}
		else {
			return "";
		}
	}
	else if (len == 4) {
		std::string buffer = "0000";
		// check for last 4
		for (unsigned i = 0;i < len;i++) {
			buffer[i] = command[i];
		}
		if (_validate_last_4_bits(buffer)) {
			for (unsigned i = 0;i < 4;++i) {
				out_command[2 + i] = buffer[i];
			}
			return out_command;
		}
		else {
			return "";
		}
	}
	else {
		std::cerr << "unexpected command length: " << len;
		std::cerr << " for command: " << command << std::endl;
	}

	return out_command;
}

bool _validate_FF(const std::string& buffer) {
	//assumes buffer to be two digits long
	if (buffer.length() != 2) return false;
	if (!(buffer[0] == 'F') || !(buffer[1] == 'F')) {
		return false;
	}
	return true;
}


bool _validate_last_4_bits(const std::string& buffer) {
	//

	/*
	- assumes buffer to be 4 digits long and actually containing the command's value
	- return true, unless (...)
	- https://info.kmtronic.com/lan-ethernet-ip-8-channels-udp-relay-board.html
	- (accessed June 2024)
	*
	UDP Commands ( ASCII Encoded ):

	FF0000 - Status Read command

	FF0100 - Relay 1 OFF command
	FF0101 - Relay 1 ON command

	FF0200 - Relay 2 OFF command
	FF0201 - Relay 2 ON command

	FF0300 - Relay 3 OFF command
	FF0301 - Relay 3 ON command

	FF0400 - Relay 4 OFF command
	FF0401 - Relay 4 ON command

	FF0500 - Relay 5 OFF command
	FF0501 - Relay 5 ON command

	FF0600 - Relay 6 OFF command
	FF0601 - Relay 6 ON command

	FF0700 - Relay 7 OFF command
	FF0701 - Relay 7 ON command

	FF0800 - Relay 8 OFF command
	FF0801 - Relay 8 ON command



	FFE000 - All relays OFF command
	FFE0FF - All relays ON command
	*/
	if (buffer.length() != 4) {
		std::cerr << "unexpected command length: " << buffer.length();
		std::cerr << "and command: " << buffer << std::endl;
		return false;
	}

	if ((buffer[0] == '0')) {
		std::regex rgx_digit1("[0-8]"); //of digit 0 to 3
		std::regex rgx_digit3("[01]");
		std::string strBuffer1(1, buffer[1]);
		if (!std::regex_search(strBuffer1, rgx_digit1)) {
			return false;
		}
		if (buffer[2] != '0') {
			return false;
		}
		std::string strBuffer3(1, buffer[3]);
		if (!std::regex_search(strBuffer3, rgx_digit3)) {
			return false;
		}


	}
	else if ((buffer[0] == 'E')) {
		if (!(buffer[1] == '0')) {
			return false;
		}
		std::regex rgx_digit2("[0F]");
		std::string strBuffer2(1, buffer[2]);
		if (!std::regex_search(strBuffer2, rgx_digit2)) {
			return false;
		}
		std::regex rgx_digit3("[01F]");
		std::string strBuffer3(1, buffer[3]);
		if (!std::regex_search(strBuffer3, rgx_digit3)) {
			return false;
		}
		return true;
	}
	else return false;

	return true;
}
