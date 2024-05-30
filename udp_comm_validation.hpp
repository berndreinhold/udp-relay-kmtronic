#ifndef UDP_COMM_VALIDATION
#define UDP_COMM_VALIDATION


#include <algorithm>

static char* validate_command(const std::string& command_, const int length = 6) {
	// https://info.kmtronic.com/lan-ethernet-ip-8-channels-udp-relay-board.html
	// change to upper case, change for different command lengths and 
	// depending on that make sure that values provided are agreeing to expected values.
	std::transform(command_.begin(), command_.end(), command_.begin(),
		[](unsigned char c) { return std::toupper(c); });


	char* arrCommand = new char[length + 1]; // null terminated
	arrCommand[0] = 'F';
	arrCommand[1] = 'F';



	if (command_.length() == 8) {

		// check first 2
		if (!(command_[0] == '0') || !(command_[1] == 'x')) {
			std::cerr << "unexpected command begin (expected '0x'): " << command_[0] << command_[1] << std::endl;
			return "";
		}

		//check next 2 for 'FF'
		std::string buffer2 = "00";
		buffer2[0] = command_[2];
		buffer2[1] = command_[3];

		if (!_validate_FF(buffer2)) {
			return "";
		}

		std::string buffer = "0000";
		// check for last 4
		for (int i = 4;i < command_.length();i++) {
			buffer[i - 4] = command_[i];
		}
		if (!_validate_4(buffer)) return "";
	}

	std::string buffer2 = "0000";
	if (command_.length() == 6) {
		// check the first two
		std::string buffer2 = "00";
		buffer2[0] = command_[0];
		buffer2[1] = command_[1];

		if (!_validate_FF(buffer2)) {
			return "";
		}
		std::string buffer = "0000";
		// check for last 4
		for (int i = 2;i < command_.length();i++) {
			buffer[i - 2] = command_[i];
		}
		if (!_validate_4(buffer)) return "";
	}
	else if (command_.length() == 4) {
		std::string buffer = "0000";
		// check for last 4
		for (int i = 0;i < command_.length();i++) {
			buffer[i] = command_[i];
		}
		if (!_validate_4(buffer)) return "";
	}
	else {
		std::cerr << "unexpected command length: " << command_.length();
		std::cerr << "and command: " << command_ << std::endl;
	}

}

static bool _validate_FF(const std::string& buffer) {
	//assumes buffer to be two digits long
	if (buffer.length() != 2) return false;
	if (!(buffer[0] == 'F') || !(buffer[1] == 'F')) {
		return false;
	}
	return true;
}


static bool _validate_4(const std::string& buffer) {
	//assumes buffer to be 4 digits long and actually containing the command's value
	//https://info.kmtronic.com/lan-ethernet-ip-8-channels-udp-relay-board.html
	/*
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

	if (!(buffer[0] == '0')) {
		if ()


	}
	else if (!(buffer[0] == 'E')) {

		return false;
	}
	if (!(buffer[0] == '0') && !(buffer[0] == 'E')) {
		return false;
	}
	return true;
}

