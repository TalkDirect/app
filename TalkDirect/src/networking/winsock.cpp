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

/* Function that can read data from websocket, contains an inner and outer loop.
Outer Loop: Main job is to keep on reading data from socket connection until we read entire message
Inner Loop: Job is to decode the data coming out of socket, if we're missing some data for the frame we keep looping back via Outer Loop till get the needed data
Returns the decodedBuffer (might make into std::vector so its dynamic sizable) which then gets sent out to the ReceiveData Thread in Session class*/
unsigned char* Winsock::ReceiveData(SOCKET_CONNECTION Connection) { 

    int iResult;
    u_int64 size = 10240;
    unsigned char recvbuf[size] = {};
    unsigned char* decodedBuffer = new unsigned char[size];

    u_int64 bytesDecodedFrameTotal = 0;
    u_int64 bytesDecodedTotal = 0;
    u_int64 currentFramePayloadLen = 0;
    bool waitingForHeader = true;
    bool isMessageFin = false;
    bool working = true;

    while (working && validConnection) {// Outer Loop: Main job is to read from socket when needed
        
        std::memset(recvbuf, 0, size);
        iResult = SSL_read(Connection.socket_ssl, recvbuf, size);
        
        if (iResult >= 0) { // if postive, will contain amount of bytes in message we need to decode these bytes
            std::cout << "Bytes recieved: " << iResult << std::endl;
            u_int64 readOffset = 0;
            while (readOffset < iResult) {// Inner Loop: Main job is to decode data, if reached end of payload length for current frame it breaks and reads more as needed
            
                if (waitingForHeader) { // Checking to see if we to parse for our header
                    if (iResult - readOffset < 2) {// Checking to see if we have te min amount of bytes for a header
                        std::cout << "Partial Frame header, looping back for more data" << std::endl;
                        break;
                    }
                    // Getting items in recvbuf[0] / byte 1
                    isMessageFin = recvbuf[readOffset] & 0x80;
                    unsigned char dataType = recvbuf[readOffset++] & DATA_TYPE_MASK;

                    // Getting items in recvbuf[1] / byte 2
                    unsigned int dataSize = recvbuf[readOffset++] & PAYLOAD_LENGTH_REG_SIZE_MASK;

                    // Getting payloadLen, if smaller than 126 will be inside the 7 bits above, if not it'll be in 2 bytes or 8 bytes
                    if (dataSize > 125) {
                        
                        if (dataSize == 126) { // PayloadLen is 2 unsigned bytes (16 bits) long
                            currentFramePayloadLen = ((unsigned char)(recvbuf[readOffset++]) << 8) |  (unsigned char)(recvbuf[readOffset++]);
                        
                        } else if (dataSize == 127) { // else it's encoded in a 64 bit uint that we'll have to keep looping thru
                            currentFramePayloadLen = 0;
                            
                            for (int i = 0; i < 8; i++) {
                                currentFramePayloadLen = (currentFramePayloadLen << 8) | (static_cast<uint8_t>(recvbuf[readOffset++]));
                            }
                        }
                    } else {
                        currentFramePayloadLen = dataSize;
                    }
                    
                    std::cout << "Total Message Size: " << currentFramePayloadLen << std::endl;

                    /*reason for this offset increment is that the first byte of our actual message not the header file is our personal DataID byte. This will
                    signify if the packet is an Audio, String or Video packet for example. For now, we'll assume that all packets are strings till later, so just 
                    increment past it and ignore it.
                    */
                    if (bytesDecodedTotal == 0) {
                        readOffset++;
                        currentFramePayloadLen--;
                    }
                    
                    waitingForHeader = false;
                }

                // Dictates how much bytes to copy from recvBuf into our decodedBuffer
                // Gets the bytes remaining in our frame AND chunk (how much bytes SSL_read said was supposed to be receiving)
                // Then picks what ever is lower
                u_int64 remainingInFrame = currentFramePayloadLen - bytesDecodedFrameTotal;
                u_int64 avilableInChunk = iResult - readOffset;
                u_int64 bytesToCopy = std::min(remainingInFrame, avilableInChunk);

                std::cout << "bytes to Copy: " << bytesToCopy << std::endl;
                
                for (int i = 0; i < bytesToCopy; i++) {// start to decode the received buffer by pulling out bytes starting from offset & placing at start of new buffer
                    decodedBuffer[bytesDecodedTotal++] = recvbuf[readOffset++];
                    bytesDecodedFrameTotal++;
                }
                
                std::cout << "Current Decoded Message Length: " << bytesDecodedTotal << std::endl;

                if (bytesDecodedFrameTotal == currentFramePayloadLen) {// Checks if we fully decoded the current Frame's payload, if not we loop back through to grab more data
                    std::cout << "Fully decoded Frame" << std::endl;
                    if (isMessageFin) {// Just checks if the Finished bit is flipped to 1 or 0. If 1 then this is the last frame and we're finished
                        std::cout << "Completed message" << std::endl;
                        working = false;
                        break;
                    
                    } else {// Not final frame, but this frame is finished, start process to grab new frame along with header
                        std::cout << "Fragmented Message, looping back for more data" << std::endl;
                        bytesDecodedFrameTotal = 0;
                        waitingForHeader = true;
                    }
                }
            }

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
    char buf[512];

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

    
    while (true) {
        iResult = SSL_read(currentConnection.socket_ssl, buf, 512);

        if (iResult >= 0) {
            std::cout << "Connection was Cleanly upgraded from HTTP to WS" << std::endl;
            break;
        }
        else {
            int sslError = SSL_get_error(currentConnection.socket_ssl, iResult);
                if (sslError == SSL_ERROR_SYSCALL) {
                int error = WSAGetLastError();
                
                if (error == WSAEWOULDBLOCK) {
                    // Winsock non-blocking signal: buffer is empty. We are done.
                    break; 
                } else {
                    // Fatal Winsock error
                    std::cout << "Fatal socket error during buffer upgrading clear: " << error << std::endl;
                    validConnection = false;
                    break;
                }
            }
        }
    }

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