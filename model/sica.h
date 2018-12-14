/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* 
 * Copyright (c) 2010 Universitat Politecnica the Catalunya
 * 
 * 
 * Author: Maryam Amiri Nezhad <maryam@ac.upc.edu>
 * 
 * This is doxygen module description, don't include 
 */


#ifndef SICA_H
#define SICA_H

/**
 * \ingroup mesh 
 * \defgroup sica Sica
 */

#include "ns3/simulator.h"
#include "ns3/timer.h"
#include "ns3/object.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-header.h"
#include "ns3/callback.h"
#include "ns3/wifi-mac-header.h"
#include "ns3/node.h"
#include "ns3/net-device.h"
#include "ns3/sica-queue.h"
#include "ns3/sica-packet.h"
#include "ns3/sica-neighbor.h"
#include "ns3/sica-channel.h"
#include "ns3/channel-emulation.h"
#include "ns3/sica-rtable.h"
#include "ns3/double.h"
#include "ns3/integer.h"
#include "ns3/simulator.h"
#include "ns3/ipv4.h"
#include "ns3/channel.h"
#include "ns3/log.h"
#include "ns3/boolean.h"
// #include "ns3/random-variable.h"
#include "ns3/random-variable-stream.h"
#include "ns3/inet-socket-address.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/wifi-net-device.h"
#include "ns3/adhoc-wifi-mac.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/yans-wifi-phy.h"
#include "ns3/wifi-phy.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/callback.h"
#include "ns3/delay-jitter-estimation.h"
// #include "ns3/matrix.h"
#include <algorithm>
#include <limits>
#include <vector>
#include <iostream> 
#include <ostream>
#include <iterator>


namespace ns3 {

/**
 * \brief A channel assignment mechanism for multi-channel multi-radio mesh network
 *  semi-dynamic interference avoidance channel assignment
 * It select a channel for the receiving radio of each node and switch the transmitter radio to send data to neighbors. hello messages are used to coordinate neighbors.
 *  For each node we do the following: 
 * 1- Add one  data queue for each channel 
 * 2- Add one hello-queue for each channel
 * Push every Hello to hello-queue of the channels we have neighbors on it
 * Push every received data to the data-queue of the corresponding channel
 * Each time we got a packet the module check the header and try to find the responsible neighbor for that, then it would check the channel of the neighbor and push the packet to the corresponding channel.
 * during the sending period the module check all channels and send packets corresponding to each channel.
 * 
 * 
 * 
 */
class Sica : public Object
{
 public: 
  /**
     *\brief Compare two channels based on the weights assigned to them, used to sort the list of channels based on their weight
     *
     */
  class  CompareChannelsWeight{
     Sica& that;
  public:
    ///c-tor
    CompareChannelsWeight(Sica &s) : that(s) {}
    /**
     * \brief Get two channels id and compare them  based on the weighs assigned to each channel
     *\param i  channel ID
     *\param j  channel ID
     *\return true  if the weight of first channel is less than the weight of the second channel. the channel with the bigger weight is more suitable for data transmission
     */
    bool operator()(uint32_t i, uint32_t j) const {
      return (that.m_channel.GetChannelWeight(i)< that.m_channel.GetChannelWeight(j));
    }
  };
  ///c-tor
  Sica();
  ///d-tor
  virtual ~Sica();
  //virtual void DoDispose ();
  /**
   *  \brief Makes it possible for user to change protocol parameters through calling SetAttribute
   */
 static TypeId GetTypeId (void);
/** 
   * 
   * \brief Public method used to fire a data-received  trace for a data packet being received. 
   *\param packet Received packet
   */
  void  NotifyRxReceived (Ptr< Packet> packet);
/** 
   * 
   * \brief Public method used to fire a data-sent  trace for a data packet being sent. 
   *\param packet  a copy of a packet which was sent  
   */
  void NotifyTxSent (Ptr< Packet> packet);
/** 
   * 
   * \brief Public method used to fire a hello-sent  trace for a data packet being sent. 
 *\param packet  a copy of a packet which was sent  
   */
void NotifyHelloSent (Ptr<Packet> packet);
/** 
   * 
   * \brief Public method used to fire a data-dropped  trace for a data packet during the receiving process. 
   *\param packet a pointer to the packet
  *\param snr The signal to noise ratio detected on channel
   */
// void  NotifyRxDropped (Ptr<const Packet> packet, double snr, bool isEndOfFrame);
// void  NotifyRxDropped (Ptr<Packet> packet, double snr, bool isEndOfFrame);
void  NotifyRxDropped (Ptr<Packet> packet, double snr);
/** 
   * 
   * \brief Public method used to fire a probability changes trace for channels. 
 *\param channelProb a vector containing the channel probability
   */
  void NotifyChannelProbability (std::vector<double> channelProb);
  /**
   * 
   * \brief Initialize Sica, call Sica::InitializeQueues, Sica::InitializeInterfaces, Sica::InitializeChannel and Sica::InitializeTimers
   */
  void Initialize();
  /**
   * 
   * \brief  Create and initialize channel queues 
   */
  void InitializeQueues();
  /**
   * 
   * \brief  Check the number of interfaces over a node and select one transmitter and one receiver interface
   *
   */
  void InitializeInterfaces();
 
