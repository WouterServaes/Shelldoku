
#pragma once

#include "events.h"
#include <memory>
#include <mutex>
#include <string_view>
#include <vector>

class EventQueue;
class Event;

class Listener {
public:
  Listener() = default;
  ~Listener() = default;
  Listener(const Listener &) = delete;
  Listener(Listener &&) = delete;
  Listener &operator=(const Listener &) = delete;
  Listener &operator=(Listener &&) = delete;

  // Registers listener to an event queue with the event id it is listening for
  // A listener can listen to multiple queues with different id
  void Listen(EventQueue *pEventQueue, const EventID eventId);

  // Listener receives event id
  virtual void Notify(EventID eventId);

private:
  EventQueue *pEventQueue;
};