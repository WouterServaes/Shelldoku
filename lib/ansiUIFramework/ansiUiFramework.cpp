#include "ansiUiFramework.h"
#include "ansi.h"
#include <algorithm>
#include <cstddef>
#include <functional>
#include <math.h>
#include <memory>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <string_view>
#include <vector>

std::shared_ptr<AnsiUIFramework> AnsiUIFramework::framework = nullptr;

bool UIData::AddItem(std::string_view itemName, UIData::ITEM itemData) {
  auto hash = std::hash<std::string_view>{}(itemName);
  if (uiItems.contains(hash))
    return false;
  uiItems[hash] = itemData;
  return true;
}

std::optional<UIData::ITEM> UIData::GetItem(std::string_view itemName) const {
  return GetItem(std::hash<std::string_view>{}(itemName));
}

std::optional<UIData::ITEM> UIData::GetItem(unsigned int itemNameHash) const {
  if (uiItems.contains(itemNameHash))
    return {uiItems.at(itemNameHash)};
  return {};
}

AnsiUIFramework::~AnsiUIFramework() {
  if (pCurrentItem) {
    RemoveItemTree(pCurrentItem);
  }
}

void AnsiUIFramework::PrepareAnsiUiFramework() {
  int rows{5};
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
                                        const UIData::ITEM &boxItem) {

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

void AnsiUIFramework::TravelCursor(UIItem *const targetItem) {
  if (pCurrentItem == nullptr || targetItem == nullptr) {
    return;
  }
  Ansi::BackToSaved();
  UIData::POSITION targetPosition{};
  RecursiveTraveler(targetItem, pCurrentItem, targetPosition);
  MoveCursor(targetPosition);
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

void AnsiUIFramework::Box(std::string_view boxName, UIData::ITEM boxItem) {
  if (!inBox && pCurrentItem) {
    throw std::runtime_error("all boxes should be inside a single top box");
  }

  UpdateCurrentItem(boxName, boxItem);
  TravelCursor(pCurrentItem);
}

void AnsiUIFramework::EndBox() {
  if (!inBox) {
    throw std::runtime_error("Ending box while not in it");
  }

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
                                        UIData::POSITION &targetPosition) {
  // found
  if (targetItem == nextItem) {
    const auto item{uiData.GetItem(targetItem->nameHash)};
    targetPosition.first += item->first.first;
    targetPosition.second += item->first.second;
    return;
  }

  // children
  if (!nextItem->childItems.empty()) {

    nextItem = nextItem->childItems[0];
    const auto item{uiData.GetItem(nextItem->nameHash)};
    targetPosition.first += item->first.first;
    targetPosition.second += item->first.second;
    return;
  }

  if (nextItem->parentItem != nullptr) {
    // siblings
    // iterator to current item to find its sibling
    auto thisItem =
        std::ranges::find(nextItem->parentItem->childItems, nextItem);
    // check if sibling exists
    if ((thisItem++) != nextItem->parentItem->childItems.end()) {

      nextItem = *thisItem;
      const auto item{uiData.GetItem(nextItem->nameHash)};
      targetPosition.first += item->first.first;
      targetPosition.second += item->first.second;
      return;
    }

    // no child or sibling
    if (nextItem->parentItem->parentItem != nullptr) {
      auto parentItem = std::ranges::find(
          nextItem->parentItem->parentItem->childItems, nextItem->parentItem);
      // check if sibling of parent exists
      if ((parentItem++) !=
          nextItem->parentItem->parentItem->childItems.end()) {
        nextItem = *parentItem;
        const auto item{uiData.GetItem(nextItem->nameHash)};
        targetPosition.first += item->first.first;
        targetPosition.second += item->first.second;

        return;
      }
    }
  }

  // tree end: no result
  throw std::runtime_error("target item was not found ui items");
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
