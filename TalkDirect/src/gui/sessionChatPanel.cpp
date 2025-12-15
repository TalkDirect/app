#include "gui/sessionChatPanel.hpp"


#define SOCKET_STRING_DATA_IDENTIFIER 0x02
#define BASE_BOX_WIDTH 500

sessionChatPanel::sessionChatPanel(Frame* currFrame)
    :wxPanel(currFrame, wxID_ANY)
{

    sessionChatPanel::currFrame = currFrame;
    /* Making Text & Button GUI Elements */
    wxSize chatTextBoxSize = wxSize(BASE_BOX_WIDTH, 800);
    wxSize chatInputTextBoxSize = wxSize(BASE_BOX_WIDTH, 45);

    chatInputTextField = new wxTextCtrl(this, ID_Send_Btn_Clicked, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER | wxTE_PROCESS_TAB | wxTE_MULTILINE);
    chatTextMessages = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER | wxTE_PROCESS_TAB | wxTE_READONLY | wxTE_MULTILINE);
    auto* sendButton = new wxButton(this, ID_Send_Btn_Clicked, "Send");
    auto* primaryVBox = new wxBoxSizer(wxVERTICAL);
    auto* inputHBox = new wxBoxSizer(wxHORIZONTAL);
    /* Apply Any Additional Functionality/Styles */

    // Text Field Styles

    /* Start to Format GUI Elements */

    /* Main Chat Sizer*/
    primaryVBox->Add(chatTextMessages, 1, wxEXPAND | wxALL, 10);

    /* Input Chat Sizer*/
    inputHBox->Add(chatInputTextField, 1, wxEXPAND);
    inputHBox->Add(sendButton, 0, wxLEFT, 5);

    primaryVBox->Add(inputHBox, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    SetSizer(primaryVBox);
    primaryVBox->Fit(this);
    primaryVBox->SetSizeHints(this);

    /* EVENT HANDLING */
    Bind(wxEVT_BUTTON, &sessionChatPanel::OnSendButton, this, ID_Send_Btn_Clicked);
    Bind(EVT_SOCKET_DATA_RECEIVED_PANEL, &sessionChatPanel::OnSessionMessageReceived, this);

    /* RESIZING WINDOW FRAME TO MATCH UP WITH CONTENT*/
};

void sessionChatPanel::OnSendButton(wxCommandEvent& event) {
    // Get sessionManager wrapper
    sessionManager* sessionMgr = currFrame->getSessionManager();

    // Get string message and start to setup the char buffer for it with proper length
    std::string inputMessageString = chatInputTextField->GetValue().ToStdString();
    int bufferSize = inputMessageString.size() + 1 + 1; // extra plus one is for dataID
    unsigned char* buffer = new unsigned char[bufferSize];
    
    // Get the proper data identifier for the char buffer (string in this case so use string dataID previously defined)

    /* BUILDING OUR BUFFER TO BE SENT OUT */
    // Set the first bit to be our proper Data identifier for strings
    std::memset(buffer, SOCKET_STRING_DATA_IDENTIFIER, 1);

    // Start to copy over the c string down into the buffer to be sent out via sessionManager
    std::memcpy(buffer+1, inputMessageString.c_str(), bufferSize-1);
    sessionMgr->OnSend(buffer, bufferSize);

    // For now, just add on the string we sent onto the chat window
    //chatTextMessages->AppendText("Message: " + inputMessageString + " \n");

    delete[] buffer;
};

void sessionChatPanel::OnSessionMessageReceived(wxCommandEvent& event) {
    chatTextMessages->AppendText("Message: " + event.GetString() + " \n");
};