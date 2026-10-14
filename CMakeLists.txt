cmake_minimum_required(VERSION 3.20)
project(PostalBorkenMenuImGui LANGUAGES C CXX)
enable_language(RC)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/Updater.rc" "101 RCDATA \"${CMAKE_CURRENT_SOURCE_DIR}/source/PostalUpdater.ps1\"\n")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
add_library(PostalBorkenMenu SHARED
    source/PostalBorkenMenu.cpp source/OverlayImGui.cpp source/UpdateFeature.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/Updater.rc"
    vendor/imgui/imgui.cpp vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_tables.cpp vendor/imgui/imgui_widgets.cpp
    vendor/imgui/backends/imgui_impl_win32.cpp vendor/imgui/backends/imgui_impl_dx11.cpp
    vendor/minhook/src/buffer.c vendor/minhook/src/hook.c
    vendor/minhook/src/trampoline.c vendor/minhook/src/hde/hde64.c)
target_include_directories(PostalBorkenMenu PRIVATE
    vendor/imgui vendor/imgui/backends vendor/minhook/include vendor/json)
target_compile_features(PostalBorkenMenu PRIVATE cxx_std_17)
target_compile_definitions(PostalBorkenMenu PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX PB_IMGUI UNICODE _UNICODE)
target_link_libraries(PostalBorkenMenu PRIVATE kernel32 user32 gdi32 d3d11 dxgi d3dcompiler imm32 dwmapi winhttp)
set_target_properties(PostalBorkenMenu PROPERTIES PREFIX "" SUFFIX ".asi")
