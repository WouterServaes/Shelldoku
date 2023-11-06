#pragma once

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

  void DispatchEvent(std::shared_ptr<Event> pEvent);

private:
  EventQueue *const pEventQueue;
};