#include "include/public/listener.h"
#include "include/public/eventQueue.h"
#include "include/public/events.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <mutex>

void Listener::Listen(EventQueue *pEventQueue, const EventID eventId) {
  pEventQueue->RegisterListener(this, eventId);
}

void Listener::Notify(EventID eventId) {
  std::cout << "Listener received event: " << eventId;
}
