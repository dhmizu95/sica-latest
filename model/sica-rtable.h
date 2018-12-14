/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* 
 * Copyright (c) 2010 Universitat Pollitechnica the Catalunya
 * 
 * 
 * Author: Maryam Amiri Nezhad <maryam@ac.upc.edu>
 * 
 * This is doxygen module description, don't include 
 */

#ifndef RTABLE_H
#define RTABLE_H

/**
 * \ingroup sica
 * \defgroup rtable RTABLE
 * \brief Routing table structure and functions
 * here we need to manage the routing information using a global routing protocol.
 * 
 */

#include <iostream>
#include <iterator>
#include <ostream>
#include <fstream>
#include <string>
#include <vector>
#include "ns3/node.h"
#include "ns3/object-factory.h"
#include "ns3/object.h"
#include "ns3/double.h"
#include "ns3/integer.h"
#include "ns3/log.h"
#include "ns3/boolean.h"
#include "ns3/type-id.h"
#include "node-container.h"
namespace ns3 {

/**
 *  
 * \brief A record of a routing table entry for static routing in Sica 
 */
class SicaRoutingTableEntry{
    public:
  /**
   * \brief This constructor does nothing
   */
  SicaRoutingTableEntry();
  /**
   * \brief Copy Constructor
   * \param route The route to copy
   */
  SicaRoutingTableEntry(SicaRoutingTableEntry const &route);
 /**
   * \brief Copy Constructor
   * \param route The route to copy
   */
  SicaRoutingTableEntry(SicaRoutingTableEntry const *route);
/**
   * \brief Copy Constructor
   *\param srcId source Id
   * \param dstId destination Id 
   * \param nextHopId  Id of next hop node to the destination
   * \param metric route metric
   */

  SicaRoutingTableEntry(uint32_t srcId, uint32_t dstId, uint32_t nextHopId, double metric);
 /**
   * \return The node id of the source  of this route
   */
  uint32_t GetSrc (void) const;
 /**
   * \brief Set the node id of the source of this route
   *\param srcId the id of the source node
   */
  void SetSrc(uint32_t srcId);

 /**
   * \return The node id of the destination of this route
   */
  uint32_t GetDest (void) const;
 /**
   * \brief Set the node id of the destination of this route
   *\param dstId the id of destination node
   */
  void SetDest(uint32_t dstId);
/**
   * \return The node id of the next hop  of this route
   */
  uint32_t GetNextHop (void) const;
/**
   * \brief Set the node id of the next hop of this route
   *\param nextHopId the id of the next-hop node
   */
void SetNextHop (uint32_t nextHopId);
/**
   * \return The metric  of  this route
   */
  double GetMetric (void) const;
/**
   * \brief Set the metric  of  this route
   *\param metric the metric of the path
   */
void SetMetric (double metric);
private:
  uint32_t m_src;
  uint32_t m_dst;
  uint32_t m_nextHop;
  double  m_metric;
};

std::ostream& operator<< (std::ostream& os, SicaRoutingTableEntry const& route);
/**
 *\brief  A  routing table  which contains a list of routes for static routing in Sica 
 */

class RTable : public Object
{
public:
  /// Used to set the parameters of the class
static TypeId GetTypeId (void);
///c-tor
RTable();
///d-tor
~RTable();
  /**
   *\brief Add route to the list 
   *\param route the pointer to the routing table entry 
   */
bool AddRouteToTable(SicaRoutingTableEntry* route);
  /**
   *\brief  Make a  route to a node with destId from nexthop node and add it to the list
   *\param  srcId the Id of the source node 
   *\param dstId the Id of the destination node 
   *\param nextHopId the Id of the next hop node on the path to the destination 
   *\param metric the metric of the path (reserved for future work if necessary)
   */ 
void MakeRoute(uint32_t srcId,uint32_t dstId, uint32_t nextHopId, double metric);
  /**
   *\brief  Find the route entry for the destination node with dstId
  *\param  srcId the Id of the source node 
   *\param dstId the Id of the destination node 
   */ 
SicaRoutingTableEntry* FindRoute(uint32_t srcId,uint32_t dstId);
  /**
   *\brief  Find the id of the nexthop node to  the destination node with dstId
*\param  srcId the Id of the source node 
   *\param dstId the Id of the destination node 
   */
int FindNextHop(uint32_t srcId,uint32_t dstId);
  /**
   *\brief  Fill the routing tables from file
   *\param fileName the name of the input file contains static routes with the following format (srcId dstId nextHop Id metric)
   */
  void ReadRoutesFromFile(const char* fileName);
  /**
   *\brief Print all content of the routing table
   *\param os the output stream 
   */
  void PrintRTable(std::ostream &os);
private:
  std::vector<SicaRoutingTableEntry*> m_rTable;
};

/**
 * \ingroup rtable
 * 
 * A  routing helper which reads a routing file and attaches a routing table to a node
 */

/**
 * \ingroup sicahelper
 * \brief Helper class that adds Sica channel assignment  to nodes.
 */

class RoutingHelper
{
public:
  //c_tor
  RoutingHelper();
 /**
   * \param node the node on which this routing will run
   *\param fileName the file which contains routing information
   * \returns a newly-created routing table
   *
   * This method will be called by ns3::RoutingHelper::Install
   * 
   */

  Ptr<RTable> Create (Ptr<Node> node,const char* fileName ) const;

/**
   * \param name the name of the attribute to set
   * \param value the value of the attribute to set.
   *
   * \brief This method controls the attributes of ns3::Sica::Sica
   */

  void Set (std::string name, const AttributeValue &value);
  /**
   * \brief For each node in the input container,implements  ns3::Rtable  The program will assert if this method is called on a container with a node that already has a Rtable  object aggregated to it.
   * 
   * \param c NodeContainer that holds the set of nodes on which to install the new agent.
   *\param fileName the name of the input file which contain the routing information (SrcId DestId NextHopId Metric)
   */
  void Install (NodeContainer c, const char* fileName);

private:
  ObjectFactory m_agentFactory;
};

}/*namespace ns3*/


#endif /* RTABLE_H */
