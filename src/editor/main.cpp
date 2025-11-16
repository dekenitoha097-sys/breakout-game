#include "raylib.h"
#include "editor.hpp"

int main()
{
    Editor Breakout;

    Breakout.run();

    return 0;
}

/*
g++ src/editor/main.cpp src/editor/editor.cpp src/json.cpp -o editor -I include -L C:\raylib\w64devkit\lib -lraylib -lopengl32 -lgdi32 -lwinmm
*/