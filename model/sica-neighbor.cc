/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* 
 * Copyright (c) 2010 Universitat Pollitechnica the Catalunya
 * 
 * 
 * Author: Maryam Amiri Nezhad <maryam@ac.upc.edu>
 * 
 * This is doxygen module description, don't include 
 */

#include "ns3/sica-neighbor.h"
#include "ns3/log.h"
#include <algorithm>

NS_LOG_COMPONENT_DEFINE ("SicaNeighbors");
namespace ns3 {


SicaNeighbors::SicaNeighbors():
m_ni(0)
{
}

SicaNeighbors::SicaNeighbor *
SicaNeighbors::FindNeighbor(uint32_t id)
{
  SicaNeighbors::SicaNeighbor (*ni)=0;
  if(m_neighbor.empty())
    return (ni);
    for (std::vector<SicaNeighbor>::iterator i = m_neighbor.begin (); i != m_neighbor.end (); ++i)
      {
        if (i->m_id==id)
          ni=&(*i);
      }
    return (ni);
}


int32_t 
SicaNeighbors::FindDeviceAddr(Address addr)
{
  for (std::vector<SicaNeighbor>::iterator i = m_neighbor.begin (); i != m_neighbor.end (); ++i){
  if (i->m_rAddr==addr || i->m_tAddr==addr)
    return i->m_id;
  }
    return -1;
}

Time
SicaNeighbors::GetNiUpdateTime(uint32_t id)
{
  SicaNeighbor *i =FindNeighbor(id);
  if (i)
    return (i->m_updateTime);
  else 
    return (Seconds(0));
}

void  
SicaNeighbors::SetNiUpdateTime (uint32_t id, Time uTime)
{
  SicaNeighbor *i =FindNeighbor(id);
  if (i)
    i->m_updateTime=uTime;
}


Time
SicaNeighbors::GetNiSwitchTime(uint32_t id)
{
  SicaNeighbor *i =FindNeighbor(id);
  if (i)
    return (i->m_switchTime-Simulator::Now());
  else 
    return (Seconds(0));
}

void  
SicaNeighbors::SetNiSwitchTime (uint32_t id, Time sTime)
{
SicaNeighbor *i =FindNeighbor(id);
 if (i)
   i->m_switchTime=sTime+Simulator::Now();

}

uint32_t 
SicaNeighbors::GetNiHops(uint32_t id)
{
SicaNeighbor *i =FindNeighbor(id);
  if (i)
    return (i->m_hopCount);
  else 
    return (0);
}

void 
SicaNeighbors::SetNiHops(uint32_t id,uint32_t hops )
{
SicaNeighbor *i =FindNeighbor(id);
  if (i)
    i->m_hopCount=hops;
}

bool 
SicaNeighbors::Update (uint32_t id, uint32_t hops, uint32_t radio,uint32_t channel,Address rAddr,Address tAddr, Time updateTime, Time switchTime,uint32_t newChannel )
{
  SicaNeighbor *i =FindNeighbor(id);  if (i)
    {
      if (i->m_updateTime > updateTime)
        return false; // If the data in table is more update than the received once
      if ( hops== 1){ /// Update channel swtching information if you get it from neighbor directly
        i->m_switchTime=switchTime+Simulator::Now();
        i->m_neighborNewChannel=newChannel;   
        i->m_neighborRadio=radio;
        i->m_neighborChannel=channel;
        i->m_rAddr=rAddr;
        if (rAddr!=tAddr)
          i->m_tAddr=tAddr;
      }
      else if (hops>1 && i->m_hopCount > 1)
        i->m_neighborChannel=channel;
      i->m_hopCount=std::min(i->m_hopCount,hops);
      i->m_updateTime=updateTime;
      i->close=false;
      NS_LOG_DEBUG(Simulator::Now().GetSeconds()<<"-->"<< "Neighbor information is updated Node ID: "<< id<< "\nUpdate  time for entry is set to: "<< i->m_updateTime);
      return true;
    }
  else 
    {
      SicaNeighbor j((uint32_t)id,(uint32_t)hops,(uint32_t)radio,(uint32_t)channel,(Address) rAddr,(Address) tAddr, (Time)updateTime ,(Time)switchTime+Simulator::Now (),(uint32_t)newChannel);
      m_neighbor.push_back(j);
       NS_LOG_DEBUG(Simulator::Now().GetSeconds()<<"-->"<<"Neighbor information is added node ID: "<< id << "\nUpdate  time for entry is set to: "<< j.m_updateTime); 
       m_ni++;
      return true; 
    }
}

void
SicaNeighbors::RmvNeighbor(uint32_t id)
{
   for (std::vector<SicaNeighbor>::iterator i = m_neighbor.begin (); i != m_neighbor.end (); ++i)
     {
       if (i->m_id==id)
         {
           m_neighbor.erase(i);
           m_ni--;
         }
     }
}
   
int32_t 
SicaNeighbors::GetNiChannel(uint32_t id)
{
SicaNeighbor *i =FindNeighbor(id);
 if (i)
   return(i->m_neighborChannel);
 else 
   return (-1);
}


void
SicaNeighbors::SetNiChannel(uint32_t id,uint32_t nCh)
{
SicaNeighbor *i =FindNeighbor(id);
 if (i)
   i->m_neighborChannel=nCh;
}

int32_t 
SicaNeighbors::GetNiNewChannel(uint32_t id)
{
SicaNeighbor *i =FindNeighbor(id);
 if (i)
   return(i->m_neighborNewChannel);
 else 
   return (-1);
}

void 
SicaNeighbors::SetNiNewChannel(uint32_t id,uint32_t nCh)
{
SicaNeighbor *i =FindNeighbor(id);
 if (i)
   i->m_neighborNewChannel=nCh;
}

Address 
SicaNeighbors::GetNiRAddress(uint32_t id)
{
  SicaNeighbor *i =FindNeighbor(id);
  if (i)
    return i->m_rAddr;
  else 
    return (Address());
 
}

