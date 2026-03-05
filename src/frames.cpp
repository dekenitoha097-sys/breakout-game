#include "breakout.hpp"
#include "raylib.h"
#include <algorithm>
#include <cmath>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>

namespace
{
constexpr int kMaxLevels = 30;
constexpr const char *kConfigPath = "config/game_config.json";

struct PersistentConfig
{
    int highest_unlocked_level = 1;
    float master_volume = 0.75f;
    bool muted = false;
};

PersistentConfig g_config{};
bool g_config_loaded = false;

int ClampInt(int value, int minValue, int maxValue)
{
    return std::max(minValue, std::min(value, maxValue));
}

float ClampFloat(float value, float minValue, float maxValue)
{
    return std::max(minValue, std::min(value, maxValue));
}

bool IsHovered(const Rectangle &rect)
{
    return CheckCollisionPointRec(GetMousePosition(), rect);
}

void DrawButtonLike(const Rectangle &rect, const char *text, int fontSize, Color base, Color hover, Color textColor)
{
    const bool hovered = IsHovered(rect);
    DrawRectangleRec(rect, hovered ? hover : base);
    DrawRectangleLinesEx(rect, 2.0f, hovered ? RAYWHITE : BLACK);

    const int textWidth = MeasureText(text, fontSize);
    const int textX = static_cast<int>(rect.x + (rect.width - textWidth) * 0.5f);
    const int textY = static_cast<int>(rect.y + (rect.height - fontSize) * 0.5f);
    DrawText(text, textX, textY, fontSize, textColor);
}

void ApplyAudioConfig()
{
    if (!g_config_loaded || !IsAudioDeviceReady())
    {
        return;
    }

    SetMasterVolume(g_config.muted ? 0.0f : g_config.master_volume);
}

void SaveConfigToDisk()
{
    std::filesystem::create_directories("config");

    json j;
    j["highest_unlocked_level"] = g_config.highest_unlocked_level;
    j["master_volume"] = g_config.master_volume;
    j["muted"] = g_config.muted;

    std::ofstream outFile(kConfigPath);
    if (outFile.is_open())
    {
        outFile << j.dump(4);
    }
}

void EnsureConfigLoaded()
{
    if (g_config_loaded)
    {
        return;
    }

    PersistentConfig loaded = g_config;
    std::ifstream inFile(kConfigPath);
    if (inFile.is_open())
    {
        try
        {
            json j;
            inFile >> j;
            loaded.highest_unlocked_level = j.value("highest_unlocked_level", 1);
            loaded.master_volume = j.value("master_volume", 0.75f);
            loaded.muted = j.value("muted", false);
        }
        catch (const std::exception &err)
        {
            std::cout << "Config invalide, reset: " << err.what() << '\n';
        }
    }

    loaded.highest_unlocked_level = ClampInt(loaded.highest_unlocked_level, 1, kMaxLevels);
    loaded.master_volume = ClampFloat(loaded.master_volume, 0.0f, 1.0f);
    g_config = loaded;
    g_config_loaded = true;

    SaveConfigToDisk();
    ApplyAudioConfig();
}

bool IsLevelUnlocked(int levelNumber)
{
    EnsureConfigLoaded();
    return levelNumber <= g_config.highest_unlocked_level;
}

int HighestUnlockedLevel()
{
    EnsureConfigLoaded();
    return g_config.highest_unlocked_level;
}

void UnlockNextLevel(int completedLevel)
{
    EnsureConfigLoaded();
    const int nextLevel = ClampInt(completedLevel + 1, 1, kMaxLevels);
    if (nextLevel > g_config.highest_unlocked_level)
    {
        g_config.highest_unlocked_level = nextLevel;
        SaveConfigToDisk();
    }
}

bool IsMuted()
{
    EnsureConfigLoaded();
    return g_config.muted;
}

float MasterVolume()
{
    EnsureConfigLoaded();
    return g_config.master_volume;
}

void ToggleMute()
{
    EnsureConfigLoaded();
    g_config.muted = !g_config.muted;
    SaveConfigToDisk();
    ApplyAudioConfig();
}

void AdjustMasterVolume(float delta)
{
    EnsureConfigLoaded();
    g_config.master_volume = ClampFloat(g_config.master_volume + delta, 0.0f, 1.0f);
    if (g_config.master_volume > 0.0f && delta > 0.0f)
    {
        g_config.muted = false;
    }
    SaveConfigToDisk();
    ApplyAudioConfig();
}

void ResetProgress()
{
    EnsureConfigLoaded();
    g_config.highest_unlocked_level = 1;
    SaveConfigToDisk();
}
} // namespace

