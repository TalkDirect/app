#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>

// Basic Class to handle TCP Ports and IP of the Windows Operating System
class Winsock {
    public:
    Winsock();
    Winsock(const char* socketUrl);
    Winsock(const char* socketUrl, int SessionID);
    ~Winsock();

    // Simply send off data
    char SendData(const char* data);

    // Returns a pointer to a buffer containing the data
    char* RecieveData();

    void DisconnectSocket();

    private:
    WSADATA wsaData;
    SOCKET currentSocket;
    const char* socketUrl;
    int SessionID;

    char Init();

    // Start up a new session on the API Service with the same SessionID given
    void InitServerSession(int SessionID);
    void CloseServerSession();

    SOCKET CreateSocket();
    SOCKET CreateSocket(const char* url);
    SOCKET CreateSocket(const char* url, const char* port);

};