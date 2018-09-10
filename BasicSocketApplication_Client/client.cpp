#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdlib>
#include <cstdio>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int _cdecl main(int argc, char *argv[]) {
	WSADATA wsaData;
	auto ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = nullptr,
	                hints{};
	const char *sendbuf = "this is a test";
	char recvbuf[DEFAULT_BUFLEN];
	const auto recvbuflen = DEFAULT_BUFLEN;

	// Validate the parameters
	if (argc != 2) {
		printf("usage: %s server-name\n", argv[0]);
		return 1;
	}

	// Initialize Winsock
	auto i_result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (i_result != 0) {
		printf("WSAStartup failed with error: %d\n", i_result);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	i_result = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	if (i_result != 0) {
		printf("getaddrinfo failed with error: %d\n", i_result);
		WSACleanup();
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	for (auto ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %d\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		// Connect to server.
		i_result = connect(ConnectSocket, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen));
		if (i_result == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	// Send an initial buffer
	i_result = send(ConnectSocket, sendbuf, static_cast<int>(strlen(sendbuf)), 0);
	if (i_result == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	printf("Bytes Sent: %d\n", i_result);

	// shutdown the connection since no more data will be sent
	i_result = shutdown(ConnectSocket, SD_SEND);
	if (i_result == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// Receive until the peer closes the connection
	do {

		i_result = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (i_result > 0)
			printf("Bytes received: %d\n", i_result);
		else if (i_result == 0)
			printf("Connection closed\n");
		else
			printf("recv failed with error: %d\n", WSAGetLastError());

	} while (i_result > 0);

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();

	return i_result;
}