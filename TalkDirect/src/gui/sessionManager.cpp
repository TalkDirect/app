#include "gui/sessionManager.hpp"

sessionManager::sessionManager(wxEvtHandler* handler)
    : handler(handler)
{
    running = true;
};

sessionManager::~sessionManager() {
    DisconnectSession();
    delete currSession;
};

void sessionManager::ConnectSession(int sessionid) {
    running = true;
    std::cout << "Creating session with ID: " << sessionid << std::endl;

    currSession = new Session(sessionid);

    sessionRecvThread = std::thread(std::bind(&sessionManager::Recv, this));
    std::cout << "Started Socket Receiver Thread" << std::endl;
};

void sessionManager::DisconnectSession() {
    running = false;
    std::cout << "GUI Receieved Request to End Session, beginning the complete shutdown process." << std::endl;
    //std::cout << "Joined GUI's Recv Thread, moving towards currSession Logic.";
    currSession->CloseSession();
    std::cout << "Now deleting current session." << std::endl;
    delete currSession;
};

void sessionManager::Recv() {
    networkQueue<unsigned char*>* dataQueue = currSession->getNQueue();
    while (running) {
        unsigned char* recvData = dataQueue->pop();

        // First check to see if data if string or a file
        // Remember if file to decrement max array size by one since last byte was originally reserved for string null character
        unsigned int dataId = (unsigned int)recvData[0];

        // Decode data length
        unsigned int messageLength = 0; // Don't forget to subtract by one to remove DataID
        int counter = 1;
        for (int i = 0; i < 8; i++) {
            messageLength = (messageLength << 8) | (static_cast<uint8_t>(recvData[counter++]));
        }
        
        // Make the final buffer to be used
        unsigned char* buffer = new unsigned char[messageLength];

        // Now get the subarray for use in real use
        std::cout << "Length of GUI Received Message: " << messageLength << std::endl;

        wxThreadEvent* evt = new wxThreadEvent(EVT_SOCKET_DATA_RECEIVED_FRAME);
        
        switch (dataId) {
            case 2://String
                std::memcpy(buffer, recvData+counter, messageLength); // Only copy the actual payload
                std::cout << "GUI Recevied String" << std::endl;
                evt->SetPayload(buffer);
                evt->SetInt(dataId);
                wxQueueEvent(handler, evt);
                break;
            case 3://File
                std::cout << "GUI Received File" << std::endl;
                std::memcpy(buffer, recvData, messageLength+9); // Copy everything over BUT DataID byte
                evt->SetPayload(buffer);
                evt->SetInt(dataId);
                wxQueueEvent(handler, evt);
                break;
        }
    }
};

void sessionManager::OnSend(unsigned char* data, u_int64 dataSize) {
    /* Setting up Thread */
    Send(data, dataSize);
};

void sessionManager::Send(unsigned char* data, u_int64 dataSize) {
    currSession->SendData(data, dataSize);
};