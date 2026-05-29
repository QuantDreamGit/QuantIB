#pragma once

#include "Contract.h"
#include "DefaultEWrapper.h"

#include "quantib/core/tags.hpp"
#include "quantib/network/connection.hpp"

#include <iostream>
#include <memory>

// This class is used to override default wrapper functions
class ResponseWrapper : public DefaultEWrapper {
public:
  explicit ResponseWrapper(std::shared_ptr<BlockingHub> hub) : hub_(hub) {}
  // Deprecated
  // void connectAck() override { hub_->send(typeid(connectTag), true); }

  void nextValidId(int orderId) override {
    if (hub_->containsKey(typeid(connectTag))) {
      hub_->send(typeid(connectTag), orderId);
    } else
      hub_->send(typeid(nextValidIdTag), orderId);
  }

  void position(std::string account, Contract contract, Decimal pos,
                double avgCost) {
    std::cout << account << std::endl;
  }

private:
  std::shared_ptr<BlockingHub> hub_;
};
