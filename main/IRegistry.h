#pragma once


template <typename Key, typename Value>
class IRegistry
{
public:
  virtual bool Register(const Key&, const Value&) = 0;
  virtual bool Unregister(const Key&) = 0;
};