  /**
   * 
   * \brief  set the receiving channel for the node and decide a new channel for the initial phase.
   */
  void InitializeChannel();

  /**
   * 
   * \brief  Set the timers for hello interval, sensing period and channel assignment
   */
  void InitializeTimers();
 /** 
   * 
   * \brief Set the channel emulators for all channels
   *\param channelsEmu the channels emulation container which contains emulation objects to be added to the Sica object
   */ 
  void SetChannelsEmulationObject(ChannelEmuContainer channelsEmu){m_channelEmuObjects=channelsEmu;}
 /** 
   * 
   * \brief Return the channel emulators for the given  channel id
   *\param id the channel id
   *\return the channel emulation object related to the given channel id
   */ 
  Ptr<ChannelEmu> GetChannelEmu(uint32_t id){return m_channelEmuObjects.GetId(id);}
  /**
   * 
   * \brief return the receiving channel of the R interface
   */
  uint32_t GetRChannel(){return m_rChannel;}
 /**
   * 
   * \brief return the transmitting channel of the T interface
   */
  uint32_t GetTChannel();
  /**
   * 
   * \brief return the future  receiving channel to which the R interface will switch after a period of time
   */
  uint32_t GetRNewChannel(){return m_rNewChannel;}
  /**
   * 
   * \brief return the Id of the attaching node to the Sica object
   */
  uint32_t GetId(){return m_id;}
  /**
   * 
   *\brief Return the address of Sica::m_nb
   * \return  Sica::m_nb
   */
  SicaNeighbors *GetSicaNeighbors();

  /**
   * 
   *\brief Return the address of  Sica::m_queue
   * \return  Sica::m_queue
   */
  SicaQueue  *GetSicaQueue();
   /**
   * 
   *\brief Return the address of  Sica::m_channel
   * \return  Sica::m_channel
   */
  SicaChannels *GetSicaChannel();
   /**
   * 
   * \brief It calls the same function in neighbor class to print neighbor table of node
   */
  void PrintNeighborTable(std::ostream &os);
  /**
   * 
   * \brief It calls the same function in channels class to print the channel information table
   */
  void PrintChannelTable(std::ostream &os);
 /**
   * 
   * \brief Check whether the channel associated to the net device is busy or not
   */
  bool ChannelIsBusy(uint32_t chId);
  /**
   *\brief Set the Sense channel flag to prevent other functions from sending data during the sensing period 
   */
  void ModifySenseChannelFlag(uint32_t c);
  /**
   *\brief Senses the current channel of receiving interface and estimate the external bandwidth
   */
  void StartSenseCurrentChannel();
  /**
   * 
   *\brief Start checking the channel situation every Sica::ChannelSensePeriod and count the number of times that a channel is busy, is called by  Sica::StartSenseCurrentChannel 
   */
  void SenseCurrentChannel();
  /**
   *\brief Estimate the channel busy time at the end of the sensing period and write it in the channel table is called by  Sica::StartSenseCurrentChannel 
   */
  void EndSenseCurrentChannel();
 /**
   * 
   * \brief Re-schedule timer with minDelay if  minDelay is less than the delay left for timer t 
   *\param t the pointer to the timer
   *\param minDelay the time to which the timer will be rescheduled if it is less than the remaining time to  the end of the timer
   */
  void ReScheduleTimer(Timer *t, Time minDelay );

