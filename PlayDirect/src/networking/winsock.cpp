#include "networking/winsock.hpp"
#include <iostream>

#define DEFAULT_PORT "9998"

Winsock::Winsock() {
    Winsock("localhost", 500);
};

Winsock::Winsock(const char* socketUrl) {
    Winsock(socketUrl, 500);
};

Winsock::Winsock(const char* socketUrl, int SessionID) {

    // Init Winsock
    Winsock::socketUrl = socketUrl;
    Winsock::SessionID = SessionID;
    Init();
};

Winsock::~Winsock() {
    DisconnectSocket();
};


char Winsock::SendData(char data[], int dataSize, int dataType) {
    int iResult;

    struct socketMessageHeader msgHeaderField = {1, 0, 0, 0, dataType, 1, dataSize};
    char maskingKey[4] = {0x12, 0x34, 0x56, 0x78};
    memcpy(msgHeaderField.maskKey, maskingKey, 4);

    // Adding in the header bytes first before we add in the data bytes to the buffer to be sent off
    char buffer[6+dataSize];
    buffer[0] = (msgHeaderField.finishedBit << 7) 
                | (msgHeaderField.rsv1 << 6)
                | (msgHeaderField.rsv2 << 5)
                | (msgHeaderField.rsv3 << 4)
                | (msgHeaderField.Opcode);

    buffer[1] = (msgHeaderField.mask) << 7 | (msgHeaderField.payloadLen);
    memcpy(buffer+2, maskingKey, 4);
    int offset = 6; // current number of bits written to buffer

    // XOR the data buffer before we copy it over to be sent off
    for (int i = 0; i < dataSize; i++) {
        data[i] ^= maskingKey[i % 4];
    }

    memcpy(buffer + offset, data, dataSize);

    iResult = send(currentSocket, buffer, dataSize+offset, 0);
    if (iResult == SOCKET_ERROR) {
        WSACleanup();
        std::cout << "error sending bytes" << std::endl;
        return 0x01;
    }
    std::cout << "sent bytes" << std::endl;
    return 0x00;
};

char* Winsock::RecieveData() {
    //TODO: Need to get this properly send data via return later
    int iResult;

    // Recvbuf will be the buffer containing the recieved data from server
    char recvbuf[512];
    char decodedBuffer[512];
    iResult = recv(currentSocket, recvbuf, 512, 0);
    if (iResult > 0) {
        int recvBuffLen = recvbuf[1];
        std::cout << "Bytes recieved: " << iResult << std::endl;

        for (int i = 2; i < iResult; i++) {
            if (recvbuf[i] >= 32 || recvbuf[i] == '\n' || recvbuf[i] == '\r') {
                decodedBuffer[i] = recvbuf[i];
            }
        }
        std::cout << recvbuf << std::endl;
        return decodedBuffer;
    }

    else if (iResult == 0)
        std::cout << "Connectioned Closed" << std::endl;
    
    else
        std::cout << "Recv failed with error: \n" << WSAGetLastError() << std::endl;
    return nullptr;

};

char Winsock::Init() {
    int iResult;

    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
        if (iResult != 0)
            return 0x01;
    
    InitServerSession(SessionID);

    // Create a new socket & test to make sure socket is properly created if not, return
    currentSocket = CreateSocket();
    if (currentSocket == INVALID_SOCKET) {
        std::cout << "socket has not been created properly." << std::endl;
        return 0x01;
    }

    std::string GET_HTTP = "GET /" + std::to_string(SessionID) + " HTTP/1.1\r\nHost: localhost:9998\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Version: 13\r\nSec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\nSec-WebSocket-Protocol: chat\r\n\r\n";

    iResult = send(currentSocket, GET_HTTP.c_str(), strlen(GET_HTTP.c_str()), 0);
    if (iResult == SOCKET_ERROR) {
        std::cout << "send failed: \n" << WSAGetLastError() << std::endl;
        WSACleanup();
        return 0x01;
    }

    // Make a Test Buffer to send to server to ensure API working nicely
    char sendbuf[] = {0x02, 0x04, 0x74, 0x65, 0x73, 0x74, 0x69, 0x6E, 0x67, 0x20, 0x61, 0x70, 0x70};

    int size = 13;

    // Sending Initial Buffer
    SendData(sendbuf, size, 0x1);

    std::cout << "sent test bytes" << std::endl;
    return 0x00;
};

void Winsock::DisconnectSocket() {
    closesocket(currentSocket);
    Winsock::CloseServerSession();
    WSACleanup();
};

SOCKET Winsock::CreateSocket() {
    return CreateSocket(socketUrl, DEFAULT_PORT);
}

SOCKET Winsock::CreateSocket(const char* url) {
    return CreateSocket(url, DEFAULT_PORT);
};

SOCKET Winsock::CreateSocket(const char* url, const char* port) {
    int iResult;

    struct addrinfo 
        *result = NULL,
        *ptr = NULL,
        hints;
    ZeroMemory( &hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve Server address & port
    iResult = getaddrinfo(url, port, &hints, &result);
    if (iResult != 0) {
        std::cout << "getaddrinfo failed: \n" << iResult << std::endl;
        WSACleanup();
        return INVALID_SOCKET;
    }


    SOCKET ConnectSocket = INVALID_SOCKET;

    ptr = result;
    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        std::cout << "socket failed with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return INVALID_SOCKET;
    }
    
    
    iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen );
    if (iResult == SOCKET_ERROR) {
        std::cout << "socket connection failed: \n" << iResult << std::endl;
        WSACleanup();
        return INVALID_SOCKET;
    }
    return ConnectSocket;
};

void Winsock::InitServerSession(int SessionID) {
    std::string API_ROUTE = "/api/host/" + SessionID;

    std::string GET_HTTP = "GET /api/host/" + std::to_string(SessionID) + " HTTP/1.1\r\nHost: localhost:9999\r\nConnection: close\r\n\r\n";

    int nDataLen;
    char buffer[1000];
    std::string websiteHTML;
    // Simply Create a new Socket
    SOCKET initSocket = CreateSocket("localhost", "9999");
    
    // For now, just send a request to make a new Session, later we'll add on searching if session already exists
    send(initSocket, GET_HTTP.c_str(), GET_HTTP.size(), 0);
    
    while ((nDataLen = recv(initSocket, buffer, 1000, 0)) > 0) {
        int i = 0;
        while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r') {
            websiteHTML += buffer[i];
            i += 1;
        }
    }
    // Close out socket
    closesocket(initSocket);
};

void Winsock::CloseServerSession() {
    std::string API_ROUTE = "/api/host/" + Winsock::SessionID;

    std::string GET_HTTP = "GET /api/close/" + std::to_string(SessionID) + " HTTP/1.1\r\nHost: localhost:9999\r\nConnection: close\r\n\r\n";

    int nDataLen;
    char buffer[1000];
    std::string websiteHTML;
    // Simply Create a new Socket
    SOCKET initSocket = CreateSocket("localhost", "9999");
    
    // For now, just send a request to make a new Session, later we'll add on searching if session already exists
    send(initSocket, GET_HTTP.c_str(), GET_HTTP.size(), 0);
    
    while ((nDataLen = recv(initSocket, buffer, 1000, 0)) > 0) {
        int i = 0;
        while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r') {
            websiteHTML += buffer[i];
            i += 1;
        }
    }
    // Close out socket
    closesocket(initSocket);
};