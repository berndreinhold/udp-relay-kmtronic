#ifndef UDP_COMM
#define UDP_COMM

#include <stdio.h>
#include <string>
#include <Ws2tcpip.h>
#include <WinSock2.h>

// great resources:
// https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-sendto

#define WIN32_LEAN_AND_MEAN


#pragma comment(lib, "Ws2_32.lib")

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
		Comm(const std::string& destination_address, const int destination_port);

		/*** @brief Construct a Comm object, Set source port
		*
		* @param destination_address: IP address of receiver
		* @param destination_port: Destination port
		* @param source port
		*/
		Comm(const std::string& destination_address, const int destination_port, const int source_port);
		~Comm();
		void udp_send(const std::string msg) const;
		char* udp_receive();
		int post_process_udp_receive_ret_value(char* ret); 
	};
};

#endif
