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

char Winsock::SendData(const char* data) {
    int iResult;

    iResult = send(currentSocket, data, (int)strlen(data), 0);
    if (iResult == SOCKET_ERROR) {
        WSACleanup();
        return 0x01;
    }
    return 0x00;
};

char* Winsock::RecieveData() {
    int iResult;

    // Recvbuf will be the buffer containing the recieved data from server
    char* recvbuf = (char*)malloc(sizeof(char)*64);
    iResult = recv(currentSocket, recvbuf, 512, 0);
    if (iResult > 0) {
        std::cout << "Bytes recieved: \n" << iResult << std::endl;
        return recvbuf;
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

    // Now, after we get a valid socket connection, attempt to connect to our actual session
    char temp[100];
    strcpy(temp, "/");
    strcat(temp, std::to_string(SessionID).c_str());
    const char* actualUrl = temp;

    std::string GET_HTTP = "GET " + std::string(actualUrl) + " HTTP/1.1\r\nHost: localhost:9998\r\nConnection: close\r\n\r\n";

    iResult = send(currentSocket, GET_HTTP.c_str(), strlen(GET_HTTP.c_str()), 0);
    if (iResult == SOCKET_ERROR) {
        std::cout << "send failed: \n" << WSAGetLastError() << std::endl;
        WSACleanup();
        return 0x01;
    }
    // Make a Test Buffer to send to server to ensure API working nicely
    const char* sendbuf = "test buffer";

    // Sending Initial Buffer
    iResult = send(currentSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        std::cout << "send failed: \n" << WSAGetLastError() << std::endl;
        WSACleanup();
        return 0x01;
    }
    std::cout << "sent test bytes" << std::endl;
    return 0x00;
};

void Winsock::DisconnectSocket() {
    closesocket(currentSocket);
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
    
    
    iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen );
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
    std::cout << websiteHTML.c_str() << std::endl;
    // Close out socket
    closesocket(initSocket);
};