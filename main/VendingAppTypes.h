#pragma once

#include "ICommand.h"
#include "IPayload.h"

#include <optional>
#include <stdexcept>
#include <regex>

enum class CommandId : uint16_t
{
  SELF_TEST
};

class IdPayload : public IPayload{
    public:
    IdPayload() {};
    virtual ~IdPayload() = default;

    private:
    int64_t Id;

    public:
    const int64_t & GetId() const { return Id; }
    void SetId(const int64_t & value) { this->Id = value; }
};

class SelfTestCommand : public ICommand
{
public:
  virtual uint16_t GetId() const
  {
    return static_cast<uint16_t>(m_Id);
  }

  virtual uint16_t GetOrigin() const
  {
    return m_Origin;
  }

  virtual IPayload& GetPayload() const
  {
    return m_Payload;
  }

  virtual void SetPayload(const IdPayload& p_Payload)
  {
    m_Payload = p_Payload;
  }

private:
  static constexpr CommandId m_Id = CommandId::SELF_TEST;
  uint16_t m_Origin {0};
  IdPayload m_Payload;
};