// Menu frame class
Menu::Menu()
{
    EnsureConfigLoaded();

    btn_w = 260;
    btn_h = 62;

    const float centerX = float((GetScreenWidth() - btn_w) / 2);

    play_btn.bg = {centerX, float((GetScreenHeight() / 2) - 10), float(btn_w), float(btn_h)};
    play_btn.bg_color = {39, 174, 96, 255};
    play_btn.text = "JOUER";
    play_btn.text_color = RAYWHITE;
    play_btn.fontSize = 30;

    settings_btn.bg = {centerX, float((GetScreenHeight() / 2) + 72), float(btn_w), float(btn_h)};
    settings_btn.bg_color = {52, 152, 219, 255};
    settings_btn.text = "PARAMETRES";
    settings_btn.text_color = RAYWHITE;
    settings_btn.fontSize = 28;
}

void Menu::draw()
{
    DrawRectangleGradientV(0, 0, GetScreenWidth(), GetScreenHeight(), {28, 38, 58, 255}, {11, 15, 23, 255});
    DrawRectangleGradientV(0, GetScreenHeight() / 2, GetScreenWidth(), GetScreenHeight() / 2, {23, 30, 45, 120}, {8, 10, 16, 20});

    const char *title = "BREAKOUT";
    const int titleSize = 78;
    const int titleX = (GetScreenWidth() - MeasureText(title, titleSize)) / 2;
    DrawText(title, titleX, 75, titleSize, {236, 240, 241, 255});

    const char *subtitle = "Progression + Parametres";
    const int subtitleSize = 28;
    const int subtitleX = (GetScreenWidth() - MeasureText(subtitle, subtitleSize)) / 2;
    DrawText(subtitle, subtitleX, 165, subtitleSize, {120, 170, 230, 255});

    DrawButtonLike(play_btn.bg, play_btn.text, play_btn.fontSize, play_btn.bg_color, {52, 195, 113, 255}, play_btn.text_color);
    DrawButtonLike(settings_btn.bg, settings_btn.text, settings_btn.fontSize, settings_btn.bg_color, {74, 174, 239, 255}, settings_btn.text_color);

    DrawText("Realise par DEKENI Toha", (GetScreenWidth() - MeasureText("Realise par DEKENI Toha", 24)) / 2, GetScreenHeight() - 68, 24, {224, 230, 240, 240});
    DrawText("Credits: Raylib, nlohmann/json, resources audio/images", (GetScreenWidth() - MeasureText("Credits: Raylib, nlohmann/json, resources audio/images", 18)) / 2, GetScreenHeight() - 38, 18, {170, 182, 200, 240});
}

void Menu::update(Frame_Type &frame, std::string &level)
{
    (void)level;
    if (IsHovered(play_btn.bg) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        frame = LEVELS;
        return;
    }

    if (IsHovered(settings_btn.bg) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        frame = SETTINGS;
    }
}

//--------------------------------------

// Levels frame class
Levels::Levels()
{
    EnsureConfigLoaded();

    rows = 6;
    cols = 5;
    buttonWidth = 86;
    buttonHeight = 44;
    spacing = 12;
    counter = 1;
    feedback_timer = 0;
    feedback_text.clear();

    const float totalWidth = (cols * buttonWidth) + ((cols - 1) * spacing);
    const float startX = (GetScreenWidth() - totalWidth) * 0.5f;
    const float startY = 115.0f;

    prev.bg = {22.0f, 18.0f, 110.0f, 44.0f};
    prev.bg_color = {186, 74, 74, 255};
    prev.text = "MENU";
    prev.text_color = RAYWHITE;
    prev.fontSize = 24;

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            levels[i][j].bg = {
                startX + j * (buttonWidth + spacing),
                startY + i * (buttonHeight + spacing),
                float(buttonWidth),
                float(buttonHeight)};
            levels[i][j].text_color = RAYWHITE;
            levels[i][j].fontSize = 24;

            labels[counter - 1] = std::to_string(counter);
            levels[i][j].text = labels[counter - 1].c_str();
            counter++;
        }
    }
}

int Levels::getLevelId(const Button &btn) const
{
    return std::max(1, std::atoi(btn.text));
}

