/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* 
 * Copyright (c) 2010 Universitat Pollitechnica the Catalunya
 * 
 * 
 * Author: Maryam Amiri Nezhad <maryam@ac.upc.edu>
 * 
 * This is doxygen module description, don't include 
 */

#ifndef SICAPACKET_H
#define SICAPACKET_H

#include <iostream>
#include "ns3/header.h"
#include "ns3/simulator.h"
#include "ns3/enum.h"
#include "ns3/address.h"
#include <map>
#include "ns3/nstime.h"

namespace ns3 {

/**
   * \ingroup sica
   * \defgroup helloheader SicaHelloHeader
   * \brief Sica Hello Message defines the header structure of hello messages of Sica.
   *  Hello messages used to inform neighboring nodes about channel information and switching attempts.
   * 
   * This class defines the hello message format for Sica protocol.
   *\param {Hello Sequence Number }: Sequence number of message
   *\param {Originator ID}: Node Id of the originator
   *\param {Originator Time }:Used to distinguish between old and new hello messages
   * \param {#R-Radios} : Number of receiving radios 
   * \param {Channel R-R}: current channel of receiving radio
   * \param {Bx(Channel R-R)}: Estimated consumed bandwidth by external interference over current channel of R-R
   * \param {R-R -NewChannel}: The next channel for channel switching attempt of R-R,  R-R -NewChannel=0 shows no switching
   * \param {Time To Switch R interface} : Time in milliseconds until R-R switch
 \verbatim
  0                   1                   2                   3
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                    Hello Sequence Number                      |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                    Originator ID                              |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                    Originator Time                            |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |  #Radios    |  Channel R-R  |Bx(Channel R-R)|R-R -NewChannel  |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                Receiving Radio #1  MAC Address (part 1)       | 
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                Receiving Radio #1  MAC Address (part 2)       | 
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |               Time To Switch R interface                      |    
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |               Time To Sense the current channel of R-R        |    
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  | # Neighbors   |Channel R-R N#1|Channel R-R N#2|Channel R-R N#3|
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                    Neighbor #1 ID                             |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                    Neighbor #2 ID (if exists)                 |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                    Neighbor #3 ID (if exists)                 |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                    CLCPF (for Urbanx protocol)                |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                    CLCPF (for Urbanx protocol)                |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                    TTL (for Urbanx protocol)                  |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  \endverbatim
*/ 

/// Hello Header for Sica or Urbanx hello messages
class SicaHelloHeader : public Header
{
public:
/// c-tor
  SicaHelloHeader(uint32_t sqNo=0,uint32_t origin=0,
                  Time originTime=Simulator::Now(),
                  uint8_t radios=1,uint8_t rCh=1, uint8_t extBw=0,
                  uint8_t rNewCh=0, Address rAddr=Address(), Time rSwitchTime=MilliSeconds (0),Time rSenseTime=MilliSeconds (0));
  virtual ~SicaHelloHeader(){}
///\name Header serialization/de-serialization
  //\{
  static TypeId GetTypeId ();
  TypeId GetInstanceTypeId () const;
  uint32_t GetSerializedSize () const;
  void Serialize (Buffer::Iterator i) const;
  uint32_t Deserialize (Buffer::Iterator start);
  void Print (std::ostream &os) const;
  //\}
  /// Check whether the header has valid data or not
  bool IsValid(){return (m_seqNo>0); }
  /// Set the sequence number of the hello header
  void SetSeqNo (uint32_t seqno){m_seqNo= seqno;}
  /// Return the sequence number of the  hello header
  uint32_t GetSeqNo() {return m_seqNo;}
  /// Set the originator IP address 
  void SetOrigin(uint32_t origin){m_origin=origin;}
  /// Return the originator IP address
  uint32_t GetOrigin (){return m_origin;}
 /// Set the number of available radios
  void SetRadios(uint8_t radios){m_radios=radios;}
  /// Return the number of available radios
  uint8_t GetRadios(){return m_radios;}
  /// Set the receiving channel 
  void SetRChannel(uint8_t rCh){m_rCh=rCh;}
  /// Return the receiving channel 
  uint8_t GetRChannel(){return m_rCh;}
  ///Set the external bandwidth over a channel in Mbps
  void SetExtBw (uint8_t extBw){m_extBw=extBw;}
  /// Return the external bandwidth over a channel
  uint8_t GetExtBw(){return m_extBw;}
  /// Set new channel for receiving interface
  void  SetRNewChannel (uint8_t rNCh){m_rNewCh=rNCh;}
  /// Return New channel for receiving interface
  uint8_t GetRNewChannel (){return m_rNewCh;}
  ///Set  address of the originator receiving radio
  void SetRAddress(Address rAddr){m_rAddr=rAddr;}
  ///Return  address of the originator receiving radio
  Address GetRAddress(){return (m_rAddr);}
 /// Set the originating  time
   void SetOriginTime(Time originTime){m_originTime=originTime.GetMilliSeconds ();}
  /// Return the  originating  time of hello
  Time GetOriginTime(){
    Time t (MilliSeconds (m_originTime));
    return t;}
  ///Set the switching time for receiving interface
  void SetTimeToSwitch(Time sT){m_rSwitchTime=sT.GetMilliSeconds ();}
  /// Return the switching time of receiving interface
  Time GetTimeToSwitch(){
    Time t (MilliSeconds (m_rSwitchTime));
    return t;}
   ///Synchronize a timer for sensing R-R channel for neighbors over this channel
  void SetSenseTime(Time ssT){m_rSenseTime=ssT.GetMilliSeconds ();}
  /// Return the timer for sensing R-R channel for neighbors over this channel
  Time GetSenseTime(){
    Time t (MilliSeconds (m_rSenseTime));
    return t;}
  //Set the number of neighbors 
  //void SetNiNo (uint8_t niNo){m_ni=niNo;}
  ///Set the CLCPF
  void SetCLCPF(double clcpf) {m_clcpf=clcpf;}
  ///return the clcpf
  double GetCLCPF(){return m_clcpf;}
  /// return the TTL
  uint32_t GetTTL(){return m_ttl;}
  /// Set ttl
  void SetTTL(uint32_t ttl){m_ttl=ttl;}
  /// decrease the ttl by one 
  void DecreaseTTL(){m_ttl--;}
  ///Return the number of neighbors 
  uint8_t GetNiNo() const {return m_ni;}
  /**
   * 
   * \brief Add neighbor receiving channel information to the message
   * \return false if we have already add maximum number of neighbors(255) to the message 
   *\param ni the id of neighbor node 
   *\param rCh the receiving channel of the neighbor
   */
  bool AddNiRChannel ( uint32_t ni, uint8_t rCh);
  /**
   * 
   * \brief Peek  and remove the first element of neighbor receiving channel information from the message and return it in a pair
   * \return false if there is no such information in the message 
   *\param ni the id of neighbor node 
   */
  bool RemoveNiRChannel (std::pair<uint32_t, uint8_t> & ni);
 /**
  *
   * \brief Delete neighbor receiving channel information from the message
   * \return false if there is no such information in the message 
   *\param ni the id of neighbor node 
   */
  bool DeleteNiRChannel (uint32_t ni);
  
