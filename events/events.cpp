#include "include/private/events.h"
#include <string_view>

Event::Event(const EventID id) : id(id) {}

void Event::DoEvent() {}

const EventID Event::Id() const { return id; }