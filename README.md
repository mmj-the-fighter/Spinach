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
- [render loop](https://github.com/mmj-the-fighter/Spinach/tree/main/Spinach/examples/example_with_render_loop) demonstration of rendering rapidly changing pixels. use case for software rasterization and raytracing
- [without render loop](https://github.com/mmj-the-fighter/Spinach/tree/main/Spinach/examples/example_without_render_loop)  shows how spinach can be used without render loop. examples: sierpienski triangle, rendering svg and sepia filter 
- [spirograph](https://github.com/mmj-the-fighter/Spinach/tree/main/Spinach/examples/spirograph_with_rmgui)  shows how rmgui in spinach can be used for controling a spirograph
- [mine sweeper](https://github.com/mmj-the-fighter/Spinach/tree/main/Spinach/examples/mine_sweeper) shows how a clone of mine sweeper game can be developed using spinach
- [image processing](https://github.com/mmj-the-fighter/Spinach/tree/main/Spinach/examples/image_processing_with_rmgui) shows how spinach can be used for image processing  and how rmgui of spinach can be used for that.
- [game of life](https://github.com/mmj-the-fighter/Spinach/tree/main/Spinach/examples/game_of_life) shows how spinach can be used for programming game of life. this involves using std::thread
- [flappy bird clone](https://github.com/mmj-the-fighter/Spinach/tree/main/Spinach/examples/bird_flight) shows how a flappy bird like game can be made
- [snake clone](https://github.com/mmj-the-fighter/Spinach/tree/main/Spinach/examples/snake_clone) shows how a snake like game can be made


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