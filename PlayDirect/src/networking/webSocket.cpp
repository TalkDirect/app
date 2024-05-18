#include "networking/webSocket.hpp"

webSocket::webSocket(int sessionID) {
    if (sessionID == 0) 
        return;
    readyState = ReadyStates::CLOSED;
    ConnectSocket(sessionID);
}

// Initalize up the default websocket (It's ID is 500)
webSocket::webSocket() {
    webSocket(500);
}

webSocket::~webSocket() {
    delete winsock;
}

bool webSocket::ConnectSocket(int sessionID) {
    // First attempt to retrieve a current session associated with that socket
    readyState = ReadyStates::CONNECTING;

    winsock = new Winsock(url, sessionID);

    // If we get a code that's 0x00, successfully sent data, otherwise return false
    if (winsock->SendData("websocket test") == 0x00) {
        std::cout << "client successfully sent test message to server" << std::endl;
        return true;
    }
    return false;
}

void webSocket::DiscounnectSocket() {
    // Simply leave the session
    return;
}

void webSocket::SendMessage() {
    return;
}

void webSocket::RecieveMessage() {
    return;
}