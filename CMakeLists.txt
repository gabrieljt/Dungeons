#Change this if you need to target a specific CMake version
cmake_minimum_required(VERSION 2.8)
project(Dungeons)

# Specific compiler options - set C++11 flag for g++ and clang
if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
	
	# Mac OS X: clang uses libc++ standard library
	if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
	endif()
endif()

# Enable debug symbols by default
if(CMAKE_BUILD_TYPE STREQUAL "")
	set(CMAKE_BUILD_TYPE Debug)
endif()
# (you can also set it on the command line: -D CMAKE_BUILD_TYPE=Release)

# Set version information in a config.h file
set(Dungeons_VERSION_MAJOR 1)
set(Dungeons_VERSION_MINOR 0)
configure_file(
	"${PROJECT_SOURCE_DIR}/Source/config.h.in"
	"${PROJECT_BINARY_DIR}/config.h"
)

# Add headers to compiler
include_directories("${PROJECT_SOURCE_DIR}/Include/")

# Define Executable Source and Headers
set(SOURCE
	${PROJECT_SOURCE_DIR}/Source/Animation.cpp
	${PROJECT_SOURCE_DIR}/Source/Application.cpp
	${PROJECT_SOURCE_DIR}/Source/BloomEffect.cpp
	${PROJECT_SOURCE_DIR}/Source/Character.cpp
	${PROJECT_SOURCE_DIR}/Source/Command.cpp
	${PROJECT_SOURCE_DIR}/Source/CommandQueue.cpp
	${PROJECT_SOURCE_DIR}/Source/DataTables.cpp
	${PROJECT_SOURCE_DIR}/Source/Dungeon.cpp
	${PROJECT_SOURCE_DIR}/Source/EmitterNode.cpp
	${PROJECT_SOURCE_DIR}/Source/Entity.cpp
	${PROJECT_SOURCE_DIR}/Source/GameState.cpp
	${PROJECT_SOURCE_DIR}/Source/Main.cpp
	${PROJECT_SOURCE_DIR}/Source/MusicPlayer.cpp
	${PROJECT_SOURCE_DIR}/Source/ParticleNode.cpp
	${PROJECT_SOURCE_DIR}/Source/Player.cpp
	${PROJECT_SOURCE_DIR}/Source/PostEffect.cpp
	${PROJECT_SOURCE_DIR}/Source/SceneNode.cpp
	${PROJECT_SOURCE_DIR}/Source/SoundNode.cpp
	${PROJECT_SOURCE_DIR}/Source/SoundPlayer.cpp
	${PROJECT_SOURCE_DIR}/Source/SpriteNode.cpp
	${PROJECT_SOURCE_DIR}/Source/State.cpp
	${PROJECT_SOURCE_DIR}/Source/StateStack.cpp
	${PROJECT_SOURCE_DIR}/Source/TextNode.cpp
	${PROJECT_SOURCE_DIR}/Source/Tile.cpp
	${PROJECT_SOURCE_DIR}/Source/Utility.cpp
)

set(HEADERS
	${PROJECT_SOURCE_DIR}/Include/Game/Animation.hpp
	${PROJECT_SOURCE_DIR}/Include/Game/Application.hpp
	${PROJECT_SOURCE_DIR}/Include/Game/BloomEffect.hpp
	${PROJECT_SOURCE_DIR}/Include/Game/Category.hpp
	${PROJECT_SOURCE_DIR}/Include/Game/Character.hpp
	${PROJECT_SOURCE_DIR}/Include/Game/Command.hpp
	${PROJECT_SOURCE_DIR}/Include/Game/CommandQueue.hpp
	${PROJECT_SOURCE_DIR}/Include/Game/DataTables.hpp
	${PROJECT_SOURCE_DIR}/Include/Game/Dungeon.hpp	
	${PROJECT_SOURCE_DIR}/Include/Game/EmitterNode.hpp
	${PROJECT_SOURCE_DIR}/Include/Game/Entity.hpp
	${PROJECT_SOURCE_DIR}/Include/Game/Foreach.hpp
	${PROJECT_SOURCE_DIR}/Include/Game/GameState.hpp
	${PROJECT_SOURCE_DIR}/Include/Game/MusicPlayer.hpp
	${PROJECT_SOURCE_DIR}/Include/Game/Particle.hpp
	${PROJECT_SOURCE_DIR}/Include/Game/ParticleNode.hpp
	${PROJECT_SOURCE_DIR}/Include/Game/Player.hpp
	${PROJECT_SOURCE_DIR}/Include/Game/PostEffect.hpp
	${PROJECT_SOURCE_DIR}/Include/Game/ResourceHolder.hpp
	${PROJECT_SOURCE_DIR}/Include/Game/ResourceIdentifiers.hpp
	${PROJECT_SOURCE_DIR}/Include/Game/SceneNode.hpp
	${PROJECT_SOURCE_DIR}/Include/Game/SoundNode.hpp
	${PROJECT_SOURCE_DIR}/Include/Game/SoundPlayer.hpp
	${PROJECT_SOURCE_DIR}/Include/Game/SpriteNode.hpp
	${PROJECT_SOURCE_DIR}/Include/Game/State.hpp
	${PROJECT_SOURCE_DIR}/Include/Game/StateIdentifiers.hpp
	${PROJECT_SOURCE_DIR}/Include/Game/StateStack.hpp
	${PROJECT_SOURCE_DIR}/Include/Game/TextNode.hpp
	${PROJECT_SOURCE_DIR}/Include/Game/Tile.hpp
	${PROJECT_SOURCE_DIR}/Include/Game/Utility.hpp
)

# Define Executable
set(EXECUTABLE_NAME "Dungeons")
add_executable(${EXECUTABLE_NAME} ${SOURCE} ${HEADERS})

# Detect and add SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake-2.8/Modules/" ${CMAKE_MODULE_PATH})
#Find any version 2.X of SFML
#See the FindSFML.cmake file for additional details and instructions
find_package(SFML 2 REQUIRED system window graphics network audio)
if(SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})
endif()

# Install target
install(TARGETS ${EXECUTABLE_NAME} DESTINATION .)
file(COPY Media DESTINATION .)

# CPack packaging
include(InstallRequiredSystemLibraries)
#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_VERSION_MAJOR "${Dungeons_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${Dungeons_VERSION_MINOR}")
include(CPack)
