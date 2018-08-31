/*
 * dhcpcd - DHCP client daemon -
 * Copyright (C) 1996 - 1997 Yoichi Hariguchi <yoichi@fore.com>
 * Copyright (C) January, 1998 Sergei Viznyuk <sv@phystech.com>
 * 
 * dhcpcd is an RFC2131 and RFC1541 compliant DHCP client daemon.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef BUILDMSG_H
#define BUILDMSG_H

void buildDhcpDiscover(udpipMessage *pDhcpMessage, uint8_t *ClientHwAddr, uint32_t xid, dhcpInterface *DhcpIface, dhcpOptions *DhcpOptions);
void buildDhcpRequest(udpipMessage *pDhcpMessage, uint8_t *ClientHwAddr, uint32_t xid, dhcpInterface *DhcpIface, dhcpOptions *DhcpOptions);
void buildDhcpRenew(udpipMessage *pDhcpMessage, uint8_t *ClientHwAddr, uint32_t xid, dhcpInterface *DhcpIface, dhcpOptions *DhcpOptions);
void buildDhcpRebind(udpipMessage *pDhcpMessage, uint8_t *ClientHwAddr, uint32_t xid, dhcpInterface *DhcpIface, dhcpOptions *DhcpOptions);
void buildDhcpReboot(udpipMessage *pDhcpMessage, uint8_t *ClientHwAddr, uint32_t xid, dhcpInterface *DhcpIface, dhcpOptions *DhcpOptions);
void buildDhcpRelease(udpipMessage *pDhcpMessage, uint8_t *ClientHwAddr, uint32_t xid, dhcpInterface *DhcpIface, dhcpOptions *DhcpOptions);
void buildDhcpDecline(udpipMessage *pDhcpMessage, uint8_t *ClientHwAddr, uint32_t xid, dhcpInterface *DhcpIface, dhcpOptions *DhcpOptions);
void buildDhcpInform(udpipMessage *pDhcpMessage, uint8_t *ClientHwAddr, uint32_t xid, dhcpInterface *DhcpIface, dhcpOptions *DhcpOptions);

#endif
