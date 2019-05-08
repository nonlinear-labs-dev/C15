#include "UndoActions.h"
#include "libundo/undo/Scope.h"
#include <libundo/undo/Algorithm.h>

UndoActions::UndoActions(UNDO::Scope &scope)
    : super("/undo/")
{
  addAction("undo", [&](shared_ptr<NetworkRequest> request) { scope.undo(); });

  addAction("undo-jump", [&](shared_ptr<NetworkRequest> request) mutable {
    Glib::ustring target = request->get("target");
    scope.undoJump(target);
  });

  addAction("redo", [&](shared_ptr<NetworkRequest> request) {
    int way = stoi(request->get("way", "-1"));
    scope.redo(way);
  });

  addAction("erase-branch", [&](shared_ptr<NetworkRequest> request) mutable {
    Glib::ustring id = request->get("id");
    scope.eraseBranch(id);
  });

  addAction("rebase", [&](shared_ptr<NetworkRequest> request) mutable {
    Glib::ustring id = request->get("id");

    if(auto newBase = UNDO::Algorithm::find(scope.getRootTransaction(), id))
    {
      scope.rebase(newBase);
    }
  });
}

UndoActions::~UndoActions()
{
}
