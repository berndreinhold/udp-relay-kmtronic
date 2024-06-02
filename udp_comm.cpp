#include "udp_comm.hpp"
#include <iostream>
#include <string>

using namespace udp_comm;

Comm::Comm(const std::string& destination_address, const int destination_port)
		{

			WSADATA wsaData;
			int ret = WSAStartup(MAKEWORD(2, 0), &wsaData);
			if (ret != NO_ERROR)
			{
				int err = WSAGetLastError();
				std::cerr << "WSAStartup failed with err: " << err << std::endl;
			}

			// Generate socket
			// AF_INET: IPV4, SOCK_DGRAM: UDP, 0: protocol is automoatically chosen
			sock_ = socket(AF_INET, SOCK_DGRAM, 0);
			if (sock_ == INVALID_SOCKET)
			{
				std::cerr << "[Error] Socket initialize error." << std::endl;
				exit(0);
			}

			// IP/port setting
			addr_.sin_family = AF_INET;
			addr_.sin_port = htons(destination_port);
			addr_.sin_addr.S_un.S_addr = inet_addr(destination_address.c_str());

			// Non blocking setting
			u_long val = 1; // 0: blocking, 1: non-blocking
			ioctlsocket(sock_, FIONBIO, &val);
		}

		/*** @brief Construct a Comm object, Set source port
		*
		* @param destination_address: IP address of receiver
		* @param destination_port: Destination port
		* @param source port
		*/
Comm::Comm(const std::string& destination_address, const int destination_port, const int source_port)
		{

			WSADATA wsaData;
			int ret = WSAStartup(MAKEWORD(2, 0), &wsaData);
			if (ret != NO_ERROR)
			{
				int err = WSAGetLastError();
				std::cerr << "WSAStartup failed with err: " << err << std::endl;
			}
			// Generate socket
			// AF_INET: IPV4, SOCK_DGRAM: UDP, 0: protocol is automoatically chosen
			sock_ = socket(AF_INET, SOCK_DGRAM, 0);
			if (sock_ == INVALID_SOCKET)
			{
				std::cerr << "[Error] Socket initialize error." << std::endl;
				exit(0);
			}

			// IP/port setting
			addr_.sin_family = AF_INET;
			addr_.sin_port = htons(destination_port);
			addr_.sin_addr.S_un.S_addr = inet_addr(destination_address.c_str());


			// set source port
			struct sockaddr_in source_addr;
			source_addr.sin_family = AF_INET;
			source_addr.sin_addr.S_un.S_addr = INADDR_ANY;
			source_addr.sin_port = htons(source_port);
			// InetPton(AF_INET, destination_address.c_str(), &source_addr.sin_addr.s_addr);

			// Non blocking setting
			u_long val = 1; // 0: blocking, 1: non-blocking
			ioctlsocket(sock_, FIONBIO, &val);
		}

Comm::~Comm()
		{
			closesocket(sock_);
			WSACleanup();
		}


void Comm::udp_send(const std::string msg) const
{
	int n = sendto(sock_, (char*)msg.data(), sizeof(msg.data()), 0, (SOCKADDR*)&addr_, sizeof(addr_));
	if (n == SOCKET_ERROR)
	{
		std::cerr << "[Error] UDP send Error. " << WSAGetLastError() << std::endl;
		WSACleanup();
		exit(1);
	}
}

std::string Comm::udp_receive() {
	// from: https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-recvfrom
	// TODO: might be interesting as well
			fd_set readfds;
			struct timeval tv;

			// Set up the file descriptor set. 
			FD_ZERO(&readfds);
			FD_SET(sock_, &readfds);

			// Set up the timeout. 
			tv.tv_sec = 5;  // 5 second timeout 
			tv.tv_usec = 0;

			const size_t buffer_size = 1024;
			char buffer[buffer_size];
			
			int result = select(sock_ + 1, &readfds, NULL, NULL, &tv);
			if (result == SOCKET_ERROR) {
				printf("select failed with error: %d\n", WSAGetLastError());
				closesocket(sock_);
				WSACleanup();
				sprintf(buffer, "%d", result);
				std::string outBuffer(buffer);
				return outBuffer;
			}
			else if (result == 0) {
				printf("Timeout occurred! No data received.\n");
				sprintf(buffer, "%d", result);
				std::string outBuffer(buffer);
				return outBuffer;
			}
			else {
				if (FD_ISSET(sock_, &readfds)) {
					// Data is available to read 
					socklen_t addrLen = sizeof(addr_);
					int n = recvfrom(sock_, buffer, sizeof(buffer), 0, (SOCKADDR*)&addr_, &addrLen);
					if (n == SOCKET_ERROR) {
						printf("recvfrom failed with error: %d\n", WSAGetLastError());
					}
					else {
						//n is at most 8
						int index_ = min(n, buffer_size - 1);
						buffer[index_] = '\0'; // Null-terminate the received data 
						//printf("Received message from device: %s\n", buffer);
					}
					std::string outBuffer(buffer);
					return outBuffer;
				}
				else {
					printf("No data received.\n");
					return "";
				}
			}
		}

int Comm::post_process_udp_receive_ret_value(std::string ret) {
	//post-process return value from udp_receive():
	//ret is a bit number encoding relay states in reverse order - reorder now 
	std::string strRet(ret);
	std::string strRet_reverse(ret);
	size_t n = strRet.length();
	for (unsigned i = 0;i < n;++i) {
		strRet_reverse[n - i - 1] = strRet[i];
	}
	int ret_value = strtol(strRet_reverse.c_str(), NULL, 2); //interpret as binary number

	return ret_value;
}