 void  
SicaNeighbors::SetNiRAddress(uint32_t id,Address rAddr)
 {
   SicaNeighbor *i =FindNeighbor(id);
  if (i)
    i->m_rAddr=rAddr;
 }

Address 
SicaNeighbors::GetNiTAddress(uint32_t id)
{
  SicaNeighbor *i =FindNeighbor(id);
  if (i)
    return i->m_tAddr;
  else 
    return (Address());
 
}

 void  
SicaNeighbors::SetNiTAddress(uint32_t id,Address tAddr)
 {
   SicaNeighbor *i =FindNeighbor(id);
  if (i)
    i->m_tAddr=tAddr;
 }


Address 
SicaNeighbors::GetNiCAddress(uint32_t id)
{
  SicaNeighbor *i =FindNeighbor(id);
  if (i)
    return i->m_cAddr;
  else 
    return (Address());
 
}

 void  
SicaNeighbors::SetNiCAddress(uint32_t id,Address cAddr)
 {
   SicaNeighbor *i =FindNeighbor(id);
  if (i)
    i->m_cAddr=cAddr;
 }


bool 
SicaNeighbors::IsDirectNeighbor(uint32_t id)
{
SicaNeighbor *i =FindNeighbor(id);
 if (i)
   if (i->m_hopCount==1)
     return(true);
 
 return (false);
}

bool
SicaNeighbors::IsDirectNeighborByIndex(uint32_t i)
{
  if (i > 0 &&i <= m_neighbor.size())
    if  (m_neighbor[i-1].m_hopCount==1)
      return(true);
 return (false);   
}


uint32_t 
SicaNeighbors::GetNiOnChannel(uint32_t channel)
{
  uint32_t chCount=0;
for (std::vector<SicaNeighbor>::iterator i = m_neighbor.begin (); i != m_neighbor.end (); ++i)
  {
    if (i->m_neighborChannel==channel)   
	 chCount++;
  }
 return (chCount);
}

uint32_t 
SicaNeighbors::GetNiOnChannelByHops(uint32_t channel, uint32_t fromHopC , uint32_t toHopC)
{
  uint32_t chCount=0;
for (std::vector<SicaNeighbor>::iterator i = m_neighbor.begin (); i != m_neighbor.end (); ++i)
  {
    if (i->m_neighborChannel==channel && i->m_hopCount>=fromHopC && i->m_hopCount<=toHopC)   
	 chCount++;
  }
 return (chCount);
}



int32_t 
SicaNeighbors::GetNeighborIdByIndex(uint32_t i)
 {
   if (i >0 && i <= m_neighbor.size() )
    return (m_neighbor[i-1].m_id);
  else 
    return -1;
 }

int32_t 
SicaNeighbors::GetNiChannelByIndex(uint32_t i)
{
if (i >0 && i <= m_neighbor.size() )
    return (m_neighbor[i-1].m_neighborChannel);
  else 
    return -1;
}

void 
SicaNeighbors::PrintNeighborTable(std::ostream &os)
{
  if(m_ni > 0)
    {
      os << "\n Print-Neighbor-Table: Printing neighbor informations\n"<< "----------------------------------------------\n";
      for (std::vector<SicaNeighbor>::iterator i = m_neighbor.begin (); i != m_neighbor.end (); ++i)
        {
            os << "\nNeighbor ID: "<< i->m_id ;
            os <<"\n-- Number of radios :  " << i->m_neighborRadio;
            os <<"\n-- Channel of receiving radio:  "<< i->m_neighborChannel;
            os <<"\n-- Address  of receiving radio:  "<< i->m_rAddr;
            os <<"\n-- Address  of the transmitting  radio:  "<< i->m_tAddr;
            os <<"\n-- Distance in hop counts:  "<<i->m_hopCount;
            os << "\n-- Update time :  "<< i->m_updateTime.GetSeconds();
            if (i->m_neighborNewChannel!=i->m_neighborChannel )
              {
                os << "\n-- Node will switch to channel "<< i->m_neighborNewChannel << "in " << (i->m_switchTime-Simulator::Now()).GetMilliSeconds() << "ms.";
              }
        }
    }
  else 
    {
      os << "\n Print-Neighbor-Table: There is no information in neighbor table.\n";
    }
  os <<"\n";
}

