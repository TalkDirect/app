#include "gui/frame.hpp"

Frame::Frame()
    :wxFrame(NULL, wxID_ANY, "TalkDirect", wxDefaultPosition, wxSize(300,400))
{
    sessionMgr = new sessionManager(this);
    /* Making the Top Menu Bar */
    wxMenu *menuConnectivity = new wxMenu;

    //menuConnectivity->Append(ID_Socket_Connect, "&Connect");
    menuConnectivity->Append(ID_Socket_DC, "&Disconnect");

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuConnectivity, "&Connectivity");
 
    SetMenuBar( menuBar );
 
    Bind(wxEVT_MENU, &Frame::OnConnect, this, ID_Socket_Connect);
    Bind(wxEVT_MENU, &Frame::OnDiscounnect, this, ID_Socket_DC);
    Bind(EVT_SOCKET_DATA_RECEIVED_FRAME, &Frame::OnSocketRecv, this);

    /* Show the SessionHostPanel Upon Construction*/

    ShowSessionHostPanel(this);
};

sessionManager* Frame::getSessionManager() {
    return sessionMgr;
};

void Frame::ShowSessionHostPanel(Frame* frame) {
    if (currPanel != nullptr) {
        currPanel->Destroy();
    }

    currPanel = new sessionHostPanel(this);
    this->SetSize(300, 400);
    Layout();
}

void Frame::ShowSessionChatPanel(Frame* frame) {
    if (currPanel != nullptr) {
        currPanel->Destroy();
    }

    currPanel = new sessionChatPanel(this);
    this->SetSize(900, 600);
    Layout();
}

void Frame::OnExit(wxCommandEvent& event) {
    Close(true);
};

void Frame::OnConnect(wxCommandEvent& event) {
    ShowSessionChatPanel(this);
};

void Frame::OnDiscounnect(wxCommandEvent& event) {
    sessionMgr->DisconnectSession();
    ShowSessionHostPanel(this);
};

void Frame::OnSocketRecv(wxThreadEvent& event) {
    wxThreadEvent* evt = new wxThreadEvent(EVT_SOCKET_DATA_RECEIVED_PANEL);
    switch (event.GetInt()) {
        case 2://String
            evt->SetPayload(event.GetPayload<unsigned char*>());
            evt->SetInt(event.GetInt());
            break;
        case 3://File
            evt->SetPayload(event.GetPayload<unsigned char*>());
            evt->SetInt(event.GetInt());
            break;
    }
    wxQueueEvent(currPanel, evt);
}