#include "networking/winsock.hpp"
#include <iostream>

#define DEFAULT_PORT "9998"

// Bitmasks for TCP Header bitfields
#define DATA_TYPE_MASK 0x0F;
#define PAYLOAD_LENGTH_REG_SIZE_MASK 0x7F;
#define PAYLOAD_LENGTH_EXT_SIZE_MASK 0xFF;

#define PAYLOAD_LENGTH_REG_SIZE 125;
#define PAYLOAD_LENGTH_EXT_SIZE 65535;

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


char Winsock::SendData(unsigned char data[], int dataSize, int dataType) {
    int iResult;

    // Setting up the data to become the TCP Header Information
    struct socketMessageHeader msgHeaderField = {1, 0, 0, 0, dataType, 1, dataSize};
    unsigned char maskingKey[4] = {0x12, 0x34, 0x56, 0x78};
    memcpy(msgHeaderField.maskKey, maskingKey, 4);

    // Adding in the header bytes first before we add in the data bytes to the buffer to be sent off
    int headerSize = 6;
    if (dataSize > 125) {
        headerSize += (dataSize <= 65535) ? 2 : 8; // Extended payload length
    }

    // current number of bits written to buffer
    int offset = 0;

    char buffer[headerSize+dataSize];
    buffer[offset++] = (msgHeaderField.finishedBit << 7) 
                | (msgHeaderField.rsv1 << 6)
                | (msgHeaderField.rsv2 << 5)
                | (msgHeaderField.rsv3 << 4)
                | (msgHeaderField.Opcode);

    buffer[offset++] = (msgHeaderField.mask) << 7 | (msgHeaderField.payloadLen);

    // Check if we need to extend payload length past 7 bits or not
    if (dataSize > PAYLOAD_LENGTH_REG_SIZE+0) {
        if (dataSize <= PAYLOAD_LENGTH_EXT_SIZE+0) {
            buffer[offset++] = (dataSize >> 8) & PAYLOAD_LENGTH_EXT_SIZE_MASK;
            buffer[offset++] = dataSize & PAYLOAD_LENGTH_EXT_SIZE_MASK;
        } else {
            for (int i = 7; i >= 0; i--)
                buffer[offset++] = (dataSize >> (i * 8)) & PAYLOAD_LENGTH_EXT_SIZE_MASK;
        }
    }

    memcpy(buffer + offset, maskingKey, 4);

    // XOR the data buffer before we copy it over to be sent off
    for (int i = 0; i < dataSize; i++)
        data[i] ^= maskingKey[i % 4];

    offset += 4;
    memcpy(buffer + offset, data, dataSize);

    iResult = send(currentSocket, buffer, dataSize+offset, 0);
    if (iResult == SOCKET_ERROR) {
        WSACleanup();
        std::cout << "error sending bytes" << std::endl;
        return 0x01;
    }
    return 0x00;
};

unsigned char* Winsock::RecieveData() {
    return RecieveData(Winsock::currentSocket);
}

unsigned char* Winsock::RecieveData(SOCKET socket) { 

    int iResult;
    u_int64 size = 1024;
    int decodedBufLen = 0;
    char recvbuf[size];
    unsigned char* decodedBuffer = new unsigned char[size];

    while (true) {
        iResult = recv(socket, recvbuf, size, 0);
        if (iResult > 0) { // if postive, will contain amount of bytes in message we need to decode these bytes
            std::cout << "Bytes recieved: " << iResult << std::endl;
            u_int64 offset = 0;

            // Getting items in recvbuf[0] / byte 1

            unsigned char finBit = recvbuf[offset] & 0x80;
            unsigned char dataType = recvbuf[offset++] & DATA_TYPE_MASK;

            // Getting items in recvbuf[1] / byte 2

            u_int64 dataSize = recvbuf[offset++] & PAYLOAD_LENGTH_REG_SIZE_MASK;

            // Getting payloadLen, if smaller than 126 will be inside the 7 bits above, if not it'll be in 2 bytes or 8 bytes
            if (dataSize > 125) {
                if (dataSize == 126) { // PayloadLen is 2 unsigned bytes (16 bits) long
                    dataSize = (recvbuf[offset++] << 8) | recvbuf[offset++];

                } else if (dataSize == 127) { // else it's encoded in a 64 bit uint that we'll have to keep looping thru
                    for (int i = 0; i < 8; i++) {
                        dataSize = (dataSize << 8) | recvbuf[offset++];
                    }
                }
            }

            for (int i = decodedBufLen; i < dataSize; i++) // start to decode the received buffer by pulling out bytes starting from offset & placing at start of new buffer
                decodedBuffer[decodedBufLen++] = recvbuf[offset++];

            if (finBit != 0) {// If FIN Bit in Websocket Header is 1 "True" means that this is the last message frame and we can finally send off the decodedBuffer
                std::cout << "completed message" << std::endl;
                break;
            }

            std::cout << "broken message, attempting to pull more data from recv()" << std::endl;
        }
        
        else if (iResult == 0) {
            std::cout << "Connectioned Closed" << std::endl;
            break;
        } else {
            int error = WSAGetLastError();
            if (error != WSAEWOULDBLOCK) {
                std::cout << "Recv failed with error: \n" << WSAGetLastError() << std::endl;
                return nullptr;
            }
        }
    }
    return decodedBuffer;

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

    // Make our socket be non-blocking now to allow async I/O
    u_long iMode = 1;
    iResult = ioctlsocket(currentSocket, FIONBIO, &iMode);

    std::string GET_HTTP = "GET /" + std::to_string(SessionID) + " HTTP/1.1\r\nHost: localhost:9998\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Version: 13\r\nSec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\nSec-WebSocket-Protocol: chat\r\n\r\n";

    iResult = send(currentSocket, GET_HTTP.c_str(), strlen(GET_HTTP.c_str()), 0);
    if (iResult == SOCKET_ERROR) {
        std::cout << "send failed: \n" << WSAGetLastError() << std::endl;
        WSACleanup();
        return 0x01;
    }
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

bool Winsock::validConnection() {
    return currentSocket != INVALID_SOCKET;
}