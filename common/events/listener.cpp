#include "include/private/listener.h"
#include "../include/public/logger.h"
#include "include/private/eventQueue.h"
#include "include/private/events.h"
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
