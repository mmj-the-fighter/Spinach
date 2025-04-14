# Spinach Framework
A framework for implementing and  
studying basic algorithms in computer graphics.  
It uses SDL3 library.  
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

## Parent project  
 [Graphics Lab Framework (in C and SDL2)](https://github.com/mmj-the-fighter/GraphicsLabFramework)
 

## Fixing SDL3 Dependencies

### How to Run in Visual Studio

#### 1) Download and Set Up SDL3
- Download and unzip the following package in a folder of your choice:
  - [SDL3 Development Files](https://github.com/libsdl-org/SDL/releases/download/release-3.2.0/SDL3-devel-3.2.0-VC.zip)
- This package contains the necessary `.lib`, `.dll`, and `include` files.

#### 2) Copy SDL3 DLL
- Copy `SDL3.DLL` into your project folder.

#### 3) Fix Dependencies in Project

1. **Open your project in Visual Studio**.
2. **Modify project properties**:
   
   - **C/C++ -> General**
     - Set **Additional Include Directories** to the SDL3 `include` folder.
     - Example:
       ```
       D:\Libs\SDL3-devel-3.2.0-VC\SDL3-3.2.0\include
       ```
   
   - **Linker -> General**
     - Set **Additional Library Directories** to the SDL3 `lib` folder.
     - Example:
       ```
       D:\Libs\SDL3-devel-3.2.0-VC\SDL3-3.2.0\lib\x64
       ```
   
   - **Linker -> Input**
     - Add `SDL3.lib` under **Additional Dependencies**.

### How to run in Dev-C++
#### 1) Download mingw package
- Download and unzip the following package in a folder of your choice:
- [SDL3 Development Files for MinGW](https://github.com/libsdl-org/SDL/releases/download/release-3.2.10/SDL3-devel-3.2.10-mingw.zip)
- This package contains the necessary `.lib`, `.dll`, and `include` files.
#### 2) Copy SDL3 DLL
- Copy `SDL3.DLL` into your project folder.
#### 3) Fix Dependencies in Project
1. **Open your project in Dev-C++**.
2. **Modify project properties**:
- Take project options by right clicking on the prject name in the project tab.
- **Set Header Files**
- Set the include directory from the unzipped mingw package, in Project Options -> Directories -> Include Directories 
- e.g. D:\Lib\SDL3-devel-3.2.10-mingw\SDL3-3.2.10\x86_64-w64-mingw32\include
- **Set Lib Files**
- Set the lib directory from the unzipped mingw package, in Project Options -> Directories -> Library Directories 
- e.g. D:\Lib\SDL3-devel-3.2.10-mingw\SDL3-3.2.10\x86_64-w64-mingw32\lib
- **Set Linker option**
- Set the parameter: -lSDL3 in Project Options -> Parmeters -> Linker window
### Reference
- [SDL Official Website](https://www.libsdl.org/)

