#include "networking/winsock.hpp"
#include <iostream>
#include <assert.h>

#define DEFAULT_PORT "443"

// Bitmasks for TCP Header bitfields
#define DATA_TYPE_MASK 0x0F;
#define PAYLOAD_LENGTH_REG_SIZE_MASK 0x7F;
#define PAYLOAD_LENGTH_EXT_SIZE_MASK 0xFF;

#define PAYLOAD_LENGTH_REG_SIZE 125;
#define PAYLOAD_LENGTH_EXT_SIZE 65535;

Winsock::Winsock() {
    Winsock("talkdirect-api.onrender.com", 500);
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
    shutdown(currentConnection.currentSocket, SD_BOTH);
    SSL_free(currentConnection.socket_ssl);
    SSL_CTX_free(ctx);
    closesocket(currentConnection.currentSocket);
    WSACleanup();
    std::cout << "Winsock deleted." << std::endl;
};


char Winsock::SendData(unsigned char data[], int dataSize, int dataType) {
    int iResult;
    int payloadIndicator = dataSize;
    if (dataSize > 65535) {
        payloadIndicator = 127;
    } else if (dataSize > 125) {
        payloadIndicator = 126;
    }
    // Setting up the data to become the TCP Header Information
    struct socketMessageHeader msgHeaderField = {1, 0, 0, 0, dataType, 1, payloadIndicator};
    unsigned char maskingKey[4] = {0x12, 0x34, 0x56, 0x78};

    assert(sizeof(&data) != 0 && "For some reason we're sending over no data, this should never happen it's wasteful and pointless to do this. Issa bug");

    // Adding in the header bytes first before we add in the data bytes to the buffer to be sent off
    int headerSize = 2;
    if (payloadIndicator > 125) {
        headerSize += (payloadIndicator == 126) ? 2 : 8; // Simply means if dataSize is less than or = to 65535 add on two bits else add on 8
    }
    
    // These bits are for masking key
    headerSize += 4;

    // current number of bits written to buffer
    int offset = 0;

    // Edit the header bitfield to have to proper values we want
    char buffer[headerSize+dataSize];
    buffer[offset++] = (msgHeaderField.finishedBit << 7) 
                | (msgHeaderField.rsv1 << 6)
                | (msgHeaderField.rsv2 << 5)
                | (msgHeaderField.rsv3 << 4)
                | (msgHeaderField.Opcode & 0x0F);

    buffer[offset++] = (msgHeaderField.mask) << 7 | (msgHeaderField.payloadLen & 0x7F);

    // Check if we need to extend payload length past 7 bits or not
    if (payloadIndicator == 126) {
        buffer[offset++] = (dataSize >> 8) & 0xFF;
        buffer[offset++] = dataSize & 0xFF;
    } else if (payloadIndicator == 127) {
        uint64_t fullDataSize = (uint64_t)dataSize;
        for (int i = 7; i >= 0; i--)
            buffer[offset++] = (fullDataSize >> (i*8)) & 0xFF;
    }

    memcpy(buffer + offset, maskingKey, 4);
    offset += 4;

    // XOR the data buffer before we copy it over to be sent off
    for (int i = 0; i < dataSize; i++)
        data[i] ^= maskingKey[i % 4];

    memcpy(buffer + offset, data, dataSize);
    offset +=dataSize;

    iResult = SSL_write(currentConnection.socket_ssl, buffer, offset);
    if (iResult == SOCKET_ERROR) {
        WSACleanup();
        validConnection = false;
        std::cout << "error sending bytes" << std::endl;
        return 0x01;
    }
    return 0x00;
};

unsigned char* Winsock::ReceiveData() {
    return ReceiveData(currentConnection);
}

