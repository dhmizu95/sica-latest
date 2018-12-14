/**
 * \ingroup sica
 *  \brief A channel assignment mechanism for multi-channel multi-radio mesh network
 *
 */

#include "ns3/sica.h"
#include "ns3/llc-snap-header.h"

NS_LOG_COMPONENT_DEFINE ("Sica");

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED (Sica);

//////////////////////Sica

Sica::Sica():
  SICA_DATA_PORT(550),
  SICA_HELLO_PORT(551),
  Max_CH(8),
  Min_CH(1),
  Max_BW(11),
  m_gamma(0.8),
  m_beta(0.7),
  m_alpha(0.5),
  HelloInterval(Seconds(100)),
  DataExpireTime(Seconds(2000)),
  HelloExpireTime(Seconds(100)),
  NeighborExpireTime(Seconds(200)),
  SwitchingDelay(MicroSeconds (300)),
  ChannelAssignmentInterval(Seconds (400)),
  ChannelSenseInterval(Seconds(200)),
  ChannelSensePeriod(MilliSeconds(300)),
  ChannelSenseRate(MilliSeconds(1)),
  BxExpireTime(Seconds (400)),
  ChannelBusyBackoffTime(MilliSeconds (8)),
  QueuePollTime(MilliSeconds(1)),
  m_bcastSendDelay(NanoSeconds(10)),
  m_TInterfaceSendDelay(MicroSeconds(0)),
  m_sqNo(0),
  m_rNewChannel(0),
  m_switchTimer(Timer::CANCEL_ON_DESTROY),
  m_CATimer(Timer::CANCEL_ON_DESTROY),
  m_helloTimer(Timer::CANCEL_ON_DESTROY),
  m_channelSenseTimer(Timer::CANCEL_ON_DESTROY),
  m_channelSenseRateTimer(Timer::CANCEL_ON_DESTROY),
  m_channelSenseFlag(false),
  m_niSwitchTimer(Timer::CANCEL_ON_DESTROY),
  m_TInterfaceSendTimer(Timer::CANCEL_ON_DESTROY),
  m_rInterfacePollTimer(Timer::CANCEL_ON_DESTROY),
  TMax(MilliSeconds(10))
 
{
}
  


TypeId 
Sica::GetTypeId (void)
{
  static TypeId tid = TypeId("ns3::Sica")
    .SetParent<Object> ()
    .AddConstructor<Sica> ()
    .AddAttribute("Beta","Beta parameter for calculating channel weight default is 0.7",
		  DoubleValue(0.7),
		  MakeDoubleAccessor (&Sica::m_beta),
		  MakeDoubleChecker<double> ())
    .AddAttribute("Gamma","Gamma parameter for calculating loss function of game decision defualt is 0.8",
		  DoubleValue(0.8),
		  MakeDoubleAccessor (&Sica::m_gamma),
		  MakeDoubleChecker<double> ())
    .AddAttribute("Alpha","Alpha parameter for calculating loss function of game decision defualt is 0.5",
		  DoubleValue(0.5),
		  MakeDoubleAccessor (&Sica::m_alpha),
		  MakeDoubleChecker<double> ())
    .AddAttribute("SicaDataPort","Port number used to send data packets defual is 550",
		  IntegerValue(550),
		  MakeIntegerAccessor (&Sica::SICA_DATA_PORT),
		  MakeIntegerChecker<uint32_t> ())

 .AddAttribute("SicaHelloPort","Port number used to send hello packets defual is 551",
		  IntegerValue(551),
		  MakeIntegerAccessor (&Sica::SICA_HELLO_PORT),
		  MakeIntegerChecker<uint32_t> ())

    .AddAttribute("MinChannelNumber","The first Index of  available channels defualt is 1",
		  IntegerValue(1),
		  MakeIntegerAccessor (&Sica::Min_CH),
		  MakeIntegerChecker<uint32_t> ())
    .AddAttribute("MaxChannelNumber","Maximum number of available channels defualt is 8",
		  IntegerValue(8),
		  MakeIntegerAccessor (&Sica::Max_CH),
		  MakeIntegerChecker<uint32_t> ())
    .AddAttribute("MaxChannelBW","Maximum available bandwidth of channels defualt is 11Mb",
		  IntegerValue(11),
		  MakeIntegerAccessor (&Sica::Max_BW),
		  MakeIntegerChecker<uint32_t> ())

    .AddAttribute("HelloInterval","Hello message interval defualt is 100s",
		  TimeValue(Seconds(100)),
		  MakeTimeAccessor (&Sica::HelloInterval),
		  MakeTimeChecker())

    .AddAttribute("DataExpireTime","The maximum period of time that Sica is allowed to buffer a data packet for, default 2000 seconds",
		  TimeValue(Seconds(2000)),
		  MakeTimeAccessor (&Sica::DataExpireTime),
		  MakeTimeChecker())
    
    .AddAttribute("HelloExpireTime","The maximum period of time that Sica is allowed to buffer a data packet for, default HelloInterval= 100 seconds",
		  TimeValue(Seconds(100)),
		  MakeTimeAccessor (&Sica::HelloExpireTime),
		  MakeTimeChecker())
    
    .AddAttribute("NeighborExpireTime","The maximum period of time that Sica keep neighbor information if it does not receive any new information from neighbor default 2*HelloInterval= 200 seconds",
		  TimeValue(Seconds(200)),
		  MakeTimeAccessor (&Sica::NeighborExpireTime),
		  MakeTimeChecker())
    
    .AddAttribute("SwitchingDelay","Delay for radio interface to switch from one channel to another default is 250mis",
		  TimeValue(MicroSeconds (300)),
		  MakeTimeAccessor (&Sica::SwitchingDelay),
		  MakeTimeChecker())
    
    .AddAttribute("ChannelAssignmentInterval","The period between channel assignment runs  default is 4*HelloInterval= 400s",
		  TimeValue(Seconds (400)),
		  MakeTimeAccessor (&Sica::ChannelAssignmentInterval),
		  MakeTimeChecker())
    
    .AddAttribute("ChannelSenseInterval","Sensing channel interval default is 0.5*ChannelAssignmentInterval= 200s ",
		  TimeValue(Seconds(200)),
		  MakeTimeAccessor (&Sica::ChannelSenseInterval),
		  MakeTimeChecker())
    
    .AddAttribute("ChannelSensePeriod","Sensing channel duration  default is 300 ms",
		  TimeValue(MilliSeconds(300)),
		  MakeTimeAccessor (&Sica::ChannelSensePeriod),
		  MakeTimeChecker())
    .AddAttribute("ChannelSenseRate","Sensing channel sampling rate  default is 1 ms",
		  TimeValue(MilliSeconds(1)),
		  MakeTimeAccessor (&Sica::ChannelSenseRate),
		  MakeTimeChecker())
    
    .AddAttribute("BxExpireTime","The maximum period of time that Sica keeps estimated external bandwidth consumption for a channel entry in channel list defualt is 4*SenseInterval= 400s",
		  TimeValue(Seconds (400)),
		  MakeTimeAccessor (&Sica::BxExpireTime),
		  MakeTimeChecker())
    
    .AddAttribute("ChannelBusyBackoffTime","The delay that Sica differ sending over a channel if it found it busy default is 8ms",
		  TimeValue(MilliSeconds (8)),
		  MakeTimeAccessor (&Sica::ChannelBusyBackoffTime),
		  MakeTimeChecker())
    .AddAttribute("QueuePollTime","The polling interval  that Sica used to check empty queue for data 1ms",
		  TimeValue(MilliSeconds (1)),
		  MakeTimeAccessor (&Sica::QueuePollTime),
		  MakeTimeChecker())
    .AddAttribute("BroadcastSendDelay","The maximum delay for broadcasting a packet in tight synchronized network",
		  TimeValue(NanoSeconds (10)),
		  MakeTimeAccessor (&Sica::m_bcastSendDelay),
		  MakeTimeChecker())
   .AddAttribute("TInterfaceSendDelay","The backoff delay for T interface after switching to a channel",
		  TimeValue(MicroSeconds (10)),
		  MakeTimeAccessor (&Sica::m_TInterfaceSendDelay),
		  MakeTimeChecker())
.AddAttribute("TMax","The Mazimum duration of time that  T interface stays in a channel after switching to a channel",
		  TimeValue(MilliSeconds (1)),
		  MakeTimeAccessor (&Sica::TMax),
		  MakeTimeChecker())

    .AddTraceSource ("RxOk", 
                     "Trace source indicating a packet has been received",
                     MakeTraceSourceAccessor (&Sica::m_sicaRxReceived),
                     "ns3::WifiPhyStateHelper::RxOk")
    .AddTraceSource ("TxOk", 
                     "Trace source indicating a packet has been sent",
                     MakeTraceSourceAccessor (&Sica::m_sicaTxSent),
                     "ns3::WifiPhyStateHelper::Tx")
    .AddTraceSource ("HelloSent", 
                     "Trace source indicating a Hello packet has been sent",
                     MakeTraceSourceAccessor (&Sica::m_sicaHelloSent),
                     "ns3::RegularWifiMac::TxOkHeader")
    // .AddTraceSource ("ChannelProbability", 
    //                  "Trace source indicating the channel probability has been changed",
    //                  MakeTraceSourceAccessor (&Sica:: m_sicaChannelProb))
  //   .AddTraceSource ("RxDrop", 
  //                    "Trace source indicating a packet has been dropped",
  //                    MakeTraceSourceAccessor (&Sica::m_sicaRxDropped),
  //                    "ns3::WifiPhy::PhyRxDrop")
  // .AddTraceSource ("TxDevOk", 
  //                    "Trace source indicating a packet has been sent",
  //                    MakeTraceSourceAccessor (&Sica::m_sicaTxDeviceSent))
  // .AddTraceSource ("RxDevOk", 
  //                    "Trace source indicating a packet has been received",
  //                    MakeTraceSourceAccessor (&Sica::m_sicaRxDevReceived))
    ;
  return tid;
}


