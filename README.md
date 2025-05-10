# TalkDirect

**TalkDirect** is a messaging app that enables communication between desktop clients or with the web-based TalkDirect app.  
It is built in **C++**, using **Win32 Winsock** for TCP-based socket communication and **wxWidgets** for the GUI.

---

## How to Use

1. Launch the application.
2. Choose to **host a new session**.
3. Share the generated session code with another user.
4. The other user can **connect using that code** to establish a direct connection and begin chatting.

---

## Compile & Build Instructions

> ⚠️ **Note:** You must have a properly compiled version of `wxWidgets` installed inside the `./ext` directory.

### 1. Prerequisites

- Make sure `CMake` is installed.
- Ensure `g++/gcc` (MinGW) is available in your system's PATH.

### 2. Install `wxWidgets`

1. Download the latest version of [`wxWidgets`](https://www.wxwidgets.org/downloads/) and extract it into `./ext`.
2. Open a terminal and navigate to the **root** of the extracted `wxWidgets` directory.
3. Run the following command to build:

    ```sh
    mingw32-make -f makefile.gcc BUILD=release SHARED=1 UNICODE=1
    ```

4. Once completed, verify that the required libraries and headers were generated successfully according to the [wxWidgets build guide](https://docs.wxwidgets.org).

### 3. Generate Build Files

From the root of the TalkDirect project directory:

```sh
cmake -S ./ -B Build -G "MinGW Makefiles"
```
Afterwards, naviagte into ./build and then runL
```sh
cmake --build .
```
To build the project and run the .exe file produced.