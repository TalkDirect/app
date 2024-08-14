#pragma once
#include <iostream>

#include "networking/winsock.hpp"

class webSocket {
    public:
    webSocket(int sessionID);
    webSocket();
    ~webSocket();

    bool ConnectSocket(int sessionID);
    void DiscounnectSocket();

    // Simple Functions that call the private ones
    void onSendMessage(const char data[]);
    void onRetrieveMessage();


    unsigned char readyState;

    private:

    // Mainly used to send off strings
    void SendMessages(const char data[], int dataSize);
    // Used to recieve inputs from the client
    void RecieveMessages();
    // Used to send off errors to the client & close socketConnection
    void SendError();

    int wsID;
    const char* url = "localhost";
    unsigned int bufferedAmount;
    const char* BinaryType = "arraybuffer";

    Winsock* winsock;

    enum ReadyStates : unsigned char {
        CONNECTING,
        OPEN,
        BUSY,
        CLOSING,
        CLOSED
    };
};