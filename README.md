# TalkDirect
A messaging app that can connect to other desktop apps or the web app based TalkDirect. Built in C++, using win32's winsock for TCP-based socket connections and wxWidgets for the GUI Component. 

# How to Use
Simply host a new session, and have another connect to it by the code you used to start hosting the session. By doing this you'll open up a connection between the two allowing you to start talking.


# Compile & Build
Note: This project is compiled such that wxWidgets is a subdirectory meaning that this will build wxWidgets whenever this project itself is rebuilt.

1. Ensure that CMake is properly installed within your machine with g++/gcc compilers properly installed
2. Install the latest version of `wxWidgets` onto your system, extract and place it within `./ext`
3. Within the projects root directory enter in this command to your command line:
        ```
        cmake -S ./ -B Build -G "MinGW Makefiles"
        ```
4. Navigate to the newly created build folder
5. In the build folder write the following:
        ```
        cmake --build ./
        ```
6. Click n Run the .exe file
