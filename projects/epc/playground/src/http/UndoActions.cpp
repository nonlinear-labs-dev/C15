#include "UndoActions.h"
#include "libundo/undo/Scope.h"
#include <libundo/undo/Algorithm.h>
#include <libundo/undo/StringTools.h>
#include <http/NetworkRequest.h>

UndoActions::UndoActions(UNDO::Scope &scope)
    : super("/undo/")
{
  addAction("undo", [&](std::shared_ptr<NetworkRequest> request) { scope.undo(); });

  addAction("undo-jump", [&](std::shared_ptr<NetworkRequest> request) mutable {
    Glib::ustring target = request->get("target");
    scope.undoJump(target);
  });

  addAction("jump-to-timestamp", [&](std::shared_ptr<NetworkRequest> request) mutable {
    using namespace std::chrono;
    auto time = std::stoll(request->get("timestamp"));
    if(auto transaction = scope.findTransactionAt(system_clock::time_point(system_clock::duration(time))))
      scope.undoJump(UNDO::StringTools::buildString(reinterpret_cast<size_t>(transaction)));
  });

  addAction("redo", [&](std::shared_ptr<NetworkRequest> request) {
    int way = std::stoi(request->get("way", "-1"));
    scope.redo(way);
  });

  addAction("erase-branch", [&](std::shared_ptr<NetworkRequest> request) mutable {
    Glib::ustring id = request->get("id");
    scope.eraseBranch(id);
  });

  addAction("rebase", [&](std::shared_ptr<NetworkRequest> request) mutable {
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
