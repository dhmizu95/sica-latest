/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* 
 * Copyright (c) 2010 Universitat Pollitechnica the Catalunya
 * 
 * 
 * Author: Maryam Amiri Nezhad <maryam@ac.upc.edu>
 * 
 * This is doxygen module description, don't include 
 */

#ifndef CHANNELEMU_H
#define CHANNELEMU_H

/**
 * \ingroup sica
 * \defgroup channelEmu ChannelEmu 
 */



#include <iostream>
#include <iterator>
#include <ostream>
#include <vector>
#include "ns3/header.h"
#include "ns3/timer.h"
// #include "ns3/random-variable.h"
// #include <ns3/random-variable.h>
#include "ns3/random-variable-stream.h"
#include "ns3/double.h"
#include "ns3/integer.h"
#include "ns3/nstime.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/object-factory.h"
#include "ns3/object.h"
#include "ns3/simulator.h"
#include "ns3/channel.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/callback.h"
#include "ns3/traced-callback.h"

namespace ns3 {
/**
 * \brief A channel emulation which emulate the external interference over channels
 * using some random variables, this object would be aggregated to each channel.
 * 
 */
class ChannelEmu : public Object
{
   public: 
  ///\enum Status used to differentiate packets for services.
  enum Status {
    Idle_State    = 1,///< Channel is idle
    Busy_State = 2,///< Channel is Busy with external nodes
  
  };


///c-tor
  ChannelEmu();
///d-tor
  virtual ~ChannelEmu();
  /**
   *  \brief Makes it possible for user to change emulation parameters through calling SetAttribute
   */
 static TypeId GetTypeId (void);
  /**
  *  \brief set the channel number  for which this emulator works
  */
  void SetChannelNumber(uint32_t chId){m_chId=chId;}
  /**
  *  \brief Return the channel number  for which this emulator works
  */
  uint32_t GetChannelNumber(){return m_chId;}
  /**
  *  \brief set the duration for channel busy status
  */
  void SetBusyDuration(Time duration){m_busyDuration=duration; }
  /**
  *  \brief return true if the current status is busy otherwise false
  */
 bool IsBusy(){return (m_state==ChannelEmu::Busy_State);}
  /**
  *  \brief return true if the current status is idle otherwise false
  */
 bool IsIdle(){return (m_state==ChannelEmu::Idle_State);}
  /**
  *  \brief Changes the current status and set the timer for next time
  */
 void ChangeStatus();
 /** 
   * 
   * \brief Public method used to fire a  trace for a status changes 
   */
  void NotifyStatusChanged ();
private:
/**
   * The trace source fired when an emulator status changes 
   * 
   * \see class CallBackTraceSource
   */

  TracedCallback< Status, Time> m_statusChanged;

  uint32_t m_chId; ///< the channel number for which this emulator simulate the external interferences
  Time m_busyDuration;///< maximum amount of busy time at each iteration
  // RandomVariable m_nextTime; ///< exponential random variable for time duration between status
  Ptr<ExponentialRandomVariable> m_nextTime; // ns-3.25
  Status m_state; ///< current status of the channel
  Timer m_stausTimer; ///< Timer to change the status of the channel
};

  /// Container which holds channel emulators

class ChannelEmuContainer
{
 public:
  /// Iterator for vector of channel emulators
typedef std::vector<Ptr<ChannelEmu> >::const_iterator Iterator;
/**
   * Create an empty  ChannelEmuContainer
   */
   ChannelEmuContainer();
  /// the pointer to the first element in the container
Iterator Begin (void) const;
  /// the pointer to the last element of the container
Iterator End (void) const;
  /// Return the number of elements in the container
uint32_t GetN (void) const;
  /**
   *\brief Return one element in the container 
   *\param i the place of the element to be returned
   */
Ptr<ChannelEmu> Get (uint32_t i) const;
  /**
   *\brief Return  one of the elements in the container according to the channel id 
   *\param chId the channel id corresponding to the channel emulator object
   */
Ptr<ChannelEmu> GetId (uint32_t chId) const;
  /**
   *\brief Add one element to the container
   *\param c The pointer to the channel emulator object 
   */
void Add (Ptr<ChannelEmu> c);
private:
  /// vector of channel eumator objects
std::vector<Ptr<ChannelEmu> > m_channelEmuAgents;

};

/**
 * \ingroup channelEmu
 * \brief Helper class that adds channel emulation to channels objects.
 */
class ChannelEmuHelper
{
public: 
  //c_tor
  ChannelEmuHelper();
  /**
   * \param chId the  wifi channel on which emulation will run
   * \returns a newly-created channel emulator
   *
   * 
   * 
   */

  Ptr<ChannelEmu> Create (uint32_t chId) const;

/**
   * \param name the name of the attribute to set
   * \param value the value of the attribute to set.
   *   
   * This method controls the attributes of ns3::ChannelEmu::ChannelEmu
   */

  void Set (std::string name, const AttributeValue &value);

   /**
   * For each channel in the input container,implements 
   * ns3::ChannelEmu  
   * 
   * \param channels  that holds the set of channels for which to install the
   * new agent.
   */
  ChannelEmuContainer  Install (std::vector<uint32_t> channels) const;
private:

  ObjectFactory m_agentFactory;
};/* ChannelEmu Helper */



}/*namespace ns3*/



#endif /* CHANNELEMU_H */
