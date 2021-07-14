#include <utility>

#include "Transaction.h"
#include "Scope.h"
#include "Algorithm.h"
#include "SwapCommand.h"
#include <nltools/Assert.h>
#include "device-settings/DebugLevel.h"
#include <xml/Attribute.h>
#include <xml/Writer.h>
#include <tools/PerformanceTimer.h>
#include "StringTools.h"
#include <algorithm>

namespace UNDO
{

  static int numTransactionsCreated = 0;

  Transaction::Transaction(Scope &scope, const Glib::ustring &name, size_t depth)
      : Command()
      , UpdateDocumentContributor(&scope)
      , m_scope(scope)
      , m_name(name)
      , m_isClosed(false)
      , m_depth(depth)
  {
    DebugLevel::info("Creating UNDO::Transaction:", name);
    numTransactionsCreated++;
  }

  Transaction::~Transaction()
  {
    m_scope.onTransactionDestroyed(this);
  }

  int Transaction::getAndResetNumTransactions()
  {
    int ret = numTransactionsCreated;
    numTransactionsCreated = 0;
    return ret;
  }

  Glib::ustring Transaction::getName() const
  {
    return m_name;
  }

  void Transaction::setName(const Glib::ustring &name)
  {
    m_name = name;
  }

  void Transaction::close()
  {
    tCommandList todo;

    swap(todo, m_postfixCommands);

    for(const auto &h : todo)
      addCommand(h);

    m_isClosed = true;
  }

  void Transaction::reopen()
  {
    g_assert(isClosed());
    m_isClosed = false;
  }

  bool Transaction::isClosed() const
  {
    return m_isClosed;
  }

  void Transaction::addSimpleCommand(ActionCommand::tAction doAndRedo, ActionCommand::tAction undo)
  {
    tCommandPtr cmd(new ActionCommand(std::move(doAndRedo), undo));
    addCommand(cmd);
  }

  void Transaction::addSimpleCommand(ActionCommand::tAction doRedoUndo)
  {
    tCommandPtr cmd(new SwapCommand(std::move(doRedoUndo)));
    addCommand(cmd);
  }

  void Transaction::addCommand(tCommandPtr cmd)
  {
    nltools_assertAlways(!m_isClosed);
    cmd->setParentTransaction(this);
    m_commands.push_back(cmd);
    cmd->doAction();
    onChange();
  }

  void Transaction::implDoAction() const
  {
  }

  void Transaction::implUndoAction() const
  {
    nltools_assertAlways(m_isClosed);

    onImplUndoActionStart();

    for(auto it = m_commands.rbegin(); it != m_commands.rend(); ++it)
    {
      (*it)->undoAction();
    }
  }

  void Transaction::implRedoAction() const
  {
    nltools_assertAlways(m_isClosed);

    for(const auto &m_command : m_commands)
    {
      m_command->redoAction();
    }

    onImplRedoActionFinished();
  }

  void Transaction::onImplUndoActionStart() const
  {
    if(!hasParentTransaction())
      m_scope.onTransactionUndoStart();
  }

  void Transaction::onImplRedoActionFinished() const
  {
    if(!hasParentTransaction())
      m_scope.onTransactionRedone(this);
  }

  bool Transaction::hasSuccessors() const
  {
    return !m_successors.empty();
  }

  size_t Transaction::getNumSuccessors() const
  {
    return m_successors.size();
  }

  Transaction *Transaction::getSuccessor(size_t num) const
  {
    if(!hasSuccessors())
      return nullptr;

    return m_successors[num].get();
  }

  void Transaction::addSuccessor(std::unique_ptr<Transaction> successor)
  {
    m_defaultRedoRoute = successor.get();
    m_successors.push_back(std::move(successor));
    onChange();
  }

  void Transaction::setPredecessor(Transaction *predecessor)
  {
    m_predecessor = predecessor;
    onChange();
  }

  void Transaction::eraseSuccessor(const Transaction *successor)
  {
    auto end = std::remove_if(m_successors.begin(), m_successors.end(),
                              [successor](auto &p) { return p.get() == successor; });

    m_successors.erase(end, m_successors.end());

    if(m_defaultRedoRoute == successor)
    {
      m_defaultRedoRoute = nullptr;

      if(!m_successors.empty())
      {
        m_defaultRedoRoute = m_successors.front().get();
      }
    }

    onChange();
  }

  Transaction *Transaction::getPredecessor() const
  {
    return m_predecessor;
  }

