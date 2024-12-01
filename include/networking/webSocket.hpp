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
    void onSendMessage(char data[]);
    char* onRetrieveMessage();


    unsigned char readyState;

    private:

    /*
        Function that acts as a wrapper for the winsock logic, used to send messages off with the proper header buffer attached
    */
    void SendMessages(char data[], int dataSize, int dataType);
    // Used to recieve inputs from the client
    char* RecieveMessages();
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