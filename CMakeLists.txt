cmake_minimum_required(VERSION 3.28)
project(rotate LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#set(CMAKE_BUILD_TYPE Debug)
#set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
#set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 40fc524) # clone up until this commit in the 2.6.x brach) 
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(Box2D
    GIT_REPOSITORY https://github.com/erincatto/box2d.git
    GIT_TAG v2.4.0
    GIT_SHALLOW ON # only need a shallow copy (excludes full git history) (only clone current state)
    EXCLUDE_FROM_ALL) # box2d doesn't work without this
FetchContent_MakeAvailable(Box2D)

FetchContent_Declare(Candle
    GIT_REPOSITORY https://github.com/MiguelMJ/Candle.git
    GIT_TAG v1.0.2
    GIT_SHALLOW ON)
FetchContent_MakeAvailable(Candle)

add_executable(rotate 
    src/main.cpp 
    src/Game.cpp 
    src/Player.cpp 
    src/Box.cpp 
    src/BoxMap.cpp
    src/BoxParticles.cpp
    src/Fade.cpp
    src/Hud.cpp
    src/ResourceManager.cpp)

target_include_directories(rotate PRIVATE ${box2d_SOURCE_DIR}/include)
target_include_directories(rotate PRIVATE ${candle_SOURCE_DIR}/include)

target_link_libraries(rotate PRIVATE sfml-graphics sfml-window sfml-system box2d Candle-s)
target_compile_features(rotate PRIVATE cxx_std_17)

add_custom_command(
    TARGET rotate
    COMMENT "Copy assets and levels directory"
    POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:rotate>/assets
    VERBATIM
    POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/levels $<TARGET_FILE_DIR:rotate>/levels
    VERBATIM)

if(WIN32)
    add_custom_command(
        TARGET rotate
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:rotate>
        VERBATIM)
endif()
