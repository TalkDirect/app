#include "networking/session.hpp"
#include "video/video.hpp"

Session::Session() {
    Session(500);
};

Session::Session(int SessionID) {
    if (SessionID < 0)
        return;
    CreateSession(SessionID);
    sessionActive.store(true);
};

Session::~Session() {
    std::cout << "Now deleting current websocket." << std::endl;
    delete websocket;
};

/*void Session::execute() {
    std::thread RecvThread(std::bind(&Session::ReceiveData, this));
    RecvThread.detach();
    std::cout << "Started Socket Receiver Thread" << std::endl;

    // For now I'll settle for just sending data continously within the function like this; wanna somehow decouple it tho, maybe by having a sendbuf's memory address
    // passed in via pointer
    unsigned char sendbuf_1[] = {0x02, 0x74, 0x65, 0x73, 0x74, 0x69, 0x6E, 0x67, 0x20, 0x74, 0x68, 0x72, 0x65, 0x61, 0x64, 0x73};
    while (sessionActive && websocket->validSocket()) {
        SendData(sendbuf_1, sizeof(sendbuf_1));
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "Exiting execute Function" << std::endl;
};*/

void Session::CreateSession(int SessionID) {
    // First, get a new websocket up and running
    websocket = new webSocket(SessionID);
    auto bound_thread = std::bind(&Session::ReceiveData, this);
    RecvThread = std::thread(bound_thread);

};

// TODO: Not a fan of this function being a void, will have to edit this later to make it a bool so its more useful
void Session::CloseSession() {
    // Mark This session inactive and flush out the queue
    sessionActive.store(false);
    nQueue.empty();
    std::cout << "Starting Disconnecting Socket Process from Current Session." << std::endl;
    // Start to disconnect the websocket
    websocket->DiscounnectSocket();
    
    // Attempt to join thread back with main thread once thread running receiveData() sucessfully leaves
    if (RecvThread.joinable()) {
        RecvThread.join();
    }
};

void Session::ReceiveData() {
    // Slight reminder; first byte is the dataID byte signaling if Text, Video, Audio, packet, ignoring for now
    while (sessionActive.load(std::memory_order_relaxed) && websocket->validSocket()) {
        unsigned char* receivedData = Session::websocket->onRetrieveMessage();

        if (receivedData == nullptr || !Session::websocket->validSocket() || !sessionActive.load()) {
            break;
        }

        if (!checkEmptyBuffer(receivedData)) {
            nQueue.push(receivedData);
            std::cout << receivedData << std::endl;
        }
    }
    std::cout << "Exiting Current Session's Recv Function Thread" << std::endl;
};

void Session::SendData(unsigned char* data, int dataSize) {
    Session::websocket->onSendMessage(data, dataSize);
};

bool Session::isActive() {
    return Session::sessionActive;
};

webSocket* Session::getWebSocket() {
    return Session::websocket;
};

networkQueue<unsigned char*>* Session::getNQueue() {
    return &nQueue;
};

// Checks for empty buffer by just checking if all bytes in the two pointers are 0 or not
bool Session::checkEmptyBuffer(unsigned char* buffer) {
    char zero_buffer[sizeof(buffer)]; 
    std::memset(zero_buffer, 0, sizeof(buffer)); 
    return std::memcmp(buffer, zero_buffer, sizeof(buffer)) == 0;
}