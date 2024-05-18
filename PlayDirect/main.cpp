#include <iostream>
#include <SDL.h>
#include <string>

#include "networking/session.hpp"
#include "networking/webSocket.hpp"

int main(int argc, char* argv[])
{
    std::cout << "Welcome to PlayDirect!" << std::endl;

    // Start up a new Session
    Session* session = new Session(500);

    system("pause");
    return 0;
}