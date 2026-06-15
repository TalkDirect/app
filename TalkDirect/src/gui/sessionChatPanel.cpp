#include "gui/sessionChatPanel.hpp"


#define SOCKET_STRING_DATA_IDENTIFIER 0x02
#define SOCKET_FILE_DATA_IDENTIFIER 0x03
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

    this->SetDropTarget(new MyFileDropTarget(this));
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

void sessionChatPanel::OnSessionMessageReceived(wxThreadEvent& event) {
    // Check to see if event is a File being received or not
    int EventId = event.GetInt();
    std::cout << "Data ID: " << EventId << std::endl;
    switch (EventId) {
        case 2://String
            chatTextMessages->AppendText("Message: " + wxString(event.GetPayload<unsigned char*>()) + " \n");
            break;
        case 3://File
            sessionChatPanel::OnFileReceive(event);
            break;
    }
};

void sessionChatPanel::OnFileSend(const wxString& filePath) {
    wxFile file;
    if (!file.Open(filePath, wxFile::read)) {
        wxMessageBox("Could not open file: " + filePath, "File Error", wxOK | wxICON_ERROR);
        return;
    }

    sessionManager* sessionMgr = currFrame->getSessionManager();

    // Get Size & create buffer to hold file data
    wxUint64 totalSize = file.Length();
    wxString* fileData = new wxString();
    
    // Extract File Data
    bool fileDataExtraction = file.ReadAll(fileData);    

    // Set up needed variables to send data over websockets
    int bufferSize = totalSize + 1 + 1; // extra plus one is for dataID
    unsigned char* buffer = new unsigned char[bufferSize];

    // Set the first bit to be our proper Data identifier for files
    std::memset(buffer, SOCKET_FILE_DATA_IDENTIFIER, 1);

    // Start to copy over the c string down into the buffer to be sent out via sessionManager
    std::memcpy(buffer+1, fileData->c_str(), bufferSize-1);
    sessionMgr->OnSend(buffer, bufferSize);

    delete[] buffer;
};

void sessionChatPanel::OnFileReceive(wxThreadEvent& event) {
    const char* FileName = "talkdirect-app-download.txt";
    wxFile file;
    
    // Just pull out the length from eventData
    unsigned char* eventData = event.GetPayload<unsigned char*>();
    unsigned int payloadLength = 0;
    int counter = 1;

    for (int i = 0; i < 8; i++) {
            payloadLength = (payloadLength << 8) | (static_cast<uint8_t>(eventData[counter++]));
    }
    // unsigned char* buffer = new unsigned char[payloadLength];
    // std::memcpy(buffer, eventData+8, payloadLength);
    // std::cout << "Buffer to write to file: " << buffer << std::endl;


    if (file.Create(FileName, true)) 
    {   
        // file.Write(event.GetString()); 
        file.Write(eventData + counter, payloadLength);
        file.Close();

        // Clean up the heap allocation passed from the worker thread
        delete[] eventData;
    } 
    else 
    {
        wxMessageBox("Failed to create the file.", "Error", wxOK | wxICON_ERROR);

        // Clean up the heap allocation passed from the worker thread
        delete[] eventData;
    }
};