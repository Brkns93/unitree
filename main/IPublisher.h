#pragma once


#include "IMessage.h"
#include "ISubscriber.h"

class IPublisher
{
protected:
  virtual bool PublishMessage(const IMessage&) = 0;
  virtual bool RegisterSubscriber(const ISubscriber&) = 0;
};
