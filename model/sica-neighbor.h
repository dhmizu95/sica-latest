/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* 
 * Copyright (c) 2010 Universitat Pollitechnica the Catalunya
 * 
 * 
 * Author: Maryam Amiri Nezhad <maryam@ac.upc.edu>
 * 
 * This is doxygen module description, don't include 
 */

#ifndef SICANEIGHBOR_H
#define SICANEIGHBOR_H

#include <iostream>
#include <vector>
#include "ns3/simulator.h"
#include "ns3/header.h"
#include "ns3/timer.h"
#include "ns3/enum.h"
#include "ns3/ipv4-address.h"
#include "ns3/wifi-mac-header.h"
#include "ns3/arp-cache.h"
#include "ns3/nstime.h"

namespace ns3 {

class Sica;

/**
   * \ingroup sica
   * \defgroup neighbors SicaNeighbors
   */
/**
   * \brief SicaNeighbors  defines the table  structure for saving neighboring nodes' information in  Sica.
   * 
*/ 
class SicaNeighbors 
{
public:
  /// c-tor
  SicaNeighbors();
  ///d-tor
  virtual ~SicaNeighbors(){}
  /// Neighbor description
  struct SicaNeighbor
  {
    uint32_t m_id; ///< Node Id of the neighbor
    uint32_t m_hopCount; ///< Distance to the neighbor
    uint32_t m_neighborRadio;///< Number of radio interface it has
    uint32_t m_neighborChannel; ///< Channel of the receiving radio
    Address m_rAddr;///<< Address of the receiving radio interfaces
    Address m_tAddr;///<< Address of the transmitting radio interfaces
    Address m_cAddr;///<< Address of the common radio if any
    Time m_updateTime;///< Time stamp, which shows the moment that the information is updated
    Time m_switchTime;///< Shows that when a neighbor will switch its receiving interface to another channel
    uint32_t m_neighborNewChannel;///< New channel for neighboring node where it will switch after m_switchTime
    bool close; ///< Variable for future need!!
    ///c-tor
    SicaNeighbor(uint32_t id ,uint32_t h,uint32_t r,uint32_t ch,
                 Address rAddr, Address tAddr , Time utime, Time stime,
                 uint32_t nch ):
      m_id(id),m_hopCount(h),m_neighborRadio(r),m_neighborChannel(ch),
      m_rAddr(rAddr),m_tAddr(tAddr),m_updateTime(utime),
      m_switchTime(stime+Simulator::Now()),m_neighborNewChannel(nch),
      close(false)
    {
    }
  };
  /// Find neighbor and node with ID id return the pointer
  SicaNeighbor *FindNeighbor(uint32_t id);
  /// Find Id of neighbor which has a device with the address addr
  int32_t FindDeviceAddr(Address addr);
  ///Return expire time for neighbor node with ID id, if exists, else return 0.
  Time GetNiUpdateTime (uint32_t id);
  /// Set the update time of a neighbor 
  void  SetNiUpdateTime (uint32_t id, Time uTime);
  ///Return switching  time for neighbor node with ID id, if exists, else return 0
  Time GetNiSwitchTime (uint32_t id);
  /// Set the update time of a neighbor 
  void  SetNiSwitchTime (uint32_t id, Time sTime);
  /// Return the hop counts of a neighbor 
  uint32_t GetNiHops(uint32_t id);
  /// Set  the hop counts of a neighbor 
  void SetNiHops(uint32_t id,uint32_t hops );
  ///  Update the information  for the neighbor with ID id, return false if the received information is older than the current
  bool Update (uint32_t id,uint32_t hops,uint32_t radio,uint32_t channel,Address rAddr,Address tAddr,  Time updateTime, Time switchTime,uint32_t newChannel );
  /// Remove neighbor by with ID id
  void RmvNeighbor(uint32_t id);
  /// Return the receiving channel of the neighbor with ID id
  int32_t GetNiChannel(uint32_t id);
  ///Set neighbor's  channel 
  void SetNiChannel(uint32_t id,uint32_t nCh);
  /// Return the transmitting channel of the neighbor with ID id
 int32_t GetNiTChannel(uint32_t id);
  ///Set neighbor's  channel 
  void SetNiTChannel(uint32_t id,uint32_t nCh);
  /// Return the new channel that the neighbor will switch to 
  int32_t GetNiNewChannel(uint32_t id);
  /// Set neighbor New channel 
  void  SetNiNewChannel(uint32_t id,uint32_t nCh);
  /// Return the address of the receiving interface of the neighbor
  Address GetNiRAddress(uint32_t id);
 /// Set the  address of the receiving interface of the neighbor
  void  SetNiRAddress(uint32_t id,Address rAddr);
 /// Return the  address of the transmitting  interface of the neighbor
  Address GetNiTAddress(uint32_t id);
/// Set the address of the transmitting interface of the neighbor
  void  SetNiTAddress(uint32_t id,Address rAddr);
 /// Return the address of the c interface of the neighbor
  Address GetNiCAddress(uint32_t id);
 /// Set the  address of the c interface of the neighbor
  void  SetNiCAddress(uint32_t id,Address cAddr);
  ///Check that node with ID id  is  direct neighbor
  bool IsDirectNeighbor (uint32_t id);
  /// Check that node in i th place of neighbor table is  direct neighbor
  bool IsDirectNeighborByIndex (uint32_t i);
  ///Return the number of neighbors on a specific channel
  uint32_t GetNiOnChannel(uint32_t channel);
  ///Return the number of neighbors which are far as hopC and have a radio a specific channel
  uint32_t GetNiOnChannelByHops(uint32_t channel, uint32_t fromHopC , uint32_t toHopC);
  /// Return the IP address of the receiving radio of i th neighbor from neighbor list
  int32_t GetNeighborIdByIndex(uint32_t i);
  /// Return the receiving channel of the i th neighbor from neighbor list
  int32_t GetNiChannelByIndex(uint32_t i);
  /// Print the information stored in neighbor table
  void PrintNeighborTable(std::ostream &os);
  /// Return number of entris in neighbor table
  uint32_t GetNiNo(){return m_ni;}
  /// Return the number of neighbors from distance fromHopC to toHopC 
  uint32_t GetNiNobyHops(uint32_t fromHopC , uint32_t toHopC);
  /// Remove expired neighbor information
  void RmvExpiredNi(Time maxExpireTime);
  /// Cleare neighbor list
  void Clear(){m_neighbor.clear(); m_ni=0;}
private:
  /// number of neighbors 
  uint32_t m_ni;
  /// List of Neighbors 
  std::vector<SicaNeighbor> m_neighbor;
};/*SicaNeighbors*/


}/*namespace ns3 */

#endif /* SICANEIGHBOR_H */