  ///\name Sica Data Delivery methods.
  //\{
 
  /**
   * 
   * \brief Listen to device  and receives packets
   * \param dstDevice the address of the device which receives a packet
   *\param packet received packet
   *\param protocolNumber the address of receiving protocol here is SICA_PORT
   *\param srcAddr the address of sender device
   */ 
  bool RecvPacket(Ptr<NetDevice> dstDevice,Ptr<const Packet> packet ,uint16_t protocolNumber ,const Address & srcAddr);

  /**
   * 
   * \brief Gathers information from hello and call Sica::UpdateNeighborTable and Sica::UpdateChannelTable .
   * 
   */

  void ProcessRcvHello(Ptr<Packet> p,Address srcAddr );
/**
   * 
   * \brief Evaluate the  received  data packet, send it to Sica::NotifyRxReceived if the node is the destination of the packet or call Sica:: DistributeDataPacket to send it to the receiving channel of the next hop node.
   *\param p received packet
   */
 void ProcessRcvData(Ptr<Packet> p);

  /**
   * 
   * \brief Update neighbor information based on information in hello message
   *\param sicaHelloHeader Hello header which is passed to process 
   *\param srcAddr The header of the transmitter from which the hello packet was received
   *\return true if the information is not stale
   */
  bool UpdateNeighborTable(SicaHelloHeader sicaHelloHeader, Address srcAddr );
/**
   * 
   * \brief Update channel information based on information in hello message
   *\param sicaHelloHeader Hello header which is passed to process 
   */
  bool UpdateChannelTable(SicaHelloHeader sicaHelloHeader);

    /**
   * 
   * \brief Produces hello send it to all neighbor channel for sending, call Sica::CreateHelloHeader for producing hello header
   * 
   */
  void CreateHello();
   /**
   * 
   * \brief Produces hello header information based on neighbor table and current information  and return it
   * 
   */
  SicaHelloHeader CreateHelloHeader();
 
  /**
   * 
   * \brief  distribute hello message to all channels over which the node  has a neighbor
   *\param p hello packet
   */
  void DistributeHello(Ptr<Packet> p);

    /**
   * 
   * \brief Produces uni-cast data packet with given packet size and destination address  and push it to neighbor channel
   * \param dstId the id of the destination node
   *\param pSize the size of the data packet 
   */
  void CreateData(uint32_t  dstId, uint32_t pSize);

 /**
   * 
   * \brief Create a sica header and add it to the packet this function may be called from a source node or in any of the forwarding nodes on the path to the destination
   * \param p  The packet to which header must be added
   * \param dstId the id of the destination node 
   * \param srcId the id of the source node
   *\param originTime the time of origination packet
   */
  int AddDataHeaders(Ptr<Packet> p,uint32_t srcId, uint32_t dstId, Time originTime);
 
/**
   * 
   * \brief  Distribute the  data  message to the appropriate  channel 
   *\param p  The packet produced by create Sica::CreateData
   * \param nextHopChannel The id of the channel where the data must be sent
   */
  void DistributeDataPacket(Ptr<Packet> p,uint32_t nextHopChannel );

 /**
   * 
   * \brief Get packet and the device through  which the node wants to use to send data and set the sending parameters
   *\param packet data to send
   *\param device the interface through which the data would be sent
   *\param protocolNumber used to distinguish hello and data packets
   * 
   */ 
  void SendPacket(Ptr<Packet> packet,Ptr <NetDevice> device ,uint32_t protocolNumber);
 /**
   * 
   * \brief Estimate the transition duration for physical layer
   *\param pSize The size of packet for sending
   *\param wifiphy the pointer to the  physical layer  
   */
  Time EstimateTxDuration(uint32_t pSize,Ptr<WifiPhy> wifiphy);
/**
   * 
   * \brief Get packet and device address to send packet through it
   * 
   */ 
  void  DeviceSend(Ptr<NetDevice> device, Ptr<Packet> packet,Address dstAddr,uint32_t protocolNumber );

