#pragma once

#include <bits/utility.h>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string_view>
#include <tuple>
#include <utility>
#include "eventID.h"

// Simple event with an ID, listeners implement the functionality
class Event {
public:
  Event() = delete;
  Event(const EventID id);
  virtual ~Event() = default;
  // Executes event if +this function overriden
  virtual void DoEvent();
  // Returns the event id
  const EventID Id() const;

private:
  const EventID id;
};

// Calls a function with predefined arguments when fired. Listerens are notified
// with the ID
template <class... FUNCTION_ARG_TYPES>
class FunctionEvent final : public Event {
public:
  FunctionEvent() = delete;
  FunctionEvent(const EventID id,
                std::function<void(FUNCTION_ARG_TYPES...)> function,
                std::tuple<FUNCTION_ARG_TYPES...> functionArgs)
      : Event(id), function(function), functionArgs(functionArgs) {}

  ~FunctionEvent() = default;
  // Executes the event function with arguments
  void DoEvent() noexcept override { std::apply(function, functionArgs); }

private:
  const std::tuple<FUNCTION_ARG_TYPES...> functionArgs;
  std::function<void(FUNCTION_ARG_TYPES...)> function;
};
