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

    /*
    Send data off to the server for other clients to view, going to first initalize the bitfield that we're to use, then copy it over in memory, then lastly
    send it over websockets

    NOTE: we're assuming that the payload will *always* be small, we will most certainly have to change this later one in the project, as in the payloadLen will
    alwyas be shorter than 125 bytes long so we won't have to use extended bitfield configuration

    */
    char SendData(char data[], int dataSize, int dataType);

    /*
    Recieve data over websockets, in later implementations will allow for the sorting of data to optimize it

    NOTE: For now, the returning of the data array is not yielding the proper results that we want, might need to fix later on

    */
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

struct socketMessageHeader {
    int finishedBit : 1;
    int rsv1 : 1;
    int rsv2 : 1;
    int rsv3 : 1;
    int Opcode : 4;
    int mask : 1;
    int payloadLen : 7;
    char maskKey[4];
};