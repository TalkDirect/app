add_library(gui
    ""
)

target_sources(gui
    PRIVATE
    ./frame.cpp
    ./app.cpp
    ./sessionChatPanel.cpp
    ./sessionHostPanel.cpp
    ./customEvents.cpp
    ./sessionManager.cpp

    PUBLIC
    ${INCLUDE_DIR}/gui/frame.hpp
    ${INCLUDE_DIR}/gui/app.hpp
    ${INCLUDE_DIR}/gui/sessionChatPanel.hpp
    ${INCLUDE_DIR}/gui/sessionHostPanel.hpp
    ${INCLUDE_DIR}/gui/customEvents.hpp
    ${INCLUDE_DIR}/gui/sessionManager.hpp

)

target_include_directories(gui 
    PUBLIC 
    ${INCLUDE_DIR}
    ${EXT_DIR}/wxWidgets/include
)

target_link_libraries(gui
    PRIVATE
    networking
    )