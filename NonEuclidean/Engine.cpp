#include "Engine.h"
#include <GL/glew.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "Level4.h"
#include "Level5.h"
#include "Level6.h"
#include "Physical.h"

Engine* GH_ENGINE = nullptr;
Player* GH_PLAYER = nullptr;
Input* GH_INPUT = nullptr;
int GH_REC_LEVEL = 0;
int64_t GH_FRAME = 0;

Engine::Engine() {
  GH_ENGINE = this;
  GH_INPUT = &input;
  isFullscreen = false;

  CreateGLWindow();
  InitGLObjects();

  player.reset(new Player);
  GH_PLAYER = player.get();

  vScenes.push_back(std::shared_ptr<Scene>(new Level1));
  vScenes.push_back(std::shared_ptr<Scene>(new Level2(3)));
  vScenes.push_back(std::shared_ptr<Scene>(new Level2(6)));
  vScenes.push_back(std::shared_ptr<Scene>(new Level3));
  vScenes.push_back(std::shared_ptr<Scene>(new Level4));
  vScenes.push_back(std::shared_ptr<Scene>(new Level5));
  vScenes.push_back(std::shared_ptr<Scene>(new Level6));

  LoadScene(0);

  sky.reset(new Sky);
}

Engine::~Engine() {
  // ClipCursor(NULL);
  // wglMakeCurrent(NULL, NULL);
  // ReleaseDC(hWnd, hDC);
  // wglDeleteContext(hRC);
  // DestroyWindow(hWnd);
}

int Engine::Run() {
  // Setup the timer
  const int64_t ticks_per_step = timer.SecondsToTicks(GH_DT);
  int64_t cur_ticks = timer.GetTicks();
  GH_FRAME = 0;
  window.setActive(true);
  // Game loop
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
        break;
      } else if (event.type == sf::Event::MouseButtonPressed ||
                 event.type == sf::Event::MouseButtonReleased ||
                 event.type == sf::Event::MouseMoved) {
        input.UpdateRaw(event);
      } else if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
          window.close();
          break;
        }
        input.key[event.key.code] = true;
        input.key_press[event.key.code] = true;
      } else if (event.type == sf::Event::KeyReleased) {
        input.key[event.key.code] == false;
      }
    }

    // Confine the cursor
    ConfineCursor();

    if (input.key_press[sf::Keyboard::Num1]) {
      LoadScene(0);
    } else if (input.key_press[sf::Keyboard::Num2]) {
      LoadScene(1);
    } else if (input.key_press[sf::Keyboard::Num3]) {
      LoadScene(2);
    } else if (input.key_press[sf::Keyboard::Num4]) {
      LoadScene(3);
    } else if (input.key_press[sf::Keyboard::Num5]) {
      LoadScene(4);
    } else if (input.key_press[sf::Keyboard::Num6]) {
      LoadScene(5);
    } else if (input.key_press[sf::Keyboard::Num7]) {
      LoadScene(6);
    }

    // Used fixed time steps for updates
    const int64_t new_ticks = timer.GetTicks();
    for (int i = 0; cur_ticks < new_ticks && i < GH_MAX_STEPS; ++i) {
      Update();
      cur_ticks += ticks_per_step;
      GH_FRAME += 1;
      input.EndFrame();
    }
    cur_ticks = (cur_ticks < new_ticks ? new_ticks : cur_ticks);

    // Setup camera for rendering
    const float n =
        GH_CLAMP(NearestPortalDist() * 0.5f, GH_NEAR_MIN, GH_NEAR_MAX);
    main_cam.worldView = player->WorldToCam();
    main_cam.SetSize(iWidth, iHeight, n, GH_FAR);
    main_cam.UseViewport();

    // Render scene
    GH_REC_LEVEL = GH_MAX_RECURSION;
    Render(main_cam, 0, nullptr);
    window.display();
  }

  DestroyGLObjects();
  return 0;
}

void Engine::LoadScene(int ix) {
  // Clear out old scene
  if (curScene) {
    curScene->Unload();
  }
  vObjects.clear();
  vPortals.clear();
  player->Reset();

  // Create new scene
  curScene = vScenes[ix];
  curScene->Load(vObjects, vPortals, *player);
  vObjects.push_back(player);
}

void Engine::Update() {
  // Update
  for (size_t i = 0; i < vObjects.size(); ++i) {
    assert(vObjects[i].get());
    vObjects[i]->Update();
  }

  // Collisions
  // For each physics object
  for (size_t i = 0; i < vObjects.size(); ++i) {
    Physical* physical = vObjects[i]->AsPhysical();
    if (!physical) {
      continue;
    }
    Matrix4 worldToLocal = physical->WorldToLocal();

    // For each object to collide with
    for (size_t j = 0; j < vObjects.size(); ++j) {
      if (i == j) {
        continue;
      }
      Object& obj = *vObjects[j];
      if (!obj.mesh) {
        continue;
      }

      // For each hit sphere
      for (size_t s = 0; s < physical->hitSpheres.size(); ++s) {
        // Brings point from collider's local coordinates to hits's local
        // coordinates.
        const Sphere& sphere = physical->hitSpheres[s];
        Matrix4 worldToUnit = sphere.LocalToUnit() * worldToLocal;
        Matrix4 localToUnit = worldToUnit * obj.LocalToWorld();
        Matrix4 unitToWorld = worldToUnit.Inverse();

        // For each collider
        for (size_t c = 0; c < obj.mesh->colliders.size(); ++c) {
          Vector3 push;
          const Collider& collider = obj.mesh->colliders[c];
          if (collider.Collide(localToUnit, push)) {
            // If push is too small, just ignore
            push = unitToWorld.MulDirection(push);
            vObjects[j]->OnHit(*physical, push);
            physical->OnCollide(*vObjects[j], push);

            worldToLocal = physical->WorldToLocal();
            worldToUnit = sphere.LocalToUnit() * worldToLocal;
            localToUnit = worldToUnit * obj.LocalToWorld();
            unitToWorld = worldToUnit.Inverse();
          }
        }
      }
    }
  }

  // Portals
  for (size_t i = 0; i < vObjects.size(); ++i) {
    Physical* physical = vObjects[i]->AsPhysical();
    if (physical) {
      for (size_t j = 0; j < vPortals.size(); ++j) {
        if (physical->TryPortal(*vPortals[j])) {
          break;
        }
      }
    }
  }
}

