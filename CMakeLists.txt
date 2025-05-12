cmake_minimum_required(VERSION 3.5)
project(TalkDirect_App CXX)

set(CMAKE_CXX_COMPILER "C:/msys64/ucrt64/bin/g++.exe")
set(CMAKE_C_COMPILER "C:/msys64/ucrt64/bin/gcc.exe")

# === WXWIDGETS CONFIG ===
option(wxBUILD_EXAMPLES "Disable wxWidgets examples" OFF)
option(wxBUILD_TESTS "Disable wxWidgets tests" OFF)

set(wxWidgets_ROOT_DIR "${CMAKE_SOURCE_DIR}/ext/wxWidgets")
set(wxWidgets_LIB_DIR "${wxWidgets_ROOT_DIR}/lib/gcc_dll")
set(wxWidgets_CONFIGURATION mswu)

# == OPENSSL CONFIG ===
set(OPENSSL_ROOT_DIR "${CMAKE_SOURCE_DIR}/ext/OpenSSL")
set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")
set(OPENSSL_LIBRARIES  "${OPENSSL_ROOT_DIR}/lib")
set(OPENSSL_CRYPTO_LIBRARY "${OPENSSL_LIBRARIES}/libcrypto.a")
set(OPENSSL_SSL_LIBRARY "${OPENSSL_ROOT_DIR}/lib/libssl.a")

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(EXT_DIR ${CMAKE_SOURCE_DIR}/ext)

## Libraries
find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})
add_subdirectory(TalkDirect)