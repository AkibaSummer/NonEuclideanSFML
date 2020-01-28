#pragma once

struct tagRAWINPUT;
class Input {
public:
  Input();

  void EndFrame();
  void UpdateRaw(const sf::Event);

  //Keyboard
  bool key[256];
  bool key_press[256];

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
