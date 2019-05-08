#include "Algorithm.h"
#include <stdio.h>
#include <stdlib.h>

namespace UNDO
{

  Algorithm::Algorithm()
  {
  }

  Algorithm::~Algorithm()
  {
  }

  void Algorithm::traverse(tTransactionPtr from, tTransactionPtr to)
  {
    tTransactionPtr root = getCommonRoot(from, to);

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

  Algorithm::tTransactionPtr Algorithm::getCommonRoot(tTransactionPtr one, tTransactionPtr other)
  {
    if(one == other)
      return one;

    if(one->getDepth() > other->getDepth())
      return getCommonRoot(one->getPredecessor(), other);
    else
      return getCommonRoot(one, other->getPredecessor());
  }

  std::list<Algorithm::tTransactionPtr> Algorithm::getPathAsList(tTransactionPtr target, const Transaction *stop)
  {
    std::list<Algorithm::tTransactionPtr> ret;

    while(target.get() != stop)
    {
      ret.push_front(target);
      target = target->getPredecessor();
    }

    return ret;
  }

  Algorithm::tTransactionPtr Algorithm::find(tTransactionPtr root, const Glib::ustring &targetID)
  {
    long i = strtol(targetID.c_str(), NULL, 10);
    return find(root, (void *) i);
  }

  Algorithm::tTransactionPtr Algorithm::find(tTransactionPtr root, const void *targetID)
  {
    if(root.get() == targetID)
      return root;

    size_t numChildren = root->getNumSuccessors();

    for(size_t i = 0; i < numChildren; i++)
    {
      if(tTransactionPtr found = find(root->getSuccessor(i), targetID))
        return found;
    }

    return NULL;
  }

} /* namespace UNDO */
