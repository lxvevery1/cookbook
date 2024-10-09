cmake_minimum_required (VERSION 3.11)

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG main # Important: Specify a version or a commit hash here.
)
FetchContent_MakeAvailable(ftxui)

project(cookbook LANGUAGES CXX VERSION 1.0.0)
add_executable(cookbook src/main.cpp src/FileLoader.cpp)

include_directories(cookbook PRIVATE ${CMAKE_SOURCE_DIR}/src, ${CMAKE_SOURCE_DIR}/include )

target_link_libraries(
  cookbook
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)
