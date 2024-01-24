#pragma once


#include "UnitBase.h"
#include "VendingAppTypes.h"

using ICommand = IMessage;
using CommandTask = std::function<void(const ICommand&)>;

class MdbUnit : public UnitBase
{
public:
  MdbUnit()
  {
    GetCommandRegistry().Register(static_cast<uint16_t>(CommandId::SELF_TEST), [this] (const SelfTestCommand& cmd) { SelfTestCommandHandler(cmd); });
  };

private:
  void SelfTestCommandHandler(const SelfTestCommand& command);
};
