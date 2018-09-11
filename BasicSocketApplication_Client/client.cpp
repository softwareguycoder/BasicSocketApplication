// This is a sample from the Microsoft site that I stepped throught the tutorial with, so I could
// brush up on Windows Sockets programming concepts.
//

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdlib>
#include <cstdio>

extern "C" {
#include "../../JQR.Debug.Core/JQR.Debug.Core/JQR.Debug.Core.h"
#include "../../JQR.Inetsock.Core/JQR.Inetsock.Core/SocketCommon.h"
}

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int _cdecl main(const int argc, char *argv[]) {
	log_debug("In main");

	// Default return value in case something goes sideways
	int nResult = 1;

	WSADATA wsaData;
	auto ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = nullptr,
		hints{};
	const auto sendbuf = "this is a test";
	char recvbuf[DEFAULT_BUFLEN];
	const auto recvbuflen = DEFAULT_BUFLEN;

	// Validate the parameters
	if (argc != 2) {
		printf("usage: Client.exe servername\n");
		return 1;
	}

	// Initialize the Winsock (Windows Sockets) stack
	log_debug("main: Initializing the Winsock stack...");

	if (!InitializeWinsock(&wsaData, &nResult))
		return nResult;

	log_debug("main: Winsock stack has been initialized.");

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	nResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	if (nResult != 0) {
		printf("getaddrinfo failed with error: %d\n", nResult);
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
		nResult = connect(ConnectSocket, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen));
		if (nResult == SOCKET_ERROR) {
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
	nResult = send(ConnectSocket, sendbuf, static_cast<int>(strlen(sendbuf)), 0);
	if (nResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	printf("Bytes Sent: %d\n", nResult);

	// shutdown the connection since no more data will be sent
	nResult = shutdown(ConnectSocket, SD_SEND);
	if (nResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// Receive until the peer closes the connection
	do {
		nResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (nResult > 0)
			printf("Bytes received: %d\n", nResult);
		else if (nResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed with error: %d\n", WSAGetLastError());
	} while (nResult > 0);

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();

	return nResult;
}