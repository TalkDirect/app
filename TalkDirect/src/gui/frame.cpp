#include "gui/frame.hpp"

Frame::Frame()
    :wxFrame(NULL, wxID_ANY, "TalkDirect")
{

    /* Making the Top Menu Bar */
    wxMenu *menuConnectivity = new wxMenu;

    wxMenu *menuConnect = new wxMenu;
    menuConnectivity->Append(ID_Socket_Connect, "&Connect");
    menuConnectivity->Append(ID_Socket_DC, "&Disconnect");

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuConnectivity, "&Connectivity");
 
    SetMenuBar( menuBar );
 
    Bind(wxEVT_MENU, &Frame::OnConnect, this, ID_Socket_Connect);
    Bind(wxEVT_MENU, &Frame::OnDiscounnect, this, ID_Socket_DC);
};

void Frame::OnExit(wxCommandEvent& event) {
    Close(true);
};

void Frame::OnConnect(wxCommandEvent& event) {
    wxMessageBox("Temporary Placeholder Message box, will present a popup asking for where you wish to connect to, along with a button for confirmation",
        "TEMP", wxOK | wxICON_EXCLAMATION);
};

void Frame::OnDiscounnect(wxCommandEvent& event) {
    wxMessageBox("Temporary Placeholder Message box, will simply just disconnect you from the socket if possible, if successful it'll display another msgbox with confirmation/failure",
        "TEMP", wxOK | wxICON_EXCLAMATION);
};