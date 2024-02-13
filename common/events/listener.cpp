#include "listener.h"
#include "eventQueue.h"
#include <memory>

void Listener::Listen(std::shared_ptr<EventQueue> pEventQueue,
                      const EventID eventId) {
  pEventQueue->RegisterListener(std::ref(*this), eventId);
}

void Listener::Notify(EventID eventId) {
  // Log::Debug("Listener received event");
}
