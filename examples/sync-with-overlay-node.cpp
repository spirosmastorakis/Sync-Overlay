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

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"

namespace ns3 {
namespace ndn {

/**
 *
 * This scenario simulates 2 local broadcast networks with ChronoSync peers
 * and 1 overlay Sync node per network. The overlay Sync node relays Sync
 * Interests from a local network to the other through the overlay network.
 *
 * Nodes 1, 2, 3 are the ChronoSync peers of local network 1, node 0 is the
 * overlay node of local network 1. Nodes 5, 6, 7 are the ChronoSync peers of
 * local network 2 and node 4 is the overlay node of network 2.
 *
 * Nodes 8, 9, 10, 11 consists the network topology between the 2 overlay nodes
 *
 */

int
main(int argc, char *argv[])
{
  // setting default parameters for PointToPoint links and channels
  Config::SetDefault("ns3::PointToPointNetDevice::DataRate", StringValue("1Mbps"));
  Config::SetDefault("ns3::PointToPointChannel::Delay", StringValue("10ms"));
  Config::SetDefault("ns3::DropTailQueue::MaxPackets", StringValue("20"));

  // Read optional command-line parameters (e.g., enable visualizer with ./waf --run=<> --visualize
  CommandLine cmd;
  cmd.Parse(argc, argv);

  // Creating nodes
  NodeContainer nodes;
  nodes.Create(12);

  // Connecting nodes using two links
  PointToPointHelper p2p;
  // Local Network 1
  p2p.Install(nodes.Get(0), nodes.Get(1));
  p2p.Install(nodes.Get(1), nodes.Get(2));
  p2p.Install(nodes.Get(1), nodes.Get(3));

  // Network Topology between overlay nodes
  p2p.Install(nodes.Get(0), nodes.Get(8));
  p2p.Install(nodes.Get(0), nodes.Get(9));
  p2p.Install(nodes.Get(8), nodes.Get(11));
  p2p.Install(nodes.Get(8), nodes.Get(10));
  p2p.Install(nodes.Get(8), nodes.Get(9));
  p2p.Install(nodes.Get(9), nodes.Get(10));
  p2p.Install(nodes.Get(9), nodes.Get(11));
  p2p.Install(nodes.Get(10), nodes.Get(4));
  p2p.Install(nodes.Get(10), nodes.Get(11));
  p2p.Install(nodes.Get(11), nodes.Get(4));

  // Local Network 2
  p2p.Install(nodes.Get(4), nodes.Get(5));
  p2p.Install(nodes.Get(5), nodes.Get(6));
  p2p.Install(nodes.Get(5), nodes.Get(7));

  // Install NDN stack on all nodes
  StackHelper ndnHelper;
  ndnHelper.SetDefaultRoutes(true);
  ndnHelper.InstallAll();

  // Choosing forwarding strategy
  ndn::StrategyChoiceHelper::InstallAll("/ndn", "ndn:/localhost/nfd/strategy/multicast");
  ndn::StrategyChoiceHelper::InstallAll("/Gsync", "ndn:/localhost/nfd/strategy/best-route2");

  // Installing applications

  // Peer 1
  ndn::AppHelper peer1("ChronoSyncApp");
  peer1.SetAttribute("SyncPrefix", StringValue("/ndn/broadcast/sync"));
  peer1.SetAttribute("UserPrefix", StringValue("/peer1"));
  peer1.SetAttribute("RoutingPrefix", StringValue("/ndn"));
  peer1.SetAttribute("MinNumberMessages", StringValue("1"));
  peer1.SetAttribute("MaxNumberMessages", StringValue("100"));
  peer1.SetAttribute("PeriodicPublishing", StringValue("true"));
  peer1.Install(nodes.Get(2)).Start(Seconds(2));

  // Peer 2
  ndn::AppHelper peer2("ChronoSyncApp");
  peer2.SetAttribute("SyncPrefix", StringValue("/ndn/broadcast/sync"));
  peer2.SetAttribute("UserPrefix", StringValue("/peer2"));
  peer2.SetAttribute("RoutingPrefix", StringValue("/ndn"));
  peer2.SetAttribute("MinNumberMessages", StringValue("1"));
  peer2.SetAttribute("MaxNumberMessages", StringValue("100"));
  // peer2.SetAttribute("PeriodicPublishing", StringValue("true"));
  peer2.Install(nodes.Get(3)).Start(Seconds(2));

  // Peer 3
  ndn::AppHelper peer3("ChronoSyncApp");
  peer3.SetAttribute("SyncPrefix", StringValue("/ndn/broadcast/sync"));
  peer3.SetAttribute("UserPrefix", StringValue("/peer3"));
  peer3.SetAttribute("RoutingPrefix", StringValue("/ndn"));
  peer3.SetAttribute("MinNumberMessages", StringValue("1"));
  peer3.SetAttribute("MaxNumberMessages", StringValue("100"));
  // peer3.SetAttribute("PeriodicPublishing", StringValue("true"));
  peer3.Install(nodes.Get(6)).Start(Seconds(2));

  // Peer 4
  ndn::AppHelper peer4("ChronoSyncApp");
  peer4.SetAttribute("SyncPrefix", StringValue("/ndn/broadcast/sync"));
  peer4.SetAttribute("UserPrefix", StringValue("/peer4"));
  peer4.SetAttribute("RoutingPrefix", StringValue("/ndn"));
  peer4.SetAttribute("MinNumberMessages", StringValue("1"));
  peer4.SetAttribute("MaxNumberMessages", StringValue("100"));
  // peer4.SetAttribute("PeriodicPublishing", StringValue("true"));
  peer4.Install(nodes.Get(7)).Start(Seconds(2));

  // Overlay Node
  ndn::AppHelper overlayNode("ns3::ndn::SyncOverlayNode");
  overlayNode.SetAttribute("Prefix", StringValue("/ndn/broadcast/sync"));
  overlayNode.SetAttribute("OverlayPrefix", StringValue("/Gsync"));
  overlayNode.Install(nodes.Get(0)).Start(Seconds(2));
  overlayNode.Install(nodes.Get(4)).Start(Seconds(2));

  // Manually configure FIB routes

  // Local Network 1
  ndn::FibHelper::AddRoute(nodes.Get(2), "/ndn/broadcast/sync", nodes.Get(1), 1);
  ndn::FibHelper::AddRoute(nodes.Get(3), "/ndn/broadcast/sync", nodes.Get(1), 1);
  ndn::FibHelper::AddRoute(nodes.Get(1), "/ndn/broadcast/sync", nodes.Get(2), 1);
  ndn::FibHelper::AddRoute(nodes.Get(1), "/ndn/broadcast/sync", nodes.Get(3), 1);
  ndn::FibHelper::AddRoute(nodes.Get(1), "/ndn/broadcast/sync", nodes.Get(0), 1);
  ndn::FibHelper::AddRoute(nodes.Get(0), "/ndn/broadcast/sync", nodes.Get(1), 1);

  // Network Topology between overlay nodes
  ndn::FibHelper::AddRoute(nodes.Get(0), "/Gsync/ndn/broadcast/sync", nodes.Get(8), 1);
  ndn::FibHelper::AddRoute(nodes.Get(8), "/Gsync/ndn/broadcast/sync", nodes.Get(0), 1);
  ndn::FibHelper::AddRoute(nodes.Get(0), "/Gsync/ndn/broadcast/sync", nodes.Get(9), 1);
  ndn::FibHelper::AddRoute(nodes.Get(9), "/Gsync/ndn/broadcast/sync", nodes.Get(0), 1);

  ndn::FibHelper::AddRoute(nodes.Get(8), "/Gsync/ndn/broadcast/sync", nodes.Get(9), 1);
  ndn::FibHelper::AddRoute(nodes.Get(9), "/Gsync/ndn/broadcast/sync", nodes.Get(8), 1);
  ndn::FibHelper::AddRoute(nodes.Get(8), "/Gsync/ndn/broadcast/sync", nodes.Get(11), 1);
  ndn::FibHelper::AddRoute(nodes.Get(11), "/Gsync/ndn/broadcast/sync", nodes.Get(8), 1);
  ndn::FibHelper::AddRoute(nodes.Get(8), "/Gsync/ndn/broadcast/sync", nodes.Get(10), 1);
  ndn::FibHelper::AddRoute(nodes.Get(10), "/Gsync/ndn/broadcast/sync", nodes.Get(8), 1);

  ndn::FibHelper::AddRoute(nodes.Get(9), "/Gsync/ndn/broadcast/sync", nodes.Get(10), 1);
  ndn::FibHelper::AddRoute(nodes.Get(10), "/Gsync/ndn/broadcast/sync", nodes.Get(9), 1);
  ndn::FibHelper::AddRoute(nodes.Get(9), "/Gsync/ndn/broadcast/sync", nodes.Get(11), 1);
  ndn::FibHelper::AddRoute(nodes.Get(11), "/Gsync/ndn/broadcast/sync", nodes.Get(9), 1);

  ndn::FibHelper::AddRoute(nodes.Get(10), "/Gsync/ndn/broadcast/sync", nodes.Get(4), 1);
  ndn::FibHelper::AddRoute(nodes.Get(4), "/Gsync/ndn/broadcast/sync", nodes.Get(10), 1);
  ndn::FibHelper::AddRoute(nodes.Get(10), "/Gsync/ndn/broadcast/sync", nodes.Get(11), 1);
  ndn::FibHelper::AddRoute(nodes.Get(11), "/Gsync/ndn/broadcast/sync", nodes.Get(10), 1);

  ndn::FibHelper::AddRoute(nodes.Get(11), "/Gsync/ndn/broadcast/sync", nodes.Get(4), 1);
  ndn::FibHelper::AddRoute(nodes.Get(4), "/Gsync/ndn/broadcast/sync", nodes.Get(11), 1);

  // Local Network 2
  ndn::FibHelper::AddRoute(nodes.Get(4), "/ndn/broadcast/sync", nodes.Get(5), 1);
  ndn::FibHelper::AddRoute(nodes.Get(5), "/ndn/broadcast/sync", nodes.Get(4), 1);
  ndn::FibHelper::AddRoute(nodes.Get(5), "/ndn/broadcast/sync", nodes.Get(6), 1);
  ndn::FibHelper::AddRoute(nodes.Get(5), "/ndn/broadcast/sync", nodes.Get(7), 1);
  ndn::FibHelper::AddRoute(nodes.Get(6), "/ndn/broadcast/sync", nodes.Get(5), 1);
  ndn::FibHelper::AddRoute(nodes.Get(7), "/ndn/broadcast/sync", nodes.Get(5), 1);

  Simulator::Stop(Seconds(20.0));

  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

} // namespace ndn
} // namespace ns3

int
main(int argc, char* argv[])
{
  return ns3::ndn::main(argc, argv);
}
