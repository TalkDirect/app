add_subdirectory(./src/networking)

include_directories(${INCLUDE_DIR})

add_executable(PlayDirect_App ./main.cpp)

## Link Libraries/Directories
target_link_libraries(PlayDirect_App 
    PRIVATE 
    ${SDL2_LIBRARIES}

    ## Actual Project Directorioes/Libraries
    networking

    PUBLIC
    wsock32
    ws2_32
    )

target_link_directories(PlayDirect_App 
    PRIVATE 
    networking
    )

target_include_directories(PlayDirect_App 
    PRIVATE 
    ${SDL2_INCLUDE_DIRS}
    ${INCLUDE_DIR}
)