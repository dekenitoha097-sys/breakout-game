#pragma once
#include "raylib.h"
#include <vector>
#include <string>
#include "json.hpp"

using json = nlohmann::json;

inline bool ColorsAreEqual(Color a, Color b)
{
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}
// Type of break
enum Brick_Type
{
    SINGLE_HIT,  // Breaks in one hit
    DOUBLE_HIT,  // Bricks in two hit
    TRIPLE_HIT,  // Bricks int tree hit
    UNBREAKABLE, // Cannot be breaken (metallic)
    CLEAR
};

// type of power the break can give
enum Power_Type
{
    BIG_BALL,    // Doubles the size of the ball
    LONG_PADDLE, // Doubles the lenght of de paddle
    MULTI_BALL   // Spawns three balls simultaneously
};

//
struct Brick
{
    Rectangle b;
    Color color;
    int numbers_of_lives;
    Brick_Type brick_type;
    Power_Type power_type;
    bool isActive;
};

// Structure representing a UI button in the game
struct Button
{
    Rectangle bg;
    Color bg_color = GREEN;
    const char *text = "BUTTON";
    Color text_color = BLACK;
    int fontSize = 20;
    void draw()
    {
        DrawRectangleRec(bg, bg_color);
        DrawText(text, (bg.x + (bg.width - MeasureText(text, fontSize)) / 2), (bg.y + (bg.height - fontSize) / 2), fontSize, text_color);
    }
};

class Window
{
protected:
    const int brick_width = 30;
    const int brick_height = 15;

    const int max_brick_verticale_number = 30;
    const int max_brick_horizontale_number = 12;
    const int ui_part = 10;

    Rectangle ui_frame = {float(brick_width * (max_brick_horizontale_number)), 0.0f, float(brick_width *(ui_part)), float(max_brick_verticale_number *brick_height)};

    std::vector<Brick> bricks;

public:
    Window() = default;
    virtual void draw() = 0;
    virtual void update() = 0;
    virtual void run() = 0;
};

class Editor : public Window
{
private:
    std::vector<Color> brick_type_color{RED, YELLOW, GREEN, GRAY, LIGHTGRAY};
    std::vector<std::string> brick_type_name{"1-Hit Brick", "2-Hit Brick", "3-Hit Brick", "Metal Brick", "Clear"};
    std::vector<Button> brick_type_btn;
    Rectangle current_brick_select;
    Brick_Type brick_type = SINGLE_HIT;
    std::string file_name = "file name";
    Rectangle input = {float(brick_width * (max_brick_horizontale_number)) + brick_width,
                       float(brick_width * 10),
                       float(brick_width *(ui_part - 2)),
                       50.0};
    Button save = {
        {float(brick_width * (max_brick_horizontale_number)) + brick_width,
         float(brick_width * 12),
         float(brick_width *(ui_part - 2)),
         50.0},
        ORANGE,
        "SAVE"};
    json json_file;
    std::vector<Brick> file_brick;
    Font myFont = LoadFont("cmu.ttf");

public:
    Editor();
    ~Editor();
    void draw() override;
    void update() override;
    void run() override;
    void InitBrikcs();
    void InitUiButton();
    void draw_ui();
    void changeCurrentBrick();
    void edit_brick();
    void draw_form();
    void update_input();
    void save_data();
};

// These functions allow converting structs to JSON and vice versa.
std::string BrickTypeToString(Brick_Type type);
Brick_Type StringToBrickType(const std::string &str);

std::string PowerTypeToString(Power_Type type);
Power_Type StringToPowerType(const std::string &str);

void to_json(json &j, const Brick &brick);
void from_json(const json &j, Brick &brick);