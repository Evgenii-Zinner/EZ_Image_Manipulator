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

# Set the path to your Qt6 installation
set(CMAKE_PREFIX_PATH "C:/Qt/6.9.2/mingw_64")

# Find Qt packages
find_package(Qt6 COMPONENTS 
    Core
    Gui
    Widgets
    REQUIRED
)

# WebP configuration
set(WEBP_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/third_party/libwebp-1.6.0-windows-x64/include)
set(WEBP_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/third_party/libwebp-1.6.0-windows-x64/lib)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${WEBP_INCLUDE_DIR}
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
add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS})

# Link directories
link_directories(${WEBP_LIBRARY_DIR})

## Choose WebP library filenames (prefer MinGW .a archives, fall back to .lib)
if(EXISTS "${WEBP_LIBRARY_DIR}/libwebp.a")
    set(WEBP_LIB_WEBP "${WEBP_LIBRARY_DIR}/libwebp.a")
else()
    set(WEBP_LIB_WEBP "${WEBP_LIBRARY_DIR}/libwebp.lib")
endif()

if(EXISTS "${WEBP_LIBRARY_DIR}/libwebpmux.a")
    set(WEBP_LIB_WEBPMUX "${WEBP_LIBRARY_DIR}/libwebpmux.a")
else()
    set(WEBP_LIB_WEBPMUX "${WEBP_LIBRARY_DIR}/libwebpmux.lib")
endif()

if(EXISTS "${WEBP_LIBRARY_DIR}/libwebpdemux.a")
    set(WEBP_LIB_WEBPDEMUX "${WEBP_LIBRARY_DIR}/libwebpdemux.a")
else()
    set(WEBP_LIB_WEBPDEMUX "${WEBP_LIBRARY_DIR}/libwebpdemux.lib")
endif()

if(EXISTS "${WEBP_LIBRARY_DIR}/libsharpyuv.a")
    set(WEBP_LIB_SHARPYUV "${WEBP_LIBRARY_DIR}/libsharpyuv.a")
else()
    set(WEBP_LIB_SHARPYUV "${WEBP_LIBRARY_DIR}/libsharpyuv.lib")
endif()

# Link libraries; order matters: libwebp may depend on sharpyuv in some builds, so place
# sharpyuv after libwebp to satisfy the GNU linker (objects first, then dependencies).
set(WEBP_LIBS
    ${WEBP_LIB_WEBP}
    ${WEBP_LIB_WEBPMUX}
    ${WEBP_LIB_WEBPDEMUX}
    ${WEBP_LIB_SHARPYUV}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    ${WEBP_LIBS}
)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

## Copy WebP and Qt DLLs to output directory (only if present)
if(WIN32)
    # WebP DLLs: copy only if the DLL files exist (static builds produce .a only)
    if(EXISTS "${CMAKE_SOURCE_DIR}/third_party/libwebp-1.6.0-windows-x64/bin/libwebp.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_SOURCE_DIR}/third_party/libwebp-1.6.0-windows-x64/bin/libwebp.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        )
    endif()

    if(EXISTS "${CMAKE_SOURCE_DIR}/third_party/libwebp-1.6.0-windows-x64/bin/libwebpmux.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_SOURCE_DIR}/third_party/libwebp-1.6.0-windows-x64/bin/libwebpmux.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        )
    endif()

    if(EXISTS "${CMAKE_SOURCE_DIR}/third_party/libwebp-1.6.0-windows-x64/bin/libwebpdemux.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_SOURCE_DIR}/third_party/libwebp-1.6.0-windows-x64/bin/libwebpdemux.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        )
    endif()

    # Qt DLLs (use target file locations)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:Qt6::Core>
            $<TARGET_FILE:Qt6::Gui>
            $<TARGET_FILE:Qt6::Widgets>
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    )

    # Run windeployqt to copy all necessary Qt DLLs and plugins
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "C:/Qt/6.9.2/mingw_64/bin/windeployqt.exe" "$<TARGET_FILE:${PROJECT_NAME}>"
        WORKING_DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        COMMENT "Running windeployqt for deployment"
    )
endif()

# Install rules
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)