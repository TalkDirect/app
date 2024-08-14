#include <iostream>
#include <SDL.h>
#include <string>

#include "networking/session.hpp"
#include "networking/webSocket.hpp"

int main(int argc, char* argv[])
{
    std::cout << "Welcome to PlayDirect!, please enter your desired SessionID" << std::endl;
    char desiredSessionID[3];
    std::cin >> desiredSessionID;

    // Start up a new Session
    Session* session = new Session(std::atol(desiredSessionID));

    // This is pretty much the main loop of the code, will contain all of the logic needed to preform websocket tasks
    do
    {
        session->ExecuteTasks();
    } while (session->getWebSocket()->readyState == 0x01 || session->getWebSocket()->readyState == 0x02);
    
    std::cin.get();
    return 0;
}