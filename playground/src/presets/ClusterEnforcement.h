#pragma once
#include "PresetManager.h"
#include "PresetBank.h"
#include <Application.h>

class ClusterEnforcement
{
 private:
  typedef shared_ptr<PresetBank> tBankPtr;
  struct TreeNode;
  typedef std::shared_ptr<TreeNode> tTreeNodePtr;
  struct TreeNode
  {
    tTreeNodePtr right;
    tTreeNodePtr bottom;
    tTreeNodePtr left;
    tTreeNodePtr top;
    tBankPtr bank;
  };
  typedef std::shared_ptr<TreeNode> tCluster;

 public:
  ClusterEnforcement();
  virtual ~ClusterEnforcement();
  void enforceClusterRuleOfOne(UNDO::Scope::tTransactionPtr transaction);
  void sanitizeBankThatWillBeDeleted(UNDO::Scope::tTransactionPtr transaction, tBankPtr bank);

 private:
  std::shared_ptr<PresetManager> m_presetManager;
  std::vector<tCluster> m_clusters;
  std::map<Glib::ustring, tTreeNodePtr> m_uuidToTreeNode;

  bool applyRule(UNDO::Scope::tTransactionPtr transaction);
  bool applyRule(UNDO::Scope::tTransactionPtr transaction, tTreeNodePtr node);
  void appendToBottomChildOfNode(UNDO::Scope::tTransactionPtr transaction, tTreeNodePtr node, tTreeNodePtr target);
  void buildClusterStructure();
  tTreeNodePtr findTreeNode(Glib::ustring uuid);
  void addCluster(tTreeNodePtr clusterRoot);
  void connectToClusterStructure(tTreeNodePtr masterNode, tTreeNodePtr myNode);
  void ruleDelete(UNDO::Scope::tTransactionPtr transaction, tTreeNodePtr bottom, tTreeNodePtr right,
                  Glib::ustring newMasterUuid, PresetBank::AttachmentDirection dir);

  std::vector<tBankPtr> getClusterMasters();
};
