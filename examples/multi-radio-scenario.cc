/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009 The Boeing Company
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

// 
// This script configures a multi radio mesh network on an 802.11a physical layer, with
// 802.11a NICs in adhoc mode, and by default. The script builds a network of
// 25 nodes each of them is equipped with two radio interfaces.
//
// There are a number of command-line options available to control
// the default behavior.  The list of available command-line options
// can be listed with the following command:
// ./waf --run "multiRadioScenario --help"
//
// Note that all ns-3 attributes (not just the ones exposed in the below
// script) can be changed at command line; see the documentation.
//
// This script can also be helpful to put the Wifi layer into verbose
// logging mode; this command will turn on all wifi logging:
// 
// ./waf --run "multiRadioScenario --verbose=1"
//
// When you are done, you will notice two pcap trace files in your directory.
// If you have tcpdump installed, you can try this:
//
// tcpdump -r multiRadioScenario-0-0.pcap -nn -tt
//

#include "ns3/core-module.h"
// #include "ns3/common-module.h"   
// #include "ns3/node-module.h"
#include "ns3/network-module.h"  // common-module and node-module were merged into network-module [ns-3.11] 
// #include "ns3/helper-module.h"  // helper-module was removed [ns-3.11]
#include "ns3/mobility-module.h"
// #include "ns3/contrib-module.h"
#include "ns3/config-store-module.h" // contrib-module was changed to config-store-module [ns-3.11]
#include "ns3/wifi-module.h"
#include "ns3/sica.h"
#include "ns3/sica-helper.h"
// #include "ns3/random-variable.h"
#include "ns3/random-variable-stream.h"
#include "ns3/channel-emulation.h"
#include <stdint.h>
#include "ns3/callback.h"
#include "ns3/traced-callback.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <numeric>
#include "ns3/loopback-net-device.h"
#include "ns3/global-route-manager.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"

NS_LOG_COMPONENT_DEFINE ("MrmcScenario");

using namespace ns3;

static void GenerateTrafficSica (Ptr<Sica> sica, uint32_t dstId, uint32_t pktSize
                                 ,Time pktInterval, Time end)
{
      sica->CreateData (dstId,pktSize);
      if (Simulator::Now().GetSeconds() <  end.GetSeconds())
        Simulator::Schedule (pktInterval, &GenerateTrafficSica, 
                             sica ,dstId, pktSize, pktInterval,end);
}



static void WriteData(double tx, double rx, double hello,double delay )
{
  std::filebuf fb;
  fb.open ("output",std::ios::out | std::ios::app  );
  std::ostream os(&fb);
  // double txSum=std::accumulate(tx.begin(),tx.end(),0);
  // double rxSum =std::accumulate(rx.begin(),rx.end(),0);
  // double nsdelaySum=0;
  // double mydelaySum=0;
  // for (std::vector<double>::iterator i=nsdelay.begin();i!=nsdelay.end();++i)
  //   nsdelaySum+=(*i);
  // for (std::vector<double>::iterator i=mydelay.begin();i!=mydelay.end();++i)
  //   mydelaySum+=(*i);
  // double aveDelay=delay/rx;
  double aveDelay=delay;
 os<< Simulator::Now().GetSeconds() << " "<< tx<< " "<< rx << " "<< hello << " " <<aveDelay <<"\n";
}

static void GatherInfo(const char* prType,Time delay)
{
  static double rxPackets=0;
  static double txPackets=0;
  static double helloPackets=0;
  static double nsRxDelay=0;
  switch (prType[0])
    {
    case 'f' :
      {
        NS_LOG_INFO("fin");
      WriteData(txPackets,rxPackets,helloPackets,nsRxDelay);
      rxPackets=0;
      txPackets=0;
      helloPackets=0;
      nsRxDelay=0;
      break;
      }
    case 'r':
      {
        rxPackets++;
        nsRxDelay+=delay.Time::ToDouble((Time::Unit)1);
        break;
      }
    case 'h':
      {
        //std::cout << "hello";
        helloPackets++; 
        break;
      }
    case 't':
      {
        txPackets++;
        break;
      }
    }
}

void
ChannelProbability(std::string context,uint32_t nodeId,std::vector<double> channelProb,Time updateTime)
{
 std::filebuf fb;
  fb.open ("probOutput",std::ios::out | std::ios::app  );
  std::ostream os(&fb);
  // double txSum=std::accumulate(tx.begin(),tx.end(),0);
  // double rxSum =std::accumulate(rx.begin(),rx.end(),0);
  // double nsdelaySum=0;
  // double mydelaySum=0;
  // for (std::vector<double>::iterator i=nsdelay.begin();i!=nsdelay.end();++i)
  //   nsdelaySum+=(*i);
  // for (std::vector<double>::iterator i=mydelay.begin();i!=mydelay.end();++i)
  //   mydelaySum+=(*i);
  os<< updateTime.GetSeconds() << " "<< nodeId<< " ";
  copy(channelProb.begin(), channelProb.end(), std::ostream_iterator<double>(os, " "));
 os  << "\n";
}



