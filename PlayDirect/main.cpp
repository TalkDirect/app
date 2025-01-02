#include <iostream>
#include <SDL.h>

#include "networking/session.hpp"

int main(int argc, char* argv[]) {
    
    std::cout << "Welcome!, please enter your desired SessionID (Max Length of 3 Numbers)" << std::endl;
    char desiredSessionID[3];
    std::cin >> desiredSessionID;

    // Start up a new Session
    Session* session = new Session(std::atol(desiredSessionID));

    // some test bytes inna buffer
    //unsigned char sendbuf[] = {0x02, 0x74, 0x65, 0x73, 0x74, 0x20, 0x66, 0x72, 0x6F, 0x6D, 0x20, 0x64, 0x65, 0x73, 0x6B, 0x74, 0x6F, 0x70, 0x20, 0x61, 0x70, 0x70};
    
    session->execute();
    std::this_thread::sleep_for(std::chrono::seconds(10));
    session->~Session();
    
    return 0;
}