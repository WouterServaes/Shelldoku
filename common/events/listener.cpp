#include "listener.h"
#include "eventQueue.h"
#include <memory>

void Listener::Listen(std::shared_ptr<EventQueue> pEventQueue,
                      const EventID eventId) {
  pEventQueue->RegisterListener(std::shared_ptr<Listener>(this), eventId);
}

void Listener::Notify(EventID eventId) {
  // Log::Debug("Listener received event");
}
