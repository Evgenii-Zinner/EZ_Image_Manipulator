# EZImageManipulator

## Overview

**EZImageManipulator** is a simple, cross-platform desktop application for basic image editing. Built with **C++** and the **Qt6** framework, it provides essential tools for manipulating image files, including support for the modern **WebP** format.

## Key Features

  * ✅ **Image I/O:** Open and save images in various formats, including **WebP**.
  * ✂️ **Cropping:** Interactively select and apply custom crop regions.
  * 🔄 **Rotation:** Quickly rotate images 90° to the left or right.
  * ↔️ **Flipping:** Flip images horizontally or vertically with a single click.
  * 📐 **Resizing:** Adjust image dimensions with or without maintaining the aspect ratio.

-----

## How to Build

These instructions will guide you through building the application from source on your system.

### Prerequisites

Ensure you have the following installed before you begin:

  * **CMake:** Version 3.15 or higher.
  * **Qt6:** Version 6.2 or higher. Download it from the [official Qt website](https://www.qt.io/download).
  * **C++ Compiler:** A compiler that supports C++17 (e.g., MinGW-w64 on Windows, GCC on Linux, or Clang on macOS).
  * **Git:** Required for CMake to automatically fetch the `libwebp` dependency.

### Build Steps

1.  **Clone the Repository**
    Start by cloning the project's repository and navigating to the new directory:

    ```bash
    git clone https://github.com/Evgenii-Zinner/ez-image-manipulator.git
    cd ez-image-manipulator
    ```

2.  **Configure CMake**
    Create a `build` directory and run CMake from within it. You **must** specify the path to your Qt6 installation using the `CMAKE_PREFIX_PATH` variable.

    ```bash
    mkdir build
    cd build
    cmake .. -DCMAKE_PREFIX_PATH=/path/to/your/Qt6/installation
    ```

    Here are some examples for different operating systems:

      * **Windows (MinGW):** `cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/6.9.2/mingw_64"`
      * **Linux:** `cmake .. -DCMAKE_PREFIX_PATH=~/Qt/6.2.4/gcc_64`
      * **macOS:** `cmake .. -DCMAKE_PREFIX_PATH=~/Qt/6.2.4/clang_64`

    > **Note:** CMake will automatically download and configure the `libwebp` dependency during this step.

3.  **Build the Project**
    Run the build command to compile the source code:

    ```bash
    cmake --build .
    ```

4.  **Run the Application**
    The executable will be located in the `build/bin` directory. Run the application with the following command:

    ```bash
    ./bin/EZImageManipulator
    ```

-----

## How to Use

The application's interface is designed to be intuitive. Here's a quick guide to its main features:

  * **🖼️ Open & Save:** Use the **File** menu to **Open** an image or **Save** your changes.
  * **➕➖ Zoom:** Use the zoom controls to get a closer look at your image.
  * **✂️ Crop:** Click the **Crop** button to activate the tool. Drag the handles on the overlay to define the area, then click **Apply Crop**.
  * **🔄 Transform:** Use the buttons in the toolbar to **Rotate Left/Right** or **Flip Horizontal/Vertical**.
  * **📏 Resize:** Enter new dimensions in the **Resize** dialog. You can optionally check **Keep Aspect Ratio** to maintain the image's original proportions.