void 
RxOk(std::string context,Time packetDelay,Ptr<const Packet> rxPacket,uint32_t nodeId,Time myDelay)
{
  //  std::cout << "rx"<< nodeId<< "\n";
  GatherInfo("rx",packetDelay);
}


void 
TxOk(std::string context,Ptr<const Packet> txPacket,uint32_t nodeId, Time txTime)
{
  // std::cout << "tx"<< nodeId << "\n";;
  GatherInfo("tx",txTime);
}

void 
HelloSent(std::string context,Ptr<const Packet> helloPacket,uint32_t nodeId, Time hTime)
{
  GatherInfo("hello",hTime);
}

static void SetSicaChannelProbabilityCallBack(std::vector<uint32_t> nodeId,NodeContainer c)
{
  for (std::vector<uint32_t >::iterator i=nodeId.begin(); i!=nodeId.end(); ++i)
    {
      std::ostringstream ossRx;
      NS_LOG_INFO( "Sica set channel prob call back for node " << *i);
      ossRx <<"/NodeList/"<< c.Get(*i)->GetId()<<"/$ns3::Sica/ChannelProbability";
      Config::Connect(ossRx.str(),MakeCallback(&ChannelProbability));
    }
}


static void SetSicaRxCallBack(std::vector<uint32_t> dstId,NodeContainer c)
{
  std::vector<uint32_t> dst;
  for (uint32_t j=0; j< dstId.size(); j++)
    {
      if ( std::find(dst.begin(), dst.end(), dstId[j])==dst.end())
        dst.push_back(dstId[j]);
    }
  for (std::vector<uint32_t >::iterator i=dst.begin(); i!=dst.end(); ++i)
    {
      std::ostringstream ossRx;
      NS_LOG_INFO( "Sica set call back for dst " << *i);
      ossRx <<"/NodeList/"<< c.Get(*i)->GetId()<<"/$ns3::Sica/RxOk";
      Config::Connect(ossRx.str(),MakeCallback(&RxOk));
    }
}

static void SetSicaTxCallBack(std::vector<uint32_t> srcId,NodeContainer c)
{
  std::vector<uint32_t> src;
  for (uint32_t j=0; j< srcId.size(); j++)
    {
      if ( std::find(src.begin(), src.end(), srcId[j])==src.end())
        src.push_back(srcId[j]);
    }
  for (std::vector<uint32_t >::iterator i=src.begin(); i!=src.end(); ++i)
     {
      std::ostringstream ossTx;
      NS_LOG_INFO("Sica set call back for src " << *i );
      ossTx <<"/NodeList/"<< c.Get(*i)->GetId()<<"/$ns3::Sica/TxOk";
      Config::Connect(ossTx.str(),MakeCallback(&TxOk));
    }
}


static void SetSicaHelloCallBack(uint32_t nodeId,NodeContainer c)
{
  std::ostringstream ossH;
  ossH <<"/NodeList/"<< c.Get(nodeId)->GetId()<<"/$ns3::Sica/HelloSent";
  Config::Connect(ossH.str(),MakeCallback(&HelloSent));
}


static void SetChannelsBusy(std::vector<Ptr<ChannelEmu> > emu, Time busyDuration)
{
  std::filebuf fb;
  fb.open ("channelOutput",std::ios::out | std::ios::app  );
  std::ostream os(&fb);
  for (std::vector<Ptr<ChannelEmu> >::iterator i=emu.begin(); i!=emu.end(); ++i)
    {
      NS_LOG_INFO(Simulator::Now().GetSeconds()<<  "--Set channel "<< (*i)->GetChannelNumber() << " Busy for " << busyDuration.GetMilliSeconds() );
      os<< Simulator::Now().GetSeconds() << " "<< (*i)->GetChannelNumber() << " "<< busyDuration.GetMilliSeconds() <<"\n";
    (*i)-> SetBusyDuration(busyDuration);
    }
}


void ReadNodesPosition(Ptr<ListPositionAllocator> positionAlloc, const char *fileName)
{
std::ifstream posfile;
 posfile.open(fileName);
 double nodeId,x,y,z;
 while(!posfile.eof()){
   posfile >> nodeId;
   posfile >> x;
   posfile >> y;
   posfile >> z;
   positionAlloc->Add (Vector (x, y, z));
}
}

