#include "editor.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>

// ==========================================================
// ========= CONSTRUCTOR AND INISIALIZTION FONCTIONS ========
// ==========================================================

Editor::Editor()
{
    InitWindow(brick_width * (max_brick_horizontale_number + ui_part), brick_height * max_brick_verticale_number, "Breakout");
    SetTargetFPS(60);
    InitBrikcs();
    InitUiButton();
}

Editor::~Editor()
{
    CloseWindow();
}

void Editor::InitUiButton()
{
    for (int i = 0; i < brick_type_name.size(); i++)
    {
        Button current_button = {
            {float(brick_width * (max_brick_horizontale_number)) + brick_width,
             float(i * 50) + brick_width,
             float(brick_width * (ui_part - 2)),
             50.0},
            brick_type_color[i],
            brick_type_name[i].c_str(),
        };
        brick_type_btn.push_back(current_button);
    }
    current_brick_select = brick_type_btn[0].bg;
}

void Editor::InitBrikcs()
{
    for (int i = 0; i < max_brick_verticale_number - 10; i++)
    {
        for (int j = 0; j < max_brick_horizontale_number; j++)
        {
            Brick current;

            current.b = {
                float(j * brick_width),
                float(i * brick_height),
                float(brick_width),
                float(brick_height)};

            current.color = LIGHTGRAY;
            current.numbers_of_lives = 1;
            current.brick_type = SINGLE_HIT;
            current.power_type = BIG_BALL;
            current.isActive = true;

            bricks.push_back(current);
        }
    }
}

// ==========================================================
// ========= END CONSTRUCTOR AND INISIALIZTION FONCTIONS ========
// ==========================================================

//---------------------------------------------------------------------------------------------------------------------------

// =======================================
// ========= UPDATE FUNCTIONS ========
// =======================================

void Editor::changeCurrentBrick()
{
    for (auto &btn : brick_type_btn)
    {
        if (CheckCollisionPointRec(GetMousePosition(), btn.bg) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            current_brick_select = btn.bg;
            if (std::string(btn.text) == "1-Hit Brick")
            {
                brick_type = SINGLE_HIT;
            }
            else if (std::string(btn.text) == "2-Hit Brick")
            {
                brick_type = DOUBLE_HIT;
            }
            else if (std::string(btn.text) == "3-Hit Brick")
            {
                brick_type = TRIPLE_HIT;
            }
            else if (std::string(btn.text) == "Metal Brick")
            {
                brick_type = UNBREAKABLE;
            }
            else if (std::string(btn.text) == "Clear")
            {
                brick_type = CLEAR;
            }
        }
    }
}

void Editor::edit_brick()
{
    for (auto &brick : bricks)
    {
        if (CheckCollisionPointRec(GetMousePosition(), brick.b) && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            if (brick_type == SINGLE_HIT)
            {
                brick.color = RED;
                brick.numbers_of_lives = 1;
            }
            else if (brick_type == DOUBLE_HIT)
            {
                brick.color = YELLOW;
                brick.numbers_of_lives = 2;
            }
            else if (brick_type == TRIPLE_HIT)
            {
                brick.color = GREEN;
                brick.numbers_of_lives = 3;
            }
            else if (brick_type == UNBREAKABLE)
            {
                brick.color = GRAY;
                brick.numbers_of_lives = 1000;
            }
            else if (brick_type == CLEAR)
            {
                brick.color = LIGHTGRAY;
                brick.numbers_of_lives = 0;
            }
        }
    }
}

void Editor::update_input()
{
    int key = 0;
    while ((key = GetCharPressed()) > 0)
    {
        if (file_name == "file name")
        {
            file_name = "";
        }
        if (key >= 32 && key <= 126 && CheckCollisionPointRec(GetMousePosition(), input) && file_name.size() <= 21)
        {
            file_name += char(key);
        }
    }

    if (IsKeyPressed(KEY_BACKSPACE) && !file_name.empty() && CheckCollisionPointRec(GetMousePosition(), input))
    {
        file_name.pop_back();
    }
}

void Editor::save_data()
{
    if (CheckCollisionPointRec(GetMousePosition(), save.bg) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        for (auto &brick : bricks)
        {
            if (!ColorsAreEqual(brick.color, LIGHTGRAY))
            {
                file_brick.push_back(brick);
            }
        }
        json_file["bricks"] = file_brick;
        std::cout << json_file.dump(4);
        std::ofstream("levels/" + file_name + ".json") << json_file.dump(4);
    }
}

void Editor::update()
{
    changeCurrentBrick();
    edit_brick();
    update_input();
    save_data();
}

// =======================================
// ========= END UPDATE FUNCTIONS ========
// =======================================

//---------------------------------------------------------------------------------------------------------------------------

// =======================================
// ============ DRAWING FUNCTIONS =========
// =======================================

void Editor::draw_ui()
{
    for (int i = 0; i < brick_type_name.size(); i++)
    {
        brick_type_btn[i].draw();
    }
    DrawRectangleLinesEx(current_brick_select, 5.0f, BLACK);
}

void Editor::draw_form()
{
    DrawRectangleRec(input, LIGHTGRAY);
    float textY = input.y + (input.height - 20) / 2;
    if (file_name == "file name")
    {
        DrawText(file_name.c_str(), input.x + 5, textY, 20, {190, 190, 190, 255});
    }
    else
    {
        DrawText(file_name.c_str(), input.x + 5, textY, 20, BLACK);
    }
    save.draw();
    DrawRectangleLinesEx(save.bg, 2.0, GRAY);
    DrawRectangleLinesEx(input, 2.0, GRAY);
}

void Editor::draw()
{
    for (auto &brick : bricks)
    {
        DrawRectangleRec(brick.b, brick.color);
        DrawRectangleLines(brick.b.x, brick.b.y, brick.b.width, brick.b.height, {100, 100, 100, 255});
    }

    DrawRectangleRec(ui_frame, BLUE);
    draw_ui();
    draw_form();
    //DrawTextEx(myFont,"TESTE NEW FONT",{100,100},20,2.0,WHITE);
}
// =======================================
// ========= END DRAWING FUNCTIONS ========
// =======================================

//---------------------------------------------------------------------------------------------------------------------------

void Editor::run()
{
    while (!WindowShouldClose())
    {
        update();
        BeginDrawing();
        ClearBackground(BLACK);

        draw();
        EndDrawing();
    }
}

/*
    g++ src/editor/main.cpp src/editor/editor.cpp src/json.cpp -o editor -I include -L C:\raylib\w64devkit\lib -lraylib -lopengl32 -lgdi32 -lwinmm
*/