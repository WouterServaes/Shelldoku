// https://github.com/mouuff/simple-getch/tree/master
#pragma once

#include "../../events/include/public/dispatcher.h"
#include <any>
#include <atomic>
#include <cstdint>
#include <functional>
#include <iterator>
#include <linux/input-event-codes.h>
#include <map>
#include <memory>
#include <optional>
#include <string_view>
#include <termios.h>
#include <thread>
#include <utility>

class Event;
class EventQueue;

namespace InputHandling {

using KeyCode = uint16_t;

using Key = std::tuple<KeyCode, std::optional<std::shared_ptr<Event>>>;
using KeyString = std::string_view;
using KeyMapping = std::map<KeyString, Key>;

static const KeyMapping KEYS{};

class Input final : public Dispatcher {
public:
  Input(EventQueue *pEventQueue, const KeyMapping &keymapping = KEYS);
  ~Input();

  bool Init();
  bool End();

  void AddKey(KeyString keyString, Key key);
  std::optional<KeyCode> GetKeyCode(KeyString keyString) const noexcept;
  // blocking
  [[nodiscard]] std::optional<KeyCode>
  GetKeyPressed(std::optional<char> &charValue, std::optional<int> &intValue);

  // creates thread
  void StartInputHandler();
  // stops thread
  void StopInputHandler() noexcept;

private:
  // blocking
  [[nodiscard]] std::optional<KeyString> GetKeyPressed();
  // blocking while loop
  void HandleInput();

  termios oldTerm;
  termios term;
  bool running{false};

  KeyMapping keyMapping;

  std::atomic_bool handlingInputs{false};
  std::thread handlerThread{};
};

}; // namespace InputHandling