void Engine::Render(const Camera& cam, GLuint curFBO,
                    const Portal* skipPortal) {
  // Clear buffers
  if (GH_USE_SKY) {
    glClear(GL_DEPTH_BUFFER_BIT);
    sky->Draw(cam);
  } else {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  // Create queries (if applicable)
  GLuint queries[GH_MAX_PORTALS];
  GLuint drawTest[GH_MAX_PORTALS];
  assert(vPortals.size() <= GH_MAX_PORTALS);
  if (occlusionCullingSupported) {
    GLEW_GET_FUN(__glewGenQueriesARB)((GLsizei)vPortals.size(), queries);
  }

  // Draw scene
  for (size_t i = 0; i < vObjects.size(); ++i) {
    vObjects[i]->Draw(cam, curFBO);
  }

  // Draw portals if possible
  if (GH_REC_LEVEL > 0) {
    // Draw portals
    GH_REC_LEVEL -= 1;
    if (occlusionCullingSupported && GH_REC_LEVEL > 0) {
      glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
      glDepthMask(GL_FALSE);
      for (size_t i = 0; i < vPortals.size(); ++i) {
        if (vPortals[i].get() != skipPortal) {
          __glewBeginQueryARB(GL_SAMPLES_PASSED_ARB, queries[i]);
          vPortals[i]->DrawPink(cam);
          __glewEndQueryARB(GL_SAMPLES_PASSED_ARB);
        }
      }
      for (size_t i = 0; i < vPortals.size(); ++i) {
        if (vPortals[i].get() != skipPortal) {
          __glewGetQueryObjectuivARB(queries[i], GL_QUERY_RESULT_ARB,
                                     &drawTest[i]);
        }
      };
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
      glDepthMask(GL_TRUE);
      __glewDeleteQueriesARB((GLsizei)vPortals.size(), queries);
    }
    for (size_t i = 0; i < vPortals.size(); ++i) {
      if (vPortals[i].get() != skipPortal) {
        if (occlusionCullingSupported && (GH_REC_LEVEL > 0) &&
            (drawTest[i] == 0)) {
          continue;
        } else {
          vPortals[i]->Draw(cam, curFBO);
        }
      }
    }
    GH_REC_LEVEL += 1;
  }

#if 0
  //Debug draw colliders
  for (size_t i = 0; i < vObjects.size(); ++i) {
    vObjects[i]->DebugDraw(cam);
  }
#endif
}

void Engine::CreateGLWindow() {
  // GL settings
  sf::ContextSettings settings;
  settings.majorVersion = 3;
  settings.minorVersion = 0;
  settings.depthBits = 32;
  settings.sRgbCapable = true;
  settings.antialiasingLevel = 8;

  // Always start in windowed mode
  iWidth = GH_SCREEN_WIDTH;
  iHeight = GH_SCREEN_HEIGHT;

  // Create the window
  sf::VideoMode screen_size;
  sf::Uint32 window_style;
  const sf::Vector2i screen_center(iWidth / 2, iHeight / 2);
  if (isFullscreen) {
    screen_size = sf::VideoMode::getDesktopMode();
    window_style = sf::Style::Fullscreen;
  } else {
    screen_size = sf::VideoMode(iWidth, iHeight, 24);
    window_style = sf::Style::None;
  }

  window.create(screen_size, "NonEuclideanSFML", window_style, settings);
  window.setVerticalSyncEnabled(true);
  window.setKeyRepeatEnabled(false);
  window.requestFocus();

  if (GH_HIDE_MOUSE) {
    window.setMouseCursorVisible(false);
  }
}

void Engine::InitGLObjects() {
  // Initialize extensions
  glewInit();

  // Basic global variables
  glClearColor(0.6f, 0.9f, 1.0f, 1.0f);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);

  // Check GL functionality
  GLEW_GET_FUN(__glewGetQueryiv)
  (GL_SAMPLES_PASSED_ARB, GL_QUERY_COUNTER_BITS_ARB,
   &occlusionCullingSupported);
  // glGetQueryiv(GL_SAMPLES_PASSED_ARB, GL_QUERY_COUNTER_BITS_ARB,
  //              &occlusionCullingSupported);

  // Attempt to enalbe vsync(if failure then oh well)
  // wglSwapIntervalEXT(1);
}

void Engine::DestroyGLObjects() {
  curScene->Unload();
  vObjects.clear();
  vPortals.clear();
}

void Engine::ConfineCursor() {
  if (GH_HIDE_MOUSE) {
    auto position = window.getPosition();
    auto size = (sf::Vector2i)window.getSize();
    // sf::Mouse::setPosition(position + (size / 2));
  }
}

float Engine::NearestPortalDist() const {
  float dist = FLT_MAX;
  for (size_t i = 0; i < vPortals.size(); ++i) {
    dist = GH_MIN(dist, vPortals[i]->DistTo(player->pos));
  }
  return dist;
}
