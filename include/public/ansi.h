// https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
#pragma once

#include "input.h"
#include <cstdint>
#include <functional>
#include <iostream>
#include <istream>
#include <linux/input-event-codes.h>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>

namespace Ansi {

const static std::string_view ANSI_ESCAPE{"\033"};
const static std::string_view ANSI_UP{"\033[A"};
const static std::string_view ANSI_DOWN{"\033[B"};
const static std::string_view ANSI_RIGHT{"\033[C"};
const static std::string_view ANSI_LEFT{"\033[D"};

static void MoveDown(int l = 1) noexcept;
static void MoveUp(int l = 1) noexcept;
static void MoveRight(int l = 1) noexcept;
static void MoveLeft(int l = 1) noexcept;

static void SaveCursorPos() noexcept;
static void BackToSaved() noexcept;
static void Cleanup() noexcept;

static void MoveUp(int l) noexcept {
  std::cout << ANSI_ESCAPE << "[" << l << "A";
}
static void MoveDown(int l) noexcept {
  std::cout << ANSI_ESCAPE << "[" << l << "B";
}
static void MoveRight(int l) noexcept {
  std::cout << ANSI_ESCAPE << "[" << l << "C";
}
static void MoveLeft(int l) noexcept {
  std::cout << ANSI_ESCAPE << "[" << l << "D";
}

static void GetCursorPosition(int &x, int &y) {
  std::cout << ANSI_ESCAPE << "[6n";
  std::string resp;
  std::cin >> resp;
  // ESC[#;#R
  x = std::stoi(resp.substr(resp.find("["), (resp.find(";") - resp.find("["))));
  y = std::stoi(resp.substr(resp.find(";"), (resp.find("R") - resp.find(";"))));
}

// Saves the cursor position, overwrites if already saved
static void SaveCursorPos() noexcept {
  std::cout << ANSI_ESCAPE << "[s";
  // save the cursor position into the variable
  // GetCursorPosition(&CursorPosition[0], &CursorPosition[1]);
}
// Moves the cursor back to saved position
static void BackToSaved() noexcept { std::cout << ANSI_ESCAPE << "[u"; }
//
static void Cleanup() noexcept {
  BackToSaved();
  std::cout << ANSI_ESCAPE << "[0J";
  std::cout.flush();
}
}; // namespace Ansi

class AnsiPositioning final {
public:
  void UpdatePosition() {}

private:
  std::tuple<int, int> CursorPosition{};
};
