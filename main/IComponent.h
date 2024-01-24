#pragma once


#include "IMessage.h"

class IComponent
{
public:
  using ICommand = IMessage;
  using IRequest = IMessage;
  using IResponse = IMessage;
  using IEvent = IMessage;

  virtual void ExecuteCommand(const ICommand&) = 0;
  virtual IResponse &ExecuteRequest(const IRequest&) = 0;
  virtual bool RegisterCommand(const ICommand&) = 0;
  virtual bool RegisterRequest(const IRequest&) = 0;
};