  size_t Transaction::getDepth() const
  {
    return m_depth;
  }

  void Transaction::rollBack()
  {
    close();
    m_scope.undoAndHushUp();
  }

  void Transaction::redoUntil(Transaction *target)
  {
    std::list<Transaction *> steps = Algorithm::getPathAsList(target, this);

    for(Transaction *step : steps)
    {
      step->redoAction();
    }
  }

  void Transaction::undoUntil(Transaction *target)
  {
    undoAction();

    if(getPredecessor() != target)
      getPredecessor()->undoUntil(target);
  }

  void Transaction::addChildren(std::list<Transaction *> &list) const
  {
    for(auto &c : m_successors)
      list.push_back(c.get());
  }

  void Transaction::setDefaultRedoRoute(Transaction *route)
  {
    m_defaultRedoRoute = std::move(route);
    onChange();
  }

  Transaction *Transaction::getDefaultRedoRoute() const
  {
    return m_defaultRedoRoute;
  }

  void Transaction::addPostfixCommand(ActionCommand::tAction doRedoUndo)
  {
    m_postfixCommands.emplace_back(new SwapCommand(std::move(doRedoUndo)));
  }

  void Transaction::writeDocument(Writer &writer, UpdateDocumentContributor::tUpdateID knownRevision) const
  {
    bool changed = knownRevision < getUpdateIDOfLastChange();

    if(changed)
    {
      writer.writeTag("transaction", Attribute("name", getName()), Attribute("id", StringTools::buildString(this)),
                      Attribute("predecessor", StringTools::buildString(getPredecessor())),
                      Attribute("successors", getSuccessorsString()),
                      Attribute("default-redo-route", StringTools::buildString(m_defaultRedoRoute)),
                      Attribute("changed", changed), [&]() {});
    }
  }

  Glib::ustring Transaction::getSuccessorsString() const
  {
    std::stringstream str;

    for(const auto &s : m_successors)
    {
      if(str.tellp())
        str << ',';
      str << StringTools::buildString(s.get());
    }

    return str.str();
  }

  long Transaction::traverseTree() const
  {
    auto start = std::chrono::high_resolution_clock::now();
    size_t count = 0;
    this->traverse([&](const Transaction *t) { count++; });
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
  }

  void Transaction::traverse(std::function<void(const UNDO::Transaction *)> cb) const
  {
    std::vector<const Transaction *> list { this };
    unsigned long long index = 0;

    while(index < list.size())
    {
      auto curr = list[index];
      if(curr != nullptr)
      {
        for(const auto &node : curr->m_successors)
          list.emplace_back(node.get());

        cb(curr);
        index++;
      }
    }
  }

  int Transaction::countPredecessors() const
  {
    int i = 0;
    auto walker = getPredecessor();

    while(walker)
    {
      i++;
      walker = walker->getPredecessor();
    }

    return i;
  }

  int Transaction::countSuccessorsOnDefaultRoute() const
  {
    int i = 0;
    auto walker = getDefaultRedoRoute();

    while(walker)
    {
      i++;
      walker = walker->getDefaultRedoRoute();
    }

    return i;
  }

  bool Transaction::isDefaultRouteLeft() const
  {
    return getDirectionToDefaultRoute() > 0;
  }

  bool Transaction::isDefaultRouteRight() const
  {
    return getDirectionToDefaultRoute() < 0;
  }

  int Transaction::getDirectionToDefaultRoute() const
  {
    auto path = m_scope.getUndoTransaction();
    auto parent = getPredecessor();

    while(path)
    {
      if(parent == path->getPredecessor())
      {
        return parent->getOrder(this, path);
      }
      path = path->getPredecessor();
    }

    return parent->getOrder(this, parent->m_defaultRedoRoute);
  }

  int Transaction::getOrder(const Transaction *a, const Transaction *b) const
  {
    if(a == b)
      return 0;

    for(const auto &p : m_successors)
    {
      if(p.get() == a)
        return -1;
      else if(p.get() == b)
        return 1;
    }
    return 0;
  }

  std::unique_ptr<Transaction> Transaction::exhaust(const Transaction *a)
  {
    auto it = std::find_if(m_successors.begin(), m_successors.end(), [&](auto &b) { return b.get() == a; });
    if(it != m_successors.end())
    {
      std::unique_ptr<Transaction> ret(std::move(*it));
      m_successors.erase(it);
      ret->setPredecessor(nullptr);
      return ret;
    }
    return nullptr;
  }
} /* namespace UNDO */
