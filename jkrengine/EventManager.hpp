#pragma once
#include "Window_base.hpp"

namespace Jkr {
class Window;
struct BoundRect2D {
  glm::vec2 mXy;
  glm::vec2 mWh;
  constexpr bool IsPointWithin(glm::vec2 inXy,
                               glm::vec2 inScale = glm::vec2(1, 1)) {
    return ((mXy.x < inXy.x * inScale.x) &&
            (inXy.x * inScale.x < mXy.x + mWh.x)) &&
           ((mXy.y < inXy.y * inScale.y) &&
            (inXy.y * inScale.y < mXy.y + mWh.y));
  }
};
class EventManager {
public:
  EventManager() { mBoundRect2Ds.reserve(100); }
  void ProcessEventsEXT(Window &inWindow);
  void ProcessEvents();
  bool ShouldQuit() const;
  SDL_Event GetEventHandle() const;
  bool IsKeyPressedContinous(int inScanCode) const;
  bool IsKeyReleased(SDL_Keycode inKey);
  bool IsKeyPressed(SDL_Keycode inKey);

  /// Mouse Inputs
  glm::vec2 GetMousePos() const;
  glm::vec2 GetRelativeMousePos() const;
  int GetMouseButtonValue() const;
  bool IsLeftButtonPressedContinous() const;
  bool IsRightButtonPressedContinous() const;
  bool IsLeftButtonPressed() const;
  bool IsRightButtonPressed() const;
  bool IsCloseWindowEvent() const;

  /// Touch Events
  bool IsFingerDownEvent() const;
  bool IsFingerUpEvent() const;
  bool IsFingerMotionEvent() const;
  bool IsMultiGestureEvent() const;
  glm::vec3 GetTouchLocation() const;         // x, y, pressure
  glm::vec3 GetTouchRelativeLocation() const; // x, y, pressure
  glm::vec4 GetMultiGesture() const;          // x, y, d_dist, d_theta
  uint32_t GetTouchId() const;

  /// Text Input
  void StartTextInput() { SDL_StartTextInput(); }
  void StopTextInput() { SDL_StopTextInput(); }

  /// Bounded Rectangle
  bool IsMouseWithinAtTopOfStack(uint32_t inId, uint32_t inDepthValue);
  bool IsMouseWithin(uint32_t inId, uint32_t inDepthValue);
  uint32_t SetBoundedRect(glm::vec2 inXy, glm::vec2 inWh,
                          uint32_t inDepthValue);
  void UpdateBoundedRect(uint32_t inId, glm::vec2 inXy, glm::vec2 inWh,
                         uint32_t inDepthValue);
  void RemoveBoundedRect(uint32_t inId, uint32_t inDepthValue);
  void SetEventCallBack(const std::function<void()> &inEventCallBack);
  void RemoveEventCallBacks();

private:
  SDL_Event mEvent;
  glm::ivec2 mMousePos;
  glm::ivec2 mRelativePos;
  int mCurrentPushedMouseButton;
  bool should_quit = false;
  bool mCloseWindowEvent = false;
  const uint8_t *mKeyboardState;

private:
  std::vector<std::function<void()>> mEventCallBack;
  std::unordered_map<uint32_t, std::vector<BoundRect2D>> mBoundRect2Ds;
  std::vector<glm::vec4> mTouches; // x, y, pressure, id
  glm::vec2 mFrameSize;
  glm::vec2 mWindowSize;
  glm::vec2 mOffscreenByWindowSize;
};
} // namespace Jkr
