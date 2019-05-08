#include <presets/ClusterEnforcement.h>

ClusterEnforcement::ClusterEnforcement()
{
  m_presetManager = Application::get().getPresetManager();
}

ClusterEnforcement::~ClusterEnforcement()
{
}

std::vector<ClusterEnforcement::tBankPtr> ClusterEnforcement::getClusterMasters()
{
  auto presetManager = Application::get().getPresetManager();
  auto allBanks = presetManager->getBanks();
  std::vector<tBankPtr> clusterMaster;
  for(auto bank : allBanks)
  {
    if(bank->getAttached().uuid == "")
      clusterMaster.push_back(bank);
  }
  return clusterMaster;
}

void ClusterEnforcement::buildClusterStructure()
{
  m_clusters.clear();
  m_uuidToTreeNode.clear();

  auto waitingList = m_presetManager->getBanks();

  std::vector<tBankPtr> anotherWaitingList;

  while(true)
  {
    anotherWaitingList.clear();

    for(auto bank : waitingList)
    {
      if(bank->getAttached().uuid == "")
      {
        auto clusterRoot = std::make_shared<TreeNode>();
        clusterRoot->bank = bank;
        addCluster(clusterRoot);
      }
      else if(auto masterNode = findTreeNode(bank->getAttached().uuid))
      {
        auto myNode = std::make_shared<TreeNode>();
        myNode->bank = bank;
        connectToClusterStructure(masterNode, myNode);
      }
      else
      {
        anotherWaitingList.push_back(bank);
      }
    }

    if(waitingList.size() == anotherWaitingList.size())
    {
      break;
    }

    waitingList = anotherWaitingList;
  }
}

void ClusterEnforcement::enforceClusterRuleOfOne(UNDO::Scope::tTransactionPtr transaction)
{
  do
  {
    buildClusterStructure();
  } while(applyRule(transaction));
}

void ClusterEnforcement::sanitizeBankThatWillBeDeleted(UNDO::Scope::tTransactionPtr transaction,
                                                       ClusterEnforcement::tBankPtr bank)
{
  buildClusterStructure();

  if(auto deletedTreeNode = findTreeNode(bank->getUuid()))
  {
    auto left = deletedTreeNode->left;
    auto right = deletedTreeNode->right;
    auto top = deletedTreeNode->top;
    auto bottom = deletedTreeNode->bottom;

    constexpr auto topDir = PresetBank::AttachmentDirection::top;
    constexpr auto leftDir = PresetBank::AttachmentDirection::left;
    constexpr auto noneDir = PresetBank::AttachmentDirection::none;

    if(top)
    {
      ruleDelete(transaction, bottom, right, top->bank->getUuid(), topDir);
    }
    else if(left)
    {
      ruleDelete(transaction, bottom, right, left->bank->getUuid(), leftDir);
    }
    else
    {
      ruleDelete(transaction, bottom, right, "", noneDir);

      if(bottom)
      {
        bottom->bank->undoableSetPosition(transaction, bank->getX(), bank->getY());
      }
      else if(right)
      {
        right->bank->undoableSetPosition(transaction, bank->getX(), bank->getY());
      }
    }
  }
}

void ClusterEnforcement::ruleDelete(UNDO::Scope::tTransactionPtr transaction, ClusterEnforcement::tTreeNodePtr bottom,
                                    ClusterEnforcement::tTreeNodePtr right, Glib::ustring newMasterUuid,
                                    PresetBank::AttachmentDirection dir)
{
  if(bottom)
  {
    bottom->bank->undoableAttachBank(transaction, newMasterUuid, dir);
    if(right)
    {
      right->bank->undoableAttachBank(transaction, bottom->bank->getUuid(), PresetBank::AttachmentDirection::left);
    }
  }
  else if(right)
  {
    right->bank->undoableAttachBank(transaction, newMasterUuid, dir);
  }
}

bool ClusterEnforcement::applyRule(UNDO::Scope::tTransactionPtr transaction)
{
  for(auto clusterMaster : m_clusters)
  {
    if(applyRule(transaction, clusterMaster))
    {
      return true;
    }
  }
  return false;
}

bool ClusterEnforcement::applyRule(UNDO::Scope::tTransactionPtr transaction, tTreeNodePtr node)
{
  tTreeNodePtr mostTopNode = nullptr;

  while(node != nullptr)
  {
    if(node->right != nullptr)
    {
      if(mostTopNode != nullptr)
      {
        appendToBottomChildOfNode(transaction, node->right, mostTopNode);
        return true;
      }
      else
      {
        mostTopNode = node->right;
        applyRule(transaction, mostTopNode);
      }
    }
    node = node->bottom;
  }
  return false;
}

void ClusterEnforcement::appendToBottomChildOfNode(UNDO::Scope::tTransactionPtr transaction, tTreeNodePtr node,
                                                   tTreeNodePtr target)
{
  while(target->bottom != nullptr)
  {
    target = target->bottom;
  }

  node->bank->undoableAttachBank(transaction, target->bank->getUuid(), PresetBank::AttachmentDirection::top);
}

ClusterEnforcement::tTreeNodePtr ClusterEnforcement::findTreeNode(Glib::ustring uuid)
{
  auto it = m_uuidToTreeNode.find(uuid);

  if(it != m_uuidToTreeNode.end())
    return it->second;
  else
    return nullptr;
}

void ClusterEnforcement::addCluster(tTreeNodePtr clusterRoot)
{
  m_uuidToTreeNode[clusterRoot->bank->getUuid()] = clusterRoot;
  m_clusters.push_back(clusterRoot);
}

void ClusterEnforcement::connectToClusterStructure(tTreeNodePtr masterNode, tTreeNodePtr myNode)
{
  switch(myNode->bank->getAttached().direction)
  {
    case PresetBank::AttachmentDirection::left:
      myNode->left = masterNode;
      masterNode->right = myNode;

      break;
    case PresetBank::AttachmentDirection::top:
      myNode->top = masterNode;
      masterNode->bottom = myNode;
  }

  m_uuidToTreeNode[myNode->bank->getUuid()] = myNode;
}