//////////////////////~Sica
Sica::~Sica()   
{
}

//////////////////////NotifyRxReceived
void 
Sica::NotifyRxReceived (Ptr<Packet> packet)
{
 static int i=0;
 DelayJitterEstimation destim;
 destim.RecordRx (packet);	
 SicaHeader sHeader;
 packet->PeekHeader(sHeader);
 Time delay= Simulator::Now()-sHeader.GetOriginTime();
 m_sicaRxReceived(destim.GetLastDelay(),packet->Copy(),m_id,delay);
 NS_LOG_INFO("Sica node " << m_id <<" :"<<" Received Data as Destination from "<< sHeader.GetOrigin() <<++i);
}

//////////////////////NotifyTxSent
void 
Sica::NotifyTxSent (Ptr<Packet> packet)
{
  /// I add this because it is not obvious that why NS3 add this header to the packet and would not remove it.
  // LlcSnapHeader lHeader;
 static int i=0;
  // packet->RemoveHeader(lHeader);
  SicaHeader sHeader;
  packet->PeekHeader(sHeader);
  Time originTime=sHeader.GetOriginTime();
  if (sHeader.GetOrigin()==m_id)
    {
      m_sicaTxSent(packet->Copy(),m_id,originTime);
      NS_LOG_INFO("Sica node " << m_id <<" :"<<" Sent Data as Source " << ++i);
      
    }
}

//////////////////////NotifyHelloSent
void 
Sica::NotifyHelloSent (Ptr<Packet> packet)
{
  
  m_sicaHelloSent(packet,m_id,Simulator::Now());
}

//////////////////////NotifyChannelProbability
void 
Sica::NotifyChannelProbability (std::vector<double> channelProb)
{
  
  m_sicaChannelProb(m_id,channelProb,Simulator::Now());
}

//////////////////////NotifyRxDropped 
void 
// Sica::NotifyRxDropped (Ptr<Packet> packet,double snr, bool isEndOfFrame)
Sica::NotifyRxDropped (Ptr<Packet> packet,double snr)
{
  if (packet->GetSize()> 1000)
    {
      Ptr<Packet> p=packet->Copy();
      WifiMacHeader mHeader;
      p->RemoveHeader(mHeader);
      LlcSnapHeader lHeader;
      p->RemoveHeader(lHeader);
      // p->Print(std::cout); 
      m_sicaRxDropped(p,m_id,snr,m_rChannel);
      // if (!errorCallback.IsNull ())
      // {
      //   errorCallback (packet, snr, isEndOfFrame);
      // }
    }
    
 NS_LOG_INFO("Sica node " << m_id <<" :"<<" Error in receiving packet with signal to nois ratio "<< snr << " pcakt size "<< packet->GetSize() );
}

//////////////////////Initialize
void
Sica::Initialize()
{
  
  Ptr<Node> m_node = GetObject<Node>();
  m_sqNo=0;
  NS_ASSERT_MSG(m_node,"Using Sica without attaching to any node!");
  m_id=m_node->GetId();
  // Init Queues
  InitializeQueues();
  //Init interfaces
  InitializeInterfaces();
 //Init timers
  InitializeTimers();
 //Init channels
  InitializeChannel();
  //initial loss value
  for (uint32_t i=Min_CH; i<=Max_CH; i++)
    {
      m_loss.push_back(0);
    }
 //send hello to inform neighbors
  CreateHello();
  NS_LOG_INFO("Sica node " << m_id <<" :"<<" Initialize:");
  NS_LOG_INFO("--R interface address  "<<  m_rInterface->GetAddress());
  NS_LOG_INFO("--T interface address  "<<  m_tInterface->GetAddress());
  NS_LOG_INFO("--Current R channel  "<< m_rChannel );
  NS_LOG_INFO("--Gamma " << m_gamma );
  NS_LOG_INFO("--Beta " << m_beta );
  NS_LOG_INFO("--Alpha " << m_alpha );
  NS_LOG_INFO ("--New Channel for R is " << m_rNewChannel << " time to switch " << m_switchTimer.GetDelayLeft().GetMilliSeconds());
  NS_LOG_INFO("--ChannelAssignmentInterval " << ChannelAssignmentInterval.GetMilliSeconds() );
  NS_LOG_INFO("--HelloInterval " << HelloInterval.GetMilliSeconds());
  NS_LOG_INFO("--NeighborExpireTime "<< NeighborExpireTime.GetMilliSeconds());
  NS_LOG_INFO("--minSwitchDelay " << m_minSwitchDelay);
  NS_LOG_INFO("--maxSwitchDelay " << m_maxSwitchDelay);
  NS_LOG_INFO("--TMAX is  " << TMax);
  NS_LOG_INFO("--SwitchingDelay " << SwitchingDelay.Time::ToDouble((Time::Unit)1));
}

//////////////////////InitializeQueues
void 
Sica::InitializeQueues()
{
  
  for (uint32_t i=Min_CH; i<=Sica::Max_CH ; i++)
    {
      m_queue.CreatQueue (i);
      m_channel.UpdateChannel(i,Max_BW,0,0,BxExpireTime);
      m_channel.SetChannelWeight(i,1);
    }
}


//////////////////////InitializeInterfaces

void
Sica::InitializeInterfaces()
{
Ptr<Node> m_node = GetObject<Node>();
 NetDevice::ReceiveCallback  recevCallback=MakeCallback (&Sica::RecvPacket,this);
m_radio= m_node->GetNDevices()-1;
NS_ASSERT_MSG(m_radio==2,"SiCA is designed only for 2 radio interfaces for each node!");

 /// Initialize R-Interface
 m_rInterface= m_node->GetDevice(0);
 m_rInterface->SetReceiveCallback(recevCallback);
 NS_LOG_DEBUG("Sica node " << m_id <<" :"<<" R_interface is   "<< m_rInterface->GetAddress() );

 /// Initialize T-Interface
 m_tInterface=m_node->GetDevice(1);
 NS_LOG_DEBUG("Sica node " << m_id <<" :"<<" T_interface is   "<< m_tInterface->GetAddress() );
}

