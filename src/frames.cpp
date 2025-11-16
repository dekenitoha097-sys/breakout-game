#include "breakout.hpp"
#include "raylib.h"
#include <vector>
#include <iostream>
#include <cstdio> // pour snprintf

// Menu frame class
Menu::Menu()
{
    btn_w = 150;
    btn_h = 50;
    play_btn.bg = {float(GetScreenWidth() / 2 - btn_w / 2), 100, float(btn_w), (float)btn_h};
    play_btn.bg_color = BLUE;
    play_btn.text = "PLAY";
}

void Menu::draw()
{
    play_btn.draw();
}

void Menu::update(Frame_Type &frame, std::string &level)
{
    if (CheckCollisionPointRec(GetMousePosition(), play_btn.bg) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        frame = LEVELS;
    }
}

//--------------------------------------

// Levels frame class

Levels::Levels()
{
    rows = 6;
    cols = 5;
    buttonWidth = 60;
    buttonHeight = 30;
    spacing = 10;
    counter = 1;

    prev.bg = {0, 0, 100, 50};
    prev.text = "MENU";

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            levels[i][j].bg = {
                float(160 + j * (buttonWidth + spacing)),
                float(100 + i * (buttonHeight + spacing)),
                (float)buttonWidth,
                (float)buttonHeight};

            labels[counter - 1] = std::to_string(counter);

            levels[i][j].text = labels[counter - 1].c_str();

            counter++;
        }
    }
}

void Levels::draw()
{
    DrawText("LEVELS", 270, 60, 30, BLACK);
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            levels[i][j].draw();

    prev.draw();
}

void Levels::update(Frame_Type &frame, std::string &level)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (CheckCollisionPointRec(GetMousePosition(), levels[i][j].bg) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                level = "level" + std::string(levels[i][j].text) + ".json";
                std::cout << level << std::endl;
                frame = LEVEL1;
            }
        }
    }
    if (CheckCollisionPointRec(GetMousePosition(), prev.bg) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        frame = MENU;
    }
}

//--------------------------------------------------------

// Game frame class

Game::Game(std::string level)
{

    InitAudioDevice();
    paddle = {float(brick_width * 5), float(brick_height * 28), float(brick_width * 2), float(brick_height)};
    file.open("levels/" + level);

    if (file.is_open())
    {
        std::cout << "Ouvert" << std::endl;
    }
    file >> j;
    bricks = j["bricks"].get<std::vector<Brick>>();
    ball = {{paddle.x + brick_width, paddle.y - 5}, {2.5, -2.5}, 7, PURPLE};
    isBegin = true;
    isFinish = false;
    b_sound = LoadSound("resources/Hit1.wav");
    m_sound = LoadSound("resources/Pickup8.wav");
    restart_tex = LoadTexture("resources/restart.png");
    undo_tex = LoadTexture("resources/undo.png");
    allInactive = false;
    lives = 3;
    livesRaduis = 15;
    result_menu = {float((GetScreenWidth() / 2) - 200), float((GetScreenHeight() / 2) - 150), 400.0, 300.0};
    restart = {0, 0, 32.0, 32.0};
    undo = {0, 0, 32.0, 32.0};

    sounds_loaded = true;
    textures_loaded = true;

    score = 0;
}

Game::~Game()
{
    if (file.is_open())
        file.close();

    if (textures_loaded)
    {
        UnloadTexture(restart_tex);
        UnloadTexture(undo_tex);
        textures_loaded = false;
    }

    if (sounds_loaded)
    {
        UnloadSound(b_sound);
        UnloadSound(m_sound);
        sounds_loaded = false;
    }

    CloseAudioDevice(); // uniquement si tu as appelé InitAudioDevice() ici
}

void Game::draw()
{
    ClearBackground({58, 58, 58, 255});
    // Draw bricks
    for (auto &brick : bricks)
    {
        if (brick.isActive)
        {
            DrawRectangleRec(brick.b, brick.color);
            DrawRectangleLines(brick.b.x, brick.b.y, brick.b.width, brick.b.height, LIGHTGRAY);
        }
    }

    DrawRectangleRec(ui_frame, BLACK);
    DrawRectangleRec(paddle, GREEN);
    DrawCircleV(ball.ball_pos, ball.raduis, ball.color);

    // Draw lives
    for (int i = 0; i < lives; i++)
    {
        DrawCircle((ui_frame.x + livesRaduis + 150) + i * 50, livesRaduis + 5, livesRaduis, PURPLE);
    }
    DrawText("Lives: ", ui_frame.x + 15, 0, 40, GREEN);
    DrawText(TextFormat("Score: %d", score), ui_frame.x + 15, 50, 40, BLUE);

    if (isBegin)
    {
        DrawText("Press SPACE to start", 60, brick_height * 20, 20, GREEN);
    }

    if (lives <= 0)
    {
        DrawText("Perdu", 60, brick_height * 20, 20, GREEN);
    }

    // draw particule
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (particles[i].active)
        {
            DrawCircle(particles[i].pos.x, particles[i].pos.y, particles[i].raduis, particles[i].color);
        }
    }

    if (allInactive)
    {
        DrawRectangleRec(result_menu, ORANGE);
        DrawText("You win", float((GetScreenWidth() / 2) - 65), float((GetScreenHeight() / 2) - 100), 40, GREEN);
        DrawTextureRec(restart_tex, restart, {float((GetScreenWidth() / 2 + 50)), float((GetScreenHeight() / 2))}, WHITE);
        DrawTextureRec(undo_tex, undo, {float((GetScreenWidth() / 2) - 80), float((GetScreenHeight() / 2))}, WHITE);
    }
}

