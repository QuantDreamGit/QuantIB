#pragma once

#include "Contract.h"
#include "DefaultEWrapper.h"

#include "quantib/core/tags.hpp"
#include "quantib/network/connection.hpp"

#include <iostream>
#include <memory>

#include "quantib/core/account.hpp"

// This class is used to override default wrapper functions
class ResponseWrapper : public DefaultEWrapper {
public:
  explicit ResponseWrapper(std::shared_ptr<BlockingHub> hub, std::shared_ptr<ObjectHub> obj) : hub_(hub), obj_(obj) {}
  ~ResponseWrapper() override = default;
  // Deprecated
  // void connectAck() override { hub_->send(typeid(connectTag), true); }

  void nextValidId(int orderId) override {
    if (hub_->containsKey(typeid(connectTag))) {
      hub_->send<connectTag>(orderId);
    } else
      hub_->send<nextValidIdTag>(orderId);
  }

  void position(std::string account, Contract contract, Decimal pos,
                double avgCost) {
    std::cout << account << std::endl;
  }

  void accountSummaryEnd(int reqId) override {
    std::cout << "End of summary update!" << std::endl;
  }

  void accountSummary(int reqId, const std::string &account, const std::string &tag, const std::string &value,
                      const std::string &currency);

private:
  std::shared_ptr<BlockingHub> hub_;
  std::shared_ptr<ObjectHub> obj_;
};
