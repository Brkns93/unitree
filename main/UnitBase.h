#pragma once


#include "IComponent.h"
#include "ISubscriber.h"
#include "IPublisher.h"
#include "IdItemRegistry.h"
// #include <thread>
#include <functional>
#include <future>
#include <map>

class UnitBase : public IComponent, public IPublisher
{
public:
  using ICommand = IMessage;
  using IRequest = IMessage;
  using IResponse = IMessage;
  using IEvent = IMessage;
  using CommandTask = std::function<void(const ICommand&)>;
  using RequestTask = std::function<IResponse&(const IRequest&)>;
  using EventTask = std::function<void(const IEvent&)>;

  virtual void ExecuteCommand(const ICommand& p_Cmd) {
    auto task = m_CommandRegistry.GetItemById(p_Cmd.GetId());
    if(task)
    {
      std::future<void> future = std::async(std::launch::async, task, std::move(p_Cmd));
    }
  }

  virtual IResponse &ExecuteRequest(const IRequest& p_Req) {
    auto task = m_RequestRegistry.GetItemById(p_Req.GetId());
    std::future<IResponse&> future = std::async(std::launch::async, task, std::move(p_Req));
    return future.get();
  }

  virtual IdItemRegistry<CommandTask> GetCommandRegistry() {
    return m_CommandRegistry;
  }

  virtual IdItemRegistry<RequestTask> GetRequestRegistry() {
    return m_RequestRegistry;
  }

  virtual IdItemRegistry<UnitBase&> GetSubUnitRegistry() {
    return m_SubUnitRegistry;
  }

protected:
  virtual bool HandleMessage(const IMessage&) {
    return true;
  }

  virtual bool PublishMessage(const IMessage&) {
    return true;
  }

  virtual bool RegisterSubscriber(const ISubscriber&) {
    return true;
  }


private:
  IdItemRegistry<CommandTask> m_CommandRegistry;
  IdItemRegistry<RequestTask> m_RequestRegistry;
  IdItemRegistry<UnitBase&> m_SubUnitRegistry;
};
