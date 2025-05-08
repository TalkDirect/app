#include "gui/frame.hpp"

Frame::Frame()
    :wxFrame(NULL, wxID_ANY, "TalkDirect")
{
    sessionMgr = new sessionManager(this);
    /* Making the Top Menu Bar */
    wxMenu *menuConnectivity = new wxMenu;

    menuConnectivity->Append(ID_Socket_Connect, "&Connect");
    menuConnectivity->Append(ID_Socket_DC, "&Disconnect");

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuConnectivity, "&Connectivity");
 
    SetMenuBar( menuBar );
 
    Bind(wxEVT_MENU, &Frame::OnConnect, this, ID_Socket_Connect);
    Bind(wxEVT_MENU, &Frame::OnDiscounnect, this, ID_Socket_DC);

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
    Layout();
}

void Frame::ShowSessionChatPanel(Frame* frame) {
    if (currPanel != nullptr) {
        currPanel->Destroy();
    }
    currPanel = new sessionChatPanel(this);
    Layout();
}

void Frame::OnExit(wxCommandEvent& event) {
    Close(true);
};

void Frame::OnConnect(wxCommandEvent& event) {
    ShowSessionChatPanel(this);
};

void Frame::OnDiscounnect(wxCommandEvent& event) {
    ShowSessionHostPanel(this);
};