void Levels::draw()
{
    EnsureConfigLoaded();
    DrawRectangleGradientV(0, 0, GetScreenWidth(), GetScreenHeight(), {25, 34, 52, 255}, {12, 16, 26, 255});

    DrawText("SELECTION DU NIVEAU", (GetScreenWidth() - MeasureText("SELECTION DU NIVEAU", 42)) / 2, 38, 42, {236, 240, 241, 255});
    DrawText("Niveaux bloques tant que le precedent n'est pas valide", (GetScreenWidth() - MeasureText("Niveaux bloques tant que le precedent n'est pas valide", 20)) / 2, 82, 20, {170, 185, 208, 255});

    const int unlockedUntil = HighestUnlockedLevel();

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            const int levelId = getLevelId(levels[i][j]);
            const bool unlocked = IsLevelUnlocked(levelId);
            const Color base = unlocked ? Color{52, 73, 94, 255} : Color{95, 95, 95, 255};
            const Color hover = unlocked ? Color{75, 104, 134, 255} : Color{115, 115, 115, 255};

            DrawButtonLike(levels[i][j].bg, levels[i][j].text, levels[i][j].fontSize, base, hover, levels[i][j].text_color);

            if (!unlocked)
            {
                DrawText("LOCK", int(levels[i][j].bg.x + 17.0f), int(levels[i][j].bg.y + 13.0f), 15, {230, 230, 230, 230});
            }
            else if (levelId == unlockedUntil)
            {
                DrawCircle(int(levels[i][j].bg.x + levels[i][j].bg.width - 9.0f), int(levels[i][j].bg.y + 9.0f), 5.0f, {70, 225, 130, 255});
            }
        }
    }

    DrawButtonLike(prev.bg, prev.text, prev.fontSize, prev.bg_color, {208, 88, 88, 255}, prev.text_color);

    if (feedback_timer > 0)
    {
        DrawText(feedback_text.c_str(), (GetScreenWidth() - MeasureText(feedback_text.c_str(), 24)) / 2, GetScreenHeight() - 40, 24, {255, 205, 120, 255});
    }
}

void Levels::update(Frame_Type &frame, std::string &level)
{
    if (feedback_timer > 0)
    {
        feedback_timer--;
    }

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (IsHovered(levels[i][j].bg) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                const int levelId = getLevelId(levels[i][j]);
                if (IsLevelUnlocked(levelId))
                {
                    level = "level" + std::to_string(levelId) + ".json";
                    frame = LEVEL1;
                    return;
                }

                feedback_text = "Termine le niveau " + std::to_string(levelId - 1) + " pour debloquer celui-ci";
                feedback_timer = 180;
            }
        }
    }

    if (IsHovered(prev.bg) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        frame = MENU;
    }
}

//--------------------------------------------------------
// Settings frame class
SettingsFrame::SettingsFrame()
{
    EnsureConfigLoaded();

    panel = {float((GetScreenWidth() / 2) - 245), 80.0f, 490.0f, 300.0f};

    mute_btn = {{panel.x + 35.0f, panel.y + 76.0f, 180.0f, 48.0f}, {52, 152, 219, 255}, "COUPER/ACTIVER", RAYWHITE, 20};
    volume_minus_btn = {{panel.x + 35.0f, panel.y + 145.0f, 85.0f, 48.0f}, {186, 74, 74, 255}, "-", RAYWHITE, 34};
    volume_plus_btn = {{panel.x + 130.0f, panel.y + 145.0f, 85.0f, 48.0f}, {39, 174, 96, 255}, "+", RAYWHITE, 34};
    reset_progress_btn = {{panel.x + panel.width - 235.0f, panel.y + 76.0f, 200.0f, 48.0f}, {160, 120, 55, 255}, "RESET PROGRESSION", RAYWHITE, 18};
    back_btn = {{panel.x + panel.width - 170.0f, panel.y + panel.height - 62.0f, 130.0f, 40.0f}, {186, 74, 74, 255}, "RETOUR", RAYWHITE, 22};
}