//////////////////////InitializeChannel
void 
Sica::InitializeChannel()
 {
   Ptr <WifiNetDevice>rInterface= m_rInterface->GetObject<WifiNetDevice>();
   Ptr<WifiPhy> wifiphy = rInterface->GetPhy();
  //  WifiPhy::RxErrorCallback errorCallback = MakeCallback(&Sica::NotifyRxDropped, this);
   errorCallback = MakeCallback(&Sica::NotifyRxDropped, this);
   wifiphy->SetReceiveErrorCallback(errorCallback);
  //  wifiphy->SetReceiveErrorCallback(MakeCallback(&Sica::NotifyRxDropped, this));
  //  UniformVariable uniRnd(Min_CH,Max_CH);
  //  Ptr<UniformRandomVariable> uniRnd = CreateObject<UniformRandomVariable> (Min_CH,Max_CH);
   Ptr<UniformRandomVariable> uniRnd = CreateObject<UniformRandomVariable> ();
   uniRnd->SetAttribute ("Min", DoubleValue (Min_CH));
   uniRnd->SetAttribute ("Max", DoubleValue (Max_CH));
  //  m_rChannel= static_cast<uint32_t>(uniRnd.GetInteger(Min_CH,Max_CH));
   m_rChannel= static_cast<uint32_t>(uniRnd->GetInteger(Min_CH,Max_CH));
   wifiphy->SetChannelNumber(m_rChannel);
   // m_rChannel= wifiphy->GetChannelNumber();
   m_rNewChannel= m_rChannel;
   // m_rNewChannel=SelectRandomChannel();
   // if (m_rNewChannel != m_rChannel)
   //   ScheduleSwitchRInterface();
 }

//////////////////////InitializeTimers

void 
Sica::InitializeTimers()
 {
   ///hello  timer
   m_helloTimer.SetDelay(HelloInterval);
   m_helloTimer.SetFunction(&Sica::CreateHello,this);
   m_helloTimer.Schedule (); 
   ///sense timer 
   m_channelSenseTimer.SetDelay(ChannelSenseInterval);
   m_channelSenseTimer.SetFunction(&Sica::StartSenseCurrentChannel,this);
   m_channelSenseTimer.Schedule();
   /// sensing rate timer 
   m_channelSenseRateTimer.SetDelay(ChannelSenseRate);
  m_channelSenseRateTimer.SetFunction(&Sica::SenseCurrentChannel,this);
  ///neighbor switch timer, It would be scheduled  when a node found switching information in hello messages
  m_niSwitchTimer.SetFunction(&Sica::HandleNeighborSwitchChannel,this);
  /// R interface switch timer
  m_minSwitchDelay=2*HelloInterval.GetMilliSeconds();
  m_maxSwitchDelay=3*HelloInterval.GetMilliSeconds();
  m_switchTimer.SetFunction(&Sica::SwitchRInterface,this);
  /// channel assignment timer
  m_CATimer.SetDelay(ChannelAssignmentInterval);
  m_CATimer.SetFunction(&Sica::GameChannelAssignment,this);
  m_CATimer.Schedule();
 
  // T interface send Timer
  // TMax=MilliSeconds(static_cast <uint64_t>(HelloInterval.Time::ToDouble((Time::Unit)1)/Max_CH));
  m_TInterfaceSendTimer.SetDelay(TMax);
  m_TInterfaceSendTimer.SetFunction(&Sica::TInterfaceStartSend,this);
  m_TInterfaceSendTimer.SetArguments(Min_CH);
  m_TInterfaceSendTimer.Schedule();
  //polling  r_channel timer
  m_rInterfacePollTimer.SetDelay(QueuePollTime);
  m_rInterfacePollTimer.SetFunction(&Sica::RInterfaceCheckChannelQueue,this);
  m_rInterfacePollTimer.Schedule();

 }

//////////////////////GetTChannel
uint32_t 
Sica::GetTChannel()
 {
   Ptr <WifiNetDevice>tInterface= m_tInterface->GetObject<WifiNetDevice>();
   Ptr<WifiPhy> wifiphy = tInterface->GetPhy();
   return(wifiphy->GetChannelNumber());
 }

//////////////////////GetSicaNeighbors
SicaNeighbors *
Sica::GetSicaNeighbors()
{
  return(&m_nb);
}


//////////////////////GetSicaQueue
SicaQueue *
Sica::GetSicaQueue()
{
  return (&m_queue);
}


//////////////////////GetSicaChannel
SicaChannels *
Sica::GetSicaChannel()
{
  return (&m_channel);
}



//////////////////////PrintNeighborTable
void 
Sica::PrintNeighborTable(std::ostream &os)
{
  m_nb.PrintNeighborTable(os);
}


//////////////////////PrintChannelTable
void 
Sica::PrintChannelTable(std::ostream &os)
{
  m_channel.PrintChannel(os);
}

//////////////////////ChannelIsBusy
bool 
Sica::ChannelIsBusy(uint32_t chId )
{  
  Ptr<ChannelEmu> emu=GetChannelEmu(chId);
 if (emu){
   if (emu->IsBusy())
    return true;
   else 
    return false;
 }
 else {
   NS_LOG_DEBUG( "Sica node " << m_id <<" :"<<"There is no emulator aggrigated to channel ");
   return false;
 }
}

//////////////////////ModifySenseChannelFlag
void
Sica::ModifySenseChannelFlag(uint32_t c)
{
  if (m_channel.IsBeingSensed(c))
    m_channel.ResetChannelSenseFlag(c);
  else 
    m_channel.SetChannelSenseFlag(c);
  return;
}
//////////////////////StartSenseCurrentChannel
void 
Sica::StartSenseCurrentChannel()
{
  m_channelSenseTimer.Cancel ();
  m_channelSenseTimer.SetDelay(ChannelSenseInterval);
  m_channelSenseTimer.Schedule();
  if (m_switchTimer.IsRunning() && m_switchTimer.GetDelayLeft()< ChannelSensePeriod)
    {
      NS_LOG_INFO( "Sica node " << m_id <<" :"<<"Very close switching attempt no time to sense "<< m_rChannel); 
    }
  else {
    NS_LOG_INFO("Sica node " << m_id <<" :"<<"Sensing starts for channel "<< m_rChannel);
    m_channelSenseFlag=true;
    ModifySenseChannelFlag(m_rChannel);
    m_idleChTime=MilliSeconds(0);
    m_busyChTime=MilliSeconds(0);
    Simulator::Schedule(ChannelSensePeriod,&Sica::EndSenseCurrentChannel,this);
    SenseCurrentChannel();
  }
  return;
}



//////////////////////SenseCurrentChannel
void 
Sica::SenseCurrentChannel()
{
  if(ChannelIsBusy(m_rChannel))
    m_busyChTime+=ChannelSenseRate;
  else 
    m_idleChTime+=ChannelSenseRate; 
  
  m_channelSenseRateTimer.Cancel ();
  m_channelSenseRateTimer.Schedule();
  return;
}

////////////////////// EndSenseCurrentChannel
void 
Sica::EndSenseCurrentChannel()
{
  m_channelSenseRateTimer.Cancel ();
  ModifySenseChannelFlag(m_rChannel);
  m_channelSenseFlag=false;
  double bx;
  double tBusy=m_busyChTime.Time::ToDouble((Time::Unit)1);
  double tIdle=m_idleChTime.Time::ToDouble((Time::Unit)1);
  bx=(tBusy/(tBusy+tIdle))*Max_BW;
  m_channel.SetChannelExtBandwidth(m_rChannel,static_cast<uint32_t>(bx),BxExpireTime);
  NS_LOG_DEBUG( "Sica node " << m_id <<" :"<<"Sensing Finished, T_busy= "<<tBusy << " T_idle= "<< tIdle << " Bx= " << static_cast<uint32_t>(bx) );
  return;
}


