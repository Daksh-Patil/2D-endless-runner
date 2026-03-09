#include "Animation.hpp"
#include <cstdio>
#include <iostream>


std::string formatFilename(const std::string &prefix, int index,
                           const std::string &suffix) {
  char buffer[256];
  std::snprintf(buffer, sizeof(buffer), "%s%05d%s", prefix.c_str(), index,
                suffix.c_str());
  return std::string(buffer);
}

void Animation::load(const std::string &folder, const std::string &prefix,
                     int count) {
  for (int i = 0; i < count; ++i) {
    sf::Texture tex;
    if (tex.loadFromFile(folder + "/" + formatFilename(prefix, i))) {
      frames.push_back(tex);
    } else {
      std::cerr << "Failed to load " << formatFilename(prefix, i) << std::endl;
    }
  }
}

void Animator::setAnimation(Animation &anim) {
  if (currentAnim != &anim) {
    currentAnim = &anim;
    elapsedTime = 0.f;
    currentFrame = 0;
  }
}

void Animator::update(float dt, sf::Sprite &sprite) {
  if (!currentAnim || currentAnim->frames.empty())
    return;
  elapsedTime += dt;
  if (elapsedTime >= currentAnim->frameDuration) {
    elapsedTime -= currentAnim->frameDuration;
    currentFrame = (currentFrame + 1) % currentAnim->frames.size();
  }
  sprite.setTexture(currentAnim->frames[currentFrame], true);
}
