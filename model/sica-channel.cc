/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* 
 * Copyright (c) 2010 Universitat Pollitechnica the Catalunya
 * 
 * 
 * Author: Maryam Amiri Nezhad <maryam@ac.upc.edu>
 * 
 * This is doxygen module description, don't include 
 */

#include"ns3/sica-channel.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("SicaChannel");

namespace ns3 {



SicaChannels::SicaChannels()
{
}

SicaChannels::SicaChannel *
SicaChannels::FindChannel(uint32_t chId)
{
  SicaChannels::SicaChannel (*c)=0;
   for (std::vector<SicaChannels::SicaChannel>::iterator i =  m_channel.begin (); i !=  m_channel.end (); ++i)
     {
       if (i->m_cId==chId)
	 c=&(*i);
     }
   return(c);
}
   
void 
SicaChannels::UpdateChannel(uint32_t chId,uint32_t bw,uint32_t bx,uint32_t niNo,Time bxExpTime)
{
  SicaChannel *i= FindChannel(chId);
  if (i)
    {
      i->m_bw=bw;
      //i->m_bx=(i->m_bx+bx)/2;
      i->m_bx=bx;
      i->m_neighborsNo=niNo;
      i->m_bxExpireTime=std::max(bxExpTime+Simulator::Now(),i->m_bxExpireTime);
      NS_LOG_DEBUG("Information for channel with ID "<< chId <<" is updated");
    }
  else 
    {
    i = new SicaChannel(chId,bw,bx,niNo,bxExpTime);
    m_channel.push_back(*i);
    NS_LOG_DEBUG("Information for channel with ID "<< chId <<" is added to channel list");
    }
}

void 
SicaChannels::SetChannelBandwidth(uint32_t chId, uint32_t bw)
{
  SicaChannel *i= FindChannel(chId);
  if (i)
    i->m_bw=bw;
}

uint32_t  
SicaChannels::GetChannelBandwidth(uint32_t chId)
{
  SicaChannel *i= FindChannel(chId);
  if (i)
    return(i->m_bw);
else 
   return (0);
}

void 
SicaChannels::SetChannelExtBandwidth(uint32_t chId, uint32_t bx,Time bxExpTime)
{
 SicaChannel *i= FindChannel(chId);
 if (i){
   if (i->m_bw >= bx ){
     i->m_bx=bx;
     i->m_bxExpireTime=std::max(bxExpTime+Simulator::Now(),i->m_bxExpireTime);
     NS_LOG_DEBUG("External bandwidth is updated "<< bx);
   }
   else 
     NS_LOG_DEBUG("Assign bigger consumption than bandwith");
 }
 
}

uint32_t
SicaChannels::GetChannelExtBandwidth(uint32_t chId)
{
SicaChannel *i= FindChannel(chId);
 if (i){
   if ( (i->m_bxExpireTime-Simulator::Now()).IsPositive())
    return (i->m_bx);
  else
    i->m_bx=0;
      }
 return (0);
}

Time
SicaChannels::GetExtBandwidthExpireTime(uint32_t chId)
{
SicaChannel *i= FindChannel(chId);
 if (i)
   return (i->m_bxExpireTime-Simulator::Now());
 return (Seconds(0));
}

void 
SicaChannels::SetChannelNeighbors(uint32_t chId,uint32_t niNo)
{
SicaChannel *i= FindChannel(chId);
if (i)
  i->m_neighborsNo=niNo;
}

uint32_t 
SicaChannels::GetChannelNeighbors(uint32_t chId)
{
SicaChannel *i= FindChannel(chId);
 if (i)
   return (i->m_neighborsNo);
 else 
 return (0);
}

void 
SicaChannels::IncChannelNeighbors(uint32_t chId)
{
SicaChannel *i= FindChannel(chId);
 if (i)
   i->m_neighborsNo++;
}

void 
SicaChannels::DecChannelNeighbors(uint32_t chId)
{
SicaChannel *i= FindChannel(chId);
 if (i)
   i->m_neighborsNo--;
}


void 
SicaChannels::SetChannelWeight(uint32_t chId,double w)
{
  SicaChannel *i= FindChannel(chId);
 if (i)
   i->m_weight=w;
 
}

 void 
SicaChannels::SetChannelSenseFlag(uint32_t chId)
 {
   SicaChannel *i= FindChannel(chId);
 if (i)
   i->m_senseFalg=true;
 }

void 
SicaChannels::ResetChannelSenseFlag(uint32_t chId)
{
   SicaChannel *i= FindChannel(chId);
 if (i)
   i->m_senseFalg=false;
 }

bool 
SicaChannels::IsBeingSensed(uint32_t chId)
{
 SicaChannel *i= FindChannel(chId);
 if (i)
   return i->m_senseFalg;
 return false;
}




void 
SicaChannels::PrintChannel(std::ostream &os)
{
  if(m_channel.empty())
    {
      os << "\n PrintChannel: There is no information in channel list.\n";
    return;
    }
  os << "\n PrintChannel: Channel list informations: ";
for (std::vector<SicaChannels::SicaChannel>::iterator i =  m_channel.begin (); i !=  m_channel.end (); ++i)
  {
    os << "\nChannel ID : " << i->m_cId;
    os << "\n-- Total Bandwidth (mbps): " << i->m_bw ;
    if ( (i->m_bxExpireTime-Simulator::Now()).IsPositive()){
      os << "\n-- External  Bandwidth Consumption  (mbps): " << i->m_bx ;
      os << "\n-- The expiration time for bx is :" << i->m_bxExpireTime;
    }
    else 
      os << "\n-- External  Bandwidth Consumption  (mbps): " << 0;
    os << "\n-- Number of neighboring radios on channel : " << i->m_neighborsNo ;
    os << "\n-- Channel Weight : " << i->m_weight << "\n";
  } 
}


double 
SicaChannels::GetChannelWeight(uint32_t chId)
{
 SicaChannel *i= FindChannel(chId);
if (i)
  return (i->m_weight);
 else 
 return (0);
}

double 
SicaChannels::CalculateCLCPF(uint32_t ccc)
{
  double clcpf=m_channel.begin()->m_weight;
  for (std::vector<SicaChannels::SicaChannel>::iterator i =  m_channel.begin (); i !=  m_channel.end (); ++i)
    if (i->m_cId != ccc && clcpf > i->m_weight)
      clcpf=i->m_weight;
  return clcpf;
}

int 
SicaChannels::FindMaxWeightChannel(uint32_t ccc)
{
  double maxW=m_channel.begin ()->m_weight;
  int chId = m_channel.begin ()->m_cId;
  for (std::vector<SicaChannels::SicaChannel>::iterator i =  m_channel.begin (); i !=  m_channel.end (); ++i)
    if (i->m_cId != ccc && maxW < i->m_weight){
     maxW =i->m_weight;
     chId=i->m_cId;
    }
  return chId;
}


}/*namespace ns3 */