void ReadTrafficFile(std::vector<uint32_t>*src,std::vector<uint32_t>*dst, const char *fileName)
{
  std::ifstream trfFile;
  trfFile.open(fileName);
  uint32_t s,d;
  while(!trfFile.eof()){
    trfFile >> s;
    trfFile >> d;
    NS_LOG_INFO("read data from traffic file: s "<< s << " d "<< d);
    (*src).push_back(s);
    (*dst).push_back(d);
    }
  NS_LOG_INFO("size of : s "<< (*src).size() );
}

bool ReadChannelFile(std::vector<uint32_t>*chId, const char *fileName)
{
  std::ifstream chFile;
  chFile.open(fileName);
  uint32_t ch;
  if (chFile.is_open())
    { 
      while(!chFile.eof()){
        chFile >> ch;
        NS_LOG_INFO("read data from channel file: ch "<< ch);
        (*chId).push_back(ch);
      }
      NS_LOG_INFO("size of : ch "<< (*chId).size() );
      return true;
    }
  else return false;
}


int main (int argc, char *argv[])
{
  //std::string phyMode ("DsssRate1Mbps");
  uint32_t packetSize = 1024; // bytes
  uint32_t interval = 80; // milliseconds
  bool verbose = false;
  uint32_t Max_CH=8;
  uint32_t Min_CH=1;
  uint32_t Max_Node=25;
  uint32_t fin=100;
  uint32_t stopTraffic=100;
  uint32_t gatherInfoDuration=100;
  uint32_t busyChannels=Max_CH/2;
  uint32_t busyStartTime=50;
  uint32_t busyDuration=10;
  uint32_t busyPeriod=50;
  uint32_t HelloInterval=20;
  uint64_t  TMax=1;
  uint32_t HelloExpireTime=20;
  uint32_t DataExpireTime=2000;
  uint32_t ChannelAssignmentInterval=10000;
  uint32_t ChannelSenseInterval=5000;
  uint32_t ChannelSensePeriod=10;
  uint32_t ChannelBusyBackoffTime=1;
  double Beta=0.2;
  double Gamma=0.8;
  double Alpha=1;
  uint32_t BroadcastSendDelay=1000;
  uint32_t  TInterfaceSendDelay=0;
  uint32_t SwitchingDelay=300;
  CommandLine cmd;
  double radioRange=250;
  const char* routingFile="rt-test";
  const char* mobilityFile="pos-test";
  const char* trafficFile="traffic-test";
  const char* channelFile="channel-test";
  //// Channel Assignment Intervals
  cmd.AddValue ("packetSize", "size of application packet sent", packetSize);
  cmd.AddValue ("packetInterval", "interval (MilliSeconds) between packets", interval);
  cmd.AddValue ("stopTraffic", "Time of end of flows",stopTraffic);
  cmd.AddValue ("verbose", "turn on all WifiNetDevice log components", verbose);
  cmd.AddValue ("channels", "maximum number of avialable channels",Max_CH );
  cmd.AddValue ("nodes", "maximum number of nodes",Max_Node );
  cmd.AddValue ("duration", "Simulation time(seconds)",fin );
  cmd.AddValue ("gatherInfoDuration", "gather inforamtion from simulation (seconds)",gatherInfoDuration );
  cmd.AddValue ("busyChannels", "number of busy channels", busyChannels);
  cmd.AddValue ("busyStartTime", "The starting time of interference(seconds)", busyStartTime);
  cmd.AddValue ("busyDuration", "The amount of interference(MilliSeconds)", busyDuration);
  cmd.AddValue ("busyPeriod", "The duration of busy channels (Seconds)", busyPeriod);
  cmd.AddValue ("range", "propagation range of nodes (m)", radioRange);
  // cmd.AddValue ("routing", "the input file for routing information",routingFile);
  cmd.AddValue ("HelloInterval", "Hello interval for CA(MilliSeconds)",HelloInterval);
  cmd.AddValue ("HelloExpireTime", "Hello message expire time (MilliSeconds)",HelloExpireTime);
  cmd.AddValue ("DataExpireTime", "data message expire time (MilliSeconds)",DataExpireTime);
  cmd.AddValue ("ChannelAssignmentInterval", "CA Interval time (MilliSeconds)",ChannelAssignmentInterval);
  cmd.AddValue ("ChannelSenseInterval", "sensing channels Interval time (MilliSeconds)",ChannelSenseInterval);
  cmd.AddValue ("ChannelSensePeriod", "sensing period (MilliSeconds)",ChannelSensePeriod);
  cmd.AddValue ("ChannelBusyBackoffTime", "the period of time that a CA siffer from sending if it founds the channel busy (MilliSeconds)",ChannelBusyBackoffTime);
  cmd.AddValue ("Beta", "beta value for Sica CA",Beta);
  cmd.AddValue ("Gamma", "gamma value for Sica CA",Gamma);
  cmd.AddValue ("Alpha", "alpha value for Sica CA",Alpha);
   cmd.AddValue ("BroadcastSendDelay", "maximum delay for broadcast jitter (NanoSeconds)",BroadcastSendDelay);
  cmd.AddValue ("TInterfaceSendDelay", "delay T for Sica (MilliSeconds)",TInterfaceSendDelay);
  cmd.AddValue ("SwitchingDelay","The  switching delay of interfaces (MicroSeconds) ",SwitchingDelay);
   cmd.AddValue ("TMax", "TMax for Sica CA(MilliSeconds)",TMax);
  
  cmd.Parse (argc, argv);
  // disable fragmentation for frames below 2200 bytes
  Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue ("2200"));
  // turn off RTS/CTS for frames below 2200 bytes
  Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("2200"));
  //prev 2200
  // Fix non-unicast data rate to be the same as that of unicast
  //Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode", 
  //                     StringValue (phyMode));

  NodeContainer c;
  c.Create (Max_Node);

  // The below set of helpers will help us to put together the wifi NICs we want
  WifiHelper wifi;
  if (verbose)
    {
      wifi.EnableLogComponents ();  // Turn on all Wifi logging
    }
  wifi.SetStandard (WIFI_PHY_STANDARD_80211a);

  YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
  // This is one parameter that matters when using FixedRssLossModel
  // set it to zero; otherwise, gain will be added
  wifiPhy.Set ("RxGain", DoubleValue (0) ); 
  wifiPhy.Set ("CcaMode1Threshold", DoubleValue (0.0) );
  // ns-3 supports RadioTap and Prism tracing extensions for 802.11a
  wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO); 

  YansWifiChannelHelper wifiChannel ;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::RangePropagationLossModel","MaxRange",DoubleValue(radioRange));
  NS_LOG_INFO("Radio range is set to " << radioRange);
 
