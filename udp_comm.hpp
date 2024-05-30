
#ifndef UDP_COM
#define UDP_COM

#include <stdio.h>
#include <ws2tcpip.h>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

namespace udp_comm
{
	class Comm
	{
	private:
		SOCKET sock_;
		struct sockaddr_in addr_;

	public:
		/*** @brief Construct a communication object capable of sending and receiving udp messages.
		*
		* @param destination_address: IP address of receiver
		* @param destination_port: Destination port
		*/
		Comm(const std::string& destination_address, const int destination_port)
		{

			WSADATA wsaData;
			int ret = WSAStartup(MAKEWORD(2, 0), &wsaData);
			if (ret != 0)
			{
				int err = WSAGetLastError();
				std::cerr << "WSAStartup failed with err: " << err << std::endl;
			}

			// Generate socket
			// AF_INET: IPV4, SOCK_DGRAM: UDP, 0: protocol is automoatically chosen
			sock_ = socket(AF_INET, SOCK_DGRAM, 0);
			if (sock_ < 0)
			{
				std::cerr << "[Error] Socket initialize error." << std::endl;
				exit(0);
			}

			// IP/port setting
			addr_.sin_family = AF_INET;
			addr_.sin_port = htons(destination_port);
			addr_.sin_addr.S_un.S_addr = INADDR_ANY;
			InetPton(AF_INET, destination_address.c_str(), &addr_.sin_addr.s_addr);

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
		Comm(const std::string& destination_address, const int destination_port, const int source_port)
		{

			WSADATA wsaData;
			int ret = WSAStartup(MAKEWORD(2, 0), &wsaData);
			if (ret != 0)
			{
				int err = WSAGetLastError();
				std::cerr << "WSAStartup failed with err: " << err << std::endl;
			}
			// Generate socket
			// AF_INET: IPV4, SOCK_DGRAM: UDP, 0: protocol is automoatically chosen
			sock_ = socket(AF_INET, SOCK_DGRAM, 0);
			if (sock_ < 0)
			{
				std::cerr << "[Error] Socket initialize error." << std::endl;
				exit(0);
			}

			// IP/port setting
			addr_.sin_family = AF_INET;
			addr_.sin_port = htons(destination_port);
			addr_.sin_addr.S_un.S_addr = INADDR_ANY;
			InetPton(AF_INET, destination_address.c_str(), &addr_.sin_addr.s_addr);

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

		~Comm()
		{
			closesocket(sock_);
			WSACleanup();
		}

		template <typename MSG_TYPE>
		void udp_send(const MSG_TYPE& msg) const
		{
			if (sendto(sock_, (char*)&msg, sizeof(msg), 0, (struct sockaddr*)&addr_, sizeof(addr_)) < 0)
			{
				std::cerr << "[Error] UDP send Error." << std::endl;
				exit(1);
			}
		}


		int udp_receive() {
			fd_set readfds;
			struct timeval tv;

			// Set up the file descriptor set. 
			FD_ZERO(&readfds);
			FD_SET(sock_, &readfds);

			// Set up the timeout. 
			tv.tv_sec = 5;  // 5 second timeout 
			tv.tv_usec = 0;

			char buffer[1024];
			int result = select(sock_ + 1, &readfds, NULL, NULL, &tv);
			if (result == SOCKET_ERROR) {
				printf("select failed with error: %d\n", WSAGetLastError());
				closesocket(sock_);
				WSACleanup();
				return 1;
			}
			else if (result == 0) {
				printf("Timeout occurred! No data received.\n");
			}
			else {
				if (FD_ISSET(sock_, &readfds)) {
					// Data is available to read 
					socklen_t addrLen = sizeof(addr_);
					int n = recvfrom(sock_, buffer, sizeof(buffer), 0, (struct sockaddr*)&addr_, &addrLen);
					if (n == SOCKET_ERROR) {
						printf("recvfrom failed with error: %d\n", WSAGetLastError());
					}
					else {
						//n is at most 8
						buffer[n] = '\0'; // Null-terminate the received data 
						printf("Received message from device: %s\n", buffer);
					}
				}
			}
		}
	};
};

#endif
