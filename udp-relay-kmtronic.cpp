#include <iostream>
#include "udp_comm.hpp"
#include "udp_comm_validation.hpp"
#include "string"
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <cassert>

void printUsage(const char* programName) {
	std::cout << "Usage: " << programName << " --config <config_file>\n";
	std::cout << "  e.g. " << programName << " --config config.txt\n";
	std::cout << "       " << programName << " --url:port <url>:<port> --command <cmd>\n";
	std::cout << "  e.g. " << programName << " --url:port 192.168.0.0:54321 --command FF0000\n";
	std::cout << std::endl;
}

bool readConfigFile(const std::string& configFile, std::string& url_port, std::string& command) {
	// fill url_port and command from configFile
	std::ifstream file(configFile);
	if (!file.is_open()) {
		std::cerr << "Could not open config file: " << configFile << "\n";
		return false;
	}

	if (!std::getline(file, url_port)) {
		std::cerr << "Could not read 'url:port' from config file." << configFile << "\n";
		return false;
	}

	if (!std::getline(file, command)) {
		std::cerr << "Could not read 'command' from config file." << configFile << "\n";
		return false;
	}

	file.close();
	return true;
}




int split_url_port(std::string input, std::string& url, int& port, char delimiter = ':') {
	// Create a stringstream object from the input string
	std::stringstream ss(input);

	std::string part2;
	// Use getline to split the string
	if (std::getline(ss, url, delimiter) && std::getline(ss, part2)) {
		port = atoi(part2.c_str());
		return 0;
	} else {
		std::cerr << "Failed to split the string " << input << " by delimiter '" << delimiter << "'." << std::endl;
	}

	return -1;
}

int arg_parse(int argc, char* argv[], std::string& url, int& port, std::string& command) {
	std::string url_port = "";
	std::string configFile = "";

	char buffer[10] = "";
	if (argc == 5) {
		// Check for --url:port and --command
		for (int i = 1; i < argc; i += 2) {
			if (strcmp(argv[i], "--url_port") == 0) {
				url_port = argv[i + 1];
			}
			else if (strcmp(argv[i], "--command") == 0) {
				command = argv[i + 1];
			}
			else {
				printUsage(argv[0]);
				return -1;
			}
		}
	}
	else if (argc == 3) {
		// Check for --config
		if (strcmp(argv[1], "--config") == 0 || strcmp(argv[1], "-c") == 0) {
			configFile = argv[2];
			if (!readConfigFile(configFile, url_port, command)) {
				return -1;
			}
		}
		else {
			printUsage(argv[0]);
			return -1;
		}
	}
	else {
		printUsage(argv[0]);
		return -1;
	}

	command = trim_command(command); // length is either 4 or 6 after trim_command()
	//switch to upper:
	std::transform(command.begin(), command.end(), command.begin(),
		[](unsigned char c) { return std::toupper(c); });

	// now url_port and command are filled
	if (configFile != "") {
		std::cout << "config file: " << configFile << std::endl;
	}

	split_url_port(url_port, url, port);

	return 0;
}




int main(int argc, char* argv[]) {
	std::string url;
	int port;
	std::string command;
	if (arg_parse(argc, argv, url, port, command) < 0) {
		return -1;
	}

	//command is now guaranteed to have length 6
	std::cout << "'url:port': " << url << ":" << port << std::endl;
	std::cout << "command (length: " << command.length() << "): " << command << std::endl;

	
	udp_comm::Comm communicator(url, port);

	if (!(command == "FF0000")) {
		// if you just want the status, you get it below already.
		std::string msg = validate_command(command);
		if (msg=="") {
			std::cerr << "'" << command << "' has not the right format!" << std::endl;
			std::cerr << "see https://info.kmtronic.com/lan-ethernet-ip-8-channels-udp-relay-board.html" << std::endl;
			std::cerr << "for details (accessed June 2024)." << std::endl;
			return -1;
		}
		
		communicator.udp_send(msg);
		for (int i = 0; i < msg.length(); ++i) {
			std::cout << msg[i];
		}
		std::cout << " (" << (msg.length()) << " bytes) sent to " << url << ":" << port << std::endl;
	}


	// get status
	std::string msg_get_status = "FF0000";
	communicator.udp_send(msg_get_status);
	std::string ret = communicator.udp_receive();
	int ret_value = communicator.post_process_udp_receive_ret_value(ret.c_str());
	std::cout << "ret_value (UDP relay): " << ret_value << std::endl;

	return ret_value;
}