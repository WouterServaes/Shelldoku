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

  // Initializes the input listener
  bool Init();
  // End the input listener, also called by destructor
  bool End();
  // Adds a key string to a key code
  void AddKey(KeyString keyString, Key key);
  // Returns the optional key code for a key string 
  std::optional<KeyCode> GetKeyCode(KeyString keyString) const noexcept;
  // blocking, listens for a key press
  [[nodiscard]] std::optional<KeyCode>
  GetKeyPressed(std::optional<char> &charValue, std::optional<int> &intValue);

  // creates thread, listens to key inputs and dispatches events
  void StartInputHandler();
  // stops thread
  void StopInputHandler() noexcept;

private:
  // blocking, helper that does the key input listening
  [[nodiscard]] std::optional<KeyString> GetKeyPressed();
  // Runs a key input listening loop
  void HandleInput();

  termios oldTerm;
  termios term;
  bool running{false};

  KeyMapping keyMapping;

  std::atomic_bool handlingInputs{false};
  std::thread handlerThread{};
};

}; // namespace InputHandling
