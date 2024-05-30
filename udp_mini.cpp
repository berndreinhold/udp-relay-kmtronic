#include <iostream>
#include "udp_comm.hpp"
#include "string"
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <algorithm>


void printUsage(const char* programName) {
	std::cout << "Usage: " << programName << " --config <config_file>\n";
	std::cout << "  e.g. " << programName << " --config config.txt\n";
	std::cout << "       " << programName << " --url:port <url>:<port> --command <cmd>\n";
	std::cout << "  e.g. " << programName << " --url:port 192.168.0.0:54321 --command FF0000\n";
	std::cout << std::endl;
}

bool readConfigFile(const std::string& configFile, std::string& url_port, std::string& command_) {
	// fill url_port and command_ from configFile
	std::ifstream file(configFile);
	if (!file.is_open()) {
		std::cerr << "Could not open config file: " << configFile << "\n";
		return false;
	}

	if (!std::getline(file, url_port)) {
		std::cerr << "Could not read 'url:port' from config file." << configFile << "\n";
		return false;
	}

	if (!std::getline(file, command_)) {
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
int main(int argc, char* argv[]) {
	std::string url_port, command_;
	std::string configFile="";

	char buffer[10];
	if (argc == 5) {
		// Check for --url:port and --command
		for (int i = 1; i < argc; i += 2) {
			if (strcmp(argv[i], "--url_port") == 0) {
				url_port = argv[i + 1];
			}
			else if (strcmp(argv[i], "--command") == 0) {
				command_ = argv[i + 1];
			}
			else {
				printUsage(argv[0]);
				return -1;
			}
		}
	} else if (argc == 3) {
		// Check for --config
		if (strcmp(argv[1], "--config") == 0 || strcmp(argv[1], "-c") == 0) {
			configFile = argv[2];
			if (!readConfigFile(configFile, url_port, command_)) {
				return -1;
			}
		}
		else {
			printUsage(argv[0]);
			return -1;
		}
	} else {
		printUsage(argv[0]);
		return -1;
	}

	// now url_port and command_ are filled
	if (configFile != "") {
		std::cout << "config file: " << configFile << std::endl;
	}
	
	std::string url = "xxx.xxx.xxx.xxx";
	int port = 42;
	split_url_port(url_port, url, port);

	//switch to upper:
	std::transform(command_.begin(), command_.end(), command_.begin(),
		[](unsigned char c) { return std::toupper(c); });

	std::cout << "'url:port': " << url << ":" << port << std::endl;
	std::cout << "command: " << command_ << std::endl;


	const int msg_length = 6;
	udp_comm::Comm communicator(url, port);

	if (!(command_ == "FF0000")) {
		// if you just want the status, you get it below already.

		//validate_command
		char msg[msg_length] = { 'F', 'F', '0', '3', '0', '1' };
		//char msg[msg_length] = validate_command(command_);
		communicator.udp_send(msg);
		for (int i = 0; i < msg_length; ++i) {
			std::cout << msg[i];
		}
		std::cout << " (" << sizeof(msg) << " bytes) sent to " << url << ":" << port << std::endl;
	}


	char msg_get_status[msg_length] = { 'F', 'F', '0', '0', '0', '0' };
	communicator.udp_send(msg_get_status);
	//char status_msg[1024];
	//int ret = sender.udp_receive(&status_msg);
	char *ret = communicator.udp_receive();
	int ret_value = communicator.post_process_udp_receive_ret_value(ret);

	return ret_value;
}