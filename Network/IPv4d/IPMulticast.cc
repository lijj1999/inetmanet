//
// Copyright (C) 2000 Institut fuer Telematik, Universitaet Karlsruhe
// Copyright (C) 2004 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

//  Cleanup and rewrite: Andras Varga, 2004

#include <stdlib.h>
#include <omnetpp.h>
#include <string.h>

#include "IPMulticast.h"
#include "IPControlInfo_m.h"

Define_Module(IPMulticast);



void IPMulticast::initialize()
{
}

void IPMulticast::handleMessage(cMessage *msg)
{
    RoutingTable *rt = routingTableAccess.get();

    IPDatagram *datagram = check_and_cast<IPDatagram *>(msg);
    IPRoutingDecision *controlInfo = check_and_cast<IPRoutingDecision *>(msg->controlInfo());
    int inputPort = controlInfo->inputPort();

    IPAddress destAddress = datagram->destAddress();

    // DVMRP: process datagram only if sent locally or arrived on
    // the shortest route; otherwise discard and continue.
    if (controlInfo->inputPort()!=-1 && controlInfo->inputPort()!=rt->outputPortNo(datagram->srcAddress()))
    {
        /* debugging output
        ev  << "* outputPort (shortest path): "
        << rt->outputPortNo(datagram->srcAddress())
        << "   inputPort: " << datagram->inputPort() << "\n";
        */

        delete datagram;
        return;
    }

    // check for local delivery
    if (rt->multicastLocalDeliver(destAddress))
    {
        IPDatagram *datagramCopy = (IPDatagram *) datagram->dup();
        // FIXME control info will NOT be present in duplicate packet!
// BCH Andras -- code from UTS MPLS model  FIXME!!!!!!!!!!!!!!!!!!!!!!!!
        // find "local_addr" module parameter among our parents, and assign it to packet
        cModule *curmod = this;
        for (curmod = parentModule(); curmod != NULL; curmod = curmod->parentModule())
        {
            if (curmod->hasPar("local_addr"))
            {
                datagramCopy->setDestAddress(IPAddress(curmod->par("local_addr").stringValue()));
                break;
            }
        }
// ECH
        send(datagramCopy, "localOut");
    }

    // forward datagram only if IP forwarding is on, or sent locally
    if (inputPort!=-1 && !rt->ipForward())
    {
        delete datagram;
        return;
    }

    MulticastRoutes routes = rt->multicastRoutesFor(destAddress);
    if (routes.size()==0)
    {
        // no destination: delete datagram
        delete datagram;
    }
    else
    {
        // copy original datagram for multiple destinations
        for (int i=0; i<routes.size(); i++)
        {
            int outputPort = routes[i].interf->outputPort;

            // don't forward to input port
            if (outputPort>=0 && outputPort!=inputPort)
            {
                IPDatagram *datagramCopy = (IPDatagram *) datagram->dup();

                // add a copy of the control info (OMNeT++ doesn't copy it)
                IPRoutingDecision *newControlInfo = new IPRoutingDecision();
                newControlInfo->setOutputPort(outputPort);
                //newControlInfo->setNextHopAddr(...); FIXME TODO
                datagramCopy->setControlInfo(newControlInfo);

                send(datagramCopy, "fragmentationOut");
            }
        }

        // only copies sent, delete original datagram
        delete datagram;
    }
}
