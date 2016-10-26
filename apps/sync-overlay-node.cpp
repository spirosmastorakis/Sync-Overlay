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
          StringValue("/"), MakeNameAccessor(&SyncOverlayNode::m_postfix), MakeNameChecker());
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

   NS_LOG_FUNCTION(this << interest);

 }

 } // namespace ndn
 } // namespace ns3
