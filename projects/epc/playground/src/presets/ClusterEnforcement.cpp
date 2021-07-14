#include <presets/ClusterEnforcement.h>
#include <tools/PerformanceTimer.h>

ClusterEnforcement::ClusterEnforcement(PresetManager* pm)
{
  m_presetManager = Application::get().getPresetManager();
}

ClusterEnforcement::~ClusterEnforcement()
{
}

std::vector<Bank*> ClusterEnforcement::getClusterMasters()
{
  std::vector<Bank*> clusterMaster;

  Application::get().getPresetManager()->forEachBank([&](auto bank) {
    if(bank->getAttachedToBankUuid().empty())
      clusterMaster.push_back(bank);
  });

  return clusterMaster;
}

void ClusterEnforcement::buildClusterStructure()
{
  m_clusters.clear();
  m_uuidToTreeNode.clear();

  auto waitingList = m_presetManager->getBanks();

  std::list<Bank*> anotherWaitingList;

  while(true)
  {
    anotherWaitingList.clear();

    for(auto bank : waitingList)
    {
      if(bank->getAttachedToBankUuid().empty())
      {
        auto clusterRoot = std::make_shared<TreeNode>();
        clusterRoot->bank = bank;
        clusterRoot->master = nullptr;
        addCluster(clusterRoot);
      }
      else if(auto masterNode = findTreeNode(bank->getAttachedToBankUuid()))
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
      for(const auto& master : m_clusters)
      {
        master->assignMaster(master);
      }
      break;
    }

    waitingList = anotherWaitingList;
  }
}

void ClusterEnforcement::enforceClusterRuleOfOne(UNDO::Transaction* transaction)
{
  do
  {
    buildClusterStructure();
  } while(applyRule(transaction));
}

void ClusterEnforcement::sanitizeBankThatWillBeDeleted(UNDO::Transaction* transaction, Bank* bank)
{
  buildClusterStructure();

  if(auto deletedTreeNode = findTreeNode(bank->getUuid()))
  {
    auto left = deletedTreeNode->left;
    auto right = deletedTreeNode->right;
    auto top = deletedTreeNode->top;
    auto bottom = deletedTreeNode->bottom;

    constexpr auto topDir = Bank::AttachmentDirection::top;
    constexpr auto leftDir = Bank::AttachmentDirection::left;
    constexpr auto noneDir = Bank::AttachmentDirection::none;

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
      ruleDelete(transaction, bottom, right, Uuid::none(), noneDir);

      if(bottom)
      {
        bottom->bank->setX(transaction, bank->getX());
        bottom->bank->setY(transaction, bank->getY());
      }
      else if(right)
      {
        right->bank->setX(transaction, bank->getX());
        right->bank->setY(transaction, bank->getY());
      }
    }
  }
}

void ClusterEnforcement::ruleDelete(UNDO::Transaction* transaction, ClusterEnforcement::tTreeNodePtr bottom,
                                    ClusterEnforcement::tTreeNodePtr right, Uuid newMasterUuid,
                                    Bank::AttachmentDirection dir)
{
  if(bottom)
  {
    bottom->bank->setAttachedToBank(transaction, newMasterUuid);
    bottom->bank->setAttachedDirection(transaction, to_string(dir));

    if(right)
    {
      right->bank->setAttachedToBank(transaction, bottom->bank->getUuid());
      right->bank->setAttachedDirection(transaction, to_string(Bank::AttachmentDirection::left));
    }
  }
  else if(right)
  {
    right->bank->setAttachedToBank(transaction, newMasterUuid);
    right->bank->setAttachedDirection(transaction, to_string(dir));
  }
}

bool ClusterEnforcement::applyRule(UNDO::Transaction* transaction)
{
  for(const auto& clusterMaster : m_clusters)
  {
    if(applyRule(transaction, clusterMaster))
    {
      return true;
    }
  }
  return false;
}

bool ClusterEnforcement::applyRule(UNDO::Transaction* transaction, tTreeNodePtr node)
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

void ClusterEnforcement::appendToBottomChildOfNode(UNDO::Transaction* transaction, tTreeNodePtr node,
                                                   tTreeNodePtr target)
{
  while(target->bottom != nullptr)
  {
    target = target->bottom;
  }

  node->bank->setAttachedToBank(transaction, target->bank->getUuid());
  node->bank->setAttachedDirection(transaction, to_string(Bank::AttachmentDirection::top));
}

ClusterEnforcement::tTreeNodePtr ClusterEnforcement::findTreeNode(const Uuid& uuid)
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
  switch(toAttachDirection(myNode->bank->getAttachDirection()))
  {
    case Bank::AttachmentDirection::left:
      myNode->left = masterNode;
      masterNode->right = myNode;
      break;

    case Bank::AttachmentDirection::top:
      myNode->top = masterNode;
      masterNode->bottom = myNode;
      break;

    default:
      break;
  }

  m_uuidToTreeNode[myNode->bank->getUuid()] = myNode;
}

bool inCluster(const ClusterEnforcement::tTreeNodePtr& node)
{
  return node->top || node->bottom || node->left || node->right;
}

std::vector<ClusterEnforcement::tTreeNodePtr>
    prepareNodeVector(const std::map<Uuid, ClusterEnforcement::tTreeNodePtr>& nodeMap)
{
  std::vector<ClusterEnforcement::tTreeNodePtr> ret {};
  ret.reserve(nodeMap.size());
  for(const auto& x : nodeMap)
    ret.push_back(x.second);
  return ret;
}

bool handleBothBanksInCluster(const ClusterEnforcement::tTreeNodePtr& lhs, const ClusterEnforcement::tTreeNodePtr& rhs)
{
  if(lhs->master == rhs->master)
  {
    const auto lhsCol = lhs->getColumn();
    const auto rhsCol = rhs->getColumn();

    if(lhsCol == rhsCol)
      return lhs->getRow() < rhs->getRow();
    else
      return lhsCol < rhsCol;
  }
  else
  {
    return std::stoi(lhs->master->bank->getX()) < std::stoi(rhs->master->bank->getX());
  }
}

std::vector<Bank*> buildVectorFromNodeVector(const std::vector<ClusterEnforcement::tTreeNodePtr>& nodeVec)
{
  auto ret = std::vector<Bank*>();
  for(const auto& x : nodeVec)
    ret.push_back(x->bank);
  return ret;
}

std::vector<Bank*> ClusterEnforcement::sortBanks()
{
  buildClusterStructure();
  auto treeNodes = prepareNodeVector(m_uuidToTreeNode);

  std::sort(treeNodes.begin(), treeNodes.end(), [&](const tTreeNodePtr& lhs, const tTreeNodePtr& rhs) {
    auto lhsInCluster = inCluster(lhs);
    auto rhsInCluster = inCluster(rhs);

    if(lhsInCluster && !rhsInCluster)
      return true;
    else if(!lhsInCluster && rhsInCluster)
      return false;
    else if(lhsInCluster && rhsInCluster)
      return handleBothBanksInCluster(lhs, rhs);
    else
      return std::stoi(lhs->bank->getX()) < std::stoi(rhs->bank->getX());
  });

  return buildVectorFromNodeVector(treeNodes);
}
