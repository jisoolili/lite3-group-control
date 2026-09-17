#ifndef GROUP_CONTROL_ROBOT_CONFIG_HPP_
#define GROUP_CONTROL_ROBOT_CONFIG_HPP_

#include <cstdint>
#include <string>

namespace group_control {

struct RobotConfig {
  std::string robot_id;
  std::string ip;
  std::uint16_t target_port = 0;
  std::uint16_t local_state_port = 0;
  std::string transport = "udp";
  std::string network = "unknown";
  std::string action_interface = "transfer/lite3_stand_demo";
  bool action_verified = false;
};

}  // namespace group_control

#endif  // GROUP_CONTROL_ROBOT_CONFIG_HPP_
