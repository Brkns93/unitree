#pragma once

#include <cstdint>
#include "IPayload.h"

class IMessage
{
public:
  virtual uint16_t GetId() const = 0;
  virtual uint16_t GetOrigin() const = 0;
  virtual IPayload& GetPayload() const = 0;
  virtual void SetPayload(const IPayload&) = 0;
};
