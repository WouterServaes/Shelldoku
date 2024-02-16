#pragma once
#include "ansi.h"
#include <assert.h>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string_view>

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
struct UIData {
  using POSITION = std::pair<unsigned int, unsigned int>;
  using SIZE = std::pair<unsigned int, unsigned int>;
  using ITEM = std::pair<POSITION, SIZE>;
  bool AddItem(std::string_view itemName, ITEM itemData);
  [[nodiscard]] std::optional<ITEM> GetItem(std::string_view itemName) const;
  [[nodiscard]] std::optional<ITEM> GetItem(unsigned int itemNameHash) const;

private:
  std::unordered_map<unsigned int, ITEM> uiItems;
};

class AnsiUIFramework {
public:
  [[nodiscard]] static std::shared_ptr<AnsiUIFramework> instance() {
    if (framework == nullptr) {
      Ansi::SaveCursorPos();
      framework = std::shared_ptr<AnsiUIFramework>(new AnsiUIFramework());
    }
    return framework;
  }
  ~AnsiUIFramework();

  void Box(std::string_view boxName, UIData::ITEM boxItem);
  void EndBox();
  // void Item()
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
  void UpdateCurrentItem(const std::string_view &boxName,
                         const UIData::ITEM &boxItem);
  void TravelCursor(UIItem *const targetItem);
  void RecursiveTraveler(UIItem *const targetItem, UIItem *nextItem);
  void RemoveItemTree(UIItem *item);
  void RemoveItemChildren(std::vector<UIItem *> &childItems);
  void MoveCursor(const std::pair<int, int> &xy);
  static std::shared_ptr<AnsiUIFramework> framework;
  UIData uiData;

  UIItem *pCurrentItem;
  //
  bool inBox{false};
};

[[nodiscard]] static std::shared_ptr<AnsiUIFramework> ANSI_UI() {
  return AnsiUIFramework::instance();
}
