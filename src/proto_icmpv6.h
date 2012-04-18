/*
 * netsniff-ng - the packet sniffing beast
 * Copyright 2012 Markus Amend <markus@netsniff-ng.org>
 * Subject to the GPL, version 2.
 *
 * ICMPv6 described in RFC4443, RFC2710, RFC4861, RFC2894,
 * RFC4620, RFC3122, RFC3810, RFC3775, RFC3971, RFC4065
 * RFC4286
 */

#ifndef PROTO_ICMPV6_H
#define PROTO_ICMPV6_H

#include <stdio.h>
#include <stdint.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "built_in.h"
#include "proto_struct.h"
#include "dissector_eth.h"
#include "pkt_buff.h"
#include "built_in.h"

struct icmpv6hdr {
	uint8_t h_type;
	uint8_t h_code;
	uint16_t h_chksum;
} __packed;

static char *icmpv6_type_1_strings[] = {
	"No route to destination",
	"Communication with destination administratively prohibited",
	"Beyond scope of source address",
	"Address unreachable",
	"Port unreachable",
	"Source address failed ingress/egress policy",
	"Reject route to destination",
	"Error in Source Routing Header",
};

#define icmpv6_code_range_valid(code, sarr)	((code) < array_size((sarr)))

static inline void icmpv6_process(struct icmpv6hdr *icmp, char **type,
				  char **code, char **optional)
{
	*type = "Unknown Type";
	*code = "Unknown Code";

	switch (icmp->h_type) {
	case 1:
		*type = "Destination Unreachable";
		if (icmpv6_code_range_valid(icmp->h_code, icmpv6_type_1_strings))
			*code = icmpv6_type_1_strings[icmp->h_code];
		return;
	case 2:
		*type = "Packet Too Big";
		return;
	case 3:
		*type = "Time Exceeded";
		return;
	case 4:
		*type = "Parameter Problem";
		return;
	case 100:
		*type = "Private experimation";
		return;
	case 101:
		*type = "Private experimation";
		return;
	case 127:
		*type = "Reserved for expansion of ICMPv6 error messages";
		return;
	case 128:
		*type = "Echo Request";
		return;
	case 129:
		*type = "Echo Reply";
		return;
	case 130:
		*type = "Multicast Listener Query";
		return;
	case 131:
		*type = "Multicast Listener Report";
		return;
	case 132:
		*type = "Multicast Listener Done";
		return;
	case 133:
		*type = "Router Solicitation";
		return;
	case 134:
		*type = "Router Advertisement";
		return;
	case 135:
		*type = "Neighbor Solicitation";
		return;
	case 136:
		*type = "Neighbor Advertisement";
		return;
	case 137:
		*type = "Redirect Message";
		return;
	case 138:
		*type = "Router Renumbering";
		return;
	case 139:
		*type = "ICMP Node Information Query";
		return;
	case 140:
		*type = "ICMP Node Information Response";
		return;
	case 141:
		*type = "Inverse Neighbor Discovery Solicitation Message";
		return;
	case 142:
		*type = "Inverse Neighbor Discovery Advertisement Message";
		return;
	case 143:
		*type = "Multicast Listener Report v2";
		return;
	case 144:
		*type = "Home Agent Address Discovery Request Message";
		return;
	case 145:
		*type = "Home Agent Address Discovery Reply Message";
		return;
	case 146:
		*type = "Mobile Prefix Solicitation";
		return;
	case 147:
		*type = "Mobile Prefix Advertisement";
		return;
	case 148:
		*type = "Certification Path Solicitation";
		return;
	case 149:
		*type = "Certification Path Advertisement";
		return;
	case 150:
		*type = "ICMP messages utilized by experimental mobility "
			"protocols such as Seamoby";
		return;
	case 151:
		*type = "Multicast Router Advertisement";
		return;
	case 152:
		*type = "Multicast Router Solicitation";
		return;
	case 153:
		*type = "Multicast Router Termination";
		return;
	case 155:
		*type = "RPL Control Message";
		return;
	case 200:
		*type = "Private experimation";
		return;
	case 201:
		*type = "Private experimation";
		return;
	case 255:
		*type = "Reserved for expansion of ICMPv6 error messages";
		return;
	}
}

static inline void icmpv6(struct pkt_buff *pkt)
{
	char *type = NULL, *code = NULL, *optional = NULL;
	struct icmpv6hdr *icmp =
		(struct icmpv6hdr *) pkt_pull(pkt, sizeof(*icmp));

	if (icmp == NULL)
		return;

	icmpv6_process(icmp, &type, &code, &optional);

	tprintf(" [ ICMPv6 ");
	tprintf("%s (%u), ", type, icmp->h_type);
	tprintf("%s (%u), ", code, icmp->h_code);
	tprintf("Chks (0x%x)", ntohs(icmp->h_chksum));
	if (optional)
		tprintf(" %s", optional);
	tprintf(" ]\n\n");
}

static inline void icmpv6_less(struct pkt_buff *pkt)
{
	struct icmpv6hdr *icmp =
		(struct icmpv6hdr *) pkt_pull(pkt, sizeof(*icmp));

	if (icmp == NULL)
		return;

	tprintf(" ICMPv6 Type (%u) Code (%u)", icmp->h_type, icmp->h_code);
}

struct protocol icmpv6_ops = {
	.key = 0x3A,
	.print_full = icmpv6,
	.print_less = icmpv6_less,
};

#endif /* PROTO_ICMPV6_H */