// This function checks if all breakable bricks are inactive
bool Game::AllBrokenBricksDeactivated(const std::vector<Brick> &bricks)
{
    for (const Brick &brick : bricks)
    {
        if (brick.isActive == true && brick.numbers_of_lives <= 3)
        {
            return false;
        }
    }
    return true; // aucune brique cassée trouvée active
}

// Initialise
void Game::spawnExplosion(Vector2 pos, Color color)
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        particles[i].active = true;
        particles[i].pos = pos;
        particles[i].life = 0.5f + (float)GetRandomValue(0, 50) / 100; // 0.5 → 1 sec
        particles[i].vel = {
            (float)GetRandomValue(-200, 200) / 100,
            (float)GetRandomValue(-200, 200) / 100};
        particles[i].raduis = GetRandomValue(1, 5);
        particles[i].color = color;
    }
}

void Game::updateParticles(float dt)
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (particles[i].active)
        {
            particles[i].pos.x += particles[i].vel.x;
            particles[i].pos.y += particles[i].vel.y;
            particles[i].life -= dt;

            if (particles[i].life <= 0)
                particles[i].active = false;
        }
    }
}

// --------------------- COLLISION PARFAITE ---------------------
void Game::handleBallBrickCollision()
{
    for (auto &brick : bricks)
    {
        if (!brick.isActive)
            continue;

        if (CheckCollisionCircleRec(ball.ball_pos, ball.raduis, brick.b))
        {
            // Calculer le centre relatif
            float ballCenterX = ball.ball_pos.x;
            float ballCenterY = ball.ball_pos.y;
            float brickCenterX = brick.b.x + brick.b.width / 2;
            float brickCenterY = brick.b.y + brick.b.height / 2;

            float dx = ballCenterX - brickCenterX;
            float dy = ballCenterY - brickCenterY;

            // Rebond selon la direction dominante
            if (fabs(dx) > fabs(dy))
                ball.vel.x *= -1; // rebond horizontal
            else
                ball.vel.y *= -1; // rebond vertical

            // Brique cassable
            if (brick.brick_type != UNBREAKABLE)
            {
                brick.numbers_of_lives--;
                spawnExplosion({brick.b.x + brick.b.width / 2, brick.b.y + brick.b.height / 2}, brick.color);

                if (brick.numbers_of_lives <= 0)
                {
                    brick.isActive = false;
                    score += 5;
                }
            }

            PlaySound(b_sound);
        }
    }
}

void Game::update(Frame_Type &frame, std::string &level)
{
    Vector2 mouse = GetMousePosition();

    // Handle events
    if ((mouse.x > 0 && mouse.x <= ui_frame.x) && !isBegin && !isFinish)
    {
        paddle.x = mouse.x - paddle.width / 2;
    }

    if (mouse.x >= ui_frame.x - paddle.width / 2 && !isBegin)
    {
        paddle.x = ui_frame.x - paddle.width;
    }
    if (mouse.x <= paddle.width / 2 && !isBegin)
    {
        paddle.x = 0;
    }

    if (IsKeyPressed(KEY_SPACE))
    {
        isBegin = false;
    }

    // --
    if (!isBegin)
    {
        ball.ball_pos.y += ball.vel.y;
        ball.ball_pos.x += ball.vel.x;
    }

    // Check collisions
    if (ball.ball_pos.x <= 0)
    {
        ball.vel.x *= -1;
        PlaySound(m_sound);
    }
    if (ball.ball_pos.x >= ui_frame.x)
    {
        ball.vel.x *= -1;
        PlaySound(m_sound);
    }
    if (ball.ball_pos.y <= 0)
    {
        ball.vel.y *= -1;
        PlaySound(m_sound);
    }

    if (!isBegin && CheckCollisionCircleRec(ball.ball_pos, ball.raduis, paddle))
    {
        ball.vel.y *= -1;
        PlaySound(m_sound);
    }
    //------------END collision cheking--------------

    if (ball.ball_pos.y >= GetScreenHeight())
    {
        isBegin = true;
        paddle = {float(brick_width * 5), float(brick_height * 28), float(brick_width * 2), float(brick_height)};
        ball.ball_pos = {paddle.x + brick_width, paddle.y - 5};
        // PlaySound(m_sound);
        lives--;
    }

    handleBallBrickCollision();
    //-------------------------------------------------

    // Check all bricks is inactive
    allInactive = AllBrokenBricksDeactivated(bricks);

    // Reset ball and paddle position if all brick are destroys
    if (allInactive || lives <= 0)
    {
        isFinish = true;
        ball = {{paddle.x + brick_width, paddle.y - 5}, {2.0, -2.0}, 5, PURPLE};
        paddle = {float(brick_width * 5), float(brick_height * 28), float(brick_width * 2), float(brick_height)};
    }

    updateParticles(GetFrameTime());

    if (CheckCollisionPointRec(mouse, {float(GetScreenWidth() / 2 - 65), float(GetScreenHeight() / 2), restart.width, restart.height}) && IsMouseButtonDown(MOUSE_BUTTON_LEFT) && allInactive)
    {
        std::cout << "before change frame\n";
        frame = LEVELS;
        std::cout << "after change frame\n";
    }
}
//-------------------------------------------------------------

// {float(GetScreenWidth() / 2 - 65), float(GetScreenHeight() / 2), restart.width, restart.height}