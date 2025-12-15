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