#include "Algorithm.h"
#include <cstdio>
#include <cstdlib>

namespace UNDO
{

  Algorithm::Algorithm()
  {
  }

  Algorithm::~Algorithm()
  {
  }

  void Algorithm::traverse(Transaction* from, Transaction* to)
  {
    Transaction* root = getCommonRoot(from, to);

    if(from == root)
    {
      from->redoUntil(to);
    }
    else if(to == root)
    {
      from->undoUntil(to);
    }
    else
    {
      from->undoUntil(root);
      root->redoUntil(to);
    }
  }

  Transaction* Algorithm::getCommonRoot(Transaction* one, Transaction* other)
  {
    if(one == other)
      return one;

    if(one->getDepth() > other->getDepth())
      return getCommonRoot(one->getPredecessor(), other);
    else
      return getCommonRoot(one, other->getPredecessor());
  }

  std::list<Transaction*> Algorithm::getPathAsList(Transaction* target, const Transaction* stop)
  {
    std::list<Transaction*> ret {};

    while(target != stop)
    {
      ret.push_front(target);
      target = target->getPredecessor();
    }

    return ret;
  }

  Transaction* Algorithm::find(Transaction* root, const Glib::ustring& targetID)
  {
    long i = strtol(targetID.c_str(), nullptr, 10);
    return find(root, (void*) i);
  }

  Transaction* Algorithm::find(Transaction* root, const void* targetID)
  {
    if(root == targetID)
      return root;

    size_t numChildren = root->getNumSuccessors();

    for(size_t i = 0; i < numChildren; i++)
    {
      if(Transaction* found = find(root->getSuccessor(i), targetID))
        return found;
    }

    return nullptr;
  }

} /* namespace UNDO */