//////////////////////////////////////////////////////////////////
///////////Create channels emulators for emulating external interferences  
  std::vector<uint32_t > channels;
  for (uint32_t i=Min_CH; i<=Max_CH;i++)
    {
    channels.push_back(i);
    }
  ChannelEmuHelper emuHelper;
  emuHelper.Set("BusyDuration", TimeValue(MilliSeconds(0)));
  ChannelEmuContainer emuContainer= emuHelper.Install(channels);
  // UniformVariable emuRNG;
  Ptr<UniformRandomVariable> emuRNG = CreateObject<UniformRandomVariable> ();; // ns-3.25
  std::vector<Ptr<ChannelEmu> > emu;
  // std::vector<Ptr<ChannelEmu> > tempEmu;/// for linear busy 
  std::vector <uint32_t> chId;
  Ptr<ChannelEmu> ch;
  NS_LOG_INFO ("Read emu");
 if (ReadChannelFile(&chId,channelFile))
   {
     emu.clear(); 
     for(std::vector<uint32_t>::iterator i=chId.begin(); i< chId.end(); ++i)
       {
         ch= emuContainer.GetId(*i);
         // busyDuration=ch->GetChannelNumber();/// for linear busy 
         if (std::find(emu.begin(), emu.end(), ch)==emu.end())
           {
             // tempEmu.clear();/// for linear busy 
             // tempEmu.push_back(ch);/// for linear busy 
             emu.push_back(ch);
             std::cout << ch->GetChannelNumber() << "\n";
             // Simulator::Schedule(Seconds(busyStartTime), &SetChannelsBusy,tempEmu,MilliSeconds(busyDuration));/// for linear busy 
           }
       }
     Simulator::Schedule(Seconds(busyStartTime), &SetChannelsBusy,emu,MilliSeconds(busyDuration));/// uncomment for other scenarios
   }
 else
   {
     while (busyStartTime < fin )
       {
         emu.clear();   
         while (emu.size()< busyChannels)
           {
            //  ch= emuContainer.GetId(emuRNG.GetInteger(Min_CH,Max_CH));
             ch= emuContainer.GetId(emuRNG->GetInteger(Min_CH,Max_CH));
             if (std::find(emu.begin(), emu.end(), ch)==emu.end())
               {
                 emu.push_back(ch);
                 std::cout << ch->GetChannelNumber() << "\n";
               }
           }
         Simulator::Schedule(Seconds(busyStartTime), &SetChannelsBusy,emu,MilliSeconds(busyDuration));
         Simulator::Schedule(Seconds(busyStartTime+busyPeriod),&SetChannelsBusy,emu,MilliSeconds(0));
         busyStartTime+=busyPeriod;
       }
   }
 
 ////////////////////////////////////////////////////////
 // set the first channel for physical layer
  wifiPhy.SetChannel (wifiChannel.Create ());
  // Add a non-QoS upper mac, and disable rate control
  // NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default ();
  NqosWifiMacHelper wifiMac = NqosWifiMacHelper();
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager");
  // Set it to adhoc mode
  wifiMac.SetType ("ns3::AdhocWifiMac");
  NetDeviceContainer devices = wifi.Install (wifiPhy, wifiMac, c);
  devices.Add(wifi.Install (wifiPhy, wifiMac, c));
  
