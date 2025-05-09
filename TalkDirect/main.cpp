#include <iostream>
#include <wx/wx.h>

#include "networking/session.hpp"
#include "gui/app.hpp"

wxIMPLEMENT_APP(App);

/*int main(int argc, char* argv[]) {
    
    std::cout << "Welcome!, please enter your desired SessionID (Max Length of 3 Numbers)" << std::endl;
    char desiredSessionID[3];
    std::cin >> desiredSessionID;

    // Start up a new Session
    Session* session = new Session(std::atol(desiredSessionID));
    
    session->execute();   
    return 0;
}*/