# Spinach
A framework for starting with computer graphics. It uses SDL3 library.  
But can be retargetted to other windowing frameworks by modifying the source code.  
[Presentation on YouTube with synthetic voice](https://www.youtube.com/watch?v=Prbd9uppZoU)
  
Useful for:  
- Writing lab programs in a first course for computer graphics  
  (pixels, lines, polygons, circles,  ellipses, filling, clipping, transforms etc.)  
- Writing raytracers  
- Writing programs for advanced curves  
- Writing software rasterizers   
- Writing image processing routines
- Games
- and so on.

## Known Issues
- [Currently Screen recording / render to GIF is unoptimized and can be dangerous](https://github.com/mmj-the-fighter/Spinach/issues/1)
 
## Examples / Tutorials
- [render loop skelton app](https://github.com/mmj-the-fighter/Spinach/tree/main/Spinach/examples/example_with_render_loop) demonstration of rendering frames of pixelbuffer which is being modified per frame
- [without render loop](https://github.com/mmj-the-fighter/Spinach/tree/main/Spinach/examples/example_without_render_loop)  demonstration of rendering a pixelbuffer once and waiting for input
- [logarithmic spiral](https://github.com/mmj-the-fighter/Spinach/tree/main/Spinach/examples/logarithmic_spiral)
- [locus generation](https://github.com/mmj-the-fighter/Spinach/tree/main/Spinach/examples/parametric_locus_generator)
- [game of life simulation](https://github.com/mmj-the-fighter/Spinach/tree/main/Spinach/examples/game_of_life) 
- [mine sweeper clone](https://github.com/mmj-the-fighter/Spinach/tree/main/Spinach/examples/mine_sweeper) 
- [flappy bird clone](https://github.com/mmj-the-fighter/Spinach/tree/main/Spinach/examples/bird_flight)
- [snake clone](https://github.com/mmj-the-fighter/Spinach/tree/main/Spinach/examples/snake_clone)
- [picture puzzle](https://github.com/mmj-the-fighter/Spinach/tree/main/Spinach/examples/picture_puzzle)
- [tic-tac-toe](https://github.com/mmj-the-fighter/Spinach/tree/main/Spinach/examples/tictactoe)
- [image processing and spn::rmgui example](https://github.com/mmj-the-fighter/Spinach/tree/main/Spinach/examples/image_processing_with_rmgui)
- [raytracing example](https://github.com/mmj-the-fighter/Spinach/tree/main/Spinach/examples/raytracing_example) 
- [raycasting example](https://github.com/mmj-the-fighter/Spinach/tree/main/Spinach/examples/raycasting_example) 
- [software rendering example](https://github.com/mmj-the-fighter/Spinach/tree/main/Spinach/examples/wireframe_rendering)

## Build / Project Generation

**Prerequisites**

* The root folder must contain the `res` directory.
* CMake must be installed.

**Steps**

1. Create a directory named `build` in the project root.
2. Open a terminal in the project root and run:

```bash
cd build
cmake ..
```

This will generate the project files inside the `build` directory.  

## Key Bindings

Five keys are handled in the engine:

| Key | Behaviour |
|-----|-----------|
| `Esc` | Application closes |
| `F12` | A screenshot is saved with the current timestamp |
| `F8` | Screen recording starts; recording indication is shown in the screen |
| `F10` | Screen recording ends; the gif is saved with the current time stamp |
| `F6` | Screen recording aborts; no file is saved |

# Projects that use Spinach  
- [Namaste3D Software rasterizer](https://github.com/mmj-the-fighter/Namaste3D)
- [Chess Game](https://github.com/RohithKKannan/Chess-Game)
- [Match 5x5 puzzle game](https://github.com/mmj-the-fighter/match5x5) 

## App Skeleton
```cpp
#include <iostream>
#include <spn_canvas.h>
#include <spn_core.h>

void UpdateAndRender(spn::Canvas* canvas) {
	//Draw something with the canvas
}

void HandleInput(const SDL_Event* sdlEvent) {
	//std::cout << "*";
}


int main(int argc, char* argv[])
{
	spn::SpinachCore sc;
	if (!sc.Init(640, 480, "../res/")) {
		std::cout << "initialization failed with error "
			<< sc.GetInitializationResult()
			<< std::endl;
		return 1;
	}
	sc.SetUpdateAndRenderHandler(UpdateAndRender);
	sc.SetInputHandler(HandleInput);	
	sc.SetWindowTitle("Spinach Demo");
	sc.GetCanvas()->SetPrimaryColor(255, 255, 0);
	sc.SetTargetFramesPerSecond(30);
	sc.LockFps(true);
	sc.MainLoop();
	return 0;
}
```