//////////////////////ReScheduleTimer
void 
Sica::ReScheduleTimer(Timer *t, Time minDelay )
 {
    //get the current delay left to switch timer 
   Time timerDelayLeft=t->GetDelayLeft();
    if (minDelay.IsStrictlyPositive() && (timerDelayLeft.IsZero() || timerDelayLeft >  minDelay))
      {
	// The timer should expired on minDelay
	if (t->IsRunning()){
	  t->Cancel();
	}
	t->SetDelay(minDelay);
	t->Schedule();	  
      } // if 	
 }



//////////////////////RecvPacket
bool
Sica::RecvPacket(Ptr<NetDevice> dstDevice,Ptr<const Packet> packet ,uint16_t protocolNumber ,const Address & srcAddr)
{
  if (protocolNumber== SICA_DATA_PORT)
    {
      NS_LOG_DEBUG ( "Sica node " << m_id <<" :"<<"One data  packet received at rInterface  "<< dstDevice->GetAddress());
      ProcessRcvData(packet->Copy());
      uint32_t rch=dstDevice->GetObject<WifiNetDevice>()->GetPhy()->GetChannelNumber();
      m_sicaRxDevReceived(packet->Copy(),m_id,rch);
	return true;
    }
  if (protocolNumber == SICA_HELLO_PORT)
    {
    NS_LOG_DEBUG ( "Sica node " << m_id <<" :"<<"One Hello packet received at rInterface  "<< dstDevice->GetAddress());
    ProcessRcvHello(packet->Copy(),srcAddr);
	return true;
    }
   NS_LOG_DEBUG ( "Sica node " << m_id <<" :"<<"One unrecognized  packet received at rInterface  "<< dstDevice->GetAddress());
  return false;
  
}



//////////////////////ProcessRcvHello
void
Sica::ProcessRcvHello(Ptr<Packet> p, Address srcAddr)
{
  SicaHelloHeader sHeader;
  Address niTAddr=srcAddr;
  p->RemoveHeader(sHeader);
  if (sHeader.IsValid()){
    NS_LOG_DEBUG("Sica node " << m_id <<" :"<< "Hello received from " << sHeader.GetOrigin()  << " SqNo "<< sHeader.GetSeqNo());
    //sHeader.Print(std::cout);
    // switch time of the neighbor to another channel
    Time niSwitchTime =sHeader.GetTimeToSwitch();
    
    if (UpdateNeighborTable(sHeader,niTAddr)){
      // Update channel table by new information    
      UpdateChannelTable(sHeader);
      // Update the number of neighbors on channels for all channels
      for (uint32_t i=Min_CH; i<= Max_CH; i++)
	m_channel.SetChannelNeighbors(i,m_nb.GetNiOnChannel(i));
      // reschedule switch timer if needed
      if (niSwitchTime.IsStrictlyPositive())
	NS_LOG_DEBUG("Sica node " << m_id <<" :"<< "Neighbor switch after  " << niSwitchTime.GetMilliSeconds());
      ReScheduleTimer(&m_niSwitchTimer,niSwitchTime);
    }// If update 
  }//if isvalid
  else  
  NS_LOG_DEBUG( "Sica node " << m_id <<" :"<<"Got an invalid hello header");
  return;
}//ProcessRcvHello
  

//////////////////////ProcessRcvData
void 
Sica::ProcessRcvData(Ptr<Packet> p)
{
  SicaHeader sHeader;
  p->PeekHeader(sHeader);
  // if there is any bug related to packet tags uncomments these two lines
// p->RemoveAllPacketTags ();
//   p->RemoveAllByteTags ();
  uint32_t dstId=sHeader.GetDest();
  uint32_t srcId=sHeader.GetOrigin();
  if (dstId==m_id){
    NS_LOG_INFO( "Sica node " << m_id <<" :"<< "Data packet received of size "<< p->GetSize());
    NotifyRxReceived(p->Copy());
    return;
  }
  else 
    {
       p->RemoveHeader(sHeader);
      int nextHopId= AddDataHeaders(p,srcId,dstId,sHeader.GetOriginTime());
      if (nextHopId != -1)
	{
	  uint32_t nextHopChannel= static_cast<uint32_t>(m_nb.GetNiChannel(nextHopId));
	DistributeDataPacket(p,nextHopChannel); 
	}
    }
}



//////////////////////UpdateNeighborTable
bool 
Sica::UpdateNeighborTable(SicaHelloHeader sicaHelloHeader, Address srcAddr)
{
 uint32_t niId=sicaHelloHeader.GetOrigin();
 if (niId==m_id) return false;
 uint32_t niHopCounts =1;
 Address niRAddr=sicaHelloHeader.GetRAddress();
 Address niTAddr=srcAddr;
 //Current channel of neighbor who sent hello
 uint32_t niRChannel=sicaHelloHeader.GetRChannel();
 //The previos channel of neighbor according to neighbor channel
 int32_t niRPrevChannel=static_cast<uint32_t>(m_nb.GetNiChannel(niId));
 // The new channel to which the neighbor will switch according to hello information
 uint32_t niRNewChannel=sicaHelloHeader.GetRNewChannel();
 //number of radios it has
 uint32_t niRadio=sicaHelloHeader.GetRadios();
 // switch time of the neighbor to another channel
 Time niSwitchTime =sicaHelloHeader.GetTimeToSwitch();
 // Time  stamp of hello message
 Time helloTimestamp=sicaHelloHeader.GetOriginTime();
 // whether update happens or not according to time stamp
 bool updateFlag= false;
 //used to get 2 hops information from hello
 std::pair<uint32_t, uint8_t>  niInf;

 //check hello information 
 if (niRChannel>Max_CH || niRChannel<Min_CH || niRNewChannel>Max_CH || niRNewChannel<Min_CH )
   {
   NS_LOG_DEBUG( "Sica node " << m_id <<" :"<<"Got invalid information from Hello message ");  
   return updateFlag;
   }
 NS_LOG_DEBUG( "Sica node " << m_id <<" :"<<" UpdateNeighborTable: for mac address  "<< niRAddr );
 updateFlag= m_nb.Update(niId,niHopCounts,niRadio,niRChannel,niRAddr,niTAddr ,helloTimestamp,niSwitchTime,niRNewChannel);
 
 if (updateFlag)
   {
     NS_LOG_DEBUG("Sica node " << m_id <<" :"<<"Neighbor information is updated for neighbor ID "<<niId);
     NS_LOG_DEBUG("--neighbor R-channel "<< niRChannel);
     NS_LOG_DEBUG("--neighbor prev R-channel "<< niRPrevChannel);
     NS_LOG_DEBUG("--neighbor hopcounts "<< niHopCounts);
     if (niRPrevChannel !=-1 && static_cast<uint32_t>(niRPrevChannel)!= niRChannel) // Information pushed into  neighbor table and  the  neighbor changes its channel immediately, check packet 
       m_queue.ShuffleData(static_cast<uint32_t>(niRPrevChannel),niRChannel,niId);
    while (sicaHelloHeader.RemoveNiRChannel(niInf))
      {
	niId= niInf.first;
	if(niId != m_id)
	  {
	  bool updateFlag2= false;
	  niRChannel= niInf.second;
	  niRPrevChannel= static_cast<uint32_t>(m_nb.GetNiChannel(niId));
	  niRadio=1; // we suppose that two hops neighbor has one radio
	  niHopCounts=2;
	  niRAddr=Address();// We do not need  the  address of the two hops neighbor 
	  niTAddr=Address();
	  niRNewChannel=niRPrevChannel;// We do not have switching information of two hops neighbor

	  niSwitchTime=MilliSeconds(0);
	  updateFlag2= m_nb.Update(niId,niHopCounts,niRadio,niRChannel,niRAddr ,niTAddr,helloTimestamp,niSwitchTime,niRNewChannel);
	  if (updateFlag2 && m_nb.GetNiHops(niId)==1 && niRPrevChannel !=-1 && static_cast<uint32_t>(niRPrevChannel)!= niRChannel){
	    /// It means We have updated information for one-hop neighbor changes its channel immediately, check packet.
	    /// Do packet exchange before update
	    m_queue.ShuffleData(static_cast<uint32_t>(niRPrevChannel),niRChannel,niId);
	  }
	 
	  NS_LOG_DEBUG( "Sica node " << m_id <<" :"<<"Neighbor information is updated for neighbor ID  "<<niInf.first);
	}//if
      }//while 
    //PrintNeighborTable(std::cout);
    return (true);
   }// if flag
 else // hello message is older than current information
   return (false);
}