  //\}
  ///\name Sica Channel switching  methods.
  //\{
/**
   * 
   * \brief It handles the process necessary when a node discover that its neighbor has switched its channel.
   * 
   */
  void HandleNeighborSwitchChannel();
 
/**
   * 
   * \brief Check  whether the interface is busy, if so predict the idle time then add it to a random switch delay and schedule a timer for switching the R interface.
   * 
   */ 
  void ScheduleSwitchRInterface();
  /** 
   * \brief Switch the r interface to new channel (Sica::m_rNewChannel).
   * 
   */ 
 void SwitchRInterface();
  /**
   *\brief Switch the T interface to a channel for sending data
   *\param c The channel to which the T interface will switch
   *\return true if the switching is successful, false otherwise
   */
bool SwitchTInterface(uint32_t c);

/** 
   * \brief Switch T interface to  channels one by one and send data 
   * \param ch the channel to which the T interface will switch to send data
   * Sica::TMax maximum amount of time that node will stay on one channel to send data
   */ 
  void TInterfaceStartSend(uint32_t ch);
 
  /**
   *\brief Send data over the given channels (Sica::channelsToPoll)using the T interface
   *\param ch The channel for which the T interface start the transmission
   *
   */ 
  void TInterfaceSend(uint32_t ch);
  /**
   * 
   * \brief Check whether all conditions are held before start sending with T-Interface over the given channel
   *\param ch The id of the channel over which the T interface will send data 
   *\param txEstimation The time estimation for sending one packet 
   */
  bool TInterfaceReadyToSend(uint32_t ch,Time txEstimation );
 
/** 
   * \brief Check whether there is any packet for send in receiving channel and send it if the R interface is ready to send, call Sica::RInterfaceReadyToSend
   */
  void RInterfaceCheckChannelQueue();  
/** 
   * \brief Check whether it is possible to send data over R interface or not, check the sense flag and remaining time to the upcoming switch
   *\param txEstimation the estimation time necessary for sending data or hello packets
   *\return true if the R interface is ready to send, false otherwise
   */
  bool RInterfaceReadyToSend(Time txEstimation);
  