////for nodes position
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  ReadNodesPosition(positionAlloc,mobilityFile);
  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (c);
  /// Internet stack 
  InternetStackHelper internet;
  internet.Install (c);
  Ipv4AddressHelper ipv4;
  NS_LOG_INFO ("Assign IP Addresses.");
  ipv4.SetBase ("10.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer i = ipv4.Assign (devices);
  
 
  /// routing helper 
 NS_LOG_INFO ("Install routing");
 RoutingHelper routing;
 routing.Install(c,routingFile);

NS_LOG_INFO ("Read traffic");
std::vector <uint32_t> src;
std::vector <uint32_t> dst;
ReadTrafficFile(&src,&dst,trafficFile);

 NS_LOG_INFO ("src size "<< src.size());
 std::vector<uint32_t> chprobNode;
 ///////////////////////////////// Create SICA 
 SicaHelper CA;
 SicaContainer CAContainer;
 TMax=static_cast <uint64_t>(HelloInterval/Max_CH);
 //Set parameters for SICA
 CA.Set("HelloInterval", TimeValue(MilliSeconds(HelloInterval)));
 CA.Set("HelloExpireTime", TimeValue(MilliSeconds(HelloExpireTime)));
 CA.Set("DataExpireTime", TimeValue(MilliSeconds(DataExpireTime)));
 CA.Set("ChannelAssignmentInterval", TimeValue(MilliSeconds(ChannelAssignmentInterval)));
 CA.Set("ChannelSenseInterval", TimeValue(MilliSeconds(ChannelSenseInterval)));
 CA.Set("ChannelSensePeriod", TimeValue(MilliSeconds(ChannelSensePeriod)));
 CA.Set("MaxChannelNumber", IntegerValue(Max_CH));
 CA.Set("ChannelBusyBackoffTime",TimeValue(MilliSeconds(ChannelBusyBackoffTime)));
 CA.Set("Beta",DoubleValue(Beta));
 CA.Set("Gamma",DoubleValue(Gamma));
 CA.Set("Alpha",DoubleValue(Alpha));
 CA.Set("BroadcastSendDelay",TimeValue(NanoSeconds(0)));
 CA.Set("TInterfaceSendDelay",TimeValue(NanoSeconds(0)));
 CA.Set("SwitchingDelay",TimeValue(MicroSeconds(SwitchingDelay)));
 CA.Set("TMax",TimeValue(MilliSeconds(TMax)));
 /// Install SICA on nodes
 CAContainer=CA.Install(c ,emuContainer); 
 for (uint32_t i=0; i<Max_Node ; i++)
   {
     chprobNode.push_back(i);
     SetSicaHelloCallBack(i,c);
   }
 for (uint32_t i=0; i< src.size(); i++)
   {
     NS_LOG_INFO("Traffic from "<<src[i] << " to " << dst[i] );
     Simulator::Schedule (MilliSeconds(HelloInterval*5), &GenerateTrafficSica,CAContainer.Get(src[i]) ,dst[i],packetSize , MilliSeconds(interval),Seconds(stopTraffic));
   }  
 SetSicaTxCallBack(src,c);
 SetSicaRxCallBack(dst,c);
 SetSicaChannelProbabilityCallBack(chprobNode,c);

 //gather informations
 for (uint32_t i=gatherInfoDuration ; i<=fin; i+=gatherInfoDuration)
   {
     Simulator::Schedule(Seconds(i),&GatherInfo,"fin",Seconds(0));
   }
 


 Simulator::Stop (Seconds (fin));
 Simulator::Run ();
 Simulator::Destroy ();
 return 0;
}

