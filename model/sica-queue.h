/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* 
 * Copyright (c) 2010 Universitat Pollitechnica the Catalunya
 * 
 * 
 * Author: Maryam Amiri Nezhad <maryam@ac.upc.edu>
 * 
 * This is doxygen module description, don't include 
 */


#ifndef SICA_QUEUE_H
#define SICA_QUEUE_H

#include "ns3/simulator.h"
#include "ns3/timer.h"
#include "ns3/ipv4-address.h"
#include "ns3/callback.h"
#include "ns3/wifi-mac-header.h"
#include "ns3/packet.h"
#include "ns3/sica-packet.h"
#include "ns3/ptr.h"
#include <vector>
#include <iostream> 

namespace ns3 {

class  Sica;

 /**
   * \ingroup sica
   * \defgroup sicaqueue SicaQueue
   */

  /**
   * \brief  This class defines the entry format for either hello or packet queue. A time stamp is used to delete old entries.
   *
   */ 
class SicaQueueEntry
{
public:
    ///\enum PacketType  used to differenciate packets for services.
  enum PacketType {
    Hello_Type = 1,///< SicaQueueEntry contains hello message
    Data_Type    = 2,///< SicaQueueEntry contains data message
  };
  /// c-tor
  SicaQueueEntry (Ptr <Packet> p, PacketType ptype):
    m_type(ptype),
    m_p(p),
    m_expiretime(Seconds(0))
  {}
  /// d-tor
  virtual  ~SicaQueueEntry(){}//m_p->Unref();}
  /**
   * \brief Compare queue entries
   *\param  o the queue entry for comparison
   * \return true if equal
   */
  bool operator== (SicaQueueEntry const & o) const
  {
    return ((m_p == o.m_p) && (m_type == o.m_type) && (m_expiretime == o.m_expiretime));
  }
  /// Return the type packet in queue entry
  PacketType GetPacketType () {return m_type;}
  /// Return the packet type (hello or data)  in queue entry
  void SetPacketType(PacketType ptype ){m_type=ptype;}
  /// Return the pointer to the  packet in queue entry
  Ptr <Packet> GetPacket() {return m_p;}
  /// Put a packet in queue entry
  void SetPacket ( Ptr <Packet>p){m_p=p;}
  /// Set the expire time of the queue entry equal to the simulation time plus the expiration time in second 
  void SetExpireTime (Time exp){m_expiretime= exp + Simulator::Now();}
  /// Get the expire time of the queue entry, if the return value is less than zero the packet should be removed.
  Time GetExpireTime (){return m_expiretime;}
private:
  /// Type of packet in the queue entry
  PacketType m_type;
  /// Pointer to the packet   
  Ptr <Packet> m_p;
  /// Expire time of the queue entry
  Time m_expiretime;
};

  /**
   * \ingroup sica
   * \ingroup sicaqueue
   * \brief SicaQueue is used to handle more than one data and signal queue for each node 
   *
   * It would attached to each node and maintain all tasks about hello and data packets  queuing. It stores/retrieves packets to/from the specified channel queue.
  */
class SicaQueue 
{
public:
  /// c-tor
  SicaQueue ();
  /// d-tor
  virtual  ~SicaQueue();
  /**
    * \ingroup sicaqueue
    * \defgroup sicachannelqueue SicaChannelQueue
    */