void SettingsFrame::draw()
{
    EnsureConfigLoaded();

    DrawRectangleGradientV(0, 0, GetScreenWidth(), GetScreenHeight(), {25, 34, 52, 255}, {12, 16, 26, 255});
    DrawRectangleRec(panel, {18, 24, 35, 240});
    DrawRectangleLinesEx(panel, 2.0f, {78, 94, 118, 255});

    DrawText("PARAMETRES", int(panel.x + 24.0f), int(panel.y + 20.0f), 42, {236, 240, 241, 255});
    DrawText(TextFormat("Son: %s", IsMuted() ? "COUPE" : "ACTIF"), int(panel.x + 35.0f), int(panel.y + 235.0f), 26, {226, 232, 240, 255});
    DrawText(TextFormat("Volume: %d%%", int(MasterVolume() * 100.0f + 0.5f)), int(panel.x + 35.0f), int(panel.y + 266.0f), 26, {226, 232, 240, 255});
    DrawText(TextFormat("Niveau debloque max: %d", HighestUnlockedLevel()), int(panel.x + panel.width - 245.0f), int(panel.y + 145.0f), 23, {220, 228, 238, 255});

    DrawButtonLike(mute_btn.bg, mute_btn.text, mute_btn.fontSize, mute_btn.bg_color, {74, 174, 239, 255}, mute_btn.text_color);
    DrawButtonLike(volume_minus_btn.bg, volume_minus_btn.text, volume_minus_btn.fontSize, volume_minus_btn.bg_color, {208, 88, 88, 255}, volume_minus_btn.text_color);
    DrawButtonLike(volume_plus_btn.bg, volume_plus_btn.text, volume_plus_btn.fontSize, volume_plus_btn.bg_color, {52, 195, 113, 255}, volume_plus_btn.text_color);
    DrawButtonLike(reset_progress_btn.bg, reset_progress_btn.text, reset_progress_btn.fontSize, reset_progress_btn.bg_color, {190, 145, 65, 255}, reset_progress_btn.text_color);
    DrawButtonLike(back_btn.bg, back_btn.text, back_btn.fontSize, back_btn.bg_color, {208, 88, 88, 255}, back_btn.text_color);

    DrawText("Raccourcis en jeu: M mute, +/- volume, Q ou ESC quitter le niveau", (GetScreenWidth() - MeasureText("Raccourcis en jeu: M mute, +/- volume, Q ou ESC quitter le niveau", 20)) / 2, GetScreenHeight() - 58, 20, {178, 194, 215, 255});
    DrawText("Realise par DEKENI Toha", (GetScreenWidth() - MeasureText("Realise par DEKENI Toha", 22)) / 2, GetScreenHeight() - 30, 22, {224, 230, 240, 240});
}

void SettingsFrame::update(Frame_Type &frame, std::string &level)
{
    (void)level;

    if (IsHovered(mute_btn.bg) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        ToggleMute();
    }
    if (IsHovered(volume_minus_btn.bg) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        AdjustMasterVolume(-0.05f);
    }
    if (IsHovered(volume_plus_btn.bg) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        AdjustMasterVolume(0.05f);
    }
    if (IsHovered(reset_progress_btn.bg) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        ResetProgress();
    }
    if ((IsHovered(back_btn.bg) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) || IsKeyPressed(KEY_ESCAPE))
    {
        frame = MENU;
    }
}

//--------------------------------------------------------

// Game frame class
Game::Game(std::string level)
    : isBegin(true),
      paused(false),
      isFinish(false),
      allInactive(false),
      playerWon(false),
      lives(3),
      livesRaduis(10.0f),
      levelNumber(1),
      sounds_loaded(false),
      owns_audio_device(false),
      unlock_reward_applied(false),
      lev(std::move(level)),
      score(0)
{
    EnsureConfigLoaded();

    paddle = {float(brick_width * 5), float(brick_height * 28), float(brick_width * 2), float(brick_height)};
    ball = {{paddle.x + paddle.width * 0.5f, paddle.y - 12.0f}, {3.0f, -3.0f}, 7, {236, 240, 241, 255}};

    if (!IsAudioDeviceReady())
    {
        InitAudioDevice();
        owns_audio_device = true;
    }

    if (IsAudioDeviceReady() && FileExists("resources/Hit1.wav") && FileExists("resources/Pickup8.wav"))
    {
        b_sound = LoadSound("resources/Hit1.wav");
        m_sound = LoadSound("resources/Pickup8.wav");
        sounds_loaded = true;
        ApplyAudioConfig();
    }

    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        particles[i].active = false;
        particles[i].life = 0.0f;
        particles[i].raduis = 0;
        particles[i].pos = {0.0f, 0.0f};
        particles[i].vel = {0.0f, 0.0f};
        particles[i].color = WHITE;
    }

    result_menu = {float((GetScreenWidth() / 2) - 210), float((GetScreenHeight() / 2) - 140), 420.0f, 280.0f};
    restart_btn = {result_menu.x + 35.0f, result_menu.y + result_menu.height - 70.0f, 155.0f, 42.0f};
    levels_btn = {result_menu.x + result_menu.width - 190.0f, result_menu.y + result_menu.height - 70.0f, 155.0f, 42.0f};
    quit_btn = {ui_frame.x + 16.0f, float(GetScreenHeight()) - 52.0f, ui_frame.width - 32.0f, 36.0f};

    levelNumber = getLevelNumber(lev);
    loadLevelData(lev);
    resetRound(true);
}

Game::~Game()
{
    if (file.is_open())
    {
        file.close();
    }

    if (sounds_loaded)
    {
        UnloadSound(b_sound);
        UnloadSound(m_sound);
        sounds_loaded = false;
    }

    if (owns_audio_device && IsAudioDeviceReady())
    {
        CloseAudioDevice();
    }
}