  //\}
 ///\name Sica Channel Decision making mechanisms
  //\{
 /** 
   * \brief Select a channel for R interface
   * 
   */ 
  void GameChannelAssignment();
/** 
   * \brief Update the loss matrix based on different formulas  which is indicated by Sica::LossFormulaNum
   * 
   */ 
  void UpdateLossMatrix();
/** 
   * \brief Compute the loss for the given channel based on its weight
   * \param c the id of a channel for which the loss will be computed
   */ 
  double ComputeStageLoss(uint32_t c);
/** 
   * \brief Compute the new  weight for all available channels based on their previous weight and the given loss vector
   */ 
  void UpdateChannelWeight();
/** 
   * \brief select a random channel based on the probability assigned to each channel
   * \return the random channel
   */ 
  uint32_t SelectRandomChannel();
/** 
   * \brief compute the cumulative probability of channels
   * \param p the probability vector  assigned to all channels
   *\return The cumulative probability 
   */ 
  std::vector<double> ComputeCumulativeProbability(std::vector<double> p);
/** 
   * \brief Print game information, it is called by Sica::GameChannelAssignment this function must be called by Sica::GameChannelAssignment
   * \param os the output media
   *\param channels the id of channels
   *\param prob the probability vector assigned to channels
   *\param weight the weight assigned to channels
   *\param cProb the cumulative probability
   */ 
  void PrintGameData(std::ostream &os ,std::vector<uint32_t> channels,std::vector<double> prob,std::vector<double> weight,std::vector<double> cProb );
 //\}
///\name Sica Parameters
  //\{
  uint32_t LossFormulaNum;
  uint32_t SICA_DATA_PORT; ///< protocol id used to send sica data packets (Default=550)
  uint32_t SICA_HELLO_PORT; ///< protocol id used to send sica broadcast packets (Default=551)
  uint32_t Max_CH; ///< Maximum Number of available channels (Default=8)
  uint32_t Min_CH; ///< The first Index of the available channels (Default=1)
  uint32_t Max_BW; ///< Maximum available bandwidth of each channel
  double m_gamma;///< Gamma parameter for channel decision
  double m_beta;///< Beta parameter for calculating the channel weights in decision game
  double m_alpha;///< Alpha parameter for calculating the loss in decision game
 /// Hello message interval 
  Time HelloInterval;
  /// The maximum period of time that Sica is allowed to buffer a data packet for 2000 seconds.
  Time DataExpireTime;
  /// The maximum period of time that Sica is allowed to buffer a  hello packet for 10 seconds.
  Time HelloExpireTime;
  /// The maximum period of time that Sica keep neighbor information if it does not receive any new information from neighbor. We set it  to at least three times of the  HelloInterval
  Time NeighborExpireTime;
  ///Switching delay for interface, we set it to a fixed number for simplicity, it is equal to the switching delay in default 802.11b radio interface 250 microseconds
  Time SwitchingDelay;
  /// The period between channel assignment runs
  Time ChannelAssignmentInterval;
  /// Sensing channel interval
  Time ChannelSenseInterval;
  /// The duration of sensing receiving channel
  Time ChannelSensePeriod;
  /// Sica sense channel using sampling, this time control the sampling rate
  Time ChannelSenseRate;
  /// The maximum period of time that Sica keeps estimated external bandwidth consumption for a channel entry in channel list. If it does not receive any updated information it will reset external interference estimation to zero after this time, We set it to 40s
  Time BxExpireTime;
  /// The delay that Sica differ sending over a channel if it found it busy 
  Time ChannelBusyBackoffTime;
  ///The interval  that Sica waits before check  an empty queue (used only when all queues are empty or for R interface to check the corresponding queue of the receiving channel)
  Time QueuePollTime;
 //\}
private:
  /**
   * The trace source fired when a data packet is received.
   * 
   * \see class CallBackTraceSource
   */
  TracedCallback< Time, Ptr<const Packet> , uint32_t , Time> m_sicaRxReceived;
  TracedCallback< Ptr<const Packet> , uint32_t, uint32_t> m_sicaRxDevReceived;
 /**
   * The trace source fired when a data packet is sent.
   * 
   * \see class CallBackTraceSource
   */
  TracedCallback<Ptr<const Packet> , uint32_t , Time> m_sicaTxSent;
  TracedCallback<Ptr<const Packet> , uint32_t ,uint32_t,uint32_t ,Time> m_sicaTxDeviceSent;
/**
   * The trace source fired when a hello packet is sent.
   * 
   * \see class CallBackTraceSource
   */
  TracedCallback<Ptr<const Packet> , uint32_t , Time > m_sicaHelloSent;
 /**
   * The trace source fired when a data packet is dropped.
   * 
   * \see class CallBackTraceSource
   */
  // TracedCallback<Ptr<const Packet> , uint32_t, double, uint32_t> m_sicaRxDropped;
  TracedCallback<Ptr<const Packet>, uint32_t, double, uint32_t> m_sicaRxDropped;
/**
   * The trace source fired when the probability of channels updated
   * 
   * \see class CallBackTraceSource
   */
  TracedCallback< uint32_t ,std::vector<double> , Time > m_sicaChannelProb;
  ///used to keep busy duration of current receiving channel during channel sensing period
  Time m_busyChTime;
  ///used to keep idle duration of current receiving channel during channel sensing period 
  Time m_idleChTime;
  //// used to control the delay before broadcasting
  Time m_bcastSendDelay;
 //// used to control the delay before start sending after switching to a channel
  Time m_TInterfaceSendDelay;
  ///\name Sica local variables
  //\{
  uint32_t m_sqNo;///<sequence number for data and hello messages
  uint32_t m_id; ///< node's unique ID
  uint32_t m_radio; ///< number of radio interface
  uint32_t m_rChannel;///< The current channel of receiving interface
  Ptr <NetDevice> m_rInterface;///< Pointer to the receiving interface
  Ptr <NetDevice> m_tInterface;///< Pointer to the receiving interface
  uint32_t m_rNewChannel;///< The target channel for switching the receiving interface
  //Timer m_rSwitchTimer;/// < Timer to switch the R interface to another channel
  Timer m_switchTimer; /// < Timer to switch to another channel
  Timer m_CATimer;/// < Timer to invoke channel assignment
  Timer m_helloTimer; ///< Timer to produce and push hello messages to the channel queue during the HelloInterval 
  Timer m_channelSenseTimer; /// < Timer to sense the current channel we set it to hello interval for now.
  Timer m_channelSenseRateTimer;
  // Timer m_channelSenseEndTimer;
  bool m_channelSenseFlag; /// < Shows whether the node is in sense mode or not
  /// Switch timer invoked three functions to update  neighbor tables and channel table and move packets from one queue to another every when a switching happens it also 
  Timer m_niSwitchTimer; 
  /// T interface check channels and sending traffic timer
  Timer m_TInterfaceSendTimer;
  /// Check the channel queue attached to the receiving channel for sending data
  Timer m_rInterfacePollTimer;
  /// minimum delay time before switching R interface to a new channel
  uint64_t m_minSwitchDelay;
  /// maximum delay time before switching R interface to a new channel
  uint64_t  m_maxSwitchDelay;
  /// channel on which t interface will send data 
  std::vector<uint32_t> channelsToPoll;
  /// loss matrix for each channel a node keep a loss value
  std::vector<double> m_loss;
  /// The available bandwidth over each channel for a node, considering the fair share
  std::vector<double> m_bwAllotted;
  /// maximum amount of time that t interface is allowed to stay over one channel
  Time TMax ;
  SicaNeighbors m_nb;          ///< Neighbors table 
  SicaQueue m_queue;      ///< Queues for hello  and data message
  SicaChannels m_channel; ///< Channels' information
  ChannelEmuContainer m_channelEmuObjects;//< Channels emulation objects
  WifiPhy::RxErrorCallback errorCallback;
  //\}
  
};

}

