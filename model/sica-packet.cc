/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* 
 * Copyright (c) 2010 Universitat Pollitechnica the Catalunya
 * 
 * 
 * Author: Maryam Amiri Nezhad <maryam@ac.upc.edu>
 * 
 * This is doxygen module description, don't include 
 */

#include "ns3/sica-packet.h"
#include "ns3/log.h"
#include "ns3/address-utils.h"
#include "ns3/packet.h"

NS_LOG_COMPONENT_DEFINE ("SicaHelloHeader");

namespace ns3
{

  SicaHelloHeader::SicaHelloHeader(uint32_t sqNo,uint32_t origin,
                                   Time originTime,uint8_t radios,uint8_t rCh,
                                   uint8_t extBw,uint8_t rNewCh,
                                   Address rAddr,Time rSwitchTime,
                                   Time rSenseTime):
  m_seqNo(sqNo),
  m_origin(origin),
  m_originTime(originTime.GetMilliSeconds()),
  m_radios(radios),
  m_rCh(rCh),
  m_extBw(extBw),
  m_rNewCh(rNewCh),
  m_rAddr(rAddr),
  m_rSwitchTime(rSwitchTime.GetMilliSeconds()),
  m_rSenseTime( rSenseTime.GetMilliSeconds()),
  m_ni(0),
  m_clcpf(0),
  m_ttl(1)
{}
  
NS_OBJECT_ENSURE_REGISTERED (SicaHelloHeader);


TypeId 
SicaHelloHeader::GetTypeId ()
{
  static TypeId tid = TypeId("ns3::SicaHelloHeader")
   .SetParent<Header> ()
  .AddConstructor<SicaHelloHeader> ()
      ;
  return tid;
}


TypeId
SicaHelloHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}


uint32_t 
SicaHelloHeader::GetSerializedSize () const
{
  return (37+m_rAddr.GetLength()+5*GetNiNo());
}




void 
SicaHelloHeader::Serialize (Buffer::Iterator i) const
{
  uint32_t deciPart= static_cast <uint32_t>(m_clcpf);
  double fPart=m_clcpf-deciPart;
  uint32_t fIntPart=static_cast<uint32_t>(fPart*1000);
  i.WriteU32(m_seqNo);
  i.WriteU32(m_origin);
  i.WriteHtonU32 (m_originTime);
  i.WriteU8(m_radios);
  i.WriteU8(m_rCh);
  i.WriteU8(m_extBw);
  i.WriteU8(m_rNewCh);
  WriteTo(i,m_rAddr);
  i.WriteHtonU32 (m_rSwitchTime);
  i.WriteHtonU32 (m_rSenseTime);
  i.WriteU8(m_ni);
  i.WriteU32(deciPart);
  i.WriteU32(fIntPart);
  i.WriteU32(m_ttl);
  if (m_ni > 0){
    std::map<uint32_t, uint8_t>::const_iterator j;
    for (j = m_neighborRChannel.begin (); j != m_neighborRChannel.end (); ++j)
      {
        i.WriteU32((*j).first);
        i.WriteU8 ((*j).second);
      }
  }
}

uint32_t 
SicaHelloHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_seqNo=i.ReadU32 ();
  m_origin=i.ReadU32 ();
  m_originTime=i.ReadNtohU32 ();
  m_radios=i.ReadU8 ();
  m_rCh=i.ReadU8 ();
  m_extBw=i.ReadU8 ();
  m_rNewCh=i.ReadU8 ();
  ReadFrom(i,m_rAddr,6);
  m_rSwitchTime=i.ReadNtohU32 ();
  m_rSenseTime=i.ReadNtohU32 ();
  m_ni=i.ReadU8 ();
  uint32_t deciPart=i.ReadU32();
  double fIntPart=i.ReadU32();
  m_clcpf=deciPart+(fIntPart/1000);
  m_ttl=i.ReadU32();
  uint32_t id;
  uint8_t rch;
  for (uint8_t k = 0; k < m_ni; ++k)
    {
      id= i.ReadU32();
      rch=i.ReadU8 ();
      m_neighborRChannel.insert(std::make_pair (id,rch));
    }
  uint32_t dist = i.GetDistanceFrom (start);
  NS_ASSERT (dist == GetSerializedSize ());
  return dist;
  
}

