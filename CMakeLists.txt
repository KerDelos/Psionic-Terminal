cmake_minimum_required (VERSION 3.11)

# --- Fetch FTXUI --------------------------------------------------------------
include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  # Specify a GIT TAG here.
)

FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
  FetchContent_Populate(ftxui)
  add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# ------------------------------------------------------------------------------

# --- Fetch Psionic --------------------------------------------------------------
FetchContent_Declare(psionic
  GIT_REPOSITORY https://github.com/KerDelos/Psionic
)

FetchContent_MakeAvailable(psionic)
# ------------------------------------------------------------------------------

project(psionic-terminal
  LANGUAGES CXX
  VERSION 1.0.0
)

add_executable(psionic-terminal main.cpp)

target_link_libraries(psionic-terminal
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
  psionic
)

set_target_properties(psionic-terminal PROPERTIES CXX_STANDARD 17)
