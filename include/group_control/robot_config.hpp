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
};

}  // namespace group_control

#endif  // GROUP_CONTROL_ROBOT_CONFIG_HPP_
