add_library(networking
    ""
)

target_sources(networking
    PRIVATE
    ./session.cpp
    ./webSocket.cpp
    ./winsock.cpp

    PUBLIC
    ${INCLUDE_DIR}/networking/session.hpp
    ${INCLUDE_DIR}/networking/webSocket.hpp
    ${INCLUDE_DIR}/networking/winsock.hpp
    ${INCLUDE_DIR}/networking/networkQueue.hpp
)

target_include_directories(networking 
    PUBLIC 
    ${INCLUDE_DIR}
    )