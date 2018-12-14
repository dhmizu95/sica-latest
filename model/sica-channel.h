/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* 
 * Copyright (c) 2010 Universitat Pollitechnica the Catalunya
 * 
 * 
 * Author: Maryam Amiri Nezhad <maryam@ac.upc.edu>
 * 
 * This is doxygen module description, don't include 
 */

#ifndef SICACHANNEL_H
#define SICACHANNEL_H

#include <iostream>
#include <vector>
#include "ns3/nstime.h"
#include "ns3/simulator.h"
#include "ns3/timer.h"

namespace ns3 {


/**
   * \ingroup sica
   * \defgroup channeltable SicaChannels
   */

  /**
   * \brief Sica ChannelTable  defines the table  structure for saving information about channels in  Sica.
   * 
   */ 
class SicaChannels
{
public:
/// c-tor
  SicaChannels();
  ///d-tor
  virtual ~SicaChannels(){}
/// Structure which holds channel information for Sica or Urbanx protocols 
  struct SicaChannel
  {
    uint32_t m_cId;///< Channel ID
    uint32_t m_bw; ///< Channel's total bandwidth in mbps
    uint32_t m_bx; ///< Estimated external interference bandwidth consumption for this channel
    uint32_t m_neighborsNo; ///< Number of neighboring interface on the channel
    double m_weight;///< Channel weights according to decision game
    Time m_bxExpireTime; ///< Reset Bx after a long period of time if there is no update happens
    bool m_senseFalg; ///< Used to show whether this channel is being sensed or not, during the sense period no data transmission is done
    ///c-tor of the SicaChannel struct
    SicaChannel(uint32_t chId,uint32_t bw,uint32_t bx,uint32_t niNo,Time bxExpTime):
      m_cId(chId),
      m_bw(bw),
      m_bx(bx),
      m_neighborsNo(niNo),
      m_weight(0),
      m_bxExpireTime(Simulator::Now()+bxExpTime),
      m_senseFalg(false)
    {
    }
  };
  /**
   *\brief Return the Id of the channel table object 
   *\param ch the pointer to the channel table object
   */
  uint32_t GetChannelID(SicaChannel *ch){return (ch->m_cId);}
  /**
   *\brief  Return the pointer to the channel information with ID id
   *\param chId the id of the channel 
   */
  SicaChannel *FindChannel(uint32_t chId);
  /**
   *\brief  Update or insert information in channel list
   *\param chId the Id of the channel
   *\param bw the bandwidth of the channel in Mb
*\param bx the amount of channel  bandwidth consumed by external interference 
   *\param niNo number of neighboring radio interface tuned to the channel
   *\param bxExpTime the expire time of the information about channel
   */
  void UpdateChannel(uint32_t chId,uint32_t bw,uint32_t bx,uint32_t niNo ,Time bxExpTime);
  /**
   *\brief  Set the total bandwidth of the channel with ID id
   *\param chId the Id of the channel
   *\param bw the bandwidth of the channel in Mb
   */
  void SetChannelBandwidth(uint32_t chId, uint32_t bw);
  /**
   *\brief  Return the total bandwidth of the channel with ID id
   *\param chId the Id of the channel
   */
  uint32_t  GetChannelBandwidth(uint32_t chId);
  /**
   *\brief  Set the external bandwidth consumption for  channel with ID id
*\param chId the Id of the channel
*\param bx the amount of channel  bandwidth consumed by external interference
*\param bxExpTime the expiration time of new information
*/
  void SetChannelExtBandwidth(uint32_t chId, uint32_t bx,Time bxExpTime);
  /**
   *\brief  Return the external bandwidth consumption for  channel with ID id
  *\param chId the Id of the channel
  */
  uint32_t GetChannelExtBandwidth(uint32_t chId);
  /**
   *\brief Return the expiration time for estimated external bandwidth for channel with ID id
*\param chId the Id of the channel
*/
  Time GetExtBandwidthExpireTime(uint32_t chId);
  /**
   *\brief  Set the number of neighbors that have one receiving radio on  channel with ID id
   *\param chId the Id of the channel
   *\param niNo number of neighboring nodes radio interface tuned  over the channel 
   */
  void SetChannelNeighbors(uint32_t chId,uint32_t niNo);
  /**
   *\brief Return the number of neighbors that have one receiving radio on  channel with ID id
   *\param chId the Id of the channel
   */
  uint32_t GetChannelNeighbors(uint32_t chId);
  /**
   *\brief Increase by one the number of neighbors that have one receiving radio on  channel with ID id.
*\param chId the Id of the channel
*/
  void IncChannelNeighbors(uint32_t chId);
  /**
   *\brief Decrease by one the number of neighbors that have one receiving radio on  channel with ID id.
*\param chId the Id of the channel
*/
  void DecChannelNeighbors(uint32_t chId);
  /**
   *\brief  Set the weight of a channel with ID id
*\param chId the Id of the channel
*\param w the weight of the channel 
*/
  void SetChannelWeight(uint32_t chId,double w);
  /**
   *\brief  Return  the  weight of a channel with ID id
*\param chId the Id of the channel
*/
  double GetChannelWeight(uint32_t chId); 
  /**
   *\ Set the sense flag for the channel
*\param chId the Id of the channel
*/
  void SetChannelSenseFlag(uint32_t chId);
  /**
   *\brief Reset the sense flag for the channel
*\param chId the Id of the channel
*/
  void ResetChannelSenseFlag(uint32_t chId);
  /**
   *\brief  Return true if any neighbor is sensing this channel, otherwise false
*\param chId the Id of the channel
*/
  bool IsBeingSensed(uint32_t chId);
  /**
   *\brief  Print Channel Table 
   *\param os the output stream
   */
  void PrintChannel(std::ostream &os);
  /// Clear channel list
  void Clear(){m_channel.clear();}
  /**
   *\brief  calculate and return clcpf for Urbanx::Urbanx protocol 
   *\return channel with minimum weight
   *\param ccc the Id of the common channel to be avoided from the calculation 
   */
  double CalculateCLCPF(uint32_t ccc);
  /**
   *\brief  find channel with the maximum weight
   *\param ccc the Id of the common channel to be avoided from the calculation 
   */
  int FindMaxWeightChannel(uint32_t ccc);
private:
  /// List of channels
  std::vector<SicaChannel> m_channel; 
};/*SicaChannels*/


}/*namespace ns3 */

#endif /*SICACHANNEL_H  */

