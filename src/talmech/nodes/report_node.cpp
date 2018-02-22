#include "talmech/nodes/report_node.h"
#include "talmech/auction/auctioneer_agent.h"
#include "talmech/auction/auctioneer_robot.h"
#include "talmech/auction/bidder_agent.h"
#include "talmech/auction/bidder_robot.h"
#include "talmech/discrete_skill.h"
#include "talmech/continuous_skill.h"
#include "talmech/utility/basic/distance_utility.h"
#include "talmech/utility/basic/skill_utility.h"

namespace talmech
{
namespace nodes
{
ReportNode::ReportNode(const ros::NodeHandlePtr& nh, const ros::Rate& rate)
    : ROSNode::ROSNode(nh, rate)
{
}

void ReportNode::readParameters()
{
  ROS_INFO("Reading parameters...");
  ros::NodeHandle pnh("~");
  std::string agent_type;
  pnh.param("type", agent_type, std::string("robot"));
  std::string id;
  pnh.param("id", id, std::string(""));
  if (id.empty())
  {
    throw Exception("The agent id must not be null.");
  }
  std::string role_type;
  pnh.param("role", role_type, std::string("bidder"));
  if (agent_type == "agent")
  {
    if (role_type == "auctioneer")
    {
      agent_.reset(new auction::AuctioneerAgent(nh_, id));
      ROS_INFO("   initializing agent as an AuctioneerAgent.");
    }
    else if (role_type == "bidder")
    {
      agent_.reset(new auction::BidderAgent(nh_, id));
      ROS_INFO("   initializing agent as an BidderAgent.");
    }
    else
    {
      throw Exception("Unknown role type.");
    }
  }
  else if (agent_type == "robot")
  {
    if (role_type == "auctioneer")
    {
      agent_.reset(new auction::AuctioneerRobot(nh_, id));
      ROS_INFO("   initializing agent as an AuctioneerRobot.");
    }
    else if (role_type == "bidder")
    {
      agent_.reset(new auction::BidderRobot(nh_, id));
      ROS_INFO("   initializing agent as an BidderRobot.");
    }
    else
    {
      throw Exception("Unknown role type.");
    }
  }
  else
  {
    throw Exception("Unknown agent type.");
  }
  if (role_type == "bidder")
  {
    std::string utility;
    pnh.param("utility/expression", utility, std::string(""));
    if (!utility.empty())
    {
      agent_->setUtility(utility);
      ROS_INFO_STREAM("   ~/utility/expression: " << utility);
    }
  }
  pnh = ros::NodeHandle("~/skills");
  int size;
  pnh.param("size", size, 0);
  ROS_INFO_STREAM("   ~/skills/size: " << size);
  for (int i(0); i < size; i++)
  {
    std::stringstream ss;
    ss << "skill" << i << "/";
    std::string resource_id;
    pnh.param(ss.str() + "resource", resource_id, std::string(""));
    ROS_INFO_STREAM("   ~/skills/skill" << i << "/resource: " << resource_id);
    ResourcePtr resource(new Resource(resource_id));
    int type;
    pnh.param(ss.str() + "type", type, 0);
    ROS_INFO_STREAM("   ~/skills/skill" << i << "/type: " << type);
    SkillPtr skill;
    if (type == 0)
    {
      skill.reset(new Skill(resource));
    }
    else if (type == 1)
    {
      int level;
      pnh.param(ss.str() + "level", level, 0);
      ROS_INFO_STREAM("   ~/skills/skill" << i << "/level: " << level);
      skill.reset(new DiscreteSkill(resource, level));
    }
    else if (type == 2)
    {
      double level;
      pnh.param(ss.str() + "level", level, 0.0);
      ROS_INFO_STREAM("   ~/skills/skill" << i << "/level: " << level);
      skill.reset(new ContinuousSkill(resource, level));
    }
    else
    {
      throw Exception("Invalid type of skill.");
    }
    agent_->addSkill(skill);
  }
  utility::UtilityComponentPtr component(
      agent_->getUtilityComponent("SkillUtility"));
  if (component)
  {
    utility::basic::SkillUtilityPtr skill_component(
        boost::dynamic_pointer_cast<utility::basic::SkillUtility>(
            component));
    if (skill_component)
    {
      ROS_INFO("Initializing the SkillUtility component...");
      std::list<double> correction_factors;
      int size;
      pnh.param("size", size, 0);
      for (int i(0); i < size; i++)
      {
        std::stringstream ss;
        ss << "skill" << i << "/";
        double correction_factor;
        pnh.param(ss.str() + "utility/skill", correction_factor, 1.0);
        ROS_INFO_STREAM("   ~/skills/skill"
                        << i << "/utility/skill: " << correction_factor);
        correction_factors.push_back(correction_factor);
      }
      skill_component->init(*agent_, correction_factors);
    }
  }
  if (agent_type == "robot")
  {
    component = agent_->getUtilityComponent("DistanceUtility");
    if (component)
    {
      utility::basic::DistanceUtilityPtr distance_component(
          boost::dynamic_pointer_cast<utility::basic::DistanceUtility>(
              component));
      if (distance_component)
      {
        pnh = ros::NodeHandle("~");
        ROS_INFO("Initializing the DistanceUtility component...");
        double correction_factor;
        pnh.param("utility/distance", correction_factor, 1.0);
        ROS_INFO_STREAM("   ~/utility/distance: " << correction_factor);
        distance_component->init(
            *boost::dynamic_pointer_cast<Robot>(agent_),
            correction_factor);
      }
    }
  }
}
}
}