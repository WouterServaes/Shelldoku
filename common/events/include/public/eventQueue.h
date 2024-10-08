#pragma once
#include "eventID.h"
#include <condition_variable>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
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

  // Pushes an event on the queue
  void PushEvent(std::shared_ptr<Event> pEvent);
  // Pushes an event on the queue
  void PushEvent(EventID id);
  // Pops events from the queue
  // if stallThread is true, this function halts the thread untill an event is
  // on the queue
  void HandleQueue(bool stallThread = false);
  // Returns true if queue has any listeners
  bool HasListeners();
  // Registers a listener to this queue
  void RegisterListener(std::reference_wrapper<Listener> listener,
                        const EventID eventId);

private:
  // Pops event off queue, executes events and notifies listeners
  void PopEvent();
  // Notifies all listeners listening to given event
  void NotifyListeners(std::shared_ptr<Event> pEvent);
  // Returns true if the queue contains events
  bool HasEvents();
  std::queue<std::shared_ptr<Event>> pEvents;
  std::mutex queueMutex;
  std::mutex waitForEventsMutex;
  std::mutex listenerMutex;
  std::map<EventID, std::vector<std::reference_wrapper<Listener>>> listeners;
  std::condition_variable queueWaitCond;
  std::condition_variable listenerWaitCond;
};