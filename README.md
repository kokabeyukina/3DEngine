# 3D Engine Prototype
A C++ 3D engine prototype supporting both hardware-accelerated (OpenGL) and software (CPU) rendering.

## Usage & CLI Arguments
Since the engine does not currently feature a GUI, all configurations and file paths must be passed via the terminal.

### Graphics Modes
By default, the engine utilizes **OpenGL**. To bypass the GPU and render using only the **CPU**, use the `-noGL` flag.
### Loading Models
Currently, the engine exclusively supports the .obj format. In this repository, there are some examples inside the `objs/` directory for quick testing.
* **Material Files:** The engine automatically looks for the `.mtl` file defined within the `.obj`. If your material file is in a different directory or has a different name, use the `-mtl` flag:
```
./engine objs/example.obj -mtl another/path/example.mtl
```
* **Positioning Models:** To prevent multiple objects from overlapping at the origin (0,0,0), use the -p flag followed by x,y,z coordinates:
```
./engine objs/obj1.obj -p 1.5,0,0 objs/obj2.obj -p -1.5,-2,0
```

### Controls
| Action | Key(s) |
| - | - |
| Movement | W, A, S, D |
| Ascend / Descend | Space / Ctrl |
| Sprint | Shift|
| Lighting Direction | Q / E |
| Close Program | Esc |

## Compilation & Build
The engine is written in C++20 and compiled using GCC. It requires linking against standard Windows graphics libraries: `gdi32`, `opengl32` and `glu32`.

* **Development & Debugging:**
```
g++ *.cpp resource.o -lgdi32 -lopengl32 -lglu32 -o engine -O0 -Wall -Wextra -Wpedantic -std=c++20
```
* **Final Release:** (Optimized, no terminal window)
```
g++ *.cpp resource.o -lgdi32 -lopengl32 -lglu32 -o engine -O3 -mwindows -std=c++20
```
* **Wine Compatible:** In case you want to run the program on Linux or MacOS. This version is considerably larger, due to the inclusion of linked DLLs. Since it hasn't been optimized for these systems, the program may contain some bugs.
```
g++ *.cpp resource.o -lgdi32 -lopengl32 -lglu32 -o engine-wine -O3 -mwindows -std=c++20 -static-libgcc -static-libstdc++ -static
```