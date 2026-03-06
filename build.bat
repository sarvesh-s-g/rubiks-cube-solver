@echo off
cl /O2 /EHsc main.cpp vision/Camera.cpp cube/Cube.cpp solver/IDAStar.cpp solver/PatternDatabase.cpp /I C:\opencv\build\include /link /LIBPATH:C:\opencv\build\x64\vc16\lib opencv_world490.lib user32.lib gdi32.lib && main
