#include "gui/sessionChatPanel.hpp"

sessionChatPanel::sessionChatPanel(wxWindow* currFrame)
    :wxPanel(currFrame, wxID_ANY)
{
    /* Making Text & Button GUI Elements */
    chatInputTextField = new wxTextCtrl(this, ID_Send_Btn_Clicked, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER | wxTE_PROCESS_TAB | wxTE_MULTILINE);
    chatTextMessages = new wxTextCtrl(this, ID_Send_Btn_Clicked, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER | wxTE_PROCESS_TAB | wxTE_READONLY | wxTE_MULTILINE);
    auto* sendButton = new wxButton(this, ID_Send_Btn_Clicked, "Send");
    auto* vbox = new wxBoxSizer(wxVERTICAL);

    /* Apply Any Additional Functionality/Styles */

    // Text Field Styles

    /* Start to Format GUI Elements */
    vbox->AddStretchSpacer();
    vbox->Add(chatInputTextField, 0, wxALIGN_CENTER | wxALL, 10);
    vbox->AddStretchSpacer();
    vbox->Add(chatTextMessages, 0, wxALIGN_CENTER | wxALL, 10);
    vbox->AddStretchSpacer();
    vbox->Add(sendButton, 0, wxALIGN_CENTER | wxALL, 10);
    vbox->AddStretchSpacer();

    SetSizer(vbox);

    /* EVENT HANDLING */
    Bind(wxEVT_BUTTON, &sessionChatPanel::OnSendButton, this, ID_Send_Btn_Clicked);
    Bind(EVT_SOCKET_DATA_RECEIVED, &sessionChatPanel::OnSessionMessageReceived, this);
};

void sessionChatPanel::OnSendButton(wxCommandEvent& event) {
    sessionManager* sessionMgr = currFrame->getSessionManager();
    assert(sessionMgr != nullptr);
    std::string inputMessageString = chatInputTextField->GetValue().ToStdString();
    
    unsigned char* buffer = new unsigned char[inputMessageString.size() + 1];
    std::memcpy(buffer, inputMessageString.c_str(), inputMessageString.size() + 1);
    sessionMgr->OnSend(buffer);

    chatTextMessages->AppendText("Message: " + inputMessageString + " \n");

    delete[] buffer;
};

void sessionChatPanel::OnSessionMessageReceived(wxThreadEvent& event) {
    chatTextMessages->AppendText("Message: " + event.GetString() + " \n");
};