#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#include "Animation.hpp"
#include "Enemy.hpp"
#include <SFML/Graphics.hpp>
#include <memory>


class Platform {
public:
  sf::Sprite sprite;
  std::unique_ptr<Enemy> slime;

  Platform(const sf::Texture &tex, sf::Vector2f pos, float widthScale);
  void addSlime(Animation &slimeAnim);
  void update(float dt, float speed);
  void draw(sf::RenderWindow &window);
  sf::FloatRect getBounds() const;
};

#endif // PLATFORM_HPP
