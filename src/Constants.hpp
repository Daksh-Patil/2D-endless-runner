#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <random>

// Window Settings
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

// Physics Constants
const float GRAVITY_CONST = 2100.f;
const float JUMP_FORCE_CONST = -920.f;

// Extern for global RNG
extern std::mt19937 rng;

#endif // CONSTANTS_HPP