void
SicaHelloHeader::Print(std::ostream &os) const
{
  os<< "---------------------------------------------------" ;
  os<< "\nSica Hello Header...";
  os << "\nSequence number is : "<< m_seqNo ;
  os << "\nOriginator (ID): "<< m_origin ;
  os <<"\nTime of origin " << m_originTime ;
  os<<"\n#radios is : " <<  static_cast <uint32_t> (m_radios); 
  os <<"\nReceiving MAC is : " << m_rAddr;
  os <<"\nReceiving channel is : "<< static_cast <uint32_t>(m_rCh);
  os  <<"\nEstimated Ext. BW is: " << static_cast <uint32_t>(m_extBw);
  os << "\n CLCPF is: "<< m_clcpf;
  os << "\n TTL  is: "<< m_ttl;
  os <<"\n" ;
  if (m_rNewCh > 0)
    os << "\nThere is an announcement for  switching to channel number: " << static_cast <uint32_t>(m_rNewCh) << " in " << m_rSwitchTime << " miliseconds" ;
  if (m_rSenseTime > 0)
    os<< "\nSenseing timer is "<< m_rSenseTime << " miliseconds";  
  if (m_ni >0)
    {
      std::map<uint32_t, uint8_t>::const_iterator j;
      os << "\nThere are information about " << static_cast <uint32_t>(m_ni) << " neighbors\n";
      for (j = m_neighborRChannel.begin (); j != m_neighborRChannel.end (); ++j)
        os << "Node ID: "<<(*j).first << ",R-Channel:  " << static_cast <uint32_t>((*j).second)<<"\n";
      
    }
}

bool 
SicaHelloHeader::AddNiRChannel (uint32_t ni, uint8_t rCh)
{
  if (m_neighborRChannel.find (ni) != m_neighborRChannel.end ())
    return true;

  NS_ASSERT_MSG (GetNiNo() < 255,"can't support more than 255 neighbor information in single hello"); 
  m_neighborRChannel.insert (std::make_pair (ni,rCh));
  m_ni++;
  return true;
}

bool 
SicaHelloHeader::RemoveNiRChannel (std::pair<uint32_t, uint8_t> & ni)
{
  if (m_neighborRChannel.empty ())
    return false;

  std::map<uint32_t, uint8_t>::iterator i =m_neighborRChannel.begin();
  ni=(*i);
  m_neighborRChannel.erase (i);
  m_ni--;
  return true;
}
bool 
SicaHelloHeader::DeleteNiRChannel (uint32_t ni)
{
  if (m_neighborRChannel.empty ())
    return false;
  else { 
    std::map<uint32_t, uint8_t>::iterator i = m_neighborRChannel.find (ni);
    if (i != m_neighborRChannel.end()){
      m_neighborRChannel.erase (i);
      m_ni--;
    }
  }
  return true;
}
  

void 
SicaHelloHeader::Clear()
{
  m_neighborRChannel.clear();
  m_seqNo=0;
  m_origin=0;
  m_originTime=0;
  m_radios=0;
  m_rCh=0;
  m_extBw=0;
  m_rNewCh=0;
  m_rAddr=Address();
  m_rSwitchTime=0;
  m_rSenseTime=0;
  m_ni=0;
  m_clcpf=0;
  m_ttl=0;
}


//////////////////////////////////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED (SicaHeader);

SicaHeader::SicaHeader(uint32_t sqNo,uint32_t origin,
                       uint32_t dest,uint32_t nextHop,
                       Time originTime):
  m_seqNo(sqNo),
  m_origin(origin),
  m_dest(dest),
  m_nextHop(nextHop),
  m_originTime(originTime.GetMilliSeconds())
{}


TypeId 
SicaHeader::GetTypeId ()
{
  static TypeId tid = TypeId("ns3::SicaHeader")
   .SetParent<Header> ()
  .AddConstructor<SicaHeader> ()
      ;
  return tid;
}

TypeId
SicaHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}


uint32_t 
SicaHeader::GetSerializedSize () const
{
  return (20);
}


void 
SicaHeader::Serialize (Buffer::Iterator i) const
{
  i.WriteU32(m_seqNo);
  i.WriteU32(m_origin);
  i.WriteU32(m_dest);
  i.WriteU32(m_nextHop);
  i.WriteHtonU32 (m_originTime);
 
}

uint32_t 
SicaHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_seqNo=i.ReadU32 ();
  m_origin=i.ReadU32 ();
  m_dest=i.ReadU32 ();
  m_nextHop=i.ReadU32 ();
  m_originTime=i.ReadNtohU32 ();
  uint32_t dist = i.GetDistanceFrom (start);
  NS_ASSERT (dist == GetSerializedSize ());
  return dist;
}


void
SicaHeader::Print(std::ostream &os) const
{
  os<< "---------------------------------------------------" ;
  os<< "\nSica  Header...";
  os << "\nSequence number is : "<< m_seqNo ;
  os << "\nOriginator (ID): "<< m_origin ;
  os << "\nDestination (ID): "<< m_dest ;
  os << "\nNext Hop (ID): "<< m_nextHop ;
  os <<"\nTime of origin " << m_originTime ;
}


  
}/*namespace ns3*/