void Game::draw()
{
    DrawRectangleGradientV(0, 0, int(ui_frame.x), GetScreenHeight(), {31, 36, 48, 255}, {18, 22, 30, 255});
    DrawRectangleGradientV(int(ui_frame.x), 0, int(ui_frame.width), GetScreenHeight(), {15, 18, 26, 255}, {10, 12, 18, 255});
    DrawLineEx({ui_frame.x, 0.0f}, {ui_frame.x, float(GetScreenHeight())}, 2.0f, {80, 90, 120, 255});

    for (const auto &brick : bricks)
    {
        if (!brick.isActive)
        {
            continue;
        }
        DrawRectangleRec(brick.b, brick.color);
        DrawRectangleLinesEx(brick.b, 1.0f, {26, 32, 44, 255});
    }

    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (particles[i].active)
        {
            DrawCircleV(particles[i].pos, float(particles[i].raduis), particles[i].color);
        }
    }

    DrawRectangleRec(paddle, {52, 152, 219, 255});
    DrawRectangleLinesEx(paddle, 1.5f, {210, 220, 235, 255});
    DrawCircleV(ball.ball_pos, float(ball.raduis), ball.color);
    DrawCircleLines(int(ball.ball_pos.x), int(ball.ball_pos.y), float(ball.raduis), {70, 80, 95, 255});

    DrawText(TextFormat("Niveau %d", levelNumber), int(ui_frame.x + 18.0f), 20, 30, {232, 236, 242, 255});
    DrawText(TextFormat("Score: %d", score), int(ui_frame.x + 18.0f), 66, 33, {98, 178, 245, 255});
    DrawText("Vies", int(ui_frame.x + 18.0f), 118, 28, {232, 236, 242, 255});

    for (int i = 0; i < lives; i++)
    {
        DrawCircle(int(ui_frame.x + 55.0f + i * 32.0f), 166, livesRaduis, {230, 80, 90, 255});
        DrawCircleLines(int(ui_frame.x + 55.0f + i * 32.0f), 166, livesRaduis, {255, 220, 220, 255});
    }

    DrawText(TextFormat("Son: %s", IsMuted() ? "OFF" : "ON"), int(ui_frame.x + 18.0f), 214, 22, {210, 220, 235, 255});
    DrawText(TextFormat("Volume: %d%%", int(MasterVolume() * 100.0f + 0.5f)), int(ui_frame.x + 18.0f), 240, 22, {210, 220, 235, 255});
    DrawText("SPACE/CLIC: lancer", int(ui_frame.x + 18.0f), 279, 18, {170, 185, 208, 255});
    DrawText("P: pause", int(ui_frame.x + 18.0f), 304, 18, {170, 185, 208, 255});
    DrawText("M: mute, +/-: volume", int(ui_frame.x + 18.0f), 329, 18, {170, 185, 208, 255});
    DrawText("Q ou ESC: quitter", int(ui_frame.x + 18.0f), 354, 18, {170, 185, 208, 255});

    DrawButtonLike(quit_btn, "QUITTER NIVEAU", 20, {186, 74, 74, 255}, {208, 88, 88, 255}, RAYWHITE);

    if (isBegin && !isFinish)
    {
        DrawText("Pret ?", (int(ui_frame.x) - MeasureText("Pret ?", 42)) / 2, GetScreenHeight() - 125, 42, {228, 235, 245, 230});
    }

    if (paused && !isFinish)
    {
        DrawText("PAUSE", (int(ui_frame.x) - MeasureText("PAUSE", 56)) / 2, GetScreenHeight() / 2 - 25, 56, {251, 192, 45, 255});
    }

    if (isFinish)
    {
        drawOverlay();
    }
}

// This function checks if all breakable bricks are inactive
bool Game::AllBrokenBricksDeactivated(const std::vector<Brick> &currentBricks)
{
    bool hasBreakableBricks = false;
    for (const Brick &brick : currentBricks)
    {
        if (brick.brick_type == UNBREAKABLE)
        {
            continue;
        }

        hasBreakableBricks = true;
        if (brick.isActive)
        {
            return false;
        }
    }

    return hasBreakableBricks || currentBricks.empty();
}

void Game::spawnExplosion(Vector2 pos, Color color)
{
    int spawned = 0;
    for (int i = 0; i < MAX_PARTICLES && spawned < 12; i++)
    {
        if (particles[i].active)
        {
            continue;
        }

        particles[i].active = true;
        particles[i].pos = pos;
        particles[i].life = 0.25f + (float)GetRandomValue(0, 65) / 100.0f;
        particles[i].vel = {
            (float)GetRandomValue(-180, 180),
            (float)GetRandomValue(-180, 180)};
        particles[i].raduis = GetRandomValue(1, 4);
        particles[i].color = color;
        spawned++;
    }
}