unsigned char* Winsock::ReceiveData(SOCKET_CONNECTION Connection) { 

    int iResult;
    u_int64 size = 1024000;
    int decodedBufLen = 0;
    char recvbuf[size] = {};
    unsigned char* decodedBuffer = new unsigned char[size];
    std::memset(recvbuf, 0, sizeof(recvbuf));
    std::memset(decodedBuffer, 0, sizeof(decodedBuffer));
    bool working = true;

    while (working) {
        // TODO: Make this into a SSL_Read since the connection is now a HTTPS connection
        iResult = SSL_read(Connection.socket_ssl, recvbuf, size);
        if (iResult > 0) { // if postive, will contain amount of bytes in message we need to decode these bytes
            std::cout << "Bytes recieved: " << iResult << std::endl;
            u_int64 offset = 0;

            // Simple check to make sure we're not getting a HTTP message and interpreting it as a websocket frame
            // doing this by just checking and ensure first 4 bytes (header bitfields) are not encoded to HTT
            for (int i = 0; i < 3; i++) {
                if (recvbuf[i] == 0x72 || recvbuf[i] == 0x54 && decodedBufLen == 0) { // 0x72 = 'H' byte code; 0x54 = 'T' byte code
                    return decodedBuffer;
                }
            }
            
            // Getting items in recvbuf[0] / byte 1
            unsigned char finBit = (recvbuf[offset] >> 7) & 0x01;
            unsigned char dataType = recvbuf[offset++] & DATA_TYPE_MASK;

            // Getting items in recvbuf[1] / byte 2
            u_int64 dataSize = recvbuf[offset++] & PAYLOAD_LENGTH_REG_SIZE_MASK;
            
            // Getting payloadLen, if smaller than 126 will be inside the 7 bits above, if not it'll be in 2 bytes or 8 bytes
            if (dataSize > 125) {
                if (dataSize == 126) { // PayloadLen is 2 unsigned bytes (16 bits) long
                    dataSize = (static_cast<uint16_t>(static_cast<uint8_t>(recvbuf[offset++])) << 8) | 
                            (static_cast<uint8_t>(recvbuf[offset++]));
                } else if (dataSize == 127) { // else it's encoded in a 64 bit uint that we'll have to keep looping thru
                    dataSize = 0;
                    for (int i = 0; i < 8; i++) {
                        // Have not properly tested if this static_cast will 100% work but it should based on my knowledge
                        dataSize = (dataSize << 8) | (static_cast<uint8_t>(recvbuf[offset++]));
                    }
                }
            }
            /*reason for this offset increment is that the first byte of our actual message not the header file is our personal DataID byte. This will
            signify if the packet is an Audio, String or Video packet for example. For now, we'll assume that all packets are strings till later, so just 
            increment past it and ignore it.
            */
            offset++;
            
            for (int i = decodedBufLen; i < dataSize; i++) // start to decode the received buffer by pulling out bytes starting from offset & placing at start of new buffer
                decodedBuffer[decodedBufLen++] = recvbuf[offset++];
            if (finBit == 1) {// If FIN Bit in Websocket Header is 1 "True" means that this is the last message frame and we can finally send off the decodedBuffer
                std::cout << "Completed message" << std::endl;
                working = false;
            } else if (finBit == 0 && dataType == 0) {
                std::cout << "Broken message, attempting to pull more data from recv()" << std::endl;
            }
        }
        
        else if (iResult == 0) {
            break;
        } else {// Error could be WSAEWOULDBLOCK since we're in non-blocking mode if so, ignore it and move on, else return a nullptr and break out of function
            int sslError = SSL_get_error(currentConnection.socket_ssl, iResult);
            if (sslError == SSL_ERROR_SYSCALL) {
                int error = WSAGetLastError();
                if (error != WSAEWOULDBLOCK) {
                    std::cout << "Recv failed with error: \n" << WSAGetLastError() << std::endl;
                    validConnection = false;
                    return nullptr;
                }
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
    
    std::cout << "Connecting to desired session with given sessionid: " << SessionID << std::endl;
    InitServerSession(SessionID);
    std::cout << "Connected and Hosting Session" << std::endl;

    // Create a new socket & test to make sure socket is properly created if not, return
    currentConnection = CreateSocket();
    if (currentConnection.currentSocket == INVALID_SOCKET) {
        std::cout << "socket has not been created properly." << std::endl;
        return 0x01;
    }

    // Make our socket be non-blocking now to allow async I/O
    u_long iMode = 1;
    iResult = ioctlsocket(currentConnection.currentSocket, FIONBIO, &iMode);

    std::string GET_HTTP =
        "GET /" + std::to_string(SessionID) + " HTTP/1.1\r\n"
        "Host: talkdirect-api.onrender.com\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
        "Sec-WebSocket-Protocol: chat\r\n"
        "Origin: https://talkdirect-api.onrender.com\r\n"
        "User-Agent: TalkDirectClient/1.0\r\n"
        "\r\n";

    // Write to the Server to Upgrade Connection to Websocket
    iResult = SSL_write(currentConnection.socket_ssl, GET_HTTP.c_str(), GET_HTTP.size());
    if (iResult == SOCKET_ERROR) {
        std::cout << "send failed: \n" << WSAGetLastError() << std::endl;
        WSACleanup();
        return 0x01;
    }
    validConnection = true;

    return 0x00;
};

void Winsock::DisconnectSocket() {
    validConnection = false;
    std::cout << "Now Starting Official Server Session Shutdown Socket Side." << std::endl;
    Winsock::CloseServerSession();
    std::cout << "Finished Official Server Session Shutdown Socket Side." << std::endl;
};

SOCKET_CONNECTION Winsock::CreateSocket() {
    return CreateSocket(socketUrl, DEFAULT_PORT);
}

SOCKET_CONNECTION Winsock::CreateSocket(const char* url) {
    return CreateSocket(url, DEFAULT_PORT);
};

SOCKET_CONNECTION Winsock::CreateSocket(const char* url, const char* port) {
    int iResult;
    struct SOCKET_CONNECTION tempConnection = {nullptr, INVALID_SOCKET};
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
        
        return tempConnection;
    }

    ptr = result;
    tempConnection.currentSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (tempConnection.currentSocket == INVALID_SOCKET) {
        std::cout << "socket failed with error: " << WSAGetLastError() << std::endl;
        validConnection = false;
        WSACleanup();
        return tempConnection;
    }

    iResult = connect(tempConnection.currentSocket, ptr->ai_addr, (int)ptr->ai_addrlen );
    if (iResult == SOCKET_ERROR) {
        std::cout << "socket connection failed: \n" << WSAGetLastError() << std::endl;
        validConnection = false;
        WSACleanup();
        return tempConnection;
    }
    // Initialize OpenSSL
    SSL_library_init();
    SSL_load_error_strings();
    const SSL_METHOD* method = TLS_client_method();
    ctx = SSL_CTX_new(method);
    if (!ctx) {
        std::cerr << "SSL_CTX_new failed\n";
        return tempConnection;
    }

    SSL* ssl = SSL_new(ctx);
    SSL_set_fd(ssl, (int)tempConnection.currentSocket);
    SSL_set_tlsext_host_name(ssl, "talkdirect-api.onrender.com");
    if (SSL_connect(ssl) != 1) {
        std::cerr << "SSL_connect failed\n";
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        closesocket(tempConnection.currentSocket);
        WSACleanup();
        validConnection = false;
        return tempConnection;
    }
    tempConnection.socket_ssl = ssl;
    std::cout << "Successfully fully connected socket to an SSL" << std::endl;
    return tempConnection;
};

void Winsock::InitServerSession(int SessionID) {
    std::string GET_HTTP =
        "GET /api/host/" + std::to_string(SessionID) + " HTTP/1.1\r\n"
        "Host: talkdirect-api.onrender.com\r\n"
        "User-Agent: TalkDirectClient/1.0\r\n"
        "Accept: */*\r\n"
        "Connection: close\r\n\r\n";

    int nDataLen;
    char buffer[1000];
    std::string websiteHTML;
    // Simply Create a new Socket
    SOCKET_CONNECTION initConnection = CreateSocket("talkdirect-api.onrender.com", "443");
    
    // For now, just send a request to make a new Session, later we'll add on searching if session already exists
    // TODO: Make this into a SSL_write since the connection is now a HTTPS connection
    SSL_write(initConnection.socket_ssl, GET_HTTP.c_str(), GET_HTTP.size());
    
    // TODO: Make this into a SSL_read since the connection is now a HTTPS connection
    if ((nDataLen = SSL_read(initConnection.socket_ssl, buffer, 1000)) > 0) {
        int i = 0;
        while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r') {
            websiteHTML += buffer[i];
            i += 1;
        }
    }
    std::cout << "Able to Create Socket with SessionID:" + std::to_string(SessionID) << std::endl;
    // Close out socket
    memset(buffer, 0, 1000);
    SSL_free(initConnection.socket_ssl);
    SSL_CTX_free(ctx);
    shutdown(initConnection.currentSocket, SD_BOTH);
    closesocket(initConnection.currentSocket);
};

void Winsock::CloseServerSession() {
    std::string GET_HTTP =
        "GET /api/close/" + std::to_string(SessionID) + " HTTP/1.1\r\n"
        "Host: talkdirect-api.onrender.com\r\n"
        "User-Agent: TalkDirectClient/1.0\r\n"
        "Accept: */*\r\n"
        "Connection: close\r\n\r\n";

    int nDataLen;
    char buffer[1000];
    std::string websiteHTML;
    // Simply Create a new Socket
    SOCKET_CONNECTION finalConnection = CreateSocket("talkdirect-api.onrender.com", "443");
    
    SSL_write(finalConnection.socket_ssl, GET_HTTP.c_str(), GET_HTTP.size());
    
    while ((nDataLen = SSL_read(finalConnection.socket_ssl, buffer, 1000)) > 0) {
        int i = 0;
        while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r') {
            websiteHTML += buffer[i];
            i += 1;
        }
    }

    std::cout << "Able to Close Socket with SessionID:" + std::to_string(SessionID) << std::endl;
    // Close out socket
    memset(buffer, 0, 1000);
    SSL_free(finalConnection.socket_ssl);
    SSL_CTX_free(ctx);
    shutdown(finalConnection.currentSocket, SD_BOTH);
    closesocket(finalConnection.currentSocket);
};

bool Winsock::getValidConnection() {
    return validConnection;
}