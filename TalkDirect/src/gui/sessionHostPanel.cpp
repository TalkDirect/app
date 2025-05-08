#include "gui/sessionHostPanel.hpp"

sessionHostPanel::sessionHostPanel(wxWindow* currFrame)
    :wxPanel(currFrame, wxID_ANY)
{
    /* Making Text & Button GUI Elements */
    auto* welcomeText = new wxStaticText(this, wxID_ANY, "Welcome to TalkDirect, please Host/Join a Session via Session ID");
    auto* hostButton = new wxButton(this, ID_Host_Btn_Clicked, "Host");
    auto* joinButton = new wxButton(this, ID_Join_Btn_Clicked, "Join");
    sessionIDTextField = new wxTextCtrl(this, ID_Session_Code_Entered, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER | wxTE_PROCESS_TAB);
    auto* vbox = new wxBoxSizer(wxVERTICAL);

    /* Apply Any Additional Functionality/Styles */

    // Text Field Styles

    /* Start to Format GUI Elements */
    vbox->AddStretchSpacer();
    vbox->Add(welcomeText, 0, wxALIGN_CENTER, 10);
    vbox->AddStretchSpacer();
    vbox->Add(sessionIDTextField, 0, wxALIGN_CENTER, 10);
    vbox->AddStretchSpacer();
    vbox->Add(hostButton, 0, wxALIGN_CENTER, 10);
    vbox->AddStretchSpacer();
    vbox->Add(joinButton, 0, wxALIGN_CENTER, 10);
    SetSizer(vbox);

    /* EVENT HANDLING */

    Bind(wxEVT_BUTTON, &sessionHostPanel::OnHostButton, this, ID_Host_Btn_Clicked);
    Bind(wxEVT_BUTTON, &sessionHostPanel::OnJoinButton, this, ID_Join_Btn_Clicked);
    Bind(wxEVT_TEXT_ENTER, &sessionHostPanel::OnHostButton, this, ID_Session_Code_Entered);

};

void sessionHostPanel::OnHostButton(wxCommandEvent& event) {
    sessionManager* sessionMgr = currFrame->getSessionManager();
    std::string inputSessionCode = sessionIDTextField->GetValue().Trim().ToStdString();
    wxLogMessage(sessionIDTextField->GetValue().Trim());
    sessionMgr->ConnectSession(std::stol(inputSessionCode));
};

void sessionHostPanel::OnJoinButton(wxCommandEvent& event) {
    wxMessageBox("Placeholder Dialog, will be where you'll join a session",
        "TEMP", wxOK | wxICON_QUESTION);   
};