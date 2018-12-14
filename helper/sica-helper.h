/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* 
 * Copyright (c) 2010 Universitat Pollitechnica the Catalunya
 * 
 * 
 * Author: Maryam Amiri Nezhad <maryam@ac.upc.edu>
 * 
 * This is doxygen module description, don't include 
 */
#ifndef SICAHELPER_H
#define SICAHELPER_H

/**
 * \ingroup sica 
 * \defgroup sicahelper SicaHelper
 */


#include "ns3/sica.h"
#include "ns3/node.h"
#include <vector>
#include "ns3/object-factory.h"
#include "node-container.h"

namespace ns3 {
/**
 * 
 * \brief Container which holds Sica objects
 */

class SicaContainer
{
 public:
  /// The iterator over a vector of Sica objects 
typedef std::vector<Ptr<Sica> >::const_iterator Iterator;
/**
   * Create an empty SicaContainer.
   */
SicaContainer ();
  /// Return the pointer of first element in the container
Iterator Begin (void) const;
  /// Return the pointer of last element in the container
Iterator End (void) const;
  /// Return the number of  elements  in the container
uint32_t GetN (void) const;
  /**
   *\brief  Return one  element in the container
   *\param i the id of the element to be returned
   */
Ptr<Sica> Get (uint32_t i) const;
  /**
   *\brief Add one element to the end of the container 
   *\param s the pointer of the Sica object
   */
void Add (Ptr<Sica> s);
private:
std::vector<Ptr<Sica> > m_sicaAgents;

};

/**
 * 
 * \brief Helper class that adds Sica channel assignment  to nodes.
 */
class SicaHelper
{
public: 
  //c_tor
  SicaHelper();
  /**
   * \param node the node on which Sica will run
   *\param  channelsEmu the channel emulators which are simulated the channel situations for scenario
   * \returns a newly-created Sica
   *
   * \brief This method will be called by ns3::SicaHelper::Install to create and return one Sica object
   * 
   */

  Ptr<Sica> Create (Ptr<Node> node,ChannelEmuContainer channelsEmu ) const;

/**
   * \param name the name of the attribute to set
   * \param value the value of the attribute to set.
   *
   * \brief This method controls the attributes of ns3::Sica::Sica
   */

  void Set (std::string name, const AttributeValue &value);

   /**
   * \brief For each node in the input container,implements  ns3::Sica  The program will assert  if this method is called on a container with a node that already has a Sica  object aggregated to it.
   * 
   * \param c NodeContainer that holds the set of nodes on which to install the new agent.
   *\param channelsEmu the channel emulators which are simulated the channel situations for scenario
   */

  SicaContainer  Install (NodeContainer c,ChannelEmuContainer channelsEmu ) const;
private:
  ObjectFactory m_agentFactory;
};/*Sica Helper */

}/*namespace ns3 */
#endif /*SicaHelper_H*/
