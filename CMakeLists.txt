cmake_minimum_required(VERSION 3.15)

project(EmbroideryManager CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(SOURCES
	Precomp.cpp
	Precomp.h
	Main.cpp
	File.cpp
	File.h
	UTF16.cpp
	UTF16.h
	VP3Embroidery.cpp
	VP3Embroidery.h
	VP3Viewer.cpp
	VP3Viewer.h
	VP3Painter.cpp
	VP3Painter.h
	MainWindow.cpp
	MainWindow.h
	EmbroideryManager.rc
	resource.h
	targetver.h
)

set(RESOURCES
	Resources/Icons/sewingmachine.png
	Resources/Icons/usbkey.png
	Resources/Icons/laptop.png
	Resources/Icons/delete.png
	Resources/Icons/import.png
	Resources/Icons/options.png
)

qt_standard_project_setup()
qt_add_executable(EmbroideryManager WIN32 ${SOURCES} EmbroideryManager.manifest)

target_link_libraries(EmbroideryManager PRIVATE
	Qt6::Core
	Qt6::Gui
	Qt6::Widgets
)

qt_add_resources(EmbroideryManager "EmbroideryManager" PREFIX "/" FILES ${RESOURCES})

install(TARGETS EmbroideryManager 
	RUNTIME DESTINATION "EmbroideryManager"
	BUNDLE DESTINATION "EmbroideryManager"
	LIBRARY DESTINATION "EmbroideryManager"
)
