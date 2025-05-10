#include "gui/sessionManager.hpp"

sessionManager::sessionManager(wxEvtHandler* handler)
    : handler(handler)
{
    running = true;
};

sessionManager::~sessionManager() {
    DisconnectSession();
};

void sessionManager::ConnectSession(int sessionid) {  // <- Likely crashing here
    running = true;
    std::cout << "Creating session with ID: " << sessionid << std::endl;

    currSession = new Session(sessionid);  // <- EVEN MORE Likely crashing here

    sessionRecvThread = std::thread(std::bind(&sessionManager::Recv, this));
    std::cout << "Started Socket Receiver Thread" << std::endl;
};

void sessionManager::DisconnectSession() {
    running = false;
    if (sessionRecvThread.joinable()) {
        sessionRecvThread.join();
    }
    if (sessionSendThread.joinable()) {
        sessionSendThread.join();
    }
    delete currSession;
    currSession = nullptr;
};

void sessionManager::Recv() {
    while (running) {
        networkQueue<unsigned char*>* dataQueue = currSession->getNQueue();
        unsigned char* recvData = dataQueue->pop();
        
        wxThreadEvent* evt = new wxThreadEvent(EVT_SOCKET_DATA_RECEIVED_FRAME);
        evt->SetString(recvData);
        wxQueueEvent(handler, evt);
    }
};

void sessionManager::OnSend(unsigned char* data, int dataSize) {
    /* Setting up Thread */
    Send(data, dataSize);
};

void sessionManager::Send(unsigned char* data, int dataSize) {
    currSession->SendData(data, dataSize);
};