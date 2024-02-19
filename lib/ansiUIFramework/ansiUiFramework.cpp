#include "ansiUiFramework.h"
#include "ansi.h"
#include "logger.h"
#include <algorithm>
#include <cstddef>
#include <functional>
#include <math.h>
#include <memory>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

std::shared_ptr<AnsiUIFramework> AnsiUIFramework::framework = nullptr;

bool UIData::AddItem(std::string_view itemName, POSITION itemData) {
  auto hash = std::hash<std::string_view>{}(itemName);
  if (uiItems.contains(hash))
    return false;
  uiItems[hash] = ItemInfo(itemData);
  return true;
}

std::optional<ItemInfo> UIData::GetItem(std::string_view itemName) const {
  return GetItem(std::hash<std::string_view>{}(itemName));
}

std::optional<ItemInfo> UIData::GetItem(unsigned int itemNameHash) const {
  if (uiItems.contains(itemNameHash))
    return {uiItems.at(itemNameHash)};
  return {};
}
ItemInfo &UIData::GetItemRef(unsigned int itemNameHash) {
  if (uiItems.contains(itemNameHash))
    return uiItems.at(itemNameHash);
  throw std::runtime_error("hash not found");
}

std::optional<std::pair<unsigned int, unsigned int>>
ItemInfo::GetWorldPos() const {
  return worldPos;
}
void ItemInfo::CacheWorldPos(std::pair<unsigned int, unsigned int> pos) {
  this->worldPos = pos;
}

AnsiUIFramework::~AnsiUIFramework() {
  if (pCurrentItem) {
    RemoveItemTree(pCurrentItem);
  }
  Ansi::BackToSaved();
}

void AnsiUIFramework::PrepareAnsiUiFramework() {
  Ansi::SaveCursorPos();
  int rows{20};
  int columns{200};
  for (auto r : std::ranges::iota_view{0, rows}) {
    for (auto i : std::ranges::iota_view{0, columns / 10}) {
      std::cout << "          ";
    }
    std::cout << std::endl;
  }
  Ansi::MoveUp(rows);
  Ansi::MoveLeft(columns);
}

void AnsiUIFramework::UpdateCurrentItem(const std::string_view &boxName,
                                        const POSITION &boxItem) {

  if (uiData.GetItem(boxName).has_value()) {
    // search through the fucking tree and find the item pointing to the box
    // item with this name.
  }

  uiData.AddItem(boxName, boxItem);

  if (pCurrentItem == nullptr) {

    pCurrentItem = new UIItem(boxName);
  } else {
    auto newItem{new UIItem(boxName)};
    newItem->parentItem = pCurrentItem;
    pCurrentItem->childItems.emplace_back(newItem);
    pCurrentItem = newItem;
  }

  inBox = true;
}

void AnsiUIFramework::TravelCursor(UIItem *const fromItem,
                                   UIItem *const targetItem) {
  if (fromItem == nullptr && targetItem == nullptr) {
    return;
  }

  if (targetItem == nullptr) {
    return;
  }

  Ansi::BackToSaved();

  POSITION targetPosition{};

  auto &targetPositionInfo{uiData.GetItemRef(targetItem->nameHash)};
  if (fromItem) {
    auto fromItemInfo{uiData.GetItem(fromItem->nameHash)};
    if (fromItem == targetItem->parentItem &&
        fromItemInfo->GetWorldPos().has_value()) {
      targetPosition = fromItemInfo->GetWorldPos().value();
      targetPosition.first += targetPositionInfo.position.first;
      targetPosition.second += targetPositionInfo.position.second;
    }
  } else {
    RecursiveTraveler(targetItem, fromItem, targetPosition);
    targetPositionInfo.CacheWorldPos(targetPosition);
  }

  MoveCursor(targetPosition);
  std::cout << targetPosition.first << "," << targetPosition.second;
}

