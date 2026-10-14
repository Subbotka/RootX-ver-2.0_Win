cmake_minimum_required(VERSION 3.16)

project(RootX VERSION 1 LANGUAGES CXX)


set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/Models/Ui")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/root_install")
set(ROOT_INCLUDE_DIRS "${ROOT_DIR}/include")

set(ROOT_SAFE_LIBRARIES
    "${ROOT_DIR}/lib/libCore.lib"
    "${ROOT_DIR}/lib/libGraf.lib"
    "${ROOT_DIR}/lib/libHist.lib"
    "${ROOT_DIR}/lib/libRIO.lib"
    "${ROOT_DIR}/lib/libGpad.lib"
    "${ROOT_DIR}/lib/libROOTWebDisplay.lib"
    "${ROOT_DIR}/lib/libRHTTP.lib"
    "${ROOT_DIR}/lib/libWebGui6.lib"
)


find_package(Qt6 REQUIRED COMPONENTS Widgets WebEngineCore WebEngineWidgets Concurrent)

set(PROJECT_SOURCES
    main.cpp
    Models/Lib/draggablewidget.cpp
    Models/Lib/draggablewidget.h
    Models/Lib/flowlayout.cpp
    Models/Lib/flowlayout.h
    Models/Src/mainwindow.cpp
    Models/Src/basemenu.cpp
    Models/Src/filemanager.cpp
    Models/Src/executableobjects.cpp
    Models/Src/sideobjects.cpp
    Models/Src/imagecontroller.cpp
    Models/Src/elements.cpp
    Models/Src/gamelauncher.cpp
    Models/Src/loading.cpp
    Models/Src/notify.cpp
    Models/Src/folderobject.cpp
    Models/Src/tooltabbar.cpp

    Models/Src/Editor/code.cpp
    Models/Src/Editor/editor.cpp
    Models/Src/Editor/entitylist.cpp
    Models/Src/Editor/tree.cpp
    Models/Src/Editor/property.cpp

    Models/Head/mainwindow.h
    Models/Head/basemenu.h
    Models/Head/executableobjects.h
    Models/Head/sideobjects.h
    Models/Head/filemanager.h
    Models/Head/imagecontroller.h
    Models/Head/elements.h
    Models/Head/editor.h
    Models/Head/gamelauncher.h
    Models/Head/loading.h
    Models/Head/notify.h
    Models/Head/tooltabbar.h

    Models/Ui/mainwindow.ui
    Models/Ui/basemenu.ui
    Models/Ui/tooltabbar.ui
    Models/Ui/executableobjects.ui
    Models/Ui/loading.ui
    Models/Ui/sideobjects.ui
    Models/Ui/editor.ui
)

qt_add_executable(RootX
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    rec.qrc
    Models/Lib/Spreadsheet/printview.cpp
    Models/Lib/Spreadsheet/printview.h
    Models/Lib/Spreadsheet/spreadsheet.cpp
    Models/Lib/Spreadsheet/spreadsheet.h
    Models/Lib/Spreadsheet/spreadsheetdelegate.cpp
    Models/Lib/Spreadsheet/spreadsheetdelegate.h
    Models/Lib/Spreadsheet/spreadsheetitem.cpp
    Models/Lib/Spreadsheet/spreadsheetitem.h
    Models/Lib/TCanvasWidget.h
    Models/Lib/TCanvasWidget.cpp


)
if(WIN32)
    target_include_directories(RootX PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Models/Lib/KodeTerm)

    target_link_libraries(RootX PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/Models/Lib/KodeTerm/KodoTerm.lib"
        "${CMAKE_CURRENT_SOURCE_DIR}/Models/Lib/KodeTerm/vterm.lib"
    )
endif()

target_include_directories(RootX PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Models/Lib/QAdvanceddocking)
target_link_libraries(RootX PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Models/Lib/QAdvanceddocking/qtadvanceddocking-qt6.lib")

target_include_directories(RootX PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Models/Lib/QPropertybrowser)
target_link_libraries(RootX PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Models/Lib/QPropertybrowser/qtpropertybrowser.lib")




set(QSCINTILLA_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Models/Lib/Scintilla/scintilla-qt-edit.lib")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Models/Lib/Scintilla")

target_include_directories(RootX PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}" "${ROOT_INCLUDE_DIRS}")

target_link_libraries(RootX
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::PrintSupport
        Qt6::WebEngineCore
        Qt6::WebEngineWidgets
        Qt6::Concurrent
        ${ROOT_SAFE_LIBRARIES}
        ${QSCINTILLA_LIB_PATH}
)

set_target_properties(RootX PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(WIN32)
    # Копия ROOT в подпапку build-каталога (bin доступен по PATH).
    add_custom_command(TARGET RootX POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${ROOT_DIR}"
        "$<TARGET_FILE_DIR:RootX>/root_install"
        COMMENT "Copying ROOT runtime tree (root_install/"
    )
    add_custom_command(TARGET RootX POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/Models/Lib/Scintilla/scintilla-qt-edit.dll"
        "$<TARGET_FILE_DIR:RootX>/scintilla-qt-edit.dll"
        COMMENT "Copying Scintilla Qt DLL next to RootX.exe"
    )
    # Авторазвёртка Qt DLL рядом с exe (windeployqt)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${QT6_INSTALL_PREFIX}/bin")
    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET RootX POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                    --no-compiler-runtime
                    "$<TARGET_FILE:RootX>"
            COMMENT "Running windeployqt for Qt runtime DLLs"
        )
    endif()
endif()

# PATH для запуска: folder root_install/bin указывает Qt Creator куда смотреть
# за ROOT DLL. Здесь задаём переменную, но не PATH в системе.
set(ROOTX_BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/root_install/bin")

include(GNUInstallDirs)
install(TARGETS RootX
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(RootX)