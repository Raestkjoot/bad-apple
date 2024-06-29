cmake_minimum_required(VERSION "3.22.2")

project(mp4_to_jpeg)

add_executable(mp4_to_jpeg  "MP4_to_JPEG/test_with_cat_image.cpp" "MP4_to_JPEG/main.cpp")

set (OpenCV_DIR "D:/Datalogi_KU/_Graphics/badapple/opencv/build")
find_package( OpenCV REQUIRED )
target_link_libraries( mp4_to_jpeg ${OpenCV_LIBS} )

# Copy project data to output path
file(COPY Data DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
file(COPY Data DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release)
file(COPY Data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)