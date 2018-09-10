#include "Socket.h"

extern "C" {
#include "JQR.Debug.Core.h"
}

INT WINAPI Receive(SOCKET socket, PSTR szBuffer, const INT nLength)
{
	log_debug("In Receive");

	log_debug("Receive: Checking inputs...");

	auto nResult = SOCKET_ERROR;

	if (socket == INVALID_SOCKET)
	{
		log_error("Receive: Invalid socket handle value passed.  Stopping.");

		WSASetLastError(WSAEINVAL);

		log_debug("Receive: Done.");

		return nResult;
	}

	log_debug("Receive: A valid socket handle value has been passed.");

	log_debug("Receive: Checking whether the 'szBuffer' parameter has a valid storage address...");

	if (szBuffer == nullptr)
	{
		log_error("Receive: The 'szBuffer' parameter must point to already-allocated storage.  Stopping.");

		WSASetLastError(WSAEINVAL);

		log_debug("Receive: Done.");

		return nResult;
	}

	log_debug("Receive: The 'szBuffer' parameter references a valid storage address.");

	log_debug("Receive: Checking whether nLength is bigger than zero...");

	if (nLength <= 0)
	{
		log_error("Receive: Attempting to receive into a szBuffer that is a nonpositive number of bytes long.  Stopping.");

		WSASetLastError(WSAEINVAL);

		log_debug("Receive: Done.");

		return nResult;
	}

	log_debug("Receive: nLength is finite and positive.");

	log_debug("Receive: Attempting to receive data...");

	nResult = recv(socket, szBuffer, nLength, 0);

	log_debug("Receive: nResult = %d", nResult);

	log_debug("Receive: Done.");

	return nResult;
}

BOOL WINAPI Shutdown(const SOCKET socket, INT nHow, PINT pResult)
{
	log_debug("In Shutdown");

	log_debug("Shutdown: Checking inputs...");

	if (socket == INVALID_SOCKET)
	{
		log_error("CloseSocket: An invalid handle value was specified.  Stopping.");

		log_debug("CloseSocket: Done.");

		WSACleanup();

		return false;
	}

	log_debug("Shutdown: The socket handle passed was valid.");

	if (nHow != SD_SEND && nHow != SD_RECEIVE)
	{
		log_error("Shutdown: The value of nHow must either be SD_SEND or SD_RECEIVE.  It's not either of these, so stopping.");

		log_debug("Shutdown: Done.");

		WSACleanup();

		return false;
	}

	log_debug("Shutdown: nHow = %d", nHow);

	log_debug("Shutdown: The nHow parameter has a valid value.");

	if (pResult == nullptr)
	{
		log_error("Shutdown: The pResult parameter is required to have a non-NULL value but it has a NULL value.  Stopping.");

		log_debug("Shutdown: Done.");

		WSACleanup();

		return false;
	}

	log_debug("Shutdown: Input checks passed.  Calling the 'shutdown' function...");

	// shutdown the connection since we're done
	*pResult = shutdown(socket, nHow);
	if (*pResult == SOCKET_ERROR) {
		log_error("main: shutdown failed with error: %d", WSAGetLastError());
		CloseSocket(socket);
		WSACleanup();
		*pResult = 1;

		log_debug("Shutdown: Done.");

		return false;
	}

	log_debug("Shutdown: The operation completed successfully.");

	log_debug("Shutdown: Done.");

	return true;
}

VOID WINAPI CloseSocket(SOCKET socket)
{
	log_debug("In CloseSocket");

	log_debug("CloseSocket: Checking that a valid socket was passed in...");

	if (socket == INVALID_SOCKET)
	{
		log_error("CloseSocket: An invalid handle value was specified.  Stopping.");

		log_debug("CloseSocket: Done.");

		WSACleanup();

		return;
	}

	log_debug("CloseSocket: A valid socket handle has been passed.  Calling 'closesocket' on it...");

	closesocket(socket);

	log_debug("CloseSocket: The socket handle has been closed.");

	log_debug("CloseSocket: Done.");
}

BOOL WINAPI AcceptClientConnection(const SOCKET* pListenSocket, SOCKET* pClientSocket, PINT pResult)
{
	log_debug("In AcceptClientConnection");

	log_debug("AcceptClientConnection: Checking inputs...");

	// Input validation -- all of the passed pointers must have valid addresses

	if (pListenSocket == nullptr)
	{
		WSACleanup();
		return false;
	}

	if (pClientSocket == nullptr)
	{
		WSACleanup();
		return false;
	}

	if (pResult == nullptr)
	{
		WSACleanup();
		return false;
	}

	log_debug("AcceptClientConnection: Input checks passed.");

	*pResult = 0;

	log_debug("AcceptClientConnection: Calling the 'accept' function to accept a new client connection...");

	// Accept a client socket
	*pClientSocket = accept(*pListenSocket, nullptr, nullptr);
	if (*pClientSocket == INVALID_SOCKET) {
		log_error("main: accept failed: %d\n", WSAGetLastError());
		closesocket(*pListenSocket);
		WSACleanup();
		*pResult = 1;
		return false;
	}

	log_debug("AcceptClientConnection: A new client has connected to us.");

	log_debug("AcceptClientConnection: The operation completed successfully.");

	log_debug("AcceptClientConnection: Done.");

	// we now have a new client connection
	return true;
}