//////////////////////UpdateChannelTable
bool 
Sica::UpdateChannelTable(SicaHelloHeader sicaHelloHeader)
{
 //Current channel of neighbor who sent hello
  uint32_t niRChannel=sicaHelloHeader.GetRChannel();
  //Estimation external bandwidth by neighbor
  uint32_t niEstimationBx=static_cast <uint32_t>(sicaHelloHeader.GetExtBw());
  // number of neighbors on channel
  uint32_t niOnChannel=m_nb.GetNiOnChannel(niRChannel);
  Time niSenseTime=sicaHelloHeader.GetSenseTime();
  m_channel.UpdateChannel(niRChannel,Max_BW,niEstimationBx,niOnChannel,BxExpireTime);
  NS_LOG_DEBUG( "Sica node " << m_id <<" :"<< "Channel Information is updated for channel  " << niRChannel );
  NS_LOG_DEBUG("--Bx  " <<  niEstimationBx );
  NS_LOG_DEBUG("--Number of Ni on channel " << niOnChannel );
  NS_LOG_DEBUG("--Bandwidth information expired after "<< BxExpireTime.GetMilliSeconds());

  if (niSenseTime.IsStrictlyPositive())
    {
      if( niRChannel != m_rChannel)
	{
	  // set the sense flag for the neighbor channel to stop sending on it
	  Simulator::Schedule(niSenseTime,&Sica::ModifySenseChannelFlag,this, static_cast<uint32_t>(niRChannel));
	  // reset the sense flag for the neighbor channel after sense period
	  Simulator::Schedule(niSenseTime+ChannelSensePeriod,&Sica::ModifySenseChannelFlag,this,static_cast<uint32_t>(niRChannel));
	}
      else if  (niRChannel == m_rChannel)// If we have the same channel, set the sense timer to the shortest time
	ReScheduleTimer(&m_channelSenseTimer,niSenseTime);
    }
  return true;
}





//////////////////////SendHello
void 
Sica::CreateHello()
{
  if(m_helloTimer.IsRunning())
    m_helloTimer.Cancel ();
  m_helloTimer.Schedule ();
  SicaHelloHeader sHeader=CreateHelloHeader();
  Ptr<Packet> p= Create<Packet>(100);
  p->AddHeader(sHeader);
  DistributeHello(p);
}


//////////////////////CreateHelloHeader
SicaHelloHeader
Sica::CreateHelloHeader()
{
  // Create  hello header 
  uint32_t niCh;
  uint32_t niId;
  Address rAddr= m_rInterface->GetAddress();
  uint32_t bx= m_channel.GetChannelExtBandwidth(m_rChannel);
  Time switchTime= m_switchTimer.GetDelayLeft();
  Time  senseTime= m_channelSenseTimer.GetDelayLeft();
  SicaHelloHeader sHeader(++m_sqNo,m_id,Simulator::Now(),m_radio,m_rChannel,bx,m_rNewChannel,rAddr,switchTime,senseTime);
  
  m_nb.RmvExpiredNi(NeighborExpireTime);
  for (uint32_t i = 1; i <= m_nb.GetNiNo(); i++)
    {
      if (m_nb.IsDirectNeighborByIndex(i)){
  	niId = m_nb.GetNeighborIdByIndex(i);
  	niCh = static_cast<uint32_t>(m_nb.GetNiChannelByIndex(i));
  	sHeader.AddNiRChannel(niId,niCh);
      }
    }
  NS_LOG_DEBUG( "Sica node " << m_id <<" :"<< "One Hello is created with source Mac addr   " << rAddr<< "  and sequence number "<<sHeader.GetSeqNo());
  return sHeader;
}


//////////////////////DistributeHello
void 
Sica::DistributeHello(Ptr<Packet> p)
{
  uint32_t niChannel[Max_CH]; // number of direct neighbors on available  channels
   for (uint32_t j=Min_CH;j<=Max_CH;j++)
     niChannel[j]=0;
 // Create queue entry with packet and Hello_type
  SicaQueueEntry::PacketType ptype=SicaQueueEntry::Hello_Type;
  SicaQueueEntry *ent = new SicaQueueEntry(p,ptype);
  ent->SetExpireTime(Sica::HelloExpireTime);
  // Push packet into queue where there is a neighbor
  NS_LOG_DEBUG( "Sica node " << m_id <<" :"<< "Hello will be send over channel   ");
  if (m_nb.GetNiNo())
    {
      for (uint32_t i = 1; i <=m_nb.GetNiNo(); i++){
	if (m_nb.IsDirectNeighborByIndex(i))
	  {
	    niChannel[static_cast<uint32_t>(m_nb.GetNiChannelByIndex(i))]++;
	  }//if
      }//for
      for (uint32_t j=Min_CH;j<=Max_CH;j++)
	if (niChannel[j]>0){
	  m_queue.Enqueue(j,*(&ent));
	  NS_LOG_DEBUG("-- "<< j);
	}
    }
  else // there is no neighbortherefore push hello to the receiving interface channel
    {
      for (uint32_t j=Min_CH;j<=Max_CH;j++){
      m_queue.Enqueue(j,*(&ent));
      NS_LOG_DEBUG("-- "<< j);
      }
    }
  
  return;
}


//////////////////////CreateData
void 
Sica::CreateData(uint32_t dstId, uint32_t pSize)
{
  Ptr<Packet> p= Create<Packet>(pSize);
  DelayJitterEstimation destim;
  /// Find next hop to the destination
  int nextHopId= AddDataHeaders(p,m_id,dstId,Simulator::Now());
  if (nextHopId != -1){
    destim.PrepareTx(p);
    NotifyTxSent (p->Copy());
    uint32_t nextHopChannel= static_cast<uint32_t>(m_nb.GetNiChannel(nextHopId));
    DistributeDataPacket(p,nextHopChannel); 
    NS_LOG_INFO( "Sica node " << m_id <<" :"<< "Data created of size "<< pSize << " to send to  neighbor  id "<< dstId << "through next hop Id " << nextHopId );
    return;
  }
}


//////////////////////AddDataHeaders
int 
Sica::AddDataHeaders(Ptr<Packet> p,uint32_t srcId, uint32_t dstId, Time originTime)
 {
   uint32_t nextHopId=GetObject<Node> ()->GetObject<RTable> ()->FindNextHop(m_id,dstId);
  SicaHeader sHeader(++m_sqNo,srcId,dstId,nextHopId,originTime);
  p->AddHeader(sHeader);
  if (m_nb.GetNiRAddress(nextHopId).IsInvalid())
    {
      NS_LOG_INFO( "Sica node " << m_id <<" :"<< "No information for neighbor Id > "<<dstId );	
      return -1;
    }
 return nextHopId;
 }


//////////////////////DistributeDataPacket
 void 
 Sica::DistributeDataPacket(Ptr<Packet> p,uint32_t nextHopChannel)
 {
   SicaQueueEntry::PacketType ptype=SicaQueueEntry::Data_Type;
   SicaQueueEntry *qEntry = new SicaQueueEntry(p,ptype);
   qEntry->SetExpireTime(Sica::DataExpireTime);
   m_queue.Enqueue(nextHopChannel,*(&qEntry));
   NS_LOG_INFO( "Sica node " << m_id <<" :"<< "Data sent to queue with size "<< p->GetSize()<< " over channel "<< nextHopChannel );
  return;
 }

