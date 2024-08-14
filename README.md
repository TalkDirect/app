# How to Compile & Build
1. Ensure that CMake is properly installed within your machine with g++/gcc compilers properly installed
2. Within the projects root directory enter in this command to your command line:
        ```
        cmake -S ./ -B Build -G "MinGW Makefiles"
        ```
3. Navigate to the newly created build folder
4. In the build folder write the following:
        ```
        cmake --build ./
        ```
5. Ensure that you have SDL.dll inside the location where the .exe file has been created
6. Click n Run
