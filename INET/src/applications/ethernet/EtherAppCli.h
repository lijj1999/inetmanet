/*
 * Copyright (C) 2003 CTIE, Monash University
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef __ETHERAPPCLI_H
#define __ETHERAPPCLI_H

#include "INETDefs.h"
#include "MACAddress.h"


/**
 * Simple traffic generator for the Ethernet model.
 */
class INET_API EtherAppCli : public cSimpleModule
{
  protected:
    // send parameters
    long seqNum;
    cPar *reqLength;
    cPar *respLength;
    cPar *waitTime;

    int localSAP;
    int remoteSAP;
    MACAddress destMACAddress;

    // receive statistics
    long packetsSent;
    long packetsReceived;
    cOutVector eedVector;
    cStdDev eedStats;

  protected:
    virtual void initialize(int stage);
    virtual int numInitStages() const {return 2;}
    virtual void handleMessage(cMessage *msg);
    virtual void finish();

    virtual MACAddress resolveDestMACAddress();

    virtual void sendPacket();
    virtual void receivePacket(cMessage *msg);
    virtual void registerDSAP(int dsap);
};

#endif

