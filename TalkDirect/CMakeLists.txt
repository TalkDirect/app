add_subdirectory(./src/networking)
add_subdirectory(./src/video)

include_directories(${INCLUDE_DIR})

add_executable(TalkDirect_App ./main.cpp)

## Link Libraries/Directories
target_link_libraries(TalkDirect_App 
    PRIVATE 
    ${SDL2_LIBRARIES}

    ## Actual Project Directories/Libraries
    networking
    video

    PUBLIC
    wsock32
    ws2_32
    )

target_link_directories(TalkDirect_App 
    PRIVATE 
    networking
    video
    )

target_include_directories(TalkDirect_App 
    PRIVATE 
    ${SDL2_INCLUDE_DIRS}
    ${INCLUDE_DIR}
)