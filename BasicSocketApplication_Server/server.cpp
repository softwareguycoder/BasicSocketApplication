// This is a sample from the Microsoft site that I stepped throught the tutorial with, so I could
// brush up on Windows Sockets programming concepts.
//

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdio>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

int _cdecl main() {
	WSADATA wsa_data;

	auto listen_socket = INVALID_SOCKET;
	auto client_socket = INVALID_SOCKET;

	struct addrinfo *result = nullptr;
	struct addrinfo hints {};

	auto i_send_result = 0;
	char recvbuf[DEFAULT_BUFLEN];
	const auto recvbuflen = DEFAULT_BUFLEN;

	// Initialize Winsock
	auto i_result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (i_result != 0) {
		printf("WSAStartup failed: %d\n", i_result);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	i_result = getaddrinfo(nullptr, DEFAULT_PORT, &hints, &result);
	if (i_result != 0) {
		printf("getaddrinfo failed with error: %d\n", i_result);
		WSACleanup();
		return 1;
	}

	// Create a SOCKET for connecting to server
	listen_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (listen_socket == INVALID_SOCKET) {
		printf("socket failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	i_result = bind(listen_socket, result->ai_addr, (int)result->ai_addrlen);
	if (i_result == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(listen_socket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	i_result = listen(listen_socket, SOMAXCONN);
	if (i_result == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(listen_socket);
		WSACleanup();
		return 1;
	}

	// Accept a client socket
	client_socket = accept(listen_socket, nullptr, nullptr);
	if (client_socket == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		closesocket(listen_socket);
		WSACleanup();
		return 1;
	}

	// No longer need server socket
	closesocket(listen_socket);

	// Receive until the peer shuts down the connection
	do {
		i_result = recv(client_socket, recvbuf, recvbuflen, 0);
		if (i_result > 0) {
			printf("Bytes received: %d\n", i_result);

			// Echo the buffer back to the sender
			i_send_result = send(client_socket, recvbuf, i_result, 0);
			if (i_send_result == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(client_socket);
				WSACleanup();
				return 1;
			}
			printf("Bytes sent: %d\n", i_send_result);
		}
		else if (i_result == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(client_socket);
			WSACleanup();
			return 1;
		}
	} while (i_result > 0);

	// shutdown the connection since we're done
	i_result = shutdown(client_socket, SD_SEND);
	if (i_result == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(client_socket);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(client_socket);
	WSACleanup();

	return i_result;
}