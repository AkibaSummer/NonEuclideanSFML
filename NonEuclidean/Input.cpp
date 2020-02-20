#include "Input.h"
#include <string.h>
#include <memory>
#include "GameHeader.h"

Input::Input() { memset(this, 0, sizeof(Input)); }

void Input::EndFrame() {
  memset(key_press, 0, sizeof(key_press));
  memset(mouse_button_press, 0, sizeof(mouse_button_press));
}

void Input::UpdateRaw(const sf::Event event) {
  static int mouseMovedx = -1, mouseMovedy = -1;
  if (event.type == sf::Event::MouseMoved) {
    if (mouseMovedx != -1) {
      mouse_dx = event.mouseMove.x - mouseMovedx;
      mouse_dy = event.mouseMove.y - mouseMovedy;
    }
    mouseMovedx = event.mouseMove.x;
    mouseMovedy = event.mouseMove.y;
  }

  if (event.type == sf::Event::MouseButtonPressed) {
    if (event.mouseButton.button == sf::Mouse::Button::Left) {
      mouse_button[0] = true;
      mouse_button_press[0] = true;
    }
    if (event.mouseButton.button == sf::Mouse::Button::Middle) {
      mouse_button[1] = true;
      mouse_button_press[1] = true;
    }
    if (event.mouseButton.button == sf::Mouse::Button::Right) {
      mouse_button[2] = true;
      mouse_button_press[2] = true;
    }
  }
  if (event.type == sf::Event::MouseButtonReleased) {
    if (event.mouseButton.button == sf::Mouse::Button::Left) {
      mouse_button[0] = false;
    }
    if (event.mouseButton.button == sf::Mouse::Button::Middle) {
      mouse_button[1] = false;
    }
    if (event.mouseButton.button == sf::Mouse::Button::Right) {
      mouse_button[2] = false;
    }
  }
}
