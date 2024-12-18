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

    
    /*
        Public method that simply calls the private counterpart 

        @param data[] data to be sent off to the API, in the form of bytes
        @param dataSize the size of the data array

    */
    void onSendMessage(unsigned char data[], int dataSize);
    unsigned char* onRetrieveMessage();


    unsigned char readyState;

    private:

    /*
        Private Function that acts as a wrapper for the winsock logic, used to send messages off with the proper header buffer attached
        
        @param data[] data to be sent off to the API, in the form of bytes
        @param dataSize the size of the data array
        @param dataType  the type of the data we're sending off
    */
    void SendMessages(unsigned char data[], int dataSize, int dataType);
    
    /*
        Recieve messages from the server, returns them in an char pointer that points to a char array

    */
    unsigned char* RecieveMessages();

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