  /**
   * \brief Sica Channel Queue is a structure which stores  data and signal queue for one channel
    */
  struct SicaChannelQueue {
    /// Store Hello packets targeted to  one channel in a vector
    std::vector <SicaQueueEntry> m_helloQueue;
    /// Store Data packets targeted to  one channel in a vector
    std::vector <SicaQueueEntry> m_dataQueue;
    /// Channel number
    uint32_t m_ch;
    /// If this queue is active or not
    bool m_close;
    /// c-tor
    SicaChannelQueue(uint32_t ch):
      m_ch (ch),
      m_close(false)
    {
    }
   /// d-tor
    virtual ~SicaChannelQueue(void)
    {
      //m_helloQueue.clear();
      //m_dataQueue.clear();
    }
    /// Set channel number  
    void SetChannelQueue(uint32_t ch)
    {
      m_ch=ch;
    }
    /// Open a recently closed queue 
    void OpenChannelQueue()
    {
      m_close= false;
    }
    /// Remove expired entries in the queue 
    bool IsExpired(SicaQueueEntry ent)
    {
      return (ent.GetExpireTime() < Simulator::Now());
    }
  }; /*SicaChannelQueue */
 /// Return the pointer to the queue associated to the channel "ch"
  SicaChannelQueue * FindChannelQueue(uint32_t ch);
  /// Number of entries in the channel queue (hello queue or data queue)
  uint32_t GetSize (uint32_t ch, SicaQueueEntry::PacketType ptype);
  /// Copy the entry fields of \param origin  into \param cpy Queue entry
  void  CpQueuEntry(SicaQueueEntry *cpy,SicaQueueEntry origin );
/// Push entry in channel queue, if there is no entry with the same packet and destination address in queue.
  bool Enqueue (uint32_t ch, SicaQueueEntry* ent);
 /**
  *\brief Return first  entry of channel queue for given packet type (hello or data)
  * This function does not remove packets from queue
  * \param ch the channel ID
  * \param ptype  Hello or Data type 
  * \return A pointer which contains a copy of queue entry
  */
  SicaQueueEntry *Dequeue (uint32_t ch, SicaQueueEntry::PacketType ptype);
 /**
  *\brief Return the  earliest entry found for the given destination from data queue of the given channel 
  *\param ch The channel ID
  *\param dst ID of the destination
  *\return a pointer to a queue entry
  */
  SicaQueueEntry *DequeueWithDest (uint32_t ch,uint32_t dst);
/**
  *\brief Remove the entry for the given destination address  from data queue of the given channel 
  *\param ch The channel ID
  *\param dst ID of the destination
  */
  bool EraseWithDest(uint32_t ch,uint32_t dst);
/**
  *\brief Return the pointer to the  entry for the given destination address  from data queue of the given channel,, Return queue.end if there is no entry.
  *\param ch The channel ID
  *\param dst ID of the destination
  *\return an iterator which refer to the  queue entry
  */

  std::vector<SicaQueueEntry>::iterator FindQueueEntryForDest(uint32_t ch,uint32_t dst);
/**
  *\brief Remove the first packet in the queue corresponding to the given packet
  * type (hello or data) Close the channel queue on which we have no neighbor
  * 
  */
 void EraseFront(uint32_t ch, SicaQueueEntry::PacketType ptype);
/**
  *\brief Remove the expired packets from the  channel queue of the given channel and given type (hello or data) \return (Queue size)
  * \param ch The id of the channel
  *\param ptype the type of the queue selected from SicaQueueEntry::PacketType
  */
  uint32_t Purge(uint32_t ch, SicaQueueEntry::PacketType ptype);

/**
  *\brief Move all data packets related to the node with the given address from one channel to another channel queue  
  *\param originCh the source channel
  * \param targetCh the destination channel
  *\param addr ID of the destination node
  */
  void ShuffleData(uint32_t originCh,uint32_t targetCh ,uint32_t addr);
/**
  *\brief Move all data packets from one channel to another channel queue  
  *\param originCh the source channel
  * \param targetCh the destination channel
  */
  void  ShuffleDataALL(uint32_t originCh,uint32_t targetCh );
  /// Close a queue and erase all packets
  void CloseQueue (uint32_t ch);
/// Create the queue of a channel on which we have one neighbor
  SicaChannelQueue * CreatQueue (uint32_t ch);
  /// find how many flows are in one channel queue (packets with different sources)
  double ComputeFlowNumber(uint32_t ch);

private:
  ///Vector of channel queues for each node
std::vector<SicaChannelQueue> m_cqueue;
};/* Sica-Queue*/

}/*namespace ns3 */


#endif /* SICA_QUEUE_H */
