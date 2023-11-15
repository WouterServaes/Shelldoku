#include "include/public/listener.h"
#include "include/public/eventQueue.h"
#include "include/public/events.h"
#include "../common/include/public/logger.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <mutex>

void Listener::Listen(EventQueue *pEventQueue, const EventID eventId) {
  pEventQueue->RegisterListener(this, eventId);
}

void Listener::Notify(EventID eventId) {
  //Log::Debug("Listener received event");
}
