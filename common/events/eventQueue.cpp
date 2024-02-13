#include "eventQueue.h"
#include "events.h"
#include "listener.h"
#include <algorithm>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

void EventQueue::PushEvent(std::shared_ptr<Event> pEvent) {
  if (!pEvents.empty()) {
    return;
  }
  std::unique_lock<std::mutex> lock(queueMutex);
  pEvents.push(pEvent);
  queueWaitCond.notify_one();
  std::unique_lock<std::mutex> lock2{waitForEventsMutex};
}

void EventQueue::PushEvent(EventID id) {
  PushEvent(std::shared_ptr<Event>(new Event(id)));
}

void EventQueue::HandleQueue(bool stallThread) {
  if (stallThread) {
    std::unique_lock<std::mutex> lock{waitForEventsMutex};
    queueWaitCond.wait(lock);
  }

  PopEvent();
}

void EventQueue::RegisterListener(std::reference_wrapper<Listener> listener,
                                  const EventID eventId) {
  std::unique_lock<std::mutex> lock{listenerMutex};
  if (listeners.find(eventId) == listeners.end()) {
    listeners[eventId] =
        std::vector<std::reference_wrapper<Listener>>({listener});
  } else {
    listeners[eventId].push_back(listener);
  }
}

void EventQueue::PopEvent() {
  std::unique_lock<std::mutex> lock(queueMutex);
  if (!pEvents.empty()) {
    pEvents.front()->DoEvent();
    NotifyListeners(pEvents.front());
    pEvents.pop();
  }
}

void EventQueue::NotifyListeners(std::shared_ptr<Event> pEvent) {
  std::unique_lock<std::mutex> lock{listenerMutex};
  const auto id = pEvent->Id();
  if (listeners.find(id) != listeners.end()) {
    // Notify all listeners of this event ID with the event
    std::for_each(listeners[id].begin(), listeners[id].end(),
                  [id](std::reference_wrapper<Listener> listener) {
                    listener.get().Notify(id);
                  });
  }
}

bool EventQueue::HasListeners() {
  std::unique_lock<std::mutex> lock{listenerMutex};
  return !listeners.empty();
}

bool EventQueue::HasEvents() {
  std::unique_lock<std::mutex> lock(queueMutex);
  return !pEvents.empty();
}
