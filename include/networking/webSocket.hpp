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

    // Event Listener, activates whenever socket recieves a message
    void onMessage();

    unsigned short readyState;

    private:

    // Mainly used to send off strings
    void SendMessage();
    // Used to recieve inputs from the client
    void RecieveMessage();
    // Used to send off errors to the client & close socketConnection
    void SendError();

    int wsID;
    const char* url = "localhost";
    unsigned int bufferedAmount;
    const char* BinaryType = "arraybuffer";

    Winsock* winsock;

    enum ReadyStates : unsigned short {
        CONNECTING,
        OPEN,
        CLOSING,
        CLOSED
    };
};