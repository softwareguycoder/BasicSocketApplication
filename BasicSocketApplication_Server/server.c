// This is a sample from the Microsoft site that I stepped throught the tutorial with, so I could
// brush up on Windows Sockets programming concepts.
//

#include "ServerSocket.h"

#include "../../JQR.Debug.Core/JQR.Debug.Core/JQR.Debug.Core.h"

#pragma comment(lib, "Ws2_32.lib")

int _cdecl main() {
	log_debug("In main");

	// Default return value in case something goes sideways
	int nResult = 1;

	WSADATA wsaData;
	ZeroMemory(&wsaData, sizeof(wsaData));

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	PADDRINFOA pAddrInfo = NULL;

	char recvbuf[DEFAULT_BUFLEN];

	log_debug("main: Initializing the Winsock stack...");

	if (!InitializeWinsock(&wsaData, &nResult))
		return nResult;

	log_debug("main: Winsock stack has been initialized.");

	log_debug("main: Attempting to resolve an IP address for the server...");

	if (!ResolveServerAddress(DEFAULT_PORT, &pAddrInfo, &nResult))
		return nResult;

	log_debug("main: The server's IP address has been resolved.");

	log_debug("main: Attempting to create the server socket endpoint...");

	if (!CreateServerSocket(pAddrInfo, &ListenSocket, &nResult))
		return nResult;

	log_debug("main: Server socket endpoint successfully created.");

	log_debug("main: Attempting to bind the server socket to the endpoint...");

	if (!BindServerSocket(&ListenSocket, pAddrInfo, &nResult))
		return nResult;

	log_debug("main: The server listening socket is now bound to the endpoint.");

	log_debug("main: Setting up the server to listen for incoming connections...");

	if (!Listen(&ListenSocket, &nResult))
		return nResult;

	log_debug("main: The setup for listening for incoming connections is complete.");

	log_debug("main: Waiting for new client connections...");
	
	if (!AcceptClientConnection(&ListenSocket, &ClientSocket, &nResult))
		return nResult;

	log_info("main: Connection has been made to us by a client.");

	log_debug("main: Throwing away the listening socket...");

	CloseSocket(ListenSocket);

	log_debug("main: System resources consumed by the listening socket have been released.");

	log_debug("main: Attempting to receive data...");

	// ReSharper disable CppInitializedValueIsAlwaysRewritten
	int nBytesReceived = 0;
	// ReSharper restore CppInitializedValueIsAlwaysRewritten

	// Receive until the peer shuts down the connection
	do {
		log_debug("main: Calling Receive function...");

		nBytesReceived = Receive(ClientSocket, recvbuf, DEFAULT_BUFLEN);

		log_debug("main: Result of 'Receive' function call: %d", nBytesReceived);

		if (nBytesReceived > 0) {
			log_debug("main: nBytesRecived > 0, this means we got something");

			log_debug("main: Bytes received: %d", nBytesReceived);

			// Echo the buffer back to the sender

			log_debug("main: Echoing what we received right back to the client...");

			const int nBytesSent = send(ClientSocket, recvbuf, nBytesReceived, 0);

			log_debug("main: Checking for errors...");

			if (nBytesSent == SOCKET_ERROR) {
				log_error("main: send failed with error: %d", WSAGetLastError());
				CloseSocket(ClientSocket);
				WSACleanup();
				return 1;
			}

			log_debug("main: The send operation completed successfully.");

			log_debug("main: Bytes sent: %d", nBytesSent);
		}
		else if (nBytesReceived == 0)
		{
			log_debug("main: The nBytesReceived variable has a value of zero.");

			log_debug("main: Done receiving information.  Connection closing...");
		}
		else 
		{
			log_debug("main: nBytesReceived = %d", nBytesReceived);

			log_error("main: recv failed with error: %d", WSAGetLastError());
			CloseSocket(ClientSocket);
			WSACleanup();
			return 1;
		}
	} while (nBytesReceived > 0);

	log_debug("main: We are done with the recv loop because the number bytes received dropped to something nonpositive.");

	log_debug("main: Shutting down the sending portion of the client connection socket...");

	if (!Shutdown(ClientSocket, SD_SEND, &nResult))
		return nResult;

	log_debug("main: The sending endpoint of the client connection has been shut down.");

	log_debug("main: Freeing system resources...");

	// cleanup
	CloseSocket(ClientSocket);
	WSACleanup();

	log_debug("main: The client socket and additional system resources have been freed.");

	log_debug("main: nResult = %d", nResult);

	log_debug("main: Done.");

	return nResult;
}
