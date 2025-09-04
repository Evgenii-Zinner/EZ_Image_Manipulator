# EZImageManipulator

## Overview
EZImageManipulator is a simple, yet powerful, desktop image editing application built with C++ and Qt6. It provides essential image manipulation functionalities, including opening, saving (with WebP support), cropping, rotating, flipping, and resizing images.

## Features
*   **Open and Save Images:** Supports various image formats, including WebP.
*   **Zoom:** Zoom in, zoom out, and reset zoom for detailed editing.
*   **Cropping:** Interactively define and apply crop regions using a resizable overlay.
*   **Rotation:** Rotate images 90 degrees left or right.
*   **Flipping:** Flip images horizontally or vertically.
*   **Resizing:** Resize images with options to maintain or disregard aspect ratio.
*   **Image Information:** View basic information about the currently loaded image.

## How to Build

### Prerequisites
*   **CMake:** Version 3.15 or higher.
*   **Qt6:** Specifically, Qt 6.9.2 (MinGW 64-bit for Windows, as configured in `CMakeLists.txt`). You can download it from the [official Qt website](https://www.qt.io/download).
*   **C++ Compiler:** A C++17 compatible compiler (e.g., MinGW-w64 for Windows).
*   **libwebp:** The project uses a pre-built `libwebp` library, you can download it from the [official webp repository] (https://storage.googleapis.com/downloads.webmproject.org/releases/webp/index.html)

### Build Steps

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/your-username/EZImageManipulator.git
    cd EZImageManipulator
    ```
    *(Note: Replace `https://github.com/your-username/EZImageManipulator.git` with the actual repository URL if it's hosted on GitHub.)*

2.  **Configure CMake:**
    Open your terminal or command prompt in the project root directory and create a build directory:
    ```bash
    mkdir build
    cd build
    ```
    Now, configure the project with CMake. **Important:** Ensure `CMAKE_PREFIX_PATH` in `CMakeLists.txt` points to your Qt6 installation directory (e.g., `C:/Qt/6.9.2/mingw_64`).
    ```bash
    cmake ..
    ```
    If you are using a specific generator (e.g., Ninja, Visual Studio), you might specify it:
    ```bash
    cmake -G "Ninja" ..
    # or for Visual Studio
    # cmake -G "Visual Studio 17 2022" ..
    ```

3.  **Build the project:**
    ```bash
    cmake --build .
    ```

4.  **Run the application:**
    After a successful build, the executable will be located in the `build` directory (or a subdirectory like `build/bin` depending on your CMake configuration).
    ```bash
    ./EZImageManipulator.exe
    ```
    *(Note: The exact path to the executable might vary slightly based on your build environment and CMake generator.)*

## How to Use
1.  **Open Image:** Click "File" -> "Open" to load an image.
2.  **Save Image:** Click "File" -> "Save" to save the current image. You can choose the format, including WebP.
3.  **Zoom:** Use the "Zoom In", "Zoom Out", and "Reset Zoom" options to adjust the view.
4.  **Crop:** Click "Crop" to activate the cropping tool. Drag the handles of the overlay to define the crop area. Click "Apply Crop" to finalize or "Cancel Crop" to discard.
5.  **Rotate/Flip:** Use the "Rotate Left", "Rotate Right", "Flip Horizontal", and "Flip Vertical" options to transform the image.
6.  **Resize:** Click "Resize" to open the resize dialog. Enter new dimensions and optionally check "Keep Aspect Ratio".
7.  **Image Info:** Click "Image Info" to see details about the loaded image.
