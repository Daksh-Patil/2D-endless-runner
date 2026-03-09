#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const float GRAVITY = 1500.0f;
const float JUMP_VELOCITY = -800.0f;
const float BASE_SPEED = 400.0f;
const float GROUND_Y = 600.0f;

string padZero(int num, int zeros) {
  std::ostringstream ss;
  ss << std::setw(zeros) << std::setfill('0') << num;
  return ss.str();
}

struct AnimData {
  vector<sf::Texture> textures;
  float frameTime;

  void load(const string &path, const string &prefix, int start, int end,
            int pad) {
    for (int i = start; i <= end; ++i) {
      sf::Texture tex;
      string filename = path + "/" + prefix + padZero(i, pad) + ".png";
      if (!tex.loadFromFile(filename)) {
        cout << "Failed to load: " << filename << endl;
      } else {
        textures.push_back(tex);
      }
    }
  }
};

class Layer {
public:
  sf::Texture texture;
  sf::Sprite sprite1, sprite2;
  float scrollSpeedMult;
  float x;

  Layer() : x(0), scrollSpeedMult(0) {}

  void load(const string &filename, float speedMult) {
    if (texture.loadFromFile(filename)) {
      sprite1.setTexture(texture);
      sprite2.setTexture(texture);
      // Scale to screen height
      float scaleY = (float)WINDOW_HEIGHT / texture.getSize().y;
      float scaleX = scaleY; // keep aspect ratio, wait parallax usually spans,
                             // let's stretch slightly or just scale uniformly
      sprite1.setScale(scaleX, scaleY);
      sprite2.setScale(scaleX, scaleY);
      sprite2.setPosition(texture.getSize().x * scaleX, 0);
      scrollSpeedMult = speedMult;
    }
  }

  void update(float dt, float baseSpeed) {
    float speed = baseSpeed * scrollSpeedMult;
    x -= speed * dt;
    float width = texture.getSize().x * sprite1.getScale().x;
    if (x <= -width) {
      x += width;
    }
    sprite1.setPosition(x, 0);
    sprite2.setPosition(x + width - 1, 0); // -1 to avoid gaps
  }

  void draw(sf::RenderWindow &window) {
    window.draw(sprite1);
    window.draw(sprite2);
  }
};

class Entity {
public:
  sf::Sprite sprite;
  sf::FloatRect bounds;
  virtual void update(float dt, float speed) = 0;
  virtual void draw(sf::RenderWindow &window) { window.draw(sprite); }
};

class Slime : public Entity {
  AnimData *anim;
  float animTimer;
  int currentFrame;

public:
  bool active;
  Slime(AnimData *a, float x, float y)
      : anim(a), animTimer(0), currentFrame(0), active(true) {
    if (!anim->textures.empty()) {
      sprite.setTexture(anim->textures[0]);
      // Scale down slime. Originally 0.5f, user wants 60% of that -> 0.3f
      sprite.setScale(0.3f, 0.3f);
      sf::FloatRect rect = sprite.getLocalBounds();
      sprite.setOrigin(rect.width / 2.0f, rect.height);
      sprite.setPosition(x, y);
    }
  }

  void update(float dt, float speed) override {
    sprite.move(-speed * dt, 0);
    animTimer += dt;
    if (animTimer >= anim->frameTime && !anim->textures.empty()) {
      animTimer -= anim->frameTime;
      currentFrame = (currentFrame + 1) % anim->textures.size();
      sprite.setTexture(anim->textures[currentFrame]);
    }
    if (sprite.getPosition().x < -200) {
      active = false;
    }
    bounds = sprite.getGlobalBounds();
    // Shrink bounds slightly for fairer gameplay
    bounds.left += bounds.width * 0.2f;
    bounds.width *= 0.6f;
    bounds.top += bounds.height * 0.2f;
    bounds.height *= 0.8f;
  }
};

class Platform : public Entity {
public:
  bool active;
  Platform(sf::Texture &tex, float x, float y, float scale) : active(true) {
    sprite.setTexture(tex);
    sprite.setScale(scale, scale);
    sprite.setPosition(x, y);
  }

  void update(float dt, float speed) override {
    sprite.move(-speed * dt, 0);
    if (sprite.getPosition().x + sprite.getGlobalBounds().width < 0) {
      active = false;
    }
    bounds = sprite.getGlobalBounds();
    // Platform collision is usually top surface, but the image has a top margin
    // Let's adjust top and height
    bounds.top +=
        bounds.height * 0.35f; // Adjust top gap to sink player into grass
    bounds.height = 10.0f;     // Only top part is solid
  }
};

class Player {
public:
  AnimData *walkAnim;
  AnimData *jumpAnim;
  sf::Sprite sprite;
  float velocityY;
  bool isJumping;
  float animTimer;
  int currentFrame;

  Player(AnimData *walk, AnimData *jump)
      : walkAnim(walk), jumpAnim(jump), velocityY(0), isJumping(false),
        animTimer(0), currentFrame(0) {
    if (!walkAnim->textures.empty()) {
      sprite.setTexture(walkAnim->textures[0]);
      sprite.setScale(0.3f, 0.3f);
      sf::FloatRect rect = sprite.getLocalBounds();
      // Origin at center bottom
      sprite.setOrigin(rect.width / 2.0f, rect.height);
      sprite.setPosition(200.0f, GROUND_Y);
    }
  }

  void jump() {
    if (!isJumping) {
      velocityY = JUMP_VELOCITY;
      isJumping = true;
      currentFrame = 0;
    }
  }

  sf::FloatRect getBounds() {
    sf::FloatRect rect = sprite.getGlobalBounds();
    // Shrink for fair collision
    rect.left += rect.width * 0.3f;
    rect.width *= 0.4f;
    rect.top += rect.height * 0.1f;
    rect.height *= 0.9f;
    return rect;
  }

  void update(float dt) {
    velocityY += GRAVITY * dt;

    sf::Vector2f pos = sprite.getPosition();
    float nextY = pos.y + velocityY * dt;

    // Check collision with ground
    if (nextY >= GROUND_Y) {
      nextY = GROUND_Y;
      velocityY = 0;
      isJumping = false;
    }

    sprite.setPosition(pos.x, nextY);

    // Animation
    AnimData *currentAnim = isJumping ? jumpAnim : walkAnim;
    if (currentAnim && !currentAnim->textures.empty()) {
      animTimer += dt;
      if (animTimer >= currentAnim->frameTime) {
        animTimer -= currentAnim->frameTime;
        if (isJumping) {
          // Stop at last frame for jump
          if (currentFrame < currentAnim->textures.size() - 1) {
            currentFrame++;
          }
        } else {
          currentFrame = (currentFrame + 1) % currentAnim->textures.size();
        }
        sprite.setTexture(currentAnim->textures[currentFrame]);
      }
    }
  }

  void draw(sf::RenderWindow &window) { window.draw(sprite); }
};

int main() {
  sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
                          "2D Runner Wizard",
                          sf::Style::Titlebar | sf::Style::Close);
  window.setFramerateLimit(60);

  // Load Parallax
  vector<Layer> backgroundLayers(9);
  for (int i = 1; i <= 9; ++i) {
    string fname = "Parallax Cave/" + to_string(i) +
                   (i == 3 || i == 6 || i == 8 ? "fx" : "") + ".png";
    float speedStr = i * 0.1f;
    backgroundLayers[i - 1].load(fname, speedStr);
  }

  // Load platform texture for ground
  sf::Texture platformTex;
  if (!platformTex.loadFromFile("Mossy - FloatingPlatforms.png")) {
    cout << "Failed to load platform texture" << endl;
  }

  // Load Animations
  AnimData walkAnim, jumpAnim, slimeAnim;
  walkAnim.frameTime = 0.05f;
  jumpAnim.frameTime = 0.05f;
  slimeAnim.frameTime = 0.05f;

  walkAnim.load("BlueWizard/2BlueWizardWalk", "Chara_BlueWalk", 0, 19, 5);
  jumpAnim.load("BlueWizard/2BlueWizardJump", "CharaWizardJump_", 0, 7, 5);
  slimeAnim.load("SlimeGreen", "SlimeBasic_", 0, 29, 5);

  Player player(&walkAnim, &jumpAnim);
  vector<Slime> slimes;

  // Ground sprites
  sf::Sprite groundSprite1, groundSprite2;
  groundSprite1.setTexture(platformTex);
  groundSprite2.setTexture(platformTex);
  float groundScale = 0.5f;
  float scaledHeight = platformTex.getSize().y * groundScale;
  float groundY = GROUND_Y - 0.35f * scaledHeight; // Position so solid part is at GROUND_Y
  groundSprite1.setScale(groundScale, groundScale);
  groundSprite2.setScale(groundScale, groundScale);
  groundSprite1.setPosition(0, groundY);
  groundSprite2.setPosition(platformTex.getSize().x * groundScale, groundY);

  sf::Clock clock;
  float slimeSpawnTimer = 0.0f;
  float nextSpawnTime = 1.0f + static_cast<float>(rand() % 200) / 100.0f; // 1.0 to 3.0 seconds
  float gameSpeed = BASE_SPEED;
  bool gameOver = false;

  sf::Font font;
  sf::Text scoreText;
  bool hasFont = false;
  // Try to load a system font
  if (font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
    hasFont = true;
    scoreText.setFont(font);
    scoreText.setCharacterSize(30);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(20.f, 20.f);
  }

  float score = 0;

  auto resetGame = [&]() {
    slimes.clear();
    player.velocityY = 0;
    player.sprite.setPosition(200.0f, GROUND_Y);
    player.isJumping = false;
    gameOver = false;
    score = 0;
    gameSpeed = BASE_SPEED;
  };

  resetGame();

  while (window.isOpen()) {
    float dt = clock.restart().asSeconds();
    if (dt > 0.1f)
      dt = 0.1f;

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();

      if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Space ||
            event.key.code == sf::Keyboard::Up) {
          if (!gameOver) {
            player.jump();
          } else {
            resetGame();
          }
        }
      }
    }

    if (!gameOver) {
      gameSpeed += dt * 5.0f; // Gradually increase speed
      score += dt * 10.0f;

      // Background update
      for (auto &layer : backgroundLayers) {
        layer.update(dt, gameSpeed);
      }

      // Ground update
      float groundSpeed = gameSpeed;
      groundSprite1.move(-groundSpeed * dt, 0);
      groundSprite2.move(-groundSpeed * dt, 0);
      float groundWidth = platformTex.getSize().x * groundScale;
      if (groundSprite1.getPosition().x + groundWidth < 0) {
        groundSprite1.setPosition(groundSprite2.getPosition().x + groundWidth, groundY);
      }
      if (groundSprite2.getPosition().x + groundWidth < 0) {
        groundSprite2.setPosition(groundSprite1.getPosition().x + groundWidth, groundY);
      }

      // Spawn slimes
      slimeSpawnTimer += dt;
      if (slimeSpawnTimer > nextSpawnTime) {
        slimeSpawnTimer = 0;
        nextSpawnTime = 1.0f + static_cast<float>(rand() % 200) / 100.0f; // 1.0 to 3.0 seconds

        // Spawn 1 or 2 slimes
        int numSlimes = 1 + (rand() % 2); // 1 or 2
        float baseX = WINDOW_WIDTH + 50.0f;
        for (int i = 0; i < numSlimes; ++i) {
          float offsetX = i * 100.0f; // 100 pixels apart if multiple
          slimes.emplace_back(&slimeAnim, baseX + offsetX, GROUND_Y);
        }
      }

      // Update entities
      for (auto &s : slimes)
        s.update(dt, gameSpeed);

      // Cleanup
      slimes.erase(std::remove_if(slimes.begin(), slimes.end(),
                                  [](const Slime &s) { return !s.active; }),
                   slimes.end());

      player.update(dt);

      // Collision with slimes
      sf::FloatRect pb = player.getBounds();
      for (const auto &s : slimes) {
        if (pb.intersects(s.bounds)) {
          gameOver = true;
        }
      }
      // Falling Game Over
      if (player.sprite.getPosition().y > WINDOW_HEIGHT + 100.0f) {
        gameOver = true;
      }
      if (hasFont) {
        scoreText.setString("Score: " + to_string((int)score));
      }
    }

    window.clear();
    for (auto &layer : backgroundLayers)
      layer.draw(window);
    window.draw(groundSprite1);
    window.draw(groundSprite2);
    for (auto &s : slimes)
      s.draw(window);
    player.draw(window);

    if (hasFont && gameOver) {
      sf::Text goText;
      goText.setFont(font);
      goText.setCharacterSize(50);
      goText.setFillColor(sf::Color::Red);
      goText.setString("Game Over! Press SPACE to Restart");
      sf::FloatRect bounds = goText.getLocalBounds();
      goText.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
      goText.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
      window.draw(goText);
    }

    if (hasFont) {
      window.draw(scoreText);
    }

    window.display();
  }

  return 0;
}
