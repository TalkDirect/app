add_subdirectory(./src/networking)
add_subdirectory(./src/video)
add_subdirectory(./src/gui)

include_directories(
    ${INCLUDE_DIR}
    ${EXT_DIR}/wxWidgets/include
    ${EXT_DIR}/wxWidgets/lib/gcc_dll
)

add_executable(TalkDirect_App ./main.cpp)

## Link Libraries/Directories
target_link_libraries(TalkDirect_App 
    PRIVATE 
    ## Actual Project Directories/Libraries
    networking
    video
    gui

    PUBLIC
    wsock32
    ws2_32
    ${wxWidgets_LIBRARIES}
    SSL
    Crypto
    )

target_link_directories(TalkDirect_App 
    PRIVATE
    networking
    video
    gui
    )

target_include_directories(TalkDirect_App 
    PRIVATE 
    ${INCLUDE_DIR}
    PUBLIC
    ${EXT_DIR}/wxWidgets/include
    ${OPENSSL_INCLUDE_DIR}
)

target_compile_definitions(TalkDirect_App 
    PRIVATE
    ${wxWidgets_DEFINITIONS}
    )