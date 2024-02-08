#include "listener.h"
#include "eventQueue.h"
#include "events.h"
#include "logger.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <mutex>

void Listener::Listen(EventQueue *pEventQueue, const EventID eventId) {
  pEventQueue->RegisterListener(this, eventId);
}

void Listener::Notify(EventID eventId) {
  // Log::Debug("Listener received event");
}
