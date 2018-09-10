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

int _cdecl main(int argc, char *argv[]) {
	WSADATA wsa_data;
	auto connect_socket = INVALID_SOCKET;

	struct addrinfo *result = NULL, hints{};

	const auto recvbuflen = DEFAULT_BUFLEN;

	char *sendbuf = "this is a test";
	char recvbuf[DEFAULT_BUFLEN];

	// Initialize Winsock
	auto i_result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (i_result != 0) {
		printf("WSAStartup failed: %d\n", i_result);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	i_result = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	if (i_result != 0) {
		printf("getaddrinfo failed: %d\n", i_result);
		WSACleanup();
		return 1;
	}

	// Attempt to connect to the first address returned by
	// the call to getaddrinfo
	struct addrinfo* ptr = result;

	// Create a SOCKET for connecting to server
	connect_socket = socket(ptr->ai_family, ptr->ai_socktype,
		ptr->ai_protocol);

	// Check for errors to ensure that the ConnectSocket is a 
	// valid socket.
	if (connect_socket == INVALID_SOCKET) {
		printf("Error at socket(): %d\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Connect to server.
	i_result = connect(connect_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (i_result == SOCKET_ERROR) {
		closesocket(connect_socket);
		connect_socket = INVALID_SOCKET;
	}

	// Should really try the next address returned by getaddrinfo
	// if the connect call failed
	// But for this simple example we just free the resources
	// returned by getaddrinfo and print an error message

	freeaddrinfo(result);

	if (connect_socket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	// Send an initial buffer
	i_result = send(connect_socket, sendbuf, (int)strlen(sendbuf), 0);
	if (i_result == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(connect_socket);
		WSACleanup();
		return 1;
	}

	printf("Bytes Sent: %d\n", i_result);

	// shutdown the connection for sending since no more data will be sent
	// the client can still use the ConnectSocket for receiving data
	i_result = shutdown(connect_socket, SD_SEND);
	if (i_result == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(connect_socket);
		WSACleanup();
		return 1;
	}

	// Receive data until the server closes the connection
	do {
		i_result = recv(connect_socket, recvbuf, recvbuflen, 0);
		if (i_result > 0)
			printf("Bytes received: %d\n", i_result);
		else if (i_result == 0)
			printf("Connection closed\n");
		else
			printf("recv failed: %d\n", WSAGetLastError());
	} while (i_result > 0);

	// cleanup
	closesocket(connect_socket);
	WSACleanup();

	return i_result;
}