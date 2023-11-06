#include "include/public/dispatcher.h"
#include "include/public/eventQueue.h"

Dispatcher::Dispatcher(EventQueue *pEventQueue) : pEventQueue(pEventQueue) {}

void Dispatcher::DispatchEvent(std::shared_ptr<Event> pEvent) {
  pEventQueue->PushEvent(pEvent);
}