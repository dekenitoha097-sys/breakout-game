#include "raylib.h"
#include "breakout.hpp"
#include <iostream>

int main()
{
    Breakout breakout;
    breakout.run();
}

/*
g++ src/*.cpp -o prog -I include -L C:\raylib\w64devkit\lib -lraylib -lopengl32 -lgdi32 -lwinmm
*/