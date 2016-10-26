/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015  Regents of the University of California.
 *
 * This file is part of ndnSIM. See AUTHORS for complete list of ndnSIM authors and
 * contributors.
 *
 * ndnSIM is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * ndnSIM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ndnSIM, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Spyridon (Spyros) Mastorakis <mastorakis@cs.ucla.edu>
 */

#include "sync-overlay-node.hpp"
#include "ns3/log.h"
#include "ns3/string.h"
#include "ns3/uinteger.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"

#include "model/ndn-ns3.hpp"
#include "model/ndn-l3-protocol.hpp"
#include "helper/ndn-fib-helper.hpp"

#include <memory>

NS_LOG_COMPONENT_DEFINE("ndn.SyncOverlayNode");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED(SyncOverlayNode);

TypeId
SyncOverlayNode::GetTypeId(void)
{
  static TypeId tid =
    TypeId("ns3::ndn::SyncOverlayNode")
      .SetGroupName("Ndn")
      .SetParent<App>()
      .AddConstructor<SyncOverlayNode>()
      .AddAttribute("Prefix", "Prefix, which the overlay node will announce to the local network", StringValue("/"),
        MakeNameAccessor(&SyncOverlayNode::m_prefix), MakeNameChecker())
      .AddAttribute("Postfix",
        "Postfix that is added to the output data (e.g., for adding node-uniqueness)",
        StringValue("/"), MakeNameAccessor(&SyncOverlayNode::m_postfix), MakeNameChecker())
      .AddAttribute("OverlayPrefix",
        "Sync Overlay Common Prefix",
        StringValue("/"), MakeNameAccessor(&SyncOverlayNode::m_overlayPrefix), MakeNameChecker());
  return tid;
}

SyncOverlayNode::SyncOverlayNode()
{
   NS_LOG_FUNCTION_NOARGS();
}

// inherited from Application base class.
void
SyncOverlayNode::StartApplication()
{
   NS_LOG_FUNCTION_NOARGS();
   App::StartApplication();

   FibHelper::AddRoute(GetNode(), m_prefix, m_face, 0);
   FibHelper::AddRoute(GetNode(), Name(m_overlayPrefix.toUri() + m_prefix.toUri()),
                       m_face, 0);
}

void
SyncOverlayNode::StopApplication()
{
   NS_LOG_FUNCTION_NOARGS();

   App::StopApplication();
}

 void
 SyncOverlayNode::OnInterest(shared_ptr<const Interest> interest)
 {
   App::OnInterest(interest); // tracing inside

   Name receivedInterestName = interest->getName();

   NS_LOG_INFO("< Received sync Interest with name: " << receivedInterestName.toUri());

   // Check where the Interest comes from

   shared_ptr<Interest> relayInterest = make_shared<Interest>(*interest);

   // If it comes from a local network, it has to be sent towards the
   // overlay network

   if (!m_overlayPrefix.isPrefixOf(receivedInterestName)) {
     Name overlayPrefix(m_overlayPrefix.toUri() + receivedInterestName.toUri());

     relayInterest->setName(overlayPrefix);
   }
   else if (m_overlayPrefix.isPrefixOf(receivedInterestName)) {
     Name localPrefix = receivedInterestName.getSubName(m_overlayPrefix.size());

     relayInterest->setName(localPrefix);

   }
   // something went wrong
   else {
     NS_LOG_INFO("> Interest with unknown name " << receivedInterestName.toUri());
     return;
   }

   NS_LOG_INFO("> Relaying Sync Interest with name " << relayInterest->toUri());

   m_transmittedInterests(relayInterest, this, m_face);
   m_appLink->onReceiveInterest(*relayInterest);

}

void
SyncOverlayNode::OnData(shared_ptr<const Data> data)
{
   if (!m_active)
     return;

   App::OnData(data); // tracing inside

   NS_LOG_FUNCTION(this << data);

   // NS_LOG_INFO ("Received content object: " << boost::cref(*data));

   Name dataPacketName = data->getName();

   shared_ptr<Data> relayData = make_shared<Data>(*data);

   NS_LOG_INFO("< Received Data with name: " << dataPacketName.toUri());

   if (!m_overlayPrefix.isPrefixOf(dataPacketName)) {
     Name overlayPrefix(m_overlayPrefix.toUri() + dataPacketName.toUri());

     relayData->setName(overlayPrefix);
   }
   else if (m_overlayPrefix.isPrefixOf(dataPacketName)) {
     Name localPrefix = dataPacketName.getSubName(m_overlayPrefix.size());

     relayData->setName(localPrefix);

   }
   // something went wrong
   else {
     NS_LOG_INFO("> Data with unknown name " << dataPacketName.toUri());
     return;
   }

   // to create real wire encoding
   relayData->wireEncode();

   NS_LOG_INFO("< Relaying DATA packet with name: " << relayData->getName().toUri());

   m_transmittedDatas(relayData, this, m_face);
   m_appLink->onReceiveData(*relayData);
}

} // namespace ndn
} // namespace ns3
