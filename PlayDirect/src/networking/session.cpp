#include "networking/session.hpp"


Session::Session() {
    Session(500);
};

Session::Session(int SessionID) {
    if (SessionID < 0)
        return;
    CreateSession(SessionID);
    Session::sessionActive = true;
};

Session::~Session() {
    Session::sessionActive = false;
    websocket->~webSocket();
    delete Session::websocket;
};

void Session::execute() {
    std::thread RecvThread(std::bind(&Session::RecieveData, this));
    RecvThread.detach();
    std::cout << "Started Socket Receiver Thread" << std::endl;

    // For now I'll settle for just sending data continously within the function like this; wanna somehow decouple it tho, maybe by having a sendbuf's memory address
    // passed in via pointer
    unsigned char sendbuf[] = {0x02, 0x74, 0x65, 0x73, 0x74, 0x69, 0x6E, 0x67, 0x20, 0x74, 0x68, 0x72, 0x65, 0x61, 0x64, 0x73};
    while (sessionActive && websocket->validSocket()) {
        SendData(sendbuf, sizeof(sendbuf));
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    std::cout << "Exiting execute Function" << std::endl;
}

void Session::CreateSession(int SessionID) {
    // First, get a new websocket up and running
    websocket = new webSocket(SessionID);
};

void Session::RecieveData() {
    // Slight reminder; first byte is the dataID byte signaling if Text, Video, Audio, packet, ignoring for now
    while (sessionActive && websocket->validSocket()) {
        unsigned char* recievedData = Session::websocket->onRetrieveMessage();
        std::cout << recievedData << std::endl;
        delete recievedData;
    }
    std::cout << "Ended RecieveThread" << std::endl;
};

void Session::SendData(unsigned char* data, int dataSize) {
    Session::websocket->onSendMessage(data, dataSize);
}

boolean Session::isActive() {
    return Session::sessionActive;
}

webSocket* Session::getWebSocket() {
    return Session::websocket;
}