void AnsiUIFramework::MoveCursor(const std::pair<int, int> &xy) {
  if (xy.first > 0) {
    Ansi::MoveRight(xy.first);
  } else {
    Ansi::MoveLeft(std::abs(xy.first));
  }

  if (xy.second > 0) {
    Ansi::MoveDown(xy.second);
  } else {
    Ansi::MoveDown(std::abs(xy.second));
  }
}

void AnsiUIFramework::Box(std::string_view boxName, POSITION boxItem) {
  if (!inBox && pCurrentItem) {
    throw std::runtime_error("all boxes should be inside a single top box");
  }

  UIItem *fromItem = pCurrentItem;
  Log::Debug("Box:");
  // this sets the current item to a newly created item
  UpdateCurrentItem(boxName, boxItem);
  // this moves to the current item, according to the current item
  TravelCursor(fromItem, pCurrentItem);
}

void AnsiUIFramework::EndBox() {
  if (!inBox) {
    throw std::runtime_error("Ending box while not in it");
  }
  Log::Debug("EndBox");
  if (pCurrentItem->parentItem) {
    pCurrentItem = pCurrentItem->parentItem;
  } else {
    inBox = false;
  }
}

void AnsiUIFramework::Item(std::string_view item) { std::cout << item; }

///==================
// Tree function
///========

// Searches the tree for target
void AnsiUIFramework::RecursiveTraveler(UIItem *const targetItem,
                                        UIItem *nextItem,
                                        POSITION &targetPosition) {
  // found if target is next or next is null
  if (targetItem == nextItem || nextItem == nullptr) {
    const auto item{uiData.GetItem(targetItem->nameHash)};
    targetPosition.first += item->position.first;
    targetPosition.second += item->position.second;
    Log::Debug(std::string("Target found") +
               std::to_string(targetPosition.first) + "," +
               std::to_string(targetPosition.second));
    nextItem = nullptr;
    return;
  }

  // adds the position and width to the target position
  static auto addBoxPosition{
      [&targetPosition](const UIData &uiData, UIItem *const nextItem) {
        const auto item{uiData.GetItem(nextItem->nameHash)};
        targetPosition.first += item->position.first;
        targetPosition.second += item->position.second;
      }};

  addBoxPosition(uiData, nextItem);

  // if it has a kid, target this next
  if (!nextItem->childItems.empty()) {
    nextItem = nextItem->childItems[0];
  } else if (nextItem->parentItem != nullptr) {
    // siblings
    // iterator to current item to find its sibling
    auto thisItem =
        std::ranges::find(nextItem->parentItem->childItems, nextItem);
    // check if sibling exists
    if ((thisItem++) != nextItem->parentItem->childItems.end()) {
      nextItem = *thisItem;
    } else if (nextItem->parentItem->parentItem != nullptr) {
      auto parentItem = std::ranges::find(
          nextItem->parentItem->parentItem->childItems, nextItem->parentItem);
      // check if sibling of parent exists
      if ((parentItem++) !=
          nextItem->parentItem->parentItem->childItems.end()) {
        nextItem = *parentItem;
      }
    }
  } else {
    throw std::runtime_error("no more items to evaluate..?");
  }

  Log::Debug("not found, adding position");
  RecursiveTraveler(targetItem, nextItem, targetPosition);
}

// removes all children of given container of items
void AnsiUIFramework::RemoveItemChildren(std::vector<UIItem *> &childItems) {
  if (childItems.empty()) {
    return;
  }

  std::for_each(childItems.begin(), childItems.end(), [this](UIItem *i) {
    if (!i->childItems.empty()) {
      RemoveItemChildren(i->childItems);
    }
    delete i;
  });
  childItems.clear();
}

// Removes entire tree
void AnsiUIFramework::RemoveItemTree(UIItem *item) {
  if (item) {
    // climb up the tree
    if (item->parentItem) {
      RemoveItemTree(item->parentItem);
    }
    // remove children
    RemoveItemChildren(item->childItems);

    delete item;
  }
}
