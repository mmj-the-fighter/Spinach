# Spinach Framework

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

### Reference
- [SDL Official Website](https://www.libsdl.org/)