BOOL WINAPI Listen(const SOCKET* pListenSocket, PINT pResult)
{
	log_debug("In Listen");

	log_debug("Listen: Checking inputs...");

	// Input validation -- all of the pointers passed to this function must refer
	// to valid addresses
	if (pListenSocket == nullptr)
	{
		WSACleanup();
		return false;
	}

	if (pResult == nullptr)
	{
		WSACleanup();
		return false;
	}

	log_debug("Listen: Input checks passed.  Calling the 'listen' function...");

	*pResult = 0;

	*pResult = listen(*pListenSocket, SOMAXCONN);

	if (*pResult == SOCKET_ERROR) {
		log_error("main: listen failed with error: %d\n", WSAGetLastError());
		closesocket(*pListenSocket);
		WSACleanup();
		*pResult = 1;
		return false;
	}

	log_debug("Listen: The operation completed successfully.");

	log_debug("Listen: Done.");

	return true;
}

BOOL WINAPI BindServerSocket(const SOCKET* pListenSocket, PADDRINFOA pAddrInfo, PINT pResult)
{
	log_debug("In BindServerSocket");

	log_debug("BindServerSocket: Checking inputs...");

	// Input validation -- all of the pointers passed to this function must refer
	// to valid addresses
	if (pListenSocket == nullptr)
	{
		WSACleanup();
		return false;
	}

	if (pAddrInfo == nullptr)
	{
		WSACleanup();
		return false;
	}

	if (pResult == nullptr)
	{
		WSACleanup();
		return false;
	}

	log_debug("BindServerSocket: Input checks passed.  Calling the 'bind' function...");

	*pResult = 0;

	// Call the bind function to associate this TCP socket with the server endpoint
	*pResult = bind(*pListenSocket, pAddrInfo->ai_addr, static_cast<int>(pAddrInfo->ai_addrlen));
	if (*pResult == SOCKET_ERROR) {
		log_error("main: bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(pAddrInfo);
		closesocket(*pListenSocket);
		WSACleanup();
		*pResult = 1;
		return false;
	}

	log_debug("BindServerSocket: The operation completed successfully.");

	log_debug("BindServerSocket: Releasing system resources associated with the resolved server IP address and port...");

	// If we get here, we no longer need the server address informtion
	// structure, so free memory associated with it.
	freeaddrinfo(pAddrInfo);

	log_debug("BindServerSocket: System resources released.");

	log_debug("BindServerSocket: Done.");

	return true;
}

BOOL WINAPI CreateServerSocket(PADDRINFOA pAddrInfo, SOCKET *pListenSocket, PINT pResult)
{
	log_debug("In CreateServerSocket");

	log_debug("CreateServerSocket: Checking inputs...");

	// Input validation -- all of the passed pointers must have valid addresses
	if (pAddrInfo == nullptr)
	{
		WSACleanup();
		return false;
	}

	if (pListenSocket == nullptr)
	{
		WSACleanup();
		return false;
	}

	if (pResult == nullptr)
	{
		WSACleanup();
		return false;
	}

	log_debug("CreateServerSocket: Input checks passed.  Calling the socket function...");

	*pResult = 0;

	*pListenSocket = socket(pAddrInfo->ai_family, pAddrInfo->ai_socktype, pAddrInfo->ai_protocol);
	if (*pListenSocket == INVALID_SOCKET) {
		log_error("main: 'socket' function call failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(pAddrInfo);
		WSACleanup();
		*pResult = 1;
		return false;
	}

	log_debug("CreateServerSocket: The operation completed successfully.");

	log_debug("CreateServerSocket: Done.");

	return true;
}

BOOL WINAPI ResolveServerAddress(PCSTR pszPort, PADDRINFOA* ppAddressInfo, PINT pResult)
{
	log_debug("In ResolveServerAddress");

	log_debug("ResolveServerAddress: Checking inputs...");

	// Input validation -- pResult must contain a valid address of an integer that gets
	// any error codes.
	if (pResult == nullptr)
	{
		WSACleanup();
		return false;
	}

	log_debug("ResolveServerAddress: Input checks passed.  Calling getaddrinfo...");

	// Initialize the ppAddressInfo pointer to start out not pointing
	// to anything in particular
	*ppAddressInfo = nullptr;

	// Set up a structure of hints
	struct addrinfo hints {};

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	*pResult = getaddrinfo(nullptr, DEFAULT_PORT, &hints, ppAddressInfo);
	if (*pResult != 0) {
		log_error("ResolveServerAddress: getaddrinfo failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return false;
	}

	log_debug("ResolveServerAddress: The operation completed successfully.");

	log_debug("ResolveServerAddress: Done.");

	return true;
}

BOOL WINAPI InitializeWinsock(LPWSADATA lpWSADATA, PINT pResult)
{
	log_debug("In InitializeWinsock");

	log_debug("InitializeWinsock: Checking inputs...");

	// Input validation -- both the passed pointers must have valid addresses
	if (lpWSADATA == nullptr)
	{
		WSACleanup();
		return false;
	}

	if (pResult == nullptr)
	{
		WSACleanup();
		return false;
	}

	log_debug("InitializeWinsock: Input checks passed.  Calling WSAStartup...");

	*pResult = WSAStartup(MAKEWORD(2, 2), lpWSADATA);
	if (*pResult != 0) {
		log_error("InitializeWinsock: WSAStartup failed: %d", *pResult);

		log_debug("InitializeWinsock: Done.");

		WSACleanup();

		return false;
	}

	log_debug("InitializeWinsock: The operation completed successfully.");

	log_debug("InitializeWinsock: Done.");

	return true;
}