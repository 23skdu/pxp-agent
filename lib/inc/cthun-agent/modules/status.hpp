#ifndef SRC_MODULES_STATUS_H_
#define SRC_MODULES_STATUS_H_

#include <cthun-agent/module.hpp>

namespace CthunAgent {
namespace Modules {

class Status : public CthunAgent::Module {
  public:
    Status();
  private:
    ActionOutcome callAction(const ActionRequest& request);
};

}  // namespace Modules
}  // namespace CthunAgent

#endif  // SRC_MODULES_STATUS_H_