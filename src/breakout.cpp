#include "breakout.hpp"
#include <memory>
#include <iostream>

Breakout::Breakout()
{
    InitWindow(brick_width * (max_brick_horizontale_number + ui_part),
               brick_height * max_brick_verticale_number,
               "Breakout");

    SetTargetFPS(60);
    SetExitKey(KEY_NULL);

    // FRAME DE DÉPART = MENU
    frame = MENU;
    current_frame = std::make_unique<Menu>();

    level = "level1.json";
}

Breakout::~Breakout()
{
    CloseWindow();
}

void Breakout::draw()
{
    current_frame->draw();
}

void Breakout::update()
{
    // Update la frame actuelle
    current_frame->update(frame, level);

    // MACHINE A ETATS
    switch (frame)
    {
        case MENU:
            if (dynamic_cast<Menu*>(current_frame.get()) == nullptr)
            {
                current_frame = std::make_unique<Menu>();
            }
            break;

        case LEVELS:
            if (dynamic_cast<Levels*>(current_frame.get()) == nullptr)
            {
                current_frame = std::make_unique<Levels>();
            }
            break;

        case SETTINGS:
            if (dynamic_cast<SettingsFrame*>(current_frame.get()) == nullptr)
            {
                current_frame = std::make_unique<SettingsFrame>();
            }
            break;

        case LEVEL1:
            if (dynamic_cast<Game*>(current_frame.get()) == nullptr)
            {
                current_frame = std::make_unique<Game>(level);
            }
            break;
    }
}

void Breakout::run()
{
    while (!WindowShouldClose())
    {
        update();

        BeginDrawing();
        ClearBackground({100,100,100,255});
        draw();
        EndDrawing();
    }
}
