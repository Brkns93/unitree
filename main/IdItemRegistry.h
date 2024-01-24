#pragma once


#include "IRegistry.h"
#include <map>

template <typename T>
class IdItemRegistry : public IRegistry<uint16_t, T>
{
public:
  virtual bool Register(const uint16_t& p_Id, const T& p_Item) override
  {
    m_MessageMap[p_Id] = p_Item;
    
    return true;
  }
  
  virtual bool Unregister(const uint16_t& p_Id) override
  {
    auto iteratorToRemove = m_MessageMap.find(p_Id);
    if (iteratorToRemove != m_MessageMap.end()) {
        m_MessageMap.erase(iteratorToRemove);
        return true;
    }
    
    return false;
  }
  
  virtual T& GetItemById(const uint16_t p_Id)
  {
    return m_MessageMap[p_Id];
  }

private:
  std::map<uint16_t, T&> m_MessageMap;
};
