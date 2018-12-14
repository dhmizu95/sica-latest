/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* 
 * Copyright (c) 2010 Universitat Pollitechnica the Catalunya
 * 
 * 
 * Author: Maryam Amiri Nezhad <maryam@ac.upc.edu>
 * 
 * This is doxygen module description, don't include 
 */

#include "ns3/sica-helper.h"

namespace ns3 {

SicaContainer::SicaContainer ()
{ }

SicaContainer::Iterator 
SicaContainer::Begin (void) const
{
  return m_sicaAgents.begin();
}

SicaContainer::Iterator 
SicaContainer::End (void) const
{
return m_sicaAgents.end();
}

uint32_t 
SicaContainer::GetN (void) const
{
  return m_sicaAgents.size();
}

Ptr<Sica> 
SicaContainer::Get (uint32_t i) const
{
  return m_sicaAgents[i];
}

void 
SicaContainer::Add (Ptr<Sica> s)
{
  m_sicaAgents.push_back(s);
}



SicaHelper::SicaHelper()
{
m_agentFactory.SetTypeId ("ns3::Sica");
}

Ptr<Sica>
SicaHelper::Create(Ptr<Node> node,ChannelEmuContainer channelsEmu )const
{
  Ptr<Sica> sica= m_agentFactory.Create<Sica>();
  sica->SetChannelsEmulationObject(channelsEmu);
  node->AggregateObject(sica);
  return sica;
}

void 
SicaHelper::Set (std::string name, const AttributeValue &value)
{
m_agentFactory.Set (name, value);
}

SicaContainer
SicaHelper::Install (NodeContainer c,ChannelEmuContainer channelsEmu ) const
{
  SicaContainer sicas;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      sicas.Add(Create(*i,channelsEmu));
    }
  // initialaize sicas
  for (uint32_t i=0; i< sicas.GetN() ; i++)
    {
      Simulator::Schedule(MilliSeconds(i),&Sica::Initialize,sicas.Get(i));
    }
 return sicas;
}


}/*namespace ns3 */

