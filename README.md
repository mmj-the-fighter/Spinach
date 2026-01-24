# Spinach
A framework for starting with computer graphics. It uses SDL3 library.  
But can be retargetted to other windowing frameworks by modifying the source code.  
  
Useful for:  
- Writing lab programs in a first course for computer graphics  
  (pixels, lines, polygons, circles,  ellipses, filling, clipping, transforms etc.)  
- Writing raytracers  
- Writing programs for advanced curves  
- Writing software rasterizers   
- Writing image processing routines
- Games
- and so on.

## Examples / Tutorials
- [first_example](https://github.com/mmj-the-fighter/Spinach/blob/main/Spinach/first_example/main.cpp) shows how spinach can be used for displaying pixels in a render loop (use case for software rasterization and raytracing)
- [second_example](https://github.com/mmj-the-fighter/Spinach/blob/main/Spinach/second_example/main.cpp)  shows how spinach can be used without render loop  
- [third_example](https://github.com/mmj-the-fighter/Spinach/blob/main/Spinach/third_example/main.cpp)  shows how rmgui in spinach can be used
- [fourth_example](https://github.com/mmj-the-fighter/Spinach/blob/main/Spinach/fourth_example/main.cpp) shows how a clone of mine sweeper game can be developed  
- [fifth_example](https://github.com/mmj-the-fighter/Spinach/blob/main/Spinach/fifth_example/main.cpp) shows how spinach can be used for image processing  

## Build / Project Generation

**Prerequisites**

* The root folder must contain the `res` directory.
* CMake must be installed.

**Steps**

1. Create a directory named `build` in the project root.
2. Open a terminal in the project root and run:

```bash
cd build
cmake ../
```

This will generate the project files inside the `build` directory.
## Parent project  
 [Graphics Lab Framework (in C and SDL2)](https://github.com/mmj-the-fighter/GraphicsLabFramework)

 # Projects that use Spinach  
- [Namaste3D Software rasterizer](https://github.com/mmj-the-fighter/Namaste3D)
- [Chess Game](https://github.com/RohithKKannan/Chess-Game)
- [Flappy Bird Clone](https://github.com/mmj-the-fighter/FlappyBirdClone)
- [Snake 2D](https://github.com/mmj-the-fighter/Snake2D)