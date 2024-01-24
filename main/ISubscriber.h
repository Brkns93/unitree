#pragma once


#include "IMessage.h"

class ISubscriber
{
protected:
  virtual bool HandleMessage(const IMessage&) = 0;
};