//////////////////////SendPacket
void 
Sica::SendPacket(Ptr<Packet> packet,Ptr <NetDevice> device , uint32_t protocolNumber)
{
 
  //// broadcast hello with a random delay to avoid collision in a tight synchronized network
  if (protocolNumber==SICA_HELLO_PORT)
    {
      // UniformVariable uniRnd(0,m_bcastSendDelay.Time::ToDouble((Time::Unit)3));
      // Ptr<UniformRandomVariable> uniRnd = CreateObject<UniformRandomVariable> (0,m_bcastSendDelay.Time::ToDouble((Time::Unit)3));
      Ptr<UniformRandomVariable> uniRnd = CreateObject<UniformRandomVariable> ();
      uniRnd->SetAttribute ("Min", DoubleValue (0));
      uniRnd->SetAttribute ("Max", DoubleValue (m_bcastSendDelay.Time::ToDouble((Time::Unit)3)));
      // uint64_t delaySend=  static_cast<uint64_t>(uniRnd.GetValue());
      uint64_t delaySend=  static_cast<uint64_t>(uniRnd->GetValue());
      NS_LOG_INFO( "Sica node " << m_id <<" :"<< " hello will be  sent after  "<< delaySend << " ns " );
      Simulator::Schedule(NanoSeconds(delaySend), &Sica::DeviceSend ,this , device , packet,device->GetBroadcast(),SICA_HELLO_PORT);
      return;
    }
  else {
  SicaHeader sHeader;
  // copy header information to sheader
  packet->PeekHeader(sHeader);
  // extract destination address
    uint32_t nextHopId= sHeader.GetNextHop();
    Address nextHopAddr=m_nb.GetNiRAddress(nextHopId);
    DeviceSend(device,packet,nextHopAddr,SICA_DATA_PORT);
    uint32_t m_ch=device->GetObject<WifiNetDevice>()->GetPhy()->GetChannelNumber();
    m_sicaTxDeviceSent(packet->Copy(),m_id,nextHopId,m_ch,Simulator::Now());
    }
}

//////////////////////EstimateTxDuration
Time 
Sica::EstimateTxDuration(uint32_t pSize,Ptr<WifiPhy> wifiphy)
{
  WifiTxVector txVector;
  txVector.SetMode (WifiPhy::GetDsssRate11Mbps ());
  txVector.SetPreambleType ((WifiPreamble)WIFI_PREAMBLE_SHORT);
  // Time txEstimation=wifiphy->CalculateTxDuration(pSize,wifiphy->GetMode(0), (WifiPreamble)WIFI_PREAMBLE_SHORT);
  // Time txEstimation=wifiphy->CalculateTxDuration(pSize, txVector, wifiphy->GetFrequency(), (WifiPreamble)WIFI_PREAMBLE_SHORT, 0);
  Time txEstimation=wifiphy->CalculateTxDuration(pSize, txVector, wifiphy->GetFrequency());
  
  return (txEstimation);
}



//////////////////////DeviceSend
void 
Sica::DeviceSend(Ptr<NetDevice> device, Ptr<Packet> packet,Address dstAddr,uint32_t protocolNumber)
{
 device->Send(packet,dstAddr,protocolNumber);
 // if (protocolNumber== SICA_DATA_PORT )
 //   {
 //   m_sicaTxDeviceSent(packet->Copy(),m_id,Simulator::Now());
 //   }
 if (protocolNumber== SICA_HELLO_PORT ) 
   {
     NotifyHelloSent (packet->Copy());
   }
 NS_LOG_DEBUG ("Sica node " << m_id <<" :"<< "Sends  a Sica packet with port number   " << protocolNumber << " to " << dstAddr);
}


/****************Sica Channel switching  methods*/

//////////////////////HandleNeighborSwitchChannel
void 
Sica::HandleNeighborSwitchChannel()
{
  uint32_t niId;
  uint32_t niNewCh; // neighbor new channel
  uint32_t niCurrCh;// neighbor current channel, before switch
  Time niSwTime;
  Time minSwTime=MilliSeconds(0);
  m_nb.RmvExpiredNi(NeighborExpireTime);
  for (uint32_t i=1; i<=m_nb.GetNiNo(); i++){
    niId=m_nb.GetNeighborIdByIndex(i);
    niSwTime=m_nb.GetNiSwitchTime(niId);
    niCurrCh=static_cast<uint32_t>(m_nb.GetNiChannel(niId));
    niNewCh=static_cast<uint32_t>(m_nb.GetNiNewChannel(niId));
    // Check whether there was a switch initiated and the current and previous channels are not the same
    if (niNewCh != niCurrCh && niNewCh<=Max_CH && niNewCh>=Min_CH){
      if (niSwTime.IsZero() || niSwTime.IsNegative())
	{
	  NS_LOG_DEBUG( "Sica node " << m_id <<" :"<<"One neighbor has switched\n --"<<niId<< "\n --Previous channel " <<niCurrCh << "\n --Current channel "<< niNewCh);
	  /// ShuffleData packets 
	  m_queue.ShuffleData(niCurrCh,niNewCh,niId);
	  m_channel.IncChannelNeighbors(niNewCh);
	  m_channel.DecChannelNeighbors(niCurrCh);
	  /// Neighbor information updates
	  m_nb.SetNiChannel(niId,niNewCh);
	  m_nb.SetNiNewChannel(niId,niNewCh);
	}//if niSwTime
      else // Switch time is positive node would not switch yet 
	if (minSwTime.IsZero() || niSwTime < minSwTime)
	  {
	  minSwTime=niSwTime;
	  }
    }// if ch
  }//for
  // Re-schedule the switch timer for next time
  ReScheduleTimer(&m_niSwitchTimer,minSwTime);
}


//////////////////////ScheduleSwitchRInterface
void 
Sica::ScheduleSwitchRInterface()
{
  Ptr <WifiNetDevice>rInterface= m_rInterface->GetObject<WifiNetDevice>();
  Ptr<WifiPhy> wifiphy = rInterface->GetPhy();
  Time delayToIdle = MilliSeconds(0);
  // if (wifiphy->IsStateBusy()){
  if (wifiphy->IsStateCcaBusy()){
    delayToIdle= wifiphy->GetDelayUntilIdle();	
    NS_LOG_DEBUG ( "Sica node " << m_id <<" :"<< "Phy is busy to switch it would be idel after " <<delayToIdle.GetMicroSeconds() );
  }
  // UniformVariable uniRnd;
  Ptr<UniformRandomVariable> uniRnd = CreateObject<UniformRandomVariable> ();
  // uint32_t rndDelay=  uniRnd.GetInteger(m_minSwitchDelay,m_maxSwitchDelay);
  uint32_t rndDelay=  uniRnd->GetInteger(m_minSwitchDelay,m_maxSwitchDelay);
  Time switchDelay=MilliSeconds(rndDelay)+delayToIdle;
  NS_LOG_DEBUG ( "Sica node " << m_id <<" :"<< "R interface will switch to channel "<< m_rNewChannel << " after " <<switchDelay.GetMilliSeconds() );
  ReScheduleTimer(&m_switchTimer,switchDelay); 
}


