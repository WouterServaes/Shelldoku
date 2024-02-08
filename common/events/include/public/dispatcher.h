#pragma once

#include "eventID.h"
#include <memory>
class EventQueue;
class Event;

class Dispatcher {
public:
  Dispatcher(EventQueue *pEventQueue);
  ~Dispatcher() = default;
  Dispatcher() = delete;
  Dispatcher(const Dispatcher &) = delete;
  Dispatcher(Dispatcher &&) = delete;
  Dispatcher &operator=(const Dispatcher &) = delete;
  Dispatcher &operator=(Dispatcher &&) = delete;
  // Dispatches event on the event queue
  void DispatchEvent(std::shared_ptr<Event> pEvent);
  // Dispatches event on the event queue
  void DispatchEvent(EventID id);

private:
  EventQueue *const pEventQueue;
};