  /// Cleare Header
  void Clear();
private:
  /// The sequence number 
  uint32_t m_seqNo;
  /// Originator IP address
  uint32_t m_origin;
  /// The simulator time when the hello is produced
  uint32_t m_originTime;
  /// Number of available radio in originator
  uint8_t  m_radios;
  /// Channel of the receiving (R) interface on originator node
  uint8_t m_rCh;
  /// Consumed bandwidth by external interference  on receiving channel
  uint8_t m_extBw;
  /// New channel of the receiving (R) interface (0 if there is no switching attempt)
  uint8_t m_rNewCh;
  /// The MAC address of the receiving radio
  Address m_rAddr;
  /// Switching time of the receiving (R) interface in milliseconds
  uint32_t   m_rSwitchTime;
  /// Switching time of the receiving (R) interface in milliseconds
  uint32_t   m_rSenseTime;
  /// number of neighbors 
  uint8_t m_ni;
  /// CLCPF for Urbanx
  double m_clcpf;
  /// ttl for Urbanx
  uint32_t m_ttl; 
  /// List of Neighbors : Node ID  and Receiving Channel.
  std::map<uint32_t, uint8_t> m_neighborRChannel; 
};/*SicaHelloHeader*/

  /// Header used for static routing
class SicaHeader : public Header
{
public:
/// c-tor
  SicaHeader(uint32_t sqNo=0,uint32_t origin=0,uint32_t dest=0,uint32_t nextHop=0,Time originTime=Simulator::Now());
  virtual ~SicaHeader(){}
  /// Used to set parameters of the class
  static TypeId GetTypeId ();
  /// return the type id 
  TypeId GetInstanceTypeId () const;
  /// return the serialize size of the header
  uint32_t GetSerializedSize () const;
  /// Serialize the header in to bits
  void Serialize (Buffer::Iterator i) const;
  /// Deserialize the header
  uint32_t Deserialize (Buffer::Iterator start);
  /**
   *\brief Print the content of the header
   *\param os the output stream 
   */
  void Print (std::ostream &os) const;
  /// Check whether the header has valid data or not
  bool IsValid(){return (m_seqNo>0); }
  /** 
   *\brief  Set the sequence number of the hello header
   *\param seqno the sequence number
   */
  void SetSeqNo (uint32_t seqno){m_seqNo= seqno;}
  /// Return the sequence number of the  hello header
  uint32_t GetSeqNo() {return m_seqNo;}
 /** 
  *\brief  Set the originator Id
  *\param origin the id of the source 
  */
  void SetOrigin(uint32_t origin){m_origin=origin;}
  /// Return the originator Id
  uint32_t GetOrigin (){return m_origin;}
  /**
   *\brief Set  the destination Id
   *\param dest the id of the destination node
   */
  void SetDest(uint32_t dest){m_dest=dest;}
  /// Return the destination Id
  uint32_t GetDest (){return m_dest;}
  /**  
   *\brief  Set  the nexthop Id
   *\param nextHop the id of the next hop node
   */
  void SetNextHop(uint32_t nextHop){m_nextHop=nextHop;}
  /// Return the nexthop Id
  uint32_t GetNextHop (){return m_nextHop;}
  /**
   *\brief  Set the originating  time
   *\param originTime the time of originating the  message
   */
   void SetOriginTime(Time originTime){m_originTime=originTime.GetMilliSeconds ();}
  /// Return the  originating  time of packet
  Time GetOriginTime(){
    Time t (MilliSeconds (m_originTime));
    return t;}
private:
  uint32_t m_seqNo; /// Sequence number
  uint32_t m_origin; /// Id of the source node
  uint32_t m_dest; /// Id of the destination node
  uint32_t m_nextHop; /// Id of the next hop node to the destination
  uint32_t m_originTime; ///the time of originating the packet
};

}/*namespace ns3 */

#endif /* SICAPACKET_H */

