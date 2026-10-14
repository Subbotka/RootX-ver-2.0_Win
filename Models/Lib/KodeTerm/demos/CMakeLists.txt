cmake_minimum_required(VERSION 3.16)

project(KodoTermDemo VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(KODETERM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")

qt_add_executable(KodoTermSimple
    MANUAL_FINALIZATION
    KodoTermSimple.cpp
)

qt_add_executable(KodoShell
    MANUAL_FINALIZATION
    KodoShell.cpp
)

foreach(target KodoTermSimple KodoShell)
    target_include_directories(${target} PRIVATE "${KODETERM_DIR}")
    target_link_libraries(${target} PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        "${KODETERM_DIR}/KodoTerm.lib"
        "${KODETERM_DIR}/vterm.lib"
    )
    set_target_properties(${target} PROPERTIES WIN32_EXECUTABLE FALSE)
endforeach()

qt_finalize_executable(KodoTermSimple)
qt_finalize_executable(KodoShell)