#ifndef __SERVER_H__
#define __SERVER_H__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <ws2tcpip.h>

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

/**
 * \brief Calls the WSAStartup Windows API function in order to initialize the Microsoft TCP/IP stack.
 * \param lpWSADATA Address of storage that receives an initialized WSADATA structure.
 * \param pResult Address of storage that receives the integer result of the system call.
 * \return True if the operation was successful; false otherwise.  If false, pResult
 * will point to an error code which is an integer value other than zero.
 * \remarks This function must be called prior to any other Winsock calls!
 */
BOOL WINAPI InitializeWinsock(LPWSADATA lpWSADATA, PINT pResult);

/**
 * \brief Resolves the server IP address and port using the getaddrinfo Windows API function.
 * \param pszPort String containing the port number the server is listening on.
 * \param ppAddressInfo Address of a refrence to storage that should receive the result.
 * \param pResult Address of storage that receives the integer result of the system call.
 * \return True if the operation was successful; false otherwise.  If false, pResult
 * will point to an error code which is an integer value other than zero.
 */
BOOL WINAPI ResolveServerAddress(PCSTR pszPort, PADDRINFOA* ppAddressInfo, PINT pResult);

/**
 * \brief Creates a new server socket endpoint to be used for listening for connections.
 * \param pAddrInfo Address of a ADDRINFOA structure which contains the output of ResolveServerAddress.
 * \param pListenSocket Address of storage that will receive a handle to the newly-created socket.
 * \param pResult Address of storage that receives the integer result of the system call.
 * \return True if the operation was successful; false otherwise.  If false, pResult
 * will point to an error code which is an integer value other than zero.
 */
BOOL WINAPI CreateServerSocket(PADDRINFOA pAddrInfo, SOCKET* pListenSocket, PINT pResult);

/**
 * \brief Binds a server socket to the endpoint used for listening for connections from clients.
 * \param pListenSocket Address of a SOCKET that is used for listening to remote connections.
 * \param pAddrInfo Address of a ADDRINFOA structure which contains the output of ResolveServerAddress.
 * \param pResult Address of storage that receives the integer result of the system call.
 * \return True if the operation was successful; false otherwise.  If false, pResult
 * will point to an error code which is an integer value other than zero.
 */
BOOL WINAPI BindServerSocket(const SOCKET* pListenSocket, PADDRINFOA pAddrInfo, PINT pResult);

/**
 * \brief Listens for connections to the listening socket.  Prepares the server so accept() can be called.
 * \param pListenSocket Address of a SOCKET that is used for listening to remote connections.
 * \param pResult Address of storage that receives the integer result of the system call.
 * \return True if the operation was successful; false otherwise.  If false, pResult
 * will point to an error code which is an integer value other than zero.
 * \remarks The BindServerSocket function must be called prior to calling this function.
 */
BOOL WINAPI Listen(const SOCKET* pListenSocket, PINT pResult);

/**
 * \brief Accepts a new incoming client connection.
 * \param pListenSocket Address of a SOCKET that is used for listening to remote connections.
 * \param pClientSocket Address of a SOCKET that will be filled with the handle to the newly-created
 * socket that interfaces to the connected client.
 * \param pResult Address of storage that receives the integer result of the system call.
 * \return True if the operation was successful; false otherwise.  If false, pResult
 * will point to an error code which is an integer value other than zero.
 * \remarks The Listen function should be called prior to calling this function.  Suspends the
 * calling thread's execution until a client actually has connected to the server.  Once a connection
 * has come in, returns a reference to a SOCKET that represents the connection to the client.
 */
BOOL WINAPI AcceptClientConnection(const SOCKET* pListenSocket, SOCKET* pClientSocket, PINT pResult);

/**
 * \brief Attempts to receive data from a remote endpoint.  Stores the received data, if any, in
 * the buffer whose address is provided.
 * \param socket Handle of a socket that represents a remote endpoint from which to receive data.
 * \param szBuffer Address of a char buffer with storage for the received data.
 * \param nLength Size of the buffer, in bytes.
 * \return Number of bytes received; zero if done, and SOCKET_ERROR if an error occurred.
 */
INT WINAPI Receive(const SOCKET socket, PSTR szBuffer, const INT nLength);

/**
 * \brief Closes the specified socket handle and frees any system resources associated with it.
 * \param socket Handle to the socket whose resources are to be freed.
 */
VOID WINAPI CloseSocket(SOCKET socket);

/**
 * \brief Shuts down a socket's connection endpoint for either sending or receiving.
 * \param socket Handle to the socket that should be shut down.
 * \param nHow Value specifying whether to shut down the receiving end or the sending end.
 * Must be either SD_SEND or SD_RECEIVE.
 * \param pResult Address of storage that receives the integer result of the system call.
 * \return True if the operation was successful; false otherwise.  If false, pResult
 * will point to an error code which is an integer value other than zero.
 */
BOOL WINAPI Shutdown(SOCKET socket, INT nHow, PINT pResult);

#endif //__SERVER_H__
