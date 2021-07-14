#pragma once

#include "playground.h"
#include "InStream.h"
#include <tools/Signal.h>
#include <libundo/undo/Scope.h>
#include <libundo/undo/Transaction.h>

class Attributes;
class Serializer;

class Reader
{
 public:
  Reader(InStream &in, UNDO::Transaction *transaction);
  virtual ~Reader();

  typedef std::function<void(const Glib::ustring &text, const Attributes &attr)> tTextElementCB;
  typedef std::function<Serializer *(const Attributes &attr)> tTagElementCB;

  void onTextElement(size_t nameHash, tTextElementCB cb);
  void onTextElement(const Glib::ustring &name, tTextElementCB cb);

  int getFileVersion() const;

  void onTag(size_t nameHash, tTagElementCB cb);
  void onTag(const Glib::ustring &name, tTagElementCB cb);

  void preProcess();

  void loadTextElement(size_t nameHash, Glib::ustring &target);
  void loadTextElement(size_t nameHash, std::string &target);
  void loadTextElement(const Glib::ustring &name, Glib::ustring &target);
  void loadTextElement(const Glib::ustring &name, std::string &target);

  template <typename T> void loadElement(const Glib::ustring &name, T &target)
  {
    loadElement(m_hash(name), target);
  }

  template <typename T> void loadElement(size_t nameHash, T &target)
  {
    onTextElement(nameHash, [&](const Glib::ustring &text, const Attributes &attr) mutable {
      T foo;
      std::stringstream str;
      str << text;
      str >> foo;
      auto scope = UNDO::createSwapData(foo);

      getTransaction()->addSimpleCommand([=, &target](UNDO::Command::State) mutable { scope->swapWith(target); });
    });
  }

  UNDO::Transaction *getTransaction();

  enum FileVersionCheckResult
  {
    OK,
    Unsupported
  };

  sigc::connection onFileVersionRead(sigc::slot<FileVersionCheckResult, int> slot);

  template <typename tSerializer, typename... tArgs> bool read(tArgs... args)
  {
    bool foundTag = false;
    preProcess();

    onTag(tSerializer::getTagName(), [=, &foundTag](auto a) {
      foundTag = true;
      return new tSerializer(args...);
    });

    feed();
    return foundTag;
  }

 protected:
  InStream &getInStream();

  bool onStartElement(size_t nameHash, const Attributes &attributes);
  void onEndElement();
  void onTextElement(size_t nameHash, const Attributes &attributes, const Glib::ustring &text);

 private:
  virtual void feed() = 0;

  InStream &m_in;
  UNDO::Transaction *m_transaction;

  typedef std::map<size_t, tTextElementCB> tTextElementCallbacks;
  typedef std::map<size_t, tTagElementCB> tTagElementCallbacks;

  struct StackEntry
  {
    explicit StackEntry(size_t depth)
    {
      m_treeDepth = depth;
    }

    tTextElementCallbacks m_textCallbacks;
    tTagElementCallbacks m_tagCallbacks;
    size_t m_treeDepth;

    typedef std::shared_ptr<Serializer> tSerializer;
    tSerializer serializer;
  };

  std::list<StackEntry> m_stack;
  size_t m_treeDepth;
  std::hash<Glib::ustring> m_hash;
  int m_fileVersion = 0;
  Signal<FileVersionCheckResult, int> m_sigFileVersionRead;
  FileVersionCheckResult m_versionCheck = FileVersionCheckResult::OK;
};
