cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(SkeletalSkinning)

# Add .lib files
link_directories(${CMAKE_SOURCE_DIR}/libraries)

# Add source files
file(GLOB_RECURSE SOURCE_FILES 
	${CMAKE_SOURCE_DIR}/src/*.c
	${CMAKE_SOURCE_DIR}/src/*.cpp)
	
# Add header files
file(GLOB_RECURSE HEADER_FILES 
	${CMAKE_SOURCE_DIR}/src/*.h
	${CMAKE_SOURCE_DIR}/src/*.hpp)

# Configure assets header file
include_directories(${CMAKE_BINARY_DIR}/src
		${CMAKE_SOURCE_DIR}/include/imgui
		${CMAKE_SOURCE_DIR}/include/imgui/backend)
	
# Define the executable
add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

# We need a CMAKE_DIR with some code to find external dependencies
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

#######################################
# LOOK for the packages that we need! #
#######################################

# OpenGL
find_package(OpenGL REQUIRED)

# GLM
find_package(GLM REQUIRED)

# GLFW
find_package(GLFW3 REQUIRED)

# ASSIMP
find_package(ASSIMP REQUIRED)

# ASSIMP
find_package(FreeImage REQUIRED)

# GLAD
add_library(GLAD "externals/glad.c")

# imgui
add_library(imgui externals/imgui.cpp
				  externals/imgui_draw.cpp
				  externals/imgui_widgets.cpp
		          externals/imgui_impl_glfw.cpp
		          externals/imgui_impl_opengl3.cpp
				  externals/imgui_demo.cpp
				  externals/imgui_tables.cpp)

# Put all libraries into a variable
set(LIBS glfw3 opengl32 assimp GLAD imgui FreeImage)

# Define the include DIRs
include_directories(
	"${CMAKE_SOURCE_DIR}/src"
	"${CMAKE_SOURCE_DIR}/include"
)

# Define the link libraries
target_link_libraries(${PROJECT_NAME} ${LIBS})

# Create virtual folders to make it look nicer in VS
if(MSVC_IDE)
	# Macro to preserve source files hierarchy in the IDE
	macro(GroupSources curdir)
		file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir} ${PROJECT_SOURCE_DIR}/${curdir}/*)

		foreach(child ${children})
			if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
				GroupSources(${curdir}/${child})
			else()
				string(REPLACE "/" "\\" groupname ${curdir})
				string(REPLACE "src" "Sources" groupname ${groupname})
				source_group(${groupname} FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
			endif()
		endforeach()
	endmacro()

	# Run macro
	GroupSources(src)
endif()

# Copy dlls
if(WIN32)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${PROJECT_SOURCE_DIR}/libraries"
		$<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif()