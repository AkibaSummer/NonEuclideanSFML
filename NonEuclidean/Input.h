#pragma once
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
struct tagRAWINPUT;
class Input {
public:
  Input();

  void EndFrame();
  void UpdateRaw(const sf::Event);

  //Keyboard
  bool key[sf::Keyboard::KeyCount];
  bool key_press[sf::Keyboard::KeyCount];

  //Mouse
  bool mouse_button[3];
  bool mouse_button_press[3];
  int mouse_dx;
  int mouse_dy;

  //Joystick
  //TODO:

  //Bindings
  //TODO:

  //Calibration
  //TODO:
};
