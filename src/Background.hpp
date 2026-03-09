#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>


class Background {
  struct Layer {
    sf::Texture texture;
    sf::Sprite sprite1, sprite2;
    float speedMultiplier;
    float offset = 0.f;
  };
  std::vector<std::unique_ptr<Layer>> layers;

public:
  void load(const std::string &folder,
            const std::vector<std::string> &filenames,
            const std::vector<float> &speeds);
  void update(float dt, float gameSpeed);
  void draw(sf::RenderWindow &window);
};

#endif // BACKGROUND_HPP
