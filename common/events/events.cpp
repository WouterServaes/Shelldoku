#include "events.h"

Event::Event(const EventID id) : id(id) {}

void Event::DoEvent() {}

const EventID Event::Id() const { return id; }