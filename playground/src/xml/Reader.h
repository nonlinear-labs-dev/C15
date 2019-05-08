#pragma once

#include "playground.h"
#include "InStream.h"
#include "libundo/undo/Scope.h"

class Attributes;
class Serializer;

class Reader
{
 public:
  Reader(InStream &in, UNDO::Scope::tTransactionPtr transaction);
  virtual ~Reader();

  typedef function<void(const ustring &text, const Attributes &attr)> tTextElementCB;
  typedef function<Serializer *(const Attributes &attr)> tTagElementCB;

  void onTextElement(size_t nameHash, tTextElementCB cb);
  void onTextElement(const ustring &name, tTextElementCB cb);

  int getFileVersion() const;

  void onTag(size_t nameHash, tTagElementCB cb);
  void onTag(const ustring &name, tTagElementCB cb);

  void preProcess();

  void loadTextElement(size_t nameHash, ustring &target);
  void loadTextElement(const ustring &name, ustring &target);

  template <typename T> void loadElement(const ustring &name, T &target)
  {
    loadElement(m_hash(name), target);
  }

  template <typename T> void loadElement(size_t nameHash, T &target)
  {
    onTextElement(nameHash, [&](const ustring &text, const Attributes &attr) mutable {
      T foo;
      stringstream str;
      str << text;
      str >> foo;
      auto scope = UNDO::createSwapData(foo);

      getTransaction()->addSimpleCommand([=, &target](UNDO::Command::State) mutable { scope->swapWith(target); });
    });
  }

  UNDO::Scope::tTransactionPtr getTransaction();

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
  void onTextElement(size_t nameHash, const Attributes &attributes, const ustring &text);

 private:
  virtual void feed() = 0;

  InStream &m_in;
  UNDO::Scope::tTransactionPtr m_transaction;

  typedef map<size_t, tTextElementCB> tTextElementCallbacks;
  typedef map<size_t, tTagElementCB> tTagElementCallbacks;

  struct StackEntry
  {
    StackEntry(size_t depth)
    {
      m_treeDepth = depth;
    }

    tTextElementCallbacks m_textCallbacks;
    tTagElementCallbacks m_tagCallbacks;
    size_t m_treeDepth;

    typedef shared_ptr<Serializer> tSerializer;
    tSerializer serializer;
  };

  list<StackEntry> m_stack;
  size_t m_treeDepth;
  std::hash<ustring> m_hash;
  int m_fileVersion = 0;
  Signal<FileVersionCheckResult, int> m_sigFileVersionRead;
  FileVersionCheckResult m_versionCheck = FileVersionCheckResult::OK;
};
