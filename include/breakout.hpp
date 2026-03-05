#pragma once
#include "raylib.h"
#include "json.hpp"
#include <fstream>
#include "editor.hpp"
#include <vector>
#include <memory>
#include <string>

using json = nlohmann::json;

enum Frame_Type
{
    MENU,
    LEVELS,
    SETTINGS,
    LEVEL1,
};

// Ball struct
struct Ball
{
    Vector2 ball_pos;
    Vector2 vel;
    int raduis;
    Color color;
};

// Particule struct
struct Particle
{
    Vector2 pos;
    Vector2 vel;
    float life;
    bool active;
    int raduis;
    Color color;
};

// Virtual bas class for frame system manager
class Frame
{
protected:
public:
    virtual ~Frame() = default;
    virtual void draw() = 0;
    virtual void update(Frame_Type &frame, std::string &level) = 0;
};
// ------------------------------------------------

// Menu frame class
class Menu : public Frame
{
private:
    int btn_w;
    int btn_h;
    Button play_btn;
    Button settings_btn;

public:
    Menu();
    void draw() override;
    void update(Frame_Type &frame, std::string &level) override;
};

//--------------------------------------------------

// Levels frame class
class Levels : public Frame
{
private:
    Button levels[6][5];

    int rows;
    int cols;
    int buttonWidth;
    int buttonHeight;
    int spacing;
    int counter;

    std::string labels[30];
    Button prev;
    int feedback_timer;
    std::string feedback_text;

    int getLevelId(const Button &btn) const;

public:
    Levels();
    void draw() override;
    void update(Frame_Type &frame, std::string &level) override;
};

//----------------------------------------------------

class SettingsFrame : public Frame
{
private:
    Rectangle panel;
    Button mute_btn;
    Button volume_minus_btn;
    Button volume_plus_btn;
    Button reset_progress_btn;
    Button back_btn;

public:
    SettingsFrame();
    void draw() override;
    void update(Frame_Type &frame, std::string &level) override;
};

//----------------------------------------------------

// Gameplay frame class
class Game : public Frame
{
private:
    // Window seting values
    const int brick_width = 30;
    const int brick_height = 15;
    const int max_brick_verticale_number = 30;
    const int max_brick_horizontale_number = 12;
    const int ui_part = 10;
    Rectangle ui_frame = {float(brick_width * (max_brick_horizontale_number)), 0.0f, float(brick_width *(ui_part)), float(max_brick_verticale_number *brick_height)};

    Rectangle paddle;
    std::ifstream file;
    json j;
    std::vector<Brick> bricks; // The struct Brick is define in header editor.hpp
    Ball ball;
    bool isBegin;
    bool paused;

    Sound b_sound;
    Sound m_sound;

    bool isFinish;
    bool allInactive;
    bool playerWon;

    int lives;
    float livesRaduis;
    int levelNumber;

    static const int MAX_PARTICLES = 80;
    Particle particles[MAX_PARTICLES];

    Rectangle result_menu;
    Rectangle restart_btn;
    Rectangle levels_btn;
    Rectangle quit_btn;

    bool sounds_loaded;
    bool owns_audio_device;
    bool unlock_reward_applied;

    std::string lev;
    int score;

public:
    Game(std::string level);
    ~Game();
    void draw() override;
    void update(Frame_Type &frame, std::string &level) override;
    bool AllBrokenBricksDeactivated(const std::vector<Brick> &bricks);
    void spawnExplosion(Vector2 pos, Color color);
    void updateParticles(float dt);
    void handleBallBrickCollision();
    void drawOverlay() const;
    void loadLevelData(const std::string &level);
    void buildProceduralLevel(int levelSeed);
    void resetRound(bool fullReset);
    int getLevelNumber(const std::string &level) const;
    Color colorForBrickLives(int livesValue, Brick_Type type) const;
};

//----------------------------------------------------

// Breakout game manager
class Breakout
{
private:
    const int brick_width = 30;
    const int brick_height = 15;
    const int max_brick_verticale_number = 30;
    const int max_brick_horizontale_number = 12;
    const int ui_part = 10;

    std::unique_ptr<Frame> current_frame;
    Frame_Type frame;

    std::string level = "level1.json";

public:
    Breakout();
    ~Breakout();
    void draw();
    void update();
    void run();
};

//-------------------------------------------------
