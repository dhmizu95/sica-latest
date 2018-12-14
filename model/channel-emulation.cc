/**
 * \ingroup channelEmu 
 *  \brief An emulation mechanism to simulate the external interference over a channel
 *
 */

#include "ns3/channel-emulation.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("SicaEmuChannel");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (ChannelEmu);

ChannelEmu::ChannelEmu():
  m_busyDuration(MilliSeconds(8)),
  // m_nextTime(ExponentialVariable(2,8)),
  // m_nextTime(ExponentialRandomVariable()),
  // m_nextTime(CreateObject<ExponentialRandomVariable>()),
  m_state((Status)Idle_State),
  m_stausTimer(Timer::CANCEL_ON_DESTROY)
{
  double mean = 2;
  double bound = 8;
  m_nextTime = CreateObject<ExponentialRandomVariable>();
  m_nextTime->SetAttribute ("Mean", DoubleValue (mean));
  m_nextTime->SetAttribute ("Bound", DoubleValue (bound));
  // m_stausTimer.SetDelay(MilliSeconds(static_cast<uint64_t>(m_nextTime.GetValue())));	
  m_stausTimer.SetDelay(MilliSeconds(static_cast<uint64_t>(m_nextTime->GetValue())));	
  m_stausTimer.SetFunction(&ChannelEmu::ChangeStatus,this);
  m_stausTimer.Schedule();
  NS_LOG_INFO("Channel emulation is created with following parameter  >>");
NS_LOG_INFO("--BusyDuration " << m_busyDuration.GetSeconds() );
}

TypeId 
ChannelEmu::GetTypeId (void)
{static TypeId tid = TypeId("ns3::ChannelEmu")
    .SetParent<Object> ()
    .AddConstructor<ChannelEmu> ()
    .AddAttribute("BusyDuration","Busy duration for channel state default value is 8ms (channel is busy)",
		  TimeValue(MilliSeconds(8)),
		  MakeTimeAccessor (&ChannelEmu::m_busyDuration),
		  MakeTimeChecker ())
    // .AddAttribute ("Variable",
    //                "The random variable used to pick a idle time  everytime it is invoked.",
    //                StringValue ("ns3::ExponentialRandomVariable"),
    //               //  RandomVariable (CreateObject<ExponentialRandomVariable>(2,8)),  // ns-3.25
    //                MakePointerAccessor (&ChannelEmu::m_nextTime),
    //                MakePointerChecker<RandomVariableStream> ())  
    // .AddAttribute ("Variable", "The random variable used to pick a idle time  everytime it is invoked.",
    //               //  RandomVariableValue (ExponentialVariable(2,8)),
    //               //  ExponentialRandomVariableValue (CreateObject<ExponentialRandomVariable>(2,8)),
    //                RandomVariable (CreateObject<ExponentialRandomVariable>(2,8)),  // ns-3.25
    //                MakeRandomVariableAccessor (&ChannelEmu::m_nextTime),
    //                MakeRandomVariableChecker ())
    // .AddTraceSource ("StatusChanged", 
    //                  "Trace source indicating status changes",
    //                  MakeTraceSourceAccessor (&ChannelEmu::m_statusChanged))
    ;
  return tid;
}

void 
ChannelEmu::NotifyStatusChanged ()
{
  m_statusChanged(m_state,m_stausTimer.GetDelayLeft());
}



ChannelEmu::~ChannelEmu()
{
}


void 
ChannelEmu::ChangeStatus()
{
  m_stausTimer.Cancel();
  if (m_state==ChannelEmu::Idle_State && m_busyDuration.IsStrictlyPositive()){
    m_state=ChannelEmu::Busy_State;
    m_stausTimer.SetDelay(m_busyDuration);
  }
  else 
    {
     m_state=ChannelEmu::Idle_State;
    //  m_stausTimer.SetDelay(MilliSeconds(static_cast<uint64_t>(m_nextTime.GetValue())));
     m_stausTimer.SetDelay(MilliSeconds(static_cast<uint64_t>(m_nextTime->GetValue())));
    }
  m_stausTimer.Schedule();
  NotifyStatusChanged ();
}




ChannelEmuContainer:: ChannelEmuContainer()
{
}

ChannelEmuContainer::Iterator
ChannelEmuContainer::Begin (void) const
{
   return m_channelEmuAgents.begin();
}

ChannelEmuContainer::Iterator
ChannelEmuContainer::End (void) const
{
   return m_channelEmuAgents.end();
}

uint32_t
ChannelEmuContainer::GetN(void) const
{
  return m_channelEmuAgents.size();
}

Ptr<ChannelEmu> 
ChannelEmuContainer::Get (uint32_t i) const
{
  return  m_channelEmuAgents[i];
}

Ptr<ChannelEmu> 
ChannelEmuContainer:: GetId(uint32_t chId) const
{
  for (std::vector<Ptr<ChannelEmu> >::const_iterator i=m_channelEmuAgents.begin(); i != m_channelEmuAgents.end(); ++i )
    if ((*i)->GetChannelNumber()==chId)
      return (*i);
  return (*m_channelEmuAgents.end()) ;
}


void 
ChannelEmuContainer::Add (Ptr<ChannelEmu> c)
{
  m_channelEmuAgents.push_back(c);
}


ChannelEmuHelper::ChannelEmuHelper()
{
  m_agentFactory.SetTypeId("ns3::ChannelEmu");
}

Ptr<ChannelEmu> 
ChannelEmuHelper::Create (uint32_t chId ) const
{
  Ptr<ChannelEmu> emu=m_agentFactory.Create<ChannelEmu>();
  emu->SetChannelNumber(chId);
  return emu;
}

void 
ChannelEmuHelper::Set (std::string name, const AttributeValue &value)
 {
   m_agentFactory.Set(name,value);
 }

ChannelEmuContainer  
ChannelEmuHelper::Install (std::vector<uint32_t> channels) const
{
  ChannelEmuContainer emuContainer;
  for (std::vector<uint32_t >::const_iterator i=channels.begin(); i != channels.end(); ++i )
    {
    emuContainer.Add (Create(*i));
    }
  return emuContainer;
	 
}

}/*namespace ns3*/
