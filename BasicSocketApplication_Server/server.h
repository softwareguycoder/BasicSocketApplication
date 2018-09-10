#ifndef __SERVER_H__
#define __SERVER_H__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <ws2tcpip.h>
#include <cstdio>

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

/**
 * \brief Calls the WSAStartup Windows API function in order to initialize the Microsoft TCP/IP stack.
 * \param p_wsa_data Address of storage that receives an initialized WSADATA structure. 
 * \param p_result Integer result of the WSAStartup system call.
 * \return True if the operation was successful; false otherwise.  If false, p_result 
 * will point to an error code which is an integer value other than zero.
 * \remarks This function must be called prior to any other Winsock calls!
 */
bool WINAPI initialize_winsock(LPWSADATA p_wsa_data, int* p_result);

#endif //__SERVER_H__
