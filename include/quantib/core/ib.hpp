#pragma once

#include "EClientSocket.h"
#include "EReader.h"
#include "EReaderOSSignal.h"

#include "quantib/core/base_wrapper.hpp"
#include "quantib/core/tags.hpp"

#include <memory>
#include <optional>
#include <thread>

class IB {
public:
  // Default Constructor
  IB() {
    hub_ = std::make_shared<BlockingHub>();
    wrapper_ = std::make_unique<ResponseWrapper>(hub_);
    signal_ = std::make_unique<EReaderOSSignal>(2000);
    client_ = std::make_unique<EClientSocket>(wrapper_.get(), signal_.get());
  };

  ~IB() {
    isRunning_ = false;
    if (message_thread_.joinable())
      message_thread_.join();
    if (client_ && client_->isConnected())
      client_->eDisconnect();
  }

  /* Network */
  inline std::optional<int> connect() {
    hub_->insertKey(typeid(nextValidIdTag));
    client_->asyncEConnect(false);
    bool ok = client_->eConnect("127.0.0.1", 7497, 0, false);

    if (ok) {
      // Start EReader
      reader_ = std::make_unique<EReader>(client_.get(), signal_.get());
      reader_->start();
      isRunning_ = true;
      // Then start the message thread
      message_thread_ = std::thread([this] {
        while (isRunning_) {
          signal_->waitForSignal();
          reader_->processMsgs();
        }
      });

      nextId_ = hub_->wait_for<int>(typeid(connectTag), [&]() {}).value_or(-1);
      return nextId_;
    }

    return std::nullopt;
  }

  std::optional<int> getNextValidId();
  void reqNextValidId();

protected:
  std::unique_ptr<ResponseWrapper> wrapper_;
  std::unique_ptr<EReaderOSSignal> signal_;
  std::unique_ptr<EClientSocket> client_;
  std::unique_ptr<EReader> reader_;
  std::thread message_thread_;
  std::atomic_bool isRunning_{false};
  // Mode is used to indicate whether commands should be sync or async
  std::shared_ptr<BlockingHub> hub_;

  int nextId_;
};
