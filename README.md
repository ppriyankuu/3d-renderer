# 3D Renderer

This project is a 3D renderer written in C (from scratch) that takes an input object file (.obj) and converts it into a 2D image for display on the screen. The project uses the Simple DirectMedia Layer (SDL) library for rendering.

## Features

- Load .obj files to render 3D models.
- Convert 3D models into 2D images.
- Display rendered images using SDL.

## Prerequisites

- C compiler (e.g., GCC)
- SDL2 library

## Installation

1. **Clone the repository:**
    ```sh
    git clone https://github.com/ppriyankuu/3d-renderer
    cd 3d-renderer
    ```

2. **Install SDL2:**

    - On Debian-based systems:
        ```sh
        sudo apt-get install libsdl2-dev
        ```

    - On macOS using Homebrew:
        ```sh
        brew install sdl2
        ```

    - On Windows, download the SDL2 development libraries from the [SDL website](https://www.libsdl.org/download-2.0.php) and follow the installation instructions.

3. **Build the project:**
    ```sh
    make build 
   ```
   or
   ```sh
   make
   ```