//////////////////////SwitchRInterface
void 
Sica::SwitchRInterface()
{
  m_switchTimer.Cancel();
  Ptr <WifiNetDevice>rInterface= m_rInterface->GetObject<WifiNetDevice>();
  Ptr<WifiPhy> wifiphy = rInterface->GetPhy();
  wifiphy->SetChannelNumber(m_rNewChannel);
  //wifiphy->SetChannel(m_channelObjects[m_rNewChannel]);
  NS_LOG_DEBUG ( "Sica node " << m_id <<" :"<< "Switch R interface  from " << m_rChannel <<" to channel  " << m_rNewChannel);
  m_rChannel=m_rNewChannel;
  return;
}
//////////////////////SwitchTInterface
bool 
Sica::SwitchTInterface(uint32_t c)
{
  Ptr <WifiNetDevice>tInterface= m_tInterface->GetObject<WifiNetDevice>();
  Ptr<WifiPhy> wifiphy = tInterface->GetPhy();
  if (wifiphy->GetChannelNumber()== c)
    return true;
  // if (wifiphy->IsStateBusy())
  if (wifiphy->IsStateCcaBusy())
    return false;
  // Change the channel of the Tinterface
  wifiphy->SetChannelNumber(c);
  NS_LOG_DEBUG( "Sica node " << m_id <<" :"<< " switches the T Interface to channel "<< wifiphy->GetChannelNumber()  << " to send data");
  return true;
}


//////////////////////TInterfaceStartSend
void
Sica::TInterfaceStartSend(uint32_t ch)
{
  m_TInterfaceSendTimer.Cancel();
  m_TInterfaceSendTimer.SetDelay(TMax);
  bool datatosend=false;
  SicaQueueEntry::PacketType dataType=SicaQueueEntry::Data_Type;
  SicaQueueEntry::PacketType helloType=SicaQueueEntry::Hello_Type;
  while (ch <=  Max_CH && datatosend==false)
    {
      if (m_queue.GetSize(ch,helloType) > 0 || m_queue.GetSize(ch,dataType) > 0)
      {
        if (SwitchTInterface(ch))
          {
            datatosend=true;
            Simulator::Schedule(SwitchingDelay+m_TInterfaceSendDelay,&Sica::TInterfaceSend,this,ch);
          }
        else 
          ch++;
      }
    else
      {
	datatosend=false;
        ch++;
      }
    }//while
  if (ch >= Max_CH)
    m_TInterfaceSendTimer.SetArguments(Min_CH);
  else{
    m_TInterfaceSendTimer.SetArguments(++ch);
    // if (m_queue.GetSize(ch,helloType) > 0 && m_queue.GetSize(ch,dataType)== 0)
    //   {
    // 	Ptr <WifiNetDevice>tInterface= m_tInterface->GetObject<WifiNetDevice>();
    // 	Ptr<WifiPhy> wifiphy = tInterface->GetPhy();	
    // 	m_TInterfaceSendTimer.SetDelay(SwitchingDelay+m_TInterfaceSendDelay+EstimateTxDuration(200,wifiphy));
      // }
  }
  
  m_TInterfaceSendTimer.Schedule();
}


//////////////////////TInterfaceSend
void 
Sica::TInterfaceSend(uint32_t ch)
{
  Time txEstimation;
 Time endSendTime=Seconds(0);
  uint32_t maxPacketSize= 1054;
  SicaQueueEntry* qEntry;
  SicaQueueEntry::PacketType ptype;
  uint32_t protocolNumber;
  uint32_t dataQueueSize=m_queue.GetSize(ch,SicaQueueEntry::Data_Type);
  uint32_t helloQueueSize=m_queue.GetSize(ch,SicaQueueEntry::Hello_Type);
  Ptr <WifiNetDevice>tInterface= m_tInterface->GetObject<WifiNetDevice>();
  Ptr<WifiPhy> wifiphy = tInterface->GetPhy();
  uint32_t sentCount=0;
  if (helloQueueSize>0)
    txEstimation=EstimateTxDuration(200,wifiphy);
  else 
    txEstimation=EstimateTxDuration(maxPacketSize,wifiphy);
  while ((helloQueueSize>0 || dataQueueSize>0 )&& TInterfaceReadyToSend(ch,txEstimation))
    {
      sentCount++;

      if (helloQueueSize>0)
	    {
	    ptype=SicaQueueEntry::Hello_Type;
	    protocolNumber=SICA_HELLO_PORT;
	    endSendTime=endSendTime+txEstimation+m_bcastSendDelay;
	    }
	  else 
	    {
	    ptype=SicaQueueEntry::Data_Type;
	    protocolNumber=SICA_DATA_PORT;
	    endSendTime+=txEstimation;
	    }
	  qEntry= m_queue.Dequeue(ch,ptype);
	  if ( qEntry)
	    {
	      SendPacket(qEntry->GetPacket()->Copy(),m_tInterface,protocolNumber);
	      m_queue.EraseFront(ch,ptype);
	    }
	  /// I need to update it because of some expired packets
	  helloQueueSize=m_queue.GetSize(ch,SicaQueueEntry::Hello_Type);
	  dataQueueSize=m_queue.GetSize(ch,SicaQueueEntry::Data_Type);
	  if (helloQueueSize>0)
 	    txEstimation=EstimateTxDuration(200,wifiphy);
 	  else if (dataQueueSize>0)
 	    txEstimation=EstimateTxDuration(maxPacketSize,wifiphy);
	  
    }
  // if (txEstimation < m_TInterfaceSendTimer.GetDelayLeft() && dataQueueSize==0)
  //   Simulator::Schedule(MilliSeconds(1),&Urbanx::TInterfaceSend,this,ch);
  if (sentCount)
  NS_LOG_DEBUG(  "Sica node " << m_id  <<" :"<< sentCount << " packets sent to device for  channel "<< ch);
  ReScheduleTimer(&m_TInterfaceSendTimer,endSendTime);
  return;
}


//////////////////////TInterfaceReadyToSend
bool 
Sica::TInterfaceReadyToSend(uint32_t ch,Time txEstimation)
{
 Time timeToEndDuration=m_TInterfaceSendTimer.GetDelayLeft();
 Ptr <WifiNetDevice>tInterface= m_tInterface->GetObject<WifiNetDevice>();
 Ptr<WifiPhy> wifiphy = tInterface->GetPhy();
 if (m_channel.IsBeingSensed(ch))  return false;
 if ( ChannelIsBusy(ch))  return false;
 if (wifiphy->IsStateSwitching())  return false;
 if (txEstimation > timeToEndDuration) return false;
 return true;
}



//////////////////////////RInterfaceCheckChannelQueue
void 
Sica::RInterfaceCheckChannelQueue()
 {
   m_rInterfacePollTimer.Cancel();
   Time txEstimation;
   uint32_t maxPacketSize= 1054;
   uint32_t sentCount=0;
   SicaQueueEntry::PacketType ptype;
   SicaQueueEntry* qEntry;
   uint32_t protocolNumber;
   uint32_t dataQueueSize=m_queue.GetSize(m_rChannel,SicaQueueEntry::Data_Type);
   uint32_t helloQueueSize=m_queue.GetSize(m_rChannel,SicaQueueEntry::Hello_Type);
   Ptr <WifiNetDevice>rInterface= m_rInterface->GetObject<WifiNetDevice>();
   Ptr<WifiPhy> wifiphy = rInterface->GetPhy();
   if (helloQueueSize>0)
     txEstimation=EstimateTxDuration(200,wifiphy);
   else 
     txEstimation=EstimateTxDuration(maxPacketSize,wifiphy);
   // The transmission will start if there is no switching timer nor sense timer is set or we have enough time to any of these event
   while ((helloQueueSize>0 || dataQueueSize>0) && RInterfaceReadyToSend(txEstimation))  
     {
       sentCount++;
       if (helloQueueSize>0)
	 {
	   ptype=SicaQueueEntry::Hello_Type;
	   protocolNumber=SICA_HELLO_PORT;
	 }
       else 
	 {
	   ptype=SicaQueueEntry::Data_Type;
	   protocolNumber=SICA_DATA_PORT;
	 }
       qEntry= m_queue.Dequeue(m_rChannel,ptype);
       SendPacket(qEntry->GetPacket()->Copy(),m_rInterface,protocolNumber );
       m_queue.EraseFront(m_rChannel,ptype);
       dataQueueSize=m_queue.GetSize(m_rChannel,SicaQueueEntry::Data_Type);
       helloQueueSize=m_queue.GetSize(m_rChannel,SicaQueueEntry::Hello_Type);
       if (helloQueueSize>0)
	 txEstimation=EstimateTxDuration(200,wifiphy);
       else 
	 txEstimation=EstimateTxDuration(maxPacketSize,wifiphy);
     }
   if (sentCount)
     NS_LOG_DEBUG(  "Sica node " << m_id  <<" :"<< sentCount << " packets  sent to R Interface for channel "<< m_rChannel); 
   m_rInterfacePollTimer.Schedule();
   return;
 }

