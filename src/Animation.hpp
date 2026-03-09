#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

std::string formatFilename(const std::string &prefix, int index,
                           const std::string &suffix = ".png");

class Animation {
public:
  std::vector<sf::Texture> frames;
  float frameDuration = 0.05f;

  void load(const std::string &folder, const std::string &prefix, int count);
};

class Animator {
  Animation *currentAnim = nullptr;
  float elapsedTime = 0.f;
  int currentFrame = 0;

public:
  void setAnimation(Animation &anim);
  void update(float dt, sf::Sprite &sprite);
};

#endif // ANIMATION_HPP
