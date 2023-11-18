// https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
#pragma once

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
#include <string_view>

namespace Ansi {

const static std::string_view ANSI_ESCAPE{"\033"};
const static std::string_view ANSI_UP{"\033[A"};
const static std::string_view ANSI_DOWN{"\033[B"};
const static std::string_view ANSI_RIGHT{"\033[C"};
const static std::string_view ANSI_LEFT{"\033[D"};
// Moves the cursor down some length
static void MoveDown(int l = 1) noexcept;
// Moves the cursor up some length
static void MoveUp(int l = 1) noexcept;
// Moves the cursor right some length
static void MoveRight(int l = 1) noexcept;
// Moves the cursor left some length
static void MoveLeft(int l = 1) noexcept;

// Saves the cursor using ANSI save code using terminal functionality
static void SaveCursorPos() noexcept;
// Returns the cursor to the last saved position (saved with SaveCursorPos())
static void BackToSaved() noexcept;
// Cleans up the terminal output of the program
static void Cleanup() noexcept;

static void MoveUp(int l) noexcept {
  if(l > 0)
    std::cout << ANSI_ESCAPE << "[" << l << "A";
}
static void MoveDown(int l) noexcept {
  if (l > 0)
    std::cout << ANSI_ESCAPE << "[" << l << "B";
}
static void MoveRight(int l) noexcept {
  if(l >0 )
    std::cout << ANSI_ESCAPE << "[" << l << "C";
}
static void MoveLeft(int l) noexcept {
  if(l > 0)
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
