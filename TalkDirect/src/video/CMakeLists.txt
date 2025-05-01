add_library(video
    ""
)

target_sources(video
    PRIVATE
    ./video.cpp
    ./winScreenCap.cpp

    PUBLIC
    ${INCLUDE_DIR}/video/video.hpp
    ${INCLUDE_DIR}/video/winScreenCap.hpp
)

target_include_directories(video 
    PUBLIC 
    ${INCLUDE_DIR}
    )