#include "tree.h"
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

void Tree::AddNode(unsigned int hash) {
  if (nodes.contains(hash)) {
    throw std::runtime_error("Adding node: node already exists" +
                             std::to_string(hash));
  }

  TNode *pNode{(*nodes.begin()).second};
  TravelToEnd(pNode);
  if (pNode != nullptr) {
    nodeCount++;
    nodes[hash] = &nodeStorage[nodeCount];
    pNode->pRight = nodes[hash];
  }
}

TNode *const Tree::GetNode(unsigned int hash) { return nodes[hash]; }

void Tree::TravelToEnd(TNode *pNext) {
  if (pNext == nullptr) {
    return;
  }

  if (pNext->pRight == nullptr) {
    return;
  }

  pNext = pNext->pRight;
}

void Tree::TravelTo(unsigned int hash, TNode *pNext,
                    std::vector<unsigned int> &path) {
  if (pNext == nullptr) {
    return;
  }

  if (hash == pNext->nodeHash) {
    return;
  }

  pNext = pNext->pRight;
}

void Tree::RemoveTree() {
  nodes.clear();
  nodeCount = 0;
}

void Tree::RemoveNode(unsigned int hash) {
  TNode *pNode = nodes[hash];
  if (pNode) {
    TNode *pLastNode = &nodeStorage[--nodeCount];

    static auto resetNode = [](TNode *pNode) {
      pNode->nodeHash = 0;
      if (pNode->pLeft) {
        pNode->pLeft->pRight = nullptr;
        pNode->pLeft = nullptr;
      }
      if (pNode->pRight) {
        pNode->pRight->pLeft = nullptr;
        pNode->pRight = nullptr;
      }
    };

    // swap node with last node in storage container if not the same
    if (pLastNode != pNode) {
      *pNode = *pLastNode;
    }
    // reset last node
    resetNode(pLastNode);
  }
}