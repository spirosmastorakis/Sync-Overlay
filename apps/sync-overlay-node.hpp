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

#ifndef SYNC_OVERLAY_NODE_H
#define SYNC_OVERLAY_NODE_H

#include "ns3/ndnSIM/model/ndn-common.hpp"

#include "ns3/ndnSIM/apps/ndn-app.hpp"
#include "ns3/ndnSIM/model/ndn-common.hpp"

#include "ns3/nstime.h"
#include "ns3/ptr.h"

namespace ns3 {
namespace ndn {

 /**
  * @ingroup ndn-apps
  * @brief A simple Interest-sink applia simple Interest-sink application
  *
  * A simple Interest-sink applia simple Interest-sink application,
  * which replying every incoming Interest with Data packet with a specified
  * size and name same as in Interest.cation, which replying every incoming Interest
  * with Data packet with a specified size and name same as in Interest.
  */
  class SyncOverlayNode : public App {
  public:
    static TypeId
    GetTypeId(void);

    SyncOverlayNode();

    // inherited from NdnApp
    virtual void
    OnInterest(shared_ptr<const Interest> interest);

    // inherited from NdnApp
    void
    OnData(shared_ptr<const Data> data);

  protected:
    // inherited from Application base class.
    virtual void
    StartApplication(); // Called at time specified by Start

    virtual void
    StopApplication(); // Called at time specified by Stop

  private:
    Name m_prefix;
    Name m_postfix;
    Name m_overlayPrefix;
};

} // namespace ndn
} // namespace ns3

#endif // SYNC_OVERLAY_NODE_H
