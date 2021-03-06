#ifndef _TALMECH_NODES_AGENT_NODE_H_
#define _TALMECH_NODES_AGENT_NODE_H_

#include "../agent.h"
#include "ros_node.h"
#include <talmech_msgs/Task.h>

namespace talmech
{
namespace nodes
{
class AgentNode : public ROSNode
{
public:
  typedef boost::shared_ptr<AgentNode> Ptr;
  typedef boost::shared_ptr<const AgentNode> ConstPtr;
  AgentNode(const ros::NodeHandlePtr& nh,
            const ros::Rate& rate = ros::Rate(20));
  virtual ~AgentNode();
protected:
  talmech::AgentPtr agent_;
  virtual void readParameters();
private:
  ros::Subscriber task_sub_;
  ros::Subscriber pose_sub_;
  virtual bool isSettedUp() { return agent_.get(); }
  virtual void controlLoop() { agent_->process(); }
  void poseCallback(const geometry_msgs::Pose& pose);
};
typedef AgentNode::Ptr AgentNodePtr;
typedef AgentNode::ConstPtr AgentNodeConstPtr;
}
}

#endif // _TALMECH_NODES_AGENT_NODE_H_
