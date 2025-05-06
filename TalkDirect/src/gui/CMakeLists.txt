add_library(gui
    ""
)

target_sources(gui
    PRIVATE
    ./frame.cpp
    ./app.cpp
    ./panel.cpp

    PUBLIC
    ${INCLUDE_DIR}/gui/frame.hpp
    ${INCLUDE_DIR}/gui/app.hpp
    ${INCLUDE_DIR}/gui/panel.hpp


)

target_include_directories(gui 
    PUBLIC 
    ${INCLUDE_DIR}
    ${EXT_DIR}/wxWidgets/include
    )