//////////////////////////RInterfaceReadyToSend
bool 
Sica::RInterfaceReadyToSend(Time txEstimation)
{
  if (ChannelIsBusy(m_rChannel) )
    return false;
  if (m_channelSenseFlag)
    return false;
  if  ( m_switchTimer.IsRunning() && txEstimation > m_switchTimer.GetDelayLeft() )
    return false;
  if  (m_channelSenseTimer.IsRunning() && txEstimation > m_channelSenseTimer.GetDelayLeft())
    return false;
  Ptr <WifiNetDevice>rInterface= m_rInterface->GetObject<WifiNetDevice>();
  Ptr<WifiPhy> wifiphy = rInterface->GetPhy();
  if (wifiphy->IsStateSwitching())
    return false;
  return true;
}

/************************************************************/

//////////////////////GameChannelAssignment
 void 
Sica::GameChannelAssignment()
 {
   m_CATimer.Cancel();
   /// Allotted function 
   if (m_rNewChannel != m_rChannel)
     NS_LOG_DEBUG (  "Sica node " << m_id <<" :"<< "Already has a channel switching attempt. Finished!");
   else {
     // AllottedFunction();
     UpdateLossMatrix();
     // NS_LOG_INFO (  "Sica node " << m_id <<" :"<< "Game CA: Loss function is  ");
     // copy(m_loss.begin(), m_loss.end(), std::ostream_iterator<double>(std::cout, "\n"));
     UpdateChannelWeight();
     m_rNewChannel=SelectRandomChannel();
     NS_ASSERT_MSG(m_rNewChannel >= Min_CH,"Selected channel is less than Min_CH");
     NS_ASSERT_MSG(m_rNewChannel<= Max_CH,"Selected channel is bigger  than Max_CH");
     NS_LOG_DEBUG (  "Sica node " << m_id <<" :"<< "Game CA: New Channel for R would be  " << m_rNewChannel);
     if (m_rNewChannel != m_rChannel)
       ScheduleSwitchRInterface();
   }
   // PrintNeighborTable(std::cout);
   // PrintChannelTable(std::cout);
   m_CATimer.Schedule();
 }


//////////////////////UpdateLossValues
void 
Sica::UpdateLossMatrix()
{
 // std::ostream &os=std::cout;
  std::vector<double> payoff;
// copy(m_loss.begin(), m_loss.end(), std::ostream_iterator<double>(os, " , "));
 for (uint32_t i=Min_CH; i<=Max_CH; i++)
       {
	 m_loss[i-Min_CH]= ComputeStageLoss(i);
       }
 // copy(m_loss.begin(), m_loss.end(), std::ostream_iterator<double>(os, " , "));
}


//////////////////////ComputeStageLoss
double 
Sica::ComputeStageLoss(uint32_t c)
{
  double bx=static_cast<double>(m_channel.GetChannelExtBandwidth(c));
  double b= static_cast<double>(Max_BW);
  double rNiC=static_cast<double>(m_channel.GetChannelNeighbors(c));
  double rNi=static_cast<double>(m_nb.GetNiNo());
  double TH=HelloInterval.Time::ToDouble(Time::S);
  double Ds;
  double Loss;
  double ML;
 if (m_rChannel == c)
    Ds=0;
 else 
   Ds=SwitchingDelay.Time::ToDouble(Time::S);
       if (rNi!=0 && b!=0 && TH!=0)
	 {
	   ML= m_alpha*(bx/b) + (1-m_alpha)*(rNiC/rNi);
	   Loss= m_gamma * ML + (1-m_gamma)* (Ds/TH);
	 } 
 NS_LOG_INFO("Sica node " << m_id <<" :"<< "Loss for channel "<< c << " computed from formula "<< LossFormulaNum << " is  "<< Loss << " bx " << bx << " b " << b << " neighbor on ch " <<rNiC << " ni "<< rNi<< " Ds " << Ds << " TH " <<TH << " Alpha is "<< m_alpha);
 return Loss;
}

 

//////////////////////UpdateChannelWeight
void 
Sica::UpdateChannelWeight()
{
  double wT1;
  double wT;
  for (uint32_t i=Min_CH; i<=Max_CH; i++)
    {
      wT= m_channel.GetChannelWeight(i);
      wT1= wT * pow(m_beta,(m_loss[i-Min_CH]));
      m_channel.SetChannelWeight(i,wT1);
      NS_LOG_INFO("Sica node " << m_id <<" :"<<"weight for channel "<< i << " is  "<< wT1 << " loss is  " << m_loss[i-Min_CH] <<" b^loss is  " <<pow(m_beta,(m_loss[i-Min_CH]))<< " wT-1 was "<< wT);
    }
}

//////////////////////SelectRandomChannel

uint32_t 
Sica::SelectRandomChannel()
{
  std::vector<uint32_t> channels;
  std::vector<double> prob;
  std::vector<double> weight;
  std::vector<double> cProb;
  // EmpiricalVariable  emRnd;
  Ptr<EmpiricalRandomVariable> emRnd = CreateObject<EmpiricalRandomVariable> ();
  double totalWeight=0;
  double tempW;
  // read the weights of channels
  for (uint32_t i=Min_CH; i<=Max_CH; i++)
    {
      channels.push_back(i);
      tempW=m_channel.GetChannelWeight(i);
      weight.push_back(tempW);
      totalWeight+=tempW;
    }
  
  for (uint32_t i=0; i<weight.size(); i++)
    prob.push_back(weight[i]/totalWeight);
  NotifyChannelProbability (prob);
  sort(channels.begin(),channels.end(),CompareChannelsWeight(*this));
  sort(prob.begin(),prob.end());
  cProb=ComputeCumulativeProbability(prob);
  for (uint32_t i=0; i< cProb.size(); i++){
    // emRnd.CDF(i,cProb[i]);
    emRnd->CDF(i,cProb[i]);
  }
    

  //PrintGameData(std::cout ,channels, prob, weight,cProb );

  // return  channels[emRnd.GetInteger()];
  return  channels[emRnd->GetInteger()];
}



//////////////////////ComputeCumulativeProbability
std::vector<double> 
Sica::ComputeCumulativeProbability(std::vector<double> p)
{
  std::vector<double> c;
  c.push_back(0);
  NS_LOG_DEBUG("Sica Node "<< m_id <<":" << "comulative probability is >>");
  for (uint32_t i=1; i<= p.size(); i++)
    {
    c.push_back(c[i-1]+p[i-1]);
    NS_LOG_DEBUG("----- "<< i-1 <<" prob -> " <<p[i-1] << " cptob -> "<< c[i]);
    }

  return c;
}

//////////////////////PrintGameData
void 
Sica::PrintGameData(std::ostream &os ,std::vector<uint32_t> channels,std::vector<double> prob,std::vector<double> weight,std::vector<double> cProb )
{
  os << "\n";
  copy(weight.begin(), weight.end(), std::ostream_iterator<double>(os, "--"));
  os << "\n";
  copy(channels.begin(), channels.end(), std::ostream_iterator<uint32_t>(os, "--"));
  os << "\n";
  copy(prob.begin(), prob.end(), std::ostream_iterator<double>(os, "--"));
  os << "\n";
  copy(cProb.begin(), cProb.end(), std::ostream_iterator<double>(os, "--"));
  os << "\n";
}


} /* namespace ns3 */
