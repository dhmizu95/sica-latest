/**
 * \ingroup rtable
 *  
 * \brief Routing table function definition
 *
 * 
 */

#include "ns3/sica-rtable.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (RTable);

SicaRoutingTableEntry::
SicaRoutingTableEntry()
{}

SicaRoutingTableEntry::
SicaRoutingTableEntry(SicaRoutingTableEntry const &route)
:
 m_src (route.m_src),
 m_dst (route.m_dst),
 m_nextHop(route.m_nextHop),
 m_metric(route.m_metric)

{}

SicaRoutingTableEntry::
SicaRoutingTableEntry(SicaRoutingTableEntry const *route)
:
   m_src (route->m_src),
  m_dst (route->m_dst),
  m_nextHop(route->m_nextHop),
  m_metric(route->m_metric)
{}

SicaRoutingTableEntry::
SicaRoutingTableEntry(uint32_t srcId,uint32_t dstId, uint32_t nextHopId, double metric)
:
  m_src(srcId),
  m_dst (dstId),
  m_nextHop(nextHopId),
  m_metric(metric)
{}
 

uint32_t 
SicaRoutingTableEntry::GetSrc (void) const
{
  return m_src;
}

 void 
SicaRoutingTableEntry::SetSrc(uint32_t srcId)
 {
   m_src=srcId;
 }



uint32_t 
SicaRoutingTableEntry::GetDest (void) const
{
  return m_dst;
}

 void 
SicaRoutingTableEntry::SetDest(uint32_t dstId)
 {
   m_dst=dstId;
 }

uint32_t 
SicaRoutingTableEntry::GetNextHop (void) const
{
  return m_nextHop;
}

void 
SicaRoutingTableEntry::SetNextHop (uint32_t nextHopId)
{
  m_nextHop=nextHopId;
}

double 
SicaRoutingTableEntry::GetMetric (void) const
{
  return m_metric;
}

void 
SicaRoutingTableEntry::SetMetric (double metric)
{
  m_metric=metric;
}


TypeId 
RTable::GetTypeId (void)
{
 static TypeId tid = TypeId ("ns3::RTable")
    .SetParent<Object> ()
    .AddConstructor<RTable> ()
    ;
  return tid;
}

RTable::RTable()
{}

RTable::~RTable()
{}




bool 
RTable::AddRouteToTable(SicaRoutingTableEntry* route)
{
  if (route)
    {
      m_rTable.push_back(route);
      return true;
    }
  return false;
}

void 
RTable::MakeRoute(uint32_t srcId,uint32_t dstId, uint32_t nextHopId, double metric)
{
 SicaRoutingTableEntry *route=FindRoute(srcId,dstId);
  if (!route){
    route=new SicaRoutingTableEntry(srcId,dstId,nextHopId,metric);
    AddRouteToTable(route);
  }
  else 
    {
      route->SetNextHop(nextHopId);
      route->SetMetric(metric);
    }
  return;
}

SicaRoutingTableEntry*
RTable::FindRoute(uint32_t srcId,uint32_t dstId)
{
  SicaRoutingTableEntry *route=0;
  for (std::vector<SicaRoutingTableEntry*>::iterator i = m_rTable.begin (); i != m_rTable.end (); ++i)
    if ((*i)->GetSrc() == srcId && (*i)->GetDest() == dstId)
      route=(*i);
  return (route);
}

int 
RTable::FindNextHop(uint32_t srcId,uint32_t dstId)
{
  if (srcId==dstId)
    return srcId;
  SicaRoutingTableEntry *route=FindRoute(srcId,dstId);
 if (route)
   return route->GetNextHop();
 else 
   return -1;
}


void 
RTable::ReadRoutesFromFile(const char *fileName)
{
 std::ifstream rtfile;
 rtfile.open(fileName);
 int src,dst,nextHop;
 double metric;
 while(!rtfile.eof()){
   rtfile >> src;
   rtfile >> dst;
   rtfile >>nextHop;
   rtfile >>metric;
   MakeRoute(src,dst,nextHop,metric);
 }
 rtfile.close();
}


 void 
 RTable::PrintRTable(std::ostream &os)
 {
 for (std::vector<SicaRoutingTableEntry*>::iterator i = m_rTable.begin (); i != m_rTable.end (); ++i)
   {
     os<< (*i)->GetSrc()<< " ";
     os<< (*i)->GetDest()<< " ";
     os<< (*i)->GetNextHop()<< " ";
     os<< (*i)->GetMetric()<< "\n";
   }
 }

RoutingHelper::RoutingHelper()
{
  m_agentFactory.SetTypeId ("ns3::RTable");
}

Ptr<RTable> 
RoutingHelper::Create (Ptr<Node> node ,const char* fileName) const
{
  Ptr<RTable> rtable= m_agentFactory.Create<RTable>();
  rtable->ReadRoutesFromFile(fileName);
node->AggregateObject(rtable);
 return rtable;
}

void
RoutingHelper::Set (std::string name, const AttributeValue &value)
{
m_agentFactory.Set (name, value);
}

void
RoutingHelper::Install (NodeContainer c,  const char* fileName)
{
 for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Create(*i,fileName);
    }
}

}/*namespace ns3*/


