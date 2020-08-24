cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 17)
project(THEMEed)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

# source code
set(SOURCES
	main.cpp

# libraries:
	libs/ImGuiColorTextEdit/TextEditor.cpp
	libs/ImGuiFileDialog/ImGuiFileDialog.cpp
	libs/imgui/imgui_draw.cpp
	libs/imgui/imgui_widgets.cpp
	libs/imgui/imgui_demo.cpp
	libs/imgui/imgui.cpp
	libs/imgui/examples/imgui_impl_opengl3.cpp
	libs/imgui/examples/imgui_impl_sdl.cpp
)

# sdl2
if(WIN32 OR APPLE OR NOT ${USE_PKG_CHECK_SDL})
  find_package(SDL2 REQUIRED)
else()
  pkg_check_modules(SDL2 REQUIRED sdl2)
endif()

# opengl
find_package(OpenGL REQUIRED)

# glew
find_package(GLEW REQUIRED)

# create executable
add_executable(THEMEed ${SOURCES})

# properties
set_target_properties(THEMEed PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)


# include directories
target_include_directories(THEMEed PRIVATE ${SDL2_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS})
target_include_directories(THEMEed PRIVATE libs)

# link libraries 
target_link_libraries(THEMEed ${OPENGL_LIBRARIES})

# link SpvGenTwo
if (BUILD_IMMEDIATE_MODE)
	target_link_libraries(THEMEed SpvGenTwoLib SpvGenTwoCommon)
endif()



if(WIN32)
	# link specific win32 libraries
	target_link_libraries(THEMEed GLEW::GLEW SDL2::SDL2 SDL2::SDL2main)

elseif(UNIX AND NOT APPLE)
	# link linux libraries
	target_link_libraries(THEMEed ${GLEW_LIBRARIES} ${SDL2_LIBRARIES} ${GTK_LIBRARIES} ${CMAKE_DL_LIBS} stdc++fs)
elseif(APPLE)
	target_link_libraries(THEMEed GLEW::GLEW ${SDL2_LIBRARIES} ${GTK_LIBRARIES} ${CMAKE_DL_LIBS})
endif()

if (NOT MSVC)
	target_compile_options(THEMEed PRIVATE -Wno-narrowing)
endif()