 uint32_t 
SicaNeighbors::GetNiNobyHops(uint32_t fromHopC , uint32_t toHopC)
 {
 uint32_t niCount=0;
for (std::vector<SicaNeighbor>::iterator i = m_neighbor.begin (); i != m_neighbor.end (); ++i)
  {
    if (i->m_hopCount >=fromHopC && i->m_hopCount<=toHopC)   
	 niCount++;
  }
 return (niCount);
 }


void 
SicaNeighbors::RmvExpiredNi(Time maxExpireTime){
  if (m_neighbor.empty())
    return;
  std::vector<std::vector<SicaNeighbor>::iterator> eraseNi;
for (std::vector<SicaNeighbor>::iterator i = m_neighbor.begin (); i != m_neighbor.end (); ++i)
     {
       if ((Simulator::Now()-(i->m_updateTime)) > maxExpireTime){
         eraseNi.push_back(i);
       }
     }
 if (!eraseNi.empty())
 for (uint32_t i=0; i< eraseNi.size(); i++)
   {
     m_ni--;
     m_neighbor.erase(eraseNi[i]);
     NS_LOG_DEBUG("RmvExpiredNi: One neighbor is erased "<< eraseNi[i]->m_id<<" \n--Updated at "<<eraseNi[i]->m_updateTime.GetSeconds() << "\n--Expires after "<< maxExpireTime.GetSeconds() <<"\n--Sim time " << Simulator::Now().GetSeconds());
   }
}





}/*namespace ns3 */
