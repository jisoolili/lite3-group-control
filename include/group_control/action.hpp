#ifndef GROUP_CONTROL_ACTION_HPP_
#define GROUP_CONTROL_ACTION_HPP_

#include <string>

#include "group_control/robot.hpp"

namespace group_control {

class Action {
 public:
  virtual ~Action() = default;
  virtual const std::string& name() const = 0;
  virtual bool execute(Robot& robot) = 0;
};

class Lite3StandDemoDryRunAction : public Action {
 public:
  Lite3StandDemoDryRunAction(std::string name, std::string lite3_action);

  const std::string& name() const override;
  const std::string& lite3Action() const;
  const std::string& lastDryRunCommand() const;
  bool execute(Robot& robot) override;

 private:
  std::string name_;
  std::string lite3_action_;
  std::string last_dry_run_command_;
};

}  // namespace group_control

#endif  // GROUP_CONTROL_ACTION_HPP_