#endif /* SICA_H */

//  LocalWords:  const DoDispose SetAttribute TypeId GetTypeId NotifyRxReceived
//  LocalWords:  Ptr NotifyTxSent NotifyHelloSent NotifyRxDropped snr GetId os
//  LocalWords:  channelProb NotifyChannelProbability InitializeQueues rChannel
//  LocalWords:  InitializeInterfaces InitializeChannel InitializeTimers chId
//  LocalWords:  channelsEmu SetChannelsEmulationObject ChannelEmuContainer int
//  LocalWords:  channelEmuObjects ChannelEmu GetChannelEmu GetRChannel srcAddr
//  LocalWords:  GetTChannel GetRNewChannel rNewChannel SicaNeighbors SicaQueue
//  LocalWords:  GetSicaNeighbors GetSicaQueue SicaChannels GetSicaChannel ch
//  LocalWords:  PrintNeighborTable PrintChannelTable ChannelIsBusy minDelay
//  LocalWords:  ModifySenseChannelFlag StartSenseCurrentChannel dstDevice
//  LocalWords:  SenseCurrentChannel EndSenseCurrentChannel ReScheduleTimer
//  LocalWords:  protocolNumber RecvPacket NetDevice ProcessRcvHello dstId
//  LocalWords:  DistributeDataPacket ProcessRcvData sicaHelloHeader pSize
//  LocalWords:  UpdateNeighborTable SicaHelloHeader UpdateChannelTable srcId
//  LocalWords:  CreateHello CreateHelloHeader DistributeHello CreateData
//  LocalWords:  originTime AddDataHeaders nextHopChannel SendPacket wifiphy
//  LocalWords:  EstimateTxDuration WifiPhy DeviceSend dstAddr SwitchRInterface
//  LocalWords:  HandleNeighborSwitchChannel ScheduleSwitchRInterface
//  LocalWords:  SwitchTInterface
