#include "Platform.hpp"

Platform::Platform(const sf::Texture &tex, sf::Vector2f pos, float widthScale) {
  sprite.setTexture(tex);
  sprite.setPosition(pos);
  sprite.setScale(widthScale, 0.18f); // Height fixed, width varies
}

void Platform::addSlime(Animation &slimeAnim) {
  float h = sprite.getGlobalBounds().height;
  sf::Vector2f slimePos = sprite.getPosition();
  slimePos.x += sprite.getGlobalBounds().width * 0.5f; // Middle of platform
  slimePos.y += 0;                                     // Top of platform
  slime = std::make_unique<Enemy>(slimeAnim, slimePos, h);
}

void Platform::update(float dt, float speed) {
  sprite.move(-speed * dt, 0);
  if (slime) {
    slime->update(dt, speed);
  }
}

void Platform::draw(sf::RenderWindow &window) {
  window.draw(sprite);
  if (slime && slime->active) {
    window.draw(slime->sprite);
  }
}

sf::FloatRect Platform::getBounds() const { return sprite.getGlobalBounds(); }
