/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* 
 * Copyright (c) 2010 Universitat Pollitechnica the Catalunya
 * 
 * 
 * Author: Maryam Amiri Nezhad <maryam@ac.upc.edu>
 * 
 * This is doxygen module description, don't include 
 */

#include "ns3/sica-queue.h"
#include "ns3/ptr.h"
#include "ns3/log.h"
#include "ns3/sica-packet.h"
#include "ns3/ipv4-header.h"
#include "ns3/node.h"
#include "ns3/object.h"
#include <cstdlib>
#include <algorithm>
#include <functional>  
#include <vector>
#include <stdint.h>
#include <list>


NS_LOG_COMPONENT_DEFINE ("SicaQueue");

namespace ns3 {



//////////////SicaQueue
SicaQueue::SicaQueue ()
{
}

//////////////~SicaQueue
SicaQueue::~SicaQueue(void)
{
  m_cqueue.clear();
}


//////////////FindChannelQueue
SicaQueue::SicaChannelQueue * 
SicaQueue::FindChannelQueue(uint32_t ch)
{
  SicaChannelQueue (*cqueue)=0;
  for (std::vector<SicaChannelQueue>::iterator i =m_cqueue.begin() ;i != m_cqueue.end (); ++i)
    {
      if (i->m_ch == ch)
        cqueue= &(*i);
    }
 return (cqueue);
}


//////////////GetSize
uint32_t 
SicaQueue::GetSize(uint32_t ch, SicaQueueEntry::PacketType ptype)
{
  SicaChannelQueue *cqueue = FindChannelQueue(ch);
  Purge(ch,ptype);
  if (cqueue)
    if (ptype == SicaQueueEntry::Hello_Type){
      return cqueue->m_helloQueue.size();
    }
    else {
      return cqueue->m_dataQueue.size();
    }
  else 
    return (0);
}


//////////////CpQueuEntry
void  
SicaQueue::CpQueuEntry(SicaQueueEntry *cpy,SicaQueueEntry origin )
{
  cpy->SetPacketType(origin.GetPacketType());
  Packet *tempP=(Packet*)PeekPointer(origin.GetPacket());
  cpy->SetPacket(tempP->Copy());
  cpy->SetExpireTime(origin.GetExpireTime());
}



//////////////Enqueue
bool 
SicaQueue::Enqueue (uint32_t ch, SicaQueueEntry *ent )
{
  if (ent){
    SicaChannelQueue *cqueue = FindChannelQueue(ch);
    if (!cqueue)
      cqueue= CreatQueue(ch);
    else if (cqueue->m_close)
      {
        cqueue->OpenChannelQueue();
      }
    if (ent->GetPacketType() == SicaQueueEntry::Hello_Type)
      {
        cqueue->m_helloQueue.push_back(*ent);
        NS_LOG_DEBUG("Push one  queue entry to Hello-Channel-Queue #" << ch<<" hello will expire in " <<ent->GetExpireTime().GetMilliSeconds() <<"ms.");
      }
    else 
      {
        cqueue->m_dataQueue.push_back(*ent); 
        NS_LOG_DEBUG("Push one  queue entry to Data-Channel-Queue #" << ch<<" data will expire in " <<ent->GetExpireTime().GetMilliSeconds() <<"ms.");
      }
  }
  else 
    NS_LOG_WARN ("Try to add empty entry to Channel-Queue #"<< ch);
  return (true);  
}




//////////////Dequeue
SicaQueueEntry * 
SicaQueue::Dequeue (uint32_t ch, SicaQueueEntry::PacketType ptype)
{
  Ptr <Packet> p;
  SicaQueueEntry * ent=new SicaQueueEntry(p,ptype);
  // Purge expired packets
  if (Purge(ch,ptype)){
    SicaQueue::SicaChannelQueue *cqueue =SicaQueue:: FindChannelQueue(ch);
    if (cqueue && !cqueue->m_close )
      {
        if (ptype == SicaQueueEntry::Hello_Type && cqueue->m_helloQueue.size()>0 )
          {    
            //ent = &(cqueue->m_helloQueue.front());
            CpQueuEntry(ent,cqueue->m_helloQueue.front());
            NS_LOG_DEBUG("Dequeue read one packet from Hello-Channel-Queue #" << ch );
            return (ent);
          }
        else if (ptype == SicaQueueEntry::Data_Type && cqueue->m_dataQueue.size()>0)
          {
            //ent = &(cqueue->m_dataQueue.front());
            CpQueuEntry(ent,cqueue->m_dataQueue.front());
            NS_LOG_DEBUG("Dequeue read  one packet from Data-Channel-Queue #" << ch );
            return (ent);
          }
      }
  }//if purge
  NS_LOG_DEBUG("Dequeue report: Channel-Queue #" << ch <<"  is empty, return Null pointer.");
  return (NULL);
}



//////////////DequeueRemoveWithDest
SicaQueueEntry * 
SicaQueue::DequeueWithDest(uint32_t ch,uint32_t dst)
{
  Ptr <Packet> p;
  std::vector<SicaQueueEntry>::iterator  eIndex= FindQueueEntryForDest(ch,dst);
  SicaQueueEntry::PacketType ptype=SicaQueueEntry::Data_Type;
  SicaQueueEntry * ent=new SicaQueueEntry(p,ptype);
   SicaChannelQueue *cqueue =FindChannelQueue(ch);
  // Purge expired packets
   if (Purge(ch,ptype)){
     if (cqueue && !cqueue->m_close && eIndex != cqueue->m_dataQueue.end())
       {
         NS_LOG_DEBUG("One data packet with destination address : "<<dst << " is peeked from channel queue number: " << ch);
         CpQueuEntry(ent,*eIndex);
         return (ent);
         
       }
   }//if cqueue
   return (NULL);
}//DequeueRemoveWithDest
  
///////////////////FindQueueEntryForDest

std::vector<SicaQueueEntry>::iterator 
SicaQueue::FindQueueEntryForDest(uint32_t ch,uint32_t dst)
{
  Ptr <Packet> p;
  SicaHeader sHeader;
  SicaQueueEntry::PacketType ptype=SicaQueueEntry::Data_Type;
   SicaChannelQueue *cqueue =FindChannelQueue(ch);
  // Purge expired packets
  if (Purge(ch,ptype))
    {
    if (cqueue && !cqueue->m_close )
      {
        for (std::vector<SicaQueueEntry>::iterator i =cqueue->m_dataQueue.begin() ; i!= cqueue->m_dataQueue.end(); ++i)   
          {
            p=i->GetPacket();
            p->PeekHeader(sHeader);
            if (sHeader.GetDest() == dst)
              {
                NS_LOG_DEBUG("One data packet with destination address : "<<dst << " is found in channel queue number: " << ch);
                return (i);
              }
          }
      }//if cqueue
  }// if purge
  return (cqueue->m_dataQueue.end());

}


//////////////EraseWithIndex
bool
SicaQueue::EraseWithDest(uint32_t ch,uint32_t dst)
{
 
   SicaChannelQueue *cqueue =FindChannelQueue(ch);  
   std::vector<SicaQueueEntry>::iterator  eIndex= FindQueueEntryForDest(ch,dst);
    if (cqueue && !cqueue->m_close && eIndex != cqueue->m_dataQueue.end() )
      {
        
                NS_LOG_DEBUG("One data packet with destination address : "<<dst << " is erased from channel queue number: " << ch);
                //CpQueuEntry(ent,*i);
                cqueue->m_dataQueue.erase(eIndex);
                return (true);
            
      }//if cqueue

  return (false);
}//EraseWithIndex


//////////////Purge
uint32_t 
SicaQueue::Purge(uint32_t ch, SicaQueueEntry::PacketType ptype)
{
  SicaChannelQueue *cqueue =FindChannelQueue(ch);
  if (cqueue && !cqueue->m_close )
    {
    if (ptype == SicaQueueEntry::Hello_Type )
      {
        while (cqueue->m_helloQueue.size()>0 && cqueue->IsExpired(cqueue->m_helloQueue.front()))
          {
            NS_LOG_DEBUG("Purge found one expired packet in Hello-Channel-Queue #" << ch <<".");
            EraseFront(ch,ptype);
          }
        return(cqueue->m_helloQueue.size());
      }
    else 
      {
     while (cqueue->m_dataQueue.size()>0 && cqueue->IsExpired(cqueue->m_dataQueue.front()))
       {
         NS_LOG_DEBUG("Purge found one expired packet in Data-Channel-Queue #" << ch <<".");
         EraseFront(ch,ptype);
       }
     return(cqueue->m_dataQueue.size());
      }
  }
  return(0);
  
}//Purge



//////////////EraseFront
void 
SicaQueue::EraseFront(uint32_t ch, SicaQueueEntry::PacketType ptype)
{
 SicaChannelQueue *cqueue =FindChannelQueue(ch);
 if (cqueue && !cqueue->m_close )
   {
     if (ptype == SicaQueueEntry::Hello_Type && cqueue->m_helloQueue.size()>0)
       {
         cqueue->m_helloQueue.erase(cqueue->m_helloQueue.begin());
         NS_LOG_DEBUG("Erase one queue entry  from Hello-Queue #" << ch << ". Queue size is "<<cqueue->m_helloQueue.size()<<".");
       }
     else if (ptype == SicaQueueEntry::Data_Type && cqueue->m_dataQueue.size()>0 )
      {
       cqueue->m_dataQueue.erase(cqueue->m_dataQueue.begin()); 
       NS_LOG_DEBUG("Erase one queue entry from Data-Queue #" << ch<< ". Queue size is "<<cqueue->m_dataQueue.size()<<".");
      } 
   }
}



//////////////CloseQueue
void 
SicaQueue::CloseQueue (uint32_t ch)
{
  SicaChannelQueue *cqueue = FindChannelQueue(ch);
  if (cqueue)
    {
    cqueue->m_close=true;
    cqueue->m_dataQueue.clear();
    cqueue->m_helloQueue.clear();
    NS_LOG_DEBUG("Queue #" << ch << "is closed.");
    }
  else 
    NS_LOG_DEBUG("Queue #" << ch << "is not exist to be closed!");
return;
}


//////////////CreatQueue 
SicaQueue::SicaChannelQueue *
SicaQueue::CreatQueue (uint32_t ch){
  SicaChannelQueue *cqueue = FindChannelQueue(ch);
  SicaChannelQueue *NewCqueue = new SicaChannelQueue(ch);
  if (!cqueue){
    m_cqueue.push_back(*NewCqueue);
    NS_LOG_DEBUG("Queue related to channel #" << ch << " is created.");
    return (NewCqueue);
  }
  else {
    NS_LOG_DEBUG("Queue related to channel #" << ch << " exists!!");
   return (cqueue); 

  }

}


//////////////ShuffleData
void 
SicaQueue::ShuffleData(uint32_t originCh,uint32_t targetCh ,uint32_t addr)
{
  SicaChannelQueue *originQueue = FindChannelQueue(originCh);
  SicaChannelQueue *targetQueue = FindChannelQueue(targetCh);
   std::vector<SicaQueueEntry>::iterator  eIndex= FindQueueEntryForDest(originCh,addr);
   bool flag=false;
   NS_LOG_DEBUG("Shuffle data from channel " <<originCh << " to channel " << targetCh << " for node address:  " <<  addr);
   if (!targetQueue){
     targetQueue=CreatQueue(targetCh);
   }
   if (originQueue && !originQueue->m_close){
     while (eIndex != originQueue->m_dataQueue.end())
     {
       flag=true;
       targetQueue->m_dataQueue.push_back(*DequeueWithDest(originCh,addr));
       originQueue->m_dataQueue.erase(eIndex);
       eIndex= FindQueueEntryForDest(originCh,addr);    
       NS_LOG_DEBUG("One queue entry for destination " <<addr << " is poped from channel " << originCh );
       NS_LOG_DEBUG("One queue entry for destination " <<addr << " is pushed to channel " << targetCh );   
     }//while 
   }//if 
   if (!flag)
     NS_LOG_DEBUG("Shuffle found no entry in the origin channel, end up with no packet movement");
}

////////////////ShuffleDataALL
void 
SicaQueue::ShuffleDataALL(uint32_t originCh,uint32_t targetCh )
{
  if (originCh==targetCh)
    return;
  SicaChannelQueue *originQueue = FindChannelQueue(originCh);
  SicaChannelQueue *targetQueue = FindChannelQueue(targetCh);
  SicaQueueEntry::PacketType datatype=SicaQueueEntry::Data_Type;
  SicaQueueEntry::PacketType hellotype=SicaQueueEntry::Hello_Type;
  std::vector<SicaQueueEntry>::iterator i;
  NS_LOG_DEBUG("Shuffle data from channel " <<originCh << " to channel " << targetCh);
  if (!targetQueue){
    targetQueue=CreatQueue(targetCh);
  }
  if (originQueue && !originQueue->m_close){
    while (originQueue->m_dataQueue.size() > 0)
      {
        i =originQueue->m_dataQueue.begin(); 
        targetQueue->m_dataQueue.push_back(*Dequeue(originCh,datatype));
        originQueue->m_dataQueue.erase(i);
        NS_LOG_DEBUG("One data queue entry  is poped from channel " << originCh );
        NS_LOG_DEBUG("One data queue entry is pushed to channel " << targetCh ); 
        NS_LOG_DEBUG("Queue size  " << originQueue->m_dataQueue.size()  ); 
      }//while 
    
    while (originQueue->m_helloQueue.size() > 0)
      {
        i =originQueue->m_helloQueue.begin() ;
        
        targetQueue->m_helloQueue.push_back(*Dequeue(originCh,hellotype));
        originQueue->m_helloQueue.erase(i);
        NS_LOG_DEBUG("One hello queue entry  is poped from channel " << originCh );
        NS_LOG_DEBUG("One hello queue entry is pushed to channel " << targetCh );     }//for 
    
  }//if 
}


////////////////ComputeFlowNumber
double 
SicaQueue::ComputeFlowNumber(uint32_t ch)
{
SicaChannelQueue *cqueue = FindChannelQueue(ch);
 double flowNum=0;
 std::vector<uint32_t> srcAddress;
 uint32_t pAddr;
 Ptr<Packet> p;
 SicaHeader sHeader;
 SicaQueueEntry::PacketType ptype=SicaQueueEntry::Data_Type;
 Purge(ch,ptype);
 for (std::vector<SicaQueueEntry>::iterator i =cqueue->m_dataQueue.begin() ; i!= cqueue->m_dataQueue.end(); ++i)   
          {
            p=i->GetPacket();
            p->PeekHeader(sHeader);
            pAddr=sHeader.GetOrigin();
            if (std::find(srcAddress.begin(), srcAddress.end(),pAddr)==srcAddress.end());
              {
                srcAddress.push_back(pAddr);
                flowNum++;
              } 

}

 return flowNum;
}

}/*namespace ns3 */
