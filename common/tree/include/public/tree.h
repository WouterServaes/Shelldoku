#pragma once
#include <cstddef>
#include <unordered_map>
#include <vector>

struct TNode {
  TNode() = default;
  TNode(unsigned int nodeHash_) : nodeHash(nodeHash_){};
  TNode *pLeft{nullptr};
  TNode *pRight{nullptr};
  unsigned int nodeHash;
};

class Tree {
public:
  Tree() = default;
  ~Tree();
  void AddNode(unsigned int hash);
  TNode *const GetNode(unsigned int hash);
  void TravelToEnd(TNode *pNext);
  void TravelTo(unsigned int hash, TNode *pnext,
                std::vector<unsigned int> &path);
  void RemoveNode(unsigned int hash);

private:
  void RemoveTree();
  // node easy access
  std::unordered_map<unsigned int, TNode *> nodes;
  // node storage
  TNode nodeStorage[100]{};
  std::size_t nodeCount{};
};
