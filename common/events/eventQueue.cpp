#include "eventQueue.h"
#include "events.h"
#include "listener.h"
#include <algorithm>
#include <memory>
#include <mutex>
#include <stdexcept>
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

void EventQueue::RegisterListener(Listener *pListener, const EventID eventId) {
  std::unique_lock<std::mutex> lock{listenerMutex};
  if (pListeners.find(eventId) == pListeners.end()) {
    pListeners[eventId] = std::vector<Listener *>({pListener});
  } else {
    pListeners[eventId].push_back(pListener);
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
  if (pListeners.find(id) != pListeners.end()) {
    // Notify all listeners of this event ID with the event
    std::for_each(pListeners[id].begin(), pListeners[id].end(),
                  [id](Listener *pListener) { pListener->Notify(id); });
  }
}

bool EventQueue::HasListeners() {
  std::unique_lock<std::mutex> lock{listenerMutex};
  return !pListeners.empty();
}

bool EventQueue::HasEvents() {
  std::unique_lock<std::mutex> lock(queueMutex);
  return !pEvents.empty();
}
