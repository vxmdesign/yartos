/****************************************************************************
 * net/icmpv6/icmpv6.h
 *
 *   Copyright (C) 2015 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#ifndef __NET_ICMPv6_ICMPv6_H
#define __NET_ICMPv6_ICMPv6_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/types.h>

#include <nuttx/net/ip.h>

#ifdef CONFIG_NET_ICMPv6

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Type Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Data
 ****************************************************************************/

#ifdef __cplusplus
#  define EXTERN extern "C"
extern "C"
{
#else
#  define EXTERN extern
#endif

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Name: icmpv6_input
 *
 * Description:
 *   Handle incoming ICMPv6 input
 *
 * Parameters:
 *   dev - The device driver structure containing the received ICMPv6
 *         packet
 *
 * Return:
 *   None
 *
 * Assumptions:
 *   Called from the interrupt level or with interrupts disabled.
 *
 ****************************************************************************/

void icmpv6_input(FAR struct net_driver_s *dev);

/****************************************************************************
 * Name: icmpv6_poll
 *
 * Description:
 *   Poll a UDP "connection" structure for availability of TX data
 *
 * Parameters:
 *   dev - The device driver structure to use in the send operation
 *
 * Return:
 *   None
 *
 * Assumptions:
 *   Called from the interrupt level or with interrupts disabled.
 *
 ****************************************************************************/

#ifdef CONFIG_NET_ICMPv6_PING
void icmpv6_poll(FAR struct net_driver_s *dev);
#endif /* CONFIG_NET_ICMPv6_PING */

/****************************************************************************
 * Name: icmpv6_send
 *
 * Description:
 *   Setup to send an ICMPv6 packet
 *
 * Parameters:
 *   dev - The device driver structure to use in the send operation
 *
 * Return:
 *   None
 *
 * Assumptions:
 *   Called from the interrupt level or with interrupts disabled.
 *
 ****************************************************************************/

#ifdef CONFIG_NET_ICMPv6_PING
void icmpv6_send(FAR struct net_driver_s *dev, FAR net_ipv6addr_t *destaddr);
#endif /* CONFIG_NET_ICMPv6_PING */

/****************************************************************************
 * Name: icmpv6_solicit
 *
 * Description:
 *   Set up to send an ICMPv6 Neighbor Solicitation message
 *
 * Parameters:
 *   dev - Reference to an Ethernet device driver structure
 *   ipaddr - IP address of Neighbor to be solicited
 *
 * Return:
 *   None
 *
 ****************************************************************************/

void icmpv6_solicit(FAR struct net_driver_s *dev,
                    FAR const net_ipv6addr_t ipaddr);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* CONFIG_NET_ICMPv6 */
#endif /* __NET_ICMPv6_ICMPv6_H */
