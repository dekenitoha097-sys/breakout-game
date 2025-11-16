#include "json.hpp"
#include "raylib.h"
#include "editor.hpp"
#include "json.hpp"
using json = nlohmann::json;

// Brick_Type → string
inline std::string BrickTypeToString(Brick_Type type)
{
    switch (type)
    {
    case SINGLE_HIT:
        return "SINGLE_HIT";
    case DOUBLE_HIT:
        return "DOUBLE_HIT";
    case TRIPLE_HIT:
        return "TRIPLE_HIT";
    case UNBREAKABLE:
        return "UNBREAKABLE";
    case CLEAR:
        return "CLEAR";
    }
    return "CLEAR";
}

inline Brick_Type StringToBrickType(const std::string &str)
{
    if (str == "SINGLE_HIT")
        return SINGLE_HIT;
    if (str == "DOUBLE_HIT")
        return DOUBLE_HIT;
    if (str == "TRIPLE_HIT")
        return TRIPLE_HIT;
    if (str == "UNBREAKABLE")
        return UNBREAKABLE;
    return CLEAR;
}

// Power_Type → string
inline std::string PowerTypeToString(Power_Type type)
{
    switch (type)
    {
    case BIG_BALL:
        return "BIG_BALL";
    case LONG_PADDLE:
        return "LONG_PADDLE";
    case MULTI_BALL:
        return "MULTI_BALL";
    }
    return "BIG_BALL";
}

inline Power_Type StringToPowerType(const std::string &str)
{
    if (str == "BIG_BALL")
        return BIG_BALL;
    if (str == "LONG_PADDLE")
        return LONG_PADDLE;
    if (str == "MULTI_BALL")
        return MULTI_BALL;
    return BIG_BALL;
}

void to_json(json &j, const Brick &brick)
{
    j = json{
        {"x", brick.b.x},
        {"y", brick.b.y},
        {"width", brick.b.width},
        {"height", brick.b.height},
        {"color_r", brick.color.r},
        {"color_g", brick.color.g},
        {"color_b", brick.color.b},
        {"color_a", brick.color.a},
        {"numbers_of_lives", brick.numbers_of_lives},
        {"brick_type", BrickTypeToString(brick.brick_type)},
        {"power_type", PowerTypeToString(brick.power_type)},
        {"isActive", brick.isActive}};
}

void from_json(const json &j, Brick &brick)
{
    brick.b.x = j.at("x").get<float>();
    brick.b.y = j.at("y").get<float>();
    brick.b.width = j.at("width").get<float>();
    brick.b.height = j.at("height").get<float>();

    brick.color.r = j.at("color_r").get<unsigned char>();
    brick.color.g = j.at("color_g").get<unsigned char>();
    brick.color.b = j.at("color_b").get<unsigned char>();
    brick.color.a = j.at("color_a").get<unsigned char>();

    brick.numbers_of_lives = j.at("numbers_of_lives").get<int>();
    brick.brick_type = StringToBrickType(j.at("brick_type").get<std::string>());
    brick.power_type = StringToPowerType(j.at("power_type").get<std::string>());
    brick.isActive = j.at("isActive").get<bool>();
}
