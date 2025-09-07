cmake_minimum_required(VERSION 3.15)
project(EZImageManipulator VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Ensure proper handling of Qt debug/release configurations
if(WIN32)
    set(CMAKE_DEBUG_POSTFIX "d")
    set(CMAKE_MAP_IMPORTED_CONFIG_MINSIZEREL Release)
    set(CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release)
endif()

if(MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

find_package(Qt6 COMPONENTS
    Core
    Gui
    Widgets
    REQUIRED
)

include(FetchContent)
FetchContent_Declare(
    libwebp
    GIT_REPOSITORY https://github.com/webmproject/libwebp.git
    GIT_TAG v1.6.0 
)
set(WEBP_BUILD_CWEBP OFF CACHE BOOL "" FORCE)
set(WEBP_BUILD_DWEBP OFF CACHE BOOL "" FORCE)
set(WEBP_BUILD_GIF2WEBP OFF CACHE BOOL "" FORCE)
set(WEBP_BUILD_IMG2WEBP OFF CACHE BOOL "" FORCE)
set(WEBP_BUILD_WEBPMUX OFF CACHE BOOL "" FORCE)
set(WEBP_BUILD_VWEBP OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(libwebp)


# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

# Source files
set(SOURCES
    src/main.cpp
    src/ImageEditor.cpp
    src/WebPHandler.cpp
    src/CropRectItem.cpp
    src/CropTool.cpp
    src/OpenSaveTool.cpp
    src/ResizeTool.cpp
    src/RotateFlipTool.cpp
    src/ZoomTool.cpp
)

# Header files
set(HEADERS
    include/ImageEditor.hpp
    include/WebPHandler.hpp
    include/CropRectItem.hpp
    include/ImageTool.hpp
    include/CropTool.hpp
    include/OpenSaveTool.hpp
    include/ResizeTool.hpp
    include/RotateFlipTool.hpp
    include/ZoomTool.hpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    webp
)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Copy Qt DLLs to output directory
if(WIN32)
    # Find windeployqt
    get_target_property(QT_CORE_LOCATION Qt6::Core LOCATION)
    get_filename_component(QT_BIN_DIR ${QT_CORE_LOCATION} DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS ${QT_BIN_DIR})

    if(WINDEPLOYQT_EXECUTABLE)
        # Run windeployqt to copy all necessary Qt DLLs and plugins
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${WINDEPLOYQT_EXECUTABLE} "$<TARGET_FILE:${PROJECT_NAME}>"
            WORKING_DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
            COMMENT "Running windeployqt for deployment"
        )
    else()
        message(WARNING "windeployqt.exe not found. Qt DLLs will not be deployed automatically.")
    endif()
endif()

# Install rules
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)