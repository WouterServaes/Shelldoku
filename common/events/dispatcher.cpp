#include "include/private/dispatcher.h"
#include "include/private/eventQueue.h"

Dispatcher::Dispatcher(EventQueue *pEventQueue) : pEventQueue(pEventQueue) {}

void Dispatcher::DispatchEvent(std::shared_ptr<Event> pEvent) {
  pEventQueue->PushEvent(pEvent);
}

void Dispatcher::DispatchEvent(EventID id) { pEventQueue->PushEvent(id); }