void Game::updateParticles(float dt)
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (!particles[i].active)
        {
            continue;
        }

        particles[i].pos.x += particles[i].vel.x * dt;
        particles[i].pos.y += particles[i].vel.y * dt;
        particles[i].life -= dt;

        if (particles[i].life <= 0.0f)
        {
            particles[i].active = false;
        }
    }
}

void Game::handleBallBrickCollision()
{
    for (auto &brick : bricks)
    {
        if (!brick.isActive)
        {
            continue;
        }

        if (!CheckCollisionCircleRec(ball.ball_pos, ball.raduis, brick.b))
        {
            continue;
        }

        const float overlapLeft = (ball.ball_pos.x + ball.raduis) - brick.b.x;
        const float overlapRight = (brick.b.x + brick.b.width) - (ball.ball_pos.x - ball.raduis);
        const float overlapTop = (ball.ball_pos.y + ball.raduis) - brick.b.y;
        const float overlapBottom = (brick.b.y + brick.b.height) - (ball.ball_pos.y - ball.raduis);

        const float minOverlapX = std::min(overlapLeft, overlapRight);
        const float minOverlapY = std::min(overlapTop, overlapBottom);

        if (minOverlapX < minOverlapY)
        {
            ball.vel.x *= -1.0f;
            if (overlapLeft < overlapRight)
            {
                ball.ball_pos.x = brick.b.x - ball.raduis - 0.1f;
            }
            else
            {
                ball.ball_pos.x = brick.b.x + brick.b.width + ball.raduis + 0.1f;
            }
        }
        else
        {
            ball.vel.y *= -1.0f;
            if (overlapTop < overlapBottom)
            {
                ball.ball_pos.y = brick.b.y - ball.raduis - 0.1f;
            }
            else
            {
                ball.ball_pos.y = brick.b.y + brick.b.height + ball.raduis + 0.1f;
            }
        }

        if (brick.brick_type != UNBREAKABLE)
        {
            brick.numbers_of_lives--;
            if (brick.numbers_of_lives <= 0)
            {
                brick.isActive = false;
                score += 10 + levelNumber;
            }
            else
            {
                brick.color = colorForBrickLives(brick.numbers_of_lives, brick.brick_type);
            }
        }

        spawnExplosion({brick.b.x + brick.b.width * 0.5f, brick.b.y + brick.b.height * 0.5f}, brick.color);

        if (sounds_loaded)
        {
            PlaySound(b_sound);
        }
        break;
    }
}

void Game::drawOverlay() const
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {0, 0, 0, 165});
    DrawRectangleRec(result_menu, {16, 23, 36, 245});
    DrawRectangleLinesEx(result_menu, 2.0f, playerWon ? Color{52, 195, 113, 255} : Color{231, 76, 60, 255});

    const char *title = playerWon ? "VICTOIRE" : "GAME OVER";
    const int titleSize = 52;
    DrawText(title, int(result_menu.x + (result_menu.width - MeasureText(title, titleSize)) * 0.5f), int(result_menu.y + 22.0f), titleSize, playerWon ? Color{70, 225, 130, 255} : Color{255, 110, 90, 255});

    DrawText(TextFormat("Score final: %d", score), int(result_menu.x + 38.0f), int(result_menu.y + 102.0f), 30, {226, 232, 240, 255});
    DrawText(TextFormat("Niveau: %d", levelNumber), int(result_menu.x + 38.0f), int(result_menu.y + 136.0f), 28, {167, 190, 220, 255});

    if (playerWon && levelNumber < kMaxLevels)
    {
        const int unlocked = HighestUnlockedLevel();
        if (unlocked > levelNumber)
        {
            DrawText(TextFormat("Niveau %d debloque", unlocked), int(result_menu.x + 38.0f), int(result_menu.y + 172.0f), 24, {90, 220, 140, 255});
        }
    }

    DrawButtonLike(restart_btn, "REJOUER", 24, {41, 128, 185, 255}, {60, 152, 219, 255}, RAYWHITE);
    DrawButtonLike(levels_btn, "NIVEAUX", 24, {186, 74, 74, 255}, {210, 88, 88, 255}, RAYWHITE);
}

