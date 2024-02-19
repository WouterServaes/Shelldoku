#pragma once
#include "ansi.h"
#include "tree.h"
#include <assert.h>
#include <compare>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string_view>
#include <utility>
// static framework
// clang-format off
// V1
// AnsiUi::Box(relative position, size)
// AnsiUi::Print -> first line of box, clamped to box
// AnsiUi::Print
// AnsiUi::EndBox

// V2
/// AnsiUi::Box(relative position, size)
// AnsiUi::Color -> everything from here in this box is certain color 
// AnsiUi::Print
// AnsiUi::Print
// AnsiUi::EndBox

// clang-format on
using POSITION = std::pair<unsigned int, unsigned int>;
using SIZE = std::pair<unsigned int, unsigned int>;
using ITEM = std::pair<POSITION, SIZE>;

struct ItemInfo {
  ItemInfo(POSITION item) : position(item){};
  ItemInfo() = default;
  std::pair<unsigned int, unsigned int> position;
  std::optional<std::pair<unsigned int, unsigned int>> GetWorldPos() const;
  void CacheWorldPos(std::pair<unsigned int, unsigned int> pos);

private:
  std::optional<std::pair<unsigned int, unsigned int>> worldPos;
};

struct UIData {
  bool AddItem(std::string_view itemName, POSITION itemData);
  [[nodiscard]] std::optional<ItemInfo>
  GetItem(std::string_view itemName) const;
  [[nodiscard]] std::optional<ItemInfo>
  GetItem(unsigned int itemNameHash) const;

  [[nodiscard]] ItemInfo &GetItemRef(unsigned int itemNameHash);

private:
  std::unordered_map<unsigned int, ItemInfo> uiItems;
};

class AnsiUIFramework {
public:
  [[nodiscard]] static std::shared_ptr<AnsiUIFramework> instance() {
    if (framework == nullptr) {
      framework = std::shared_ptr<AnsiUIFramework>(new AnsiUIFramework());
      framework->PrepareAnsiUiFramework();
    }
    return framework;
  }
  ~AnsiUIFramework();
  void Box(std::string_view boxName, POSITION boxItem);
  void EndBox();
  void Item(std::string_view item);

private:
  // tree
  struct UIItem {
    UIItem(const std::string_view &name)
        : nameHash(std::hash<std::string_view>{}(name)) {}
    UIItem() = delete;
    const unsigned int nameHash;
    UIItem *parentItem;
    std::vector<UIItem *> childItems;
  };
  void PrepareAnsiUiFramework();
  void UpdateCurrentItem(const std::string_view &boxName,
                         const POSITION &boxItem);
  void TravelCursor(UIItem *const fromItem, UIItem *const targetItem);
  void RecursiveTraveler(UIItem *const targetItem, UIItem *nextItem,
                         POSITION &targetPosition);
  void RemoveItemTree(UIItem *item);
  void RemoveItemChildren(std::vector<UIItem *> &childItems);
  void MoveCursor(const std::pair<int, int> &xy);
  static std::shared_ptr<AnsiUIFramework> framework;
  UIData uiData;

  UIItem *pCurrentItem;
  //
  bool inBox{false};
  bool prepared{false};
};

[[nodiscard]] static std::shared_ptr<AnsiUIFramework> ANSI_UI() {
  return AnsiUIFramework::instance();
}
