#include "gui/sessionManager.hpp"

sessionManager::sessionManager(wxEvtHandler* handler)
    : handler(handler), running(false)
{

};

sessionManager::~sessionManager() {
    DisconnectSession();
};

void sessionManager::ConnectSession(int sessionid) {  // <- Likely crashing here
    running = true;
    std::cout << "Creating session with ID: " << sessionid << std::endl;

    currSession = new Session(sessionid);  // <- EVEN MORE Likely crashing here

    sessionRecvThread = std::thread(std::bind(&sessionManager::Recv, this));
    sessionRecvThread.detach();
    std::cout << "Started Socket Receiver Thread" << std::endl;
};

void sessionManager::DisconnectSession() {
    running = false;
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
        assert(recvData != nullptr);

        wxThreadEvent* evt = new wxThreadEvent(EVT_SOCKET_DATA_RECEIVED);
        evt->SetString(recvData);
        wxQueueEvent(handler, evt);
    }
};

void sessionManager::OnSend(unsigned char* data) {
    /* Setting up Thread */
    sessionSendThread = std::thread(&sessionManager::Send, this, data);
};

void sessionManager::Send(unsigned char* data) {
    currSession->SendData(data, sizeof(data));
};