void Game::loadLevelData(const std::string &level)
{
    bricks.clear();
    j = json::object();

    bool loadedFromFile = false;
    const std::string path = "levels/" + level;
    std::ifstream levelFile(path);
    if (levelFile.is_open())
    {
        try
        {
            levelFile >> j;
            if (j.contains("bricks") && j["bricks"].is_array())
            {
                bricks = j["bricks"].get<std::vector<Brick>>();

                // Difficulty scaler even when loading from JSON levels.
                const int durabilityBonus = std::min(2, std::max(0, (levelNumber - 1) / 8));

                for (auto &brick : bricks)
                {
                    if (brick.brick_type == CLEAR || brick.numbers_of_lives <= 0)
                    {
                        brick.isActive = false;
                        continue;
                    }

                    if (brick.brick_type == UNBREAKABLE)
                    {
                        brick.numbers_of_lives = std::max(brick.numbers_of_lives, 999);
                    }
                    else
                    {
                        brick.numbers_of_lives = ClampInt(brick.numbers_of_lives + durabilityBonus, 1, 3);
                    }

                    brick.color = colorForBrickLives(brick.numbers_of_lives, brick.brick_type);
                    brick.isActive = true;
                }
                loadedFromFile = !bricks.empty();
            }
        }
        catch (const std::exception &err)
        {
            std::cout << "Erreur lecture niveau " << path << ": " << err.what() << '\n';
        }
    }

    if (!loadedFromFile)
    {
        buildProceduralLevel(levelNumber);
    }
}

void Game::buildProceduralLevel(int levelSeed)
{
    bricks.clear();

    const int safeSeed = std::max(1, levelSeed);
    const int rows = ClampInt(4 + safeSeed / 2, 4, 16);
    const int gapModulo = ClampInt(7 - safeSeed / 5, 3, 7);
    const int maxLivesByTier = ClampInt(1 + safeSeed / 7, 1, 3);
    const int unbreakableModulo = ClampInt(14 - safeSeed / 2, 5, 14);
    const int topOffset = 35;

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < max_brick_horizontale_number; j++)
        {
            const bool gap = ((i + (j * 2) + safeSeed) % gapModulo == 0) && i > 0;
            if (gap)
            {
                continue;
            }

            Brick brick{};
            brick.b = {
                float(j * brick_width),
                float(topOffset + i * brick_height),
                float(brick_width),
                float(brick_height)};
            brick.power_type = BIG_BALL;
            brick.isActive = true;

            const int roll = (i * 13 + j * 7 + safeSeed) % 100;
            const bool makeUnbreakable = safeSeed >= 4 && (roll % unbreakableModulo == 0);

            if (makeUnbreakable)
            {
                brick.brick_type = UNBREAKABLE;
                brick.numbers_of_lives = 999;
            }
            else
            {
                int livesForBrick = 1;
                if (maxLivesByTier >= 2 && ((roll + i) % 3 == 0))
                {
                    livesForBrick = 2;
                }
                if (maxLivesByTier >= 3 && ((roll + j) % 5 == 0))
                {
                    livesForBrick = 3;
                }

                brick.numbers_of_lives = livesForBrick;
                brick.brick_type = (livesForBrick == 3) ? TRIPLE_HIT : (livesForBrick == 2 ? DOUBLE_HIT : SINGLE_HIT);
            }

            brick.color = colorForBrickLives(brick.numbers_of_lives, brick.brick_type);
            bricks.push_back(brick);
        }
    }

    if (bricks.empty())
    {
        for (int j = 0; j < max_brick_horizontale_number; j++)
        {
            Brick brick{};
            brick.b = {float(j * brick_width), 70.0f, float(brick_width), float(brick_height)};
            brick.brick_type = SINGLE_HIT;
            brick.power_type = BIG_BALL;
            brick.numbers_of_lives = 1;
            brick.color = colorForBrickLives(1, SINGLE_HIT);
            brick.isActive = true;
            bricks.push_back(brick);
        }
    }
}

void Game::resetRound(bool fullReset)
{
    if (fullReset)
    {
        lives = 3;
        score = 0;
        isFinish = false;
        playerWon = false;
        unlock_reward_applied = false;
    }

    allInactive = false;
    paused = false;
    isBegin = true;

    const float paddleBrickUnits = ClampFloat(3.2f - (levelNumber * 0.055f), 1.6f, 3.2f);
    paddle = {float(brick_width * 5), float(brick_height * 28), float(brick_width * paddleBrickUnits), float(brick_height)};

    const float baseSpeed = 2.2f + std::min(levelNumber, 30) * 0.08f;
    ball.raduis = int(ClampFloat(9.0f - levelNumber * 0.1f, 5.0f, 9.0f));
    ball.color = {236, 240, 241, 255};
    ball.vel = {baseSpeed, -baseSpeed};
    ball.ball_pos = {paddle.x + paddle.width * 0.5f, paddle.y - ball.raduis - 1.0f};

    if (fullReset)
    {
        for (int i = 0; i < MAX_PARTICLES; i++)
        {
            particles[i].active = false;
            particles[i].life = 0.0f;
        }
    }
}

int Game::getLevelNumber(const std::string &level) const
{
    int value = 0;
    for (char c : level)
    {
        if (std::isdigit(static_cast<unsigned char>(c)))
        {
            value = (value * 10) + (c - '0');
        }
    }

    return value > 0 ? value : 1;
}

Color Game::colorForBrickLives(int livesValue, Brick_Type type) const
{
    if (type == UNBREAKABLE)
    {
        return {130, 140, 152, 255};
    }

    if (livesValue >= 3)
    {
        return {46, 204, 113, 255};
    }
    if (livesValue == 2)
    {
        return {241, 196, 15, 255};
    }
    return {231, 76, 60, 255};
}

void Game::update(Frame_Type &frame, std::string &level)
{
    (void)level;
    const Vector2 mouse = GetMousePosition();

    if (IsKeyPressed(KEY_M))
    {
        ToggleMute();
    }
    if (IsKeyPressed(KEY_MINUS) || IsKeyPressed(KEY_KP_SUBTRACT))
    {
        AdjustMasterVolume(-0.05f);
    }
    if (IsKeyPressed(KEY_EQUAL) || IsKeyPressed(KEY_KP_ADD))
    {
        AdjustMasterVolume(0.05f);
    }

    if (IsKeyPressed(KEY_Q) || IsKeyPressed(KEY_ESCAPE) || (IsHovered(quit_btn) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)))
    {
        frame = LEVELS;
        return;
    }

    if (isFinish)
    {
        if (IsHovered(restart_btn) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            loadLevelData(lev);
            resetRound(true);
        }
        else if (IsHovered(levels_btn) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            frame = LEVELS;
            return;
        }

        updateParticles(GetFrameTime());
        return;
    }

    if (IsKeyPressed(KEY_P))
    {
        paused = !paused;
    }

    const float clampedMouseX = ClampFloat(mouse.x, paddle.width * 0.5f, ui_frame.x - paddle.width * 0.5f);
    paddle.x = clampedMouseX - (paddle.width * 0.5f);

    if (isBegin)
    {
        ball.ball_pos.x = paddle.x + paddle.width * 0.5f;
        ball.ball_pos.y = paddle.y - ball.raduis - 1.0f;

        if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            isBegin = false;
            paused = false;
        }

        updateParticles(GetFrameTime());
        return;
    }

    if (paused)
    {
        updateParticles(GetFrameTime());
        return;
    }

    ball.ball_pos.y += ball.vel.y;
    ball.ball_pos.x += ball.vel.x;

    if (ball.ball_pos.x <= ball.raduis)
    {
        ball.ball_pos.x = float(ball.raduis);
        ball.vel.x = std::fabs(ball.vel.x);
        if (sounds_loaded)
        {
            PlaySound(m_sound);
        }
    }
    if (ball.ball_pos.x >= ui_frame.x - ball.raduis)
    {
        ball.ball_pos.x = ui_frame.x - ball.raduis;
        ball.vel.x = -std::fabs(ball.vel.x);
        if (sounds_loaded)
        {
            PlaySound(m_sound);
        }
    }
    if (ball.ball_pos.y <= ball.raduis)
    {
        ball.ball_pos.y = float(ball.raduis);
        ball.vel.y = std::fabs(ball.vel.y);
        if (sounds_loaded)
        {
            PlaySound(m_sound);
        }
    }

    if (CheckCollisionCircleRec(ball.ball_pos, ball.raduis, paddle) && ball.vel.y > 0.0f)
    {
        float hitFactor = ((ball.ball_pos.x - paddle.x) / paddle.width) * 2.0f - 1.0f;
        hitFactor = ClampFloat(hitFactor, -1.0f, 1.0f);

        const float speed = std::sqrt((ball.vel.x * ball.vel.x) + (ball.vel.y * ball.vel.y));
        ball.vel.x = hitFactor * speed * 0.9f;
        const float verticalPart = std::sqrt(std::max(4.0f, (speed * speed) - (ball.vel.x * ball.vel.x)));
        ball.vel.y = -verticalPart;
        ball.ball_pos.y = paddle.y - ball.raduis - 1.0f;

        if (sounds_loaded)
        {
            PlaySound(m_sound);
        }
    }

    handleBallBrickCollision();

    if (ball.ball_pos.y - ball.raduis > GetScreenHeight())
    {
        lives--;
        if (lives <= 0)
        {
            lives = 0;
            isFinish = true;
            playerWon = false;
        }
        else
        {
            resetRound(false);
        }
    }

    allInactive = AllBrokenBricksDeactivated(bricks);
    if (allInactive && !isFinish)
    {
        isFinish = true;
        playerWon = true;
        if (!unlock_reward_applied)
        {
            UnlockNextLevel(levelNumber);
            unlock_reward_applied = true;
        }
    }

    updateParticles(GetFrameTime());
}
//-------------------------------------------------------------
