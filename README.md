Spinach Framework
*****************
Fixing SDL3 Dependencies

How to run in Visual Studio:
***************************
0) Make an empty project and copy the .c file and add it to project

1) Download and unzip the following in a folder of your convenience.
--------------------------------------------------------------------
These are lib files, dll and and include files
https://github.com/libsdl-org/SDL/releases/download/release-3.2.0/SDL3-devel-3.2.0-VC.zip

2)Copy SDL3.DLL in your project folder.

3) Fix dependencies in project.
-------------------------------
Open your project in visual studio:

In project properties
C/C++ -> General:
additional include directories:
Type in path of your SDL3 library include folder e.g: D:\Libs\SDL3-devel-3.2.0-VC\SDL3-3.2.0\include

Linker -> General:
additional library directories:
Type in path of your SDL3 library lib folder e.g: D:\Libs\SDL3-devel-3.2.0-VC\SDL3-3.2.0\lib\x64

Linker -> input: 
Type in 
SDL3.lib

Reference: https://www.libsdl.org/
