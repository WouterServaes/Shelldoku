#pragma once
#include "events.h"
#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <string_view>
#include <vector>

class Event;
class Listener;

class EventQueue {
public:
  EventQueue() = default;
  virtual ~EventQueue() = default;
  EventQueue(const EventQueue &) = delete;
  EventQueue(EventQueue &&) = delete;
  EventQueue &operator=(const EventQueue &) = delete;
  EventQueue &operator=(EventQueue &&) = delete;

  void PushEvent(std::shared_ptr<Event> pEvent);
  void HandleQueue(bool stallThread = false);

  bool HasListeners();

  void RegisterListener(Listener *pListener, const EventID eventId);

private:
  void PopEvent();
  void NotifyListeners(std::shared_ptr<Event> pEvent);
  bool HasEvents();
  std::queue<std::shared_ptr<Event>> pEvents;
  std::mutex queueMutex;
  std::mutex waitForEventsMutex;
  std::mutex listenerMutex;
  std::map<EventID, std::vector<Listener *>> pListeners;
  std::condition_variable queueWaitCond;
  std::condition_variable listenerWaitCond;
};