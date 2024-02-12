#include "dispatcher.h"

#include <cctype>
#include <cstring>
#include <iostream>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string_view>
#include <sys/ioctl.h>
#include <thread>
#include <unistd.h>

using namespace InputHandling;

std::string GetLastError() { return std::string(std::strerror(errno)); }

Input::Input(std::shared_ptr<EventQueue> pEventQueue,
             const KeyMapping &keymapping)
    : Dispatcher(pEventQueue), term(), keyMapping(keymapping) {}

Input::~Input() {
  if (running) {

    End();
  }
}

bool Input::Init() {
  if (running) {
    throw std::runtime_error(
        "input:Init: input already initialized and running");
    return false;
  }

  // get current termios
  if (0 > ioctl(STDIN_FILENO, TCGETS, &term)) {
    throw std::runtime_error("Iput:Init: ioctl tcgets failed: " +
                             GetLastError());
    return false;
  }
  oldTerm = term;

  running = true;

  term.c_lflag &= ~ECHO;
  //   canonical input
  term.c_lflag &= ~ICANON;
  term.c_cc[VMIN] = 0;
  term.c_cc[VTIME] = 1;

  if (0 > ioctl(STDIN_FILENO, TCSETS, &term)) {
    throw std::runtime_error("input:Init: ioctl tcsets failed: " +
                             GetLastError());
    return false;
  }

  return true;
}

bool Input::End() {
  StopInputHandler();
  if (!running) {
    throw std::runtime_error("input:End: input not running");
    return false;
  }

  if (0 > ioctl(STDIN_FILENO, TCSETS, &oldTerm)) {
    throw std::runtime_error("input:End: ioctl failed to set termios back: " +
                             GetLastError());
    return false;
  }

  return running = false;
}

void Input::AddKey(KeyString keyString, Key key) {
  keyMapping.insert({keyString, key});
}

std::optional<KeyCode> Input::GetKeyCode(KeyString keyString) const noexcept {
  const auto f{keyMapping.find(keyString)};
  if (f != keyMapping.end()) {
    return std::get<0>(f->second);
  }
  return {};
}

std::optional<KeyCode> Input::GetKeyPressed(std::optional<char> &charValue,
                                            std::optional<int> &intValue) {

  const auto str{GetKeyPressed()};
  if (str.has_value()) {
    // figure out if pressed key was an int or not
    if (static_cast<int>(str.value()[0]) <= 9) {
      intValue = static_cast<int>(str.value()[0]);
    } else {
      charValue = str.value()[0];
    }

    // keycode is first item of tuple in map value at key str,
    // str is always valid here
    return std::get<0>(keyMapping.find(str.value())->second);
  }

  return {};
}

void Input::StartInputHandler() {
  if (!handlingInputs) {
    handlingInputs = true;
    handlerThread = std::thread(&Input::HandleInput, this);
  }
}

void Input::StopInputHandler() noexcept {
  handlingInputs = false;
  handlerThread.join();
}

std::optional<KeyString> Input::GetKeyPressed() {
  char buffer[5];
  std::cout.flush();
  const auto r = read(STDIN_FILENO, buffer, 5);
  if (0 > r) {
    throw std::runtime_error("input:GetKeyPressed: read error: " +
                             GetLastError());
  };

  if (0 < r) {
    // multi character keys with fixed oct values(arrow up, esc, ...)
    auto keyCode{keyMapping.find(buffer)};
    if (keyCode != keyMapping.end()) {
      return keyCode->first;
    } else {
      // multi character keys with different oct values (a is a\377\177 among
      // other things depending on the character that came before it), in this
      // case, just match the first character in the buffer
      buffer[0] = std::toupper(buffer[0]);
      buffer[1] = 0;
      buffer[4] = 0;
      buffer[3] = 0;
      buffer[4] = 0;

      keyCode = keyMapping.find(buffer);
      if (keyCode != keyMapping.end()) {
        return keyCode->first;
      }
    }
  }

  return {};
}

void Input::HandleInput() {
  while (handlingInputs) {
    auto k = GetKeyPressed();
    if (k.has_value()) {
      auto event{std::get<1>(keyMapping.find(k.value())->second)};
      if (event.has_value()) {
        DispatchEvent(*event);
      }
    }
  }
}
