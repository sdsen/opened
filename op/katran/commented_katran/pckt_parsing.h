/* Copyright (C) 2018-present, Facebook, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __PCKT_PARSING_H
#define __PCKT_PARSING_H

/*
 * This file contains generic packet parsing routines (e.g. tcp/udp headers
 * parsing etc)
 */

#include <linux/icmp.h>
#include <linux/icmpv6.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/ptrace.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/version.h>
#include <stdbool.h>
#include <stddef.h>

#include "balancer_consts.h"
#include "balancer_helpers.h"
#include "bpf.h"

struct quic_long_header {
  __u8 flags;
  __u32 version;
  // Pre draft-22: Dest Conn Id Len(4 bits) | Source Conn Id Len(4 bits)
  // Post draft-22: Dest Conn Id Len (8 bits)
  __u8 conn_id_lens;
  // conn-id len can be of either 0 bytes in length or between 4 and 18 bytes
  // For routing, katran requires minimum of 'QUIC_MIN_CONNID_LEN',
  // and doesn't read beyond that
  __u8 dst_connection_id[QUIC_MIN_CONNID_LEN];
} __attribute__((__packed__));

struct quic_short_header {
  __u8 flags;
  __u8 connection_id[QUIC_MIN_CONNID_LEN];
} __attribute__((__packed__));

/* 
 OPENED COMMENT BEGIN 
{
  "capability": [],
  "helperCallParams": {},
  "startLine": 58,
  "endLine": 74,
  "File": "/home/sayandes/opened_extraction/examples/katran/pckt_parsing.h",
  "Funcname": "calc_offset",
  "Update_maps": [
    ""
  ],
  "Read_maps": [
    ""
  ],
  "Input": [
    "bool is_ipv6",
    " bool is_icmp"
  ],
  "Output": "staticinline__u64",
  "Helper": "",
  "human_func_description": [
    {
      "description": "",
      "author": "",
      "author_email": "",
      "date": ""
    }
  ],
  "AI_func_description": [
    {
      "description": "",
      "author": "",
      "author_email": "",
      "date": "",
      "params": ""
    }
  ]
}
,
 Func Description: TO BE ADDED, 
 Commentor: TO BE ADDED (<name>,<email>) 
 } 
 OPENED COMMENT END 
 */ 
__attribute__((__always_inline__)) static inline __u64 calc_offset(
    bool is_ipv6,
    bool is_icmp) {
  __u64 off = sizeof(struct ethhdr);
  if (is_ipv6) {
    off += sizeof(struct ipv6hdr);
    if (is_icmp) {
      off += (sizeof(struct icmp6hdr) + sizeof(struct ipv6hdr));
    }
  } else {
    off += sizeof(struct iphdr);
    if (is_icmp) {
      off += (sizeof(struct icmphdr) + sizeof(struct iphdr));
    }
  }
  return off;
}

/* 
 OPENED COMMENT BEGIN 
{
  "capability": [],
  "helperCallParams": {},
  "startLine": 76,
  "endLine": 100,
  "File": "/home/sayandes/opened_extraction/examples/katran/pckt_parsing.h",
  "Funcname": "parse_udp",
  "Update_maps": [
    ""
  ],
  "Read_maps": [
    ""
  ],
  "Input": [
    "void *data",
    " void *data_end",
    " bool is_ipv6",
    " struct packet_description *pckt"
  ],
  "Output": "staticinlinebool",
  "Helper": "",
  "human_func_description": [
    {
      "description": "",
      "author": "",
      "author_email": "",
      "date": ""
    }
  ],
  "AI_func_description": [
    {
      "description": "",
      "author": "",
      "author_email": "",
      "date": "",
      "params": ""
    }
  ]
}
,
 Func Description: TO BE ADDED, 
 Commentor: TO BE ADDED (<name>,<email>) 
 } 
 OPENED COMMENT END 
 */ 
__attribute__((__always_inline__)) static inline bool parse_udp(
    void* data,
    void* data_end,
    bool is_ipv6,
    struct packet_description* pckt) {
  bool is_icmp = !((pckt->flags & F_ICMP) == 0);
  __u64 off = calc_offset(is_ipv6, is_icmp);
  struct udphdr* udp;
  udp = data + off;

  if (udp + 1 > data_end) {
    return false;
  }

  if (!is_icmp) {
    pckt->flow.port16[0] = udp->source;
    pckt->flow.port16[1] = udp->dest;
  } else {
    // packet_description was created from icmp "packet too big". hence
    // we need to invert src/dst ports
    pckt->flow.port16[0] = udp->dest;
    pckt->flow.port16[1] = udp->source;
  }
  return true;
}

/* 
 OPENED COMMENT BEGIN 
{
  "capability": [],
  "helperCallParams": {},
  "startLine": 102,
  "endLine": 130,
  "File": "/home/sayandes/opened_extraction/examples/katran/pckt_parsing.h",
  "Funcname": "parse_tcp",
  "Update_maps": [
    ""
  ],
  "Read_maps": [
    ""
  ],
  "Input": [
    "void *data",
    " void *data_end",
    " bool is_ipv6",
    " struct packet_description *pckt"
  ],
  "Output": "staticinlinebool",
  "Helper": "",
  "human_func_description": [
    {
      "description": "",
      "author": "",
      "author_email": "",
      "date": ""
    }
  ],
  "AI_func_description": [
    {
      "description": "",
      "author": "",
      "author_email": "",
      "date": "",
      "params": ""
    }
  ]
}
,
 Func Description: TO BE ADDED, 
 Commentor: TO BE ADDED (<name>,<email>) 
 } 
 OPENED COMMENT END 
 */ 
__attribute__((__always_inline__)) static inline bool parse_tcp(
    void* data,
    void* data_end,
    bool is_ipv6,
    struct packet_description* pckt) {
  bool is_icmp = !((pckt->flags & F_ICMP) == 0);
  __u64 off = calc_offset(is_ipv6, is_icmp);
  struct tcphdr* tcp;
  tcp = data + off;

  if (tcp + 1 > data_end) {
    return false;
  }

  if (tcp->syn) {
    pckt->flags |= F_SYN_SET;
  }

  if (!is_icmp) {
    pckt->flow.port16[0] = tcp->source;
    pckt->flow.port16[1] = tcp->dest;
  } else {
    // packet_description was created from icmp "packet too big". hence
    // we need to invert src/dst ports
    pckt->flow.port16[0] = tcp->dest;
    pckt->flow.port16[1] = tcp->source;
  }
  return true;
}

struct hdr_opt_state {
  __u32 server_id;
  __u8 byte_offset;
  __u8 hdr_bytes_remaining;
};

#ifdef TCP_SERVER_ID_ROUTING
#ifdef TCP_HDR_OPT_SKIP_UNROLL_LOOP
__attribute__ ((noinline))
#else
__attribute__ ((__always_inline__))
#endif
/* 
 OPENED COMMENT BEGIN 
{
  "capability": [],
  "helperCallParams": {},
  "startLine": 144,
  "endLine": 197,
  "File": "/home/sayandes/opened_extraction/examples/katran/pckt_parsing.h",
  "Funcname": "parse_hdr_opt",
  "Update_maps": [
    ""
  ],
  "Read_maps": [
    ""
  ],
  "Input": [
    "const struct xdp_md *xdp",
    " struct hdr_opt_state *state"
  ],
  "Output": "int",
  "Helper": "",
  "human_func_description": [
    {
      "description": "",
      "author": "",
      "author_email": "",
      "date": ""
    }
  ],
  "AI_func_description": [
    {
      "description": "",
      "author": "",
      "author_email": "",
      "date": "",
      "params": ""
    }
  ]
}
,
 Func Description: TO BE ADDED, 
 Commentor: TO BE ADDED (<name>,<email>) 
 } 
 OPENED COMMENT END 
 */ 
int parse_hdr_opt(const struct xdp_md *xdp, struct hdr_opt_state *state)
{
  const void* data = (void*)(long)xdp->data;
  const void* data_end = (void*)(long)xdp->data_end;
  __u8 *tcp_opt, kind, hdr_len;

  // Need this check to satisify the verifier
  if (!state) {
    return -1;
  }

  tcp_opt = (__u8*)(data + state->byte_offset);
  if (tcp_opt + 1 > data_end) {
    return -1;
  }

  kind = tcp_opt[0];
  if (kind == TCP_OPT_EOL) {
    return -1;
  }

  if (kind == TCP_OPT_NOP) {
    state->hdr_bytes_remaining--;
    state->byte_offset++;
    return 0;
  }

  if (state->hdr_bytes_remaining < 2 ||
      tcp_opt + sizeof(__u8) + sizeof(__u8) > data_end) {
    return -1;
  }

  hdr_len = tcp_opt[1];
  if (hdr_len > state->hdr_bytes_remaining) {
    return -1;
  }

  if (kind == TCP_HDR_OPT_KIND_TPR) {
    if (hdr_len != TCP_HDR_OPT_LEN_TPR) {
      return -1;
    }

    if (tcp_opt + TCP_HDR_OPT_LEN_TPR > data_end) {
      return -1;
    }

    state->server_id = *(__u32*)&tcp_opt[2];
    return 1;
  }

  state->hdr_bytes_remaining -= hdr_len;
  state->byte_offset += hdr_len;
  return 0;
}

/* 
 OPENED COMMENT BEGIN 
{
  "capability": [
    {
      "map_read": [
        {
          "Description": "Perform a lookup in <[ map ]>(IP: 0) for an entry associated to key. ",
          "Return": "Map value associated to key, or NULL if no entry was found.",
          "Return Type": "void",
          "Function Name": "*bpf_map_lookup_elem",
          "Input Params": [
            "{Type: struct bpf_map ,Var: *map}",
            "{Type:  const void ,Var: *key}"
          ]
        }
      ]
    },
    {
      "map_update": [
        {
          "Description": "Add or update the <[ value ]>(IP: 2) of the entry associated to <[ key ]>(IP: 1) in <[ map ]>(IP: 0) with value. <[ flags ]>(IP: 3) is one of:BPF_NOEXIST The entry for <[ key ]>(IP: 1) must not exist in the map. BPF_EXIST The entry for <[ key ]>(IP: 1) must already exist in the map. BPF_ANY No condition on the existence of the entry for key. Flag <[ value ]>(IP: 2) BPF_NOEXIST cannot be used for maps of types BPF_MAP_TYPE_ARRAY or BPF_MAP_TYPE_PERCPU_ARRAY (all elements always exist) , the helper would return an error. ",
          "Return": "0 on success, or a negative error in case of failure.",
          "Return Type": "int",
          "Function Name": "bpf_map_update_elem",
          "Input Params": [
            "{Type: struct bpf_map ,Var: *map}",
            "{Type:  const void ,Var: *key}",
            "{Type:  const void ,Var: *value}",
            "{Type:  u64 ,Var: flags}"
          ]
        }
      ]
    }
  ],
  "helperCallParams": {
    "bpf_map_lookup_elem": [
      "{\n \"opVar\": \"  __u32* real_pos \",\n \"inpVar\": [\n  \" &server_id_map\",\n  \" &key\"\n ]\n}",
      "{\n \"opVar\": \"  *real \",\n \"inpVar\": [\n  \" &reals\",\n  \" &key\"\n ]\n}",
      "{\n \"opVar\": \"    struct real_pos_lru* dst_lru \",\n \"inpVar\": [\n  \" lru_map\",\n  \" &pckt->flow\"\n ]\n}"
    ],
    "bpf_map_update_elem": [
      "{\n \"opVar\": \"NA\",\n \"inpVar\": [\n  \"    lru_map\",\n  \" &pckt->flow\",\n  \" &new_dst_lru\",\n  \" BPF_ANY\"\n ]\n}"
    ]
  },
  "startLine": 199,
  "endLine": 273,
  "File": "/home/sayandes/opened_extraction/examples/katran/pckt_parsing.h",
  "Funcname": "tcp_hdr_opt_lookup",
  "Update_maps": [
    " lru_map",
    ""
  ],
  "Read_maps": [
    " server_id_map",
    " reals",
    " lru_map",
    ""
  ],
  "Input": [
    "const struct xdp_md *xdp",
    " bool is_ipv6",
    " struct real_definition **real",
    " struct packet_description *pckt",
    " bool bypass_lru",
    " void *lru_map"
  ],
  "Output": "staticinlineint",
  "Helper": "bpf_map_update_elem,bpf_map_lookup_elem,",
  "human_func_description": [
    {
      "description": "",
      "author": "",
      "author_email": "",
      "date": ""
    }
  ],
  "AI_func_description": [
    {
      "description": "",
      "author": "",
      "author_email": "",
      "date": "",
      "params": ""
    }
  ]
}
,
 Func Description: TO BE ADDED, 
 Commentor: TO BE ADDED (<name>,<email>) 
 } 
 OPENED COMMENT END 
 */ 
__attribute__((__always_inline__)) static inline int tcp_hdr_opt_lookup(
    const struct xdp_md* xdp,
    bool is_ipv6,
    struct real_definition** real,
    struct packet_description* pckt,
    bool bypass_lru,
    void* lru_map) {
  const void* data = (void*)(long)xdp->data;
  const void* data_end = (void*)(long)xdp->data_end;
  struct real_pos_lru* dst_lru;
  struct tcphdr* tcp_hdr;
  __u8 tcp_hdr_opt_len = 0;
  __u64 tcp_offset = 0;
  struct hdr_opt_state opt_state = {};
  int err = 0;

  tcp_offset = calc_offset(is_ipv6, false /* is_icmp */);
  tcp_hdr = (struct tcphdr*)(data + tcp_offset);
  if (tcp_hdr + 1 > data_end) {
    return FURTHER_PROCESSING;
  }
  tcp_hdr_opt_len = (tcp_hdr->doff * 4) - sizeof(struct tcphdr);
  if (tcp_hdr_opt_len < TCP_HDR_OPT_LEN_TPR) {
    return FURTHER_PROCESSING;
  }

  opt_state.hdr_bytes_remaining = tcp_hdr_opt_len;
  opt_state.byte_offset = sizeof(struct tcphdr) + tcp_offset;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 3, 0) || \
    !defined TCP_HDR_OPT_SKIP_UNROLL_LOOP
  // For linux kernel version < 5.3, there isn't support in the bpf verifier
  // for validating bounded loops, so we need to unroll the loop
#pragma clang loop unroll(full)
#endif
  for (int i = 0; i < TCP_HDR_OPT_MAX_OPT_CHECKS; i++) {
    err = parse_hdr_opt(xdp, &opt_state);
    if (err || !opt_state.hdr_bytes_remaining) {
      break;
    }
  }

  if (!opt_state.server_id) {
    return FURTHER_PROCESSING;
  }

  __u32 key = opt_state.server_id;
  __u32* real_pos = bpf_map_lookup_elem(&server_id_map, &key);
  if (!real_pos) {
    return FURTHER_PROCESSING;
  }
  key = *real_pos;
  if (key == 0) {
    // Since server_id_map is a bpf_map_array all its members are 0-initialized
    // This can lead to a false match for non-existing key to real at index 0.
    // So, just skip key of value 0 to avoid misrouting of packets.
    return FURTHER_PROCESSING;
  }
  pckt->real_index = key;
  *real = bpf_map_lookup_elem(&reals, &key);
  if (!(*real)) {
    return FURTHER_PROCESSING;
  }
  // update this routing decision in the lru_map as well
  if (!bypass_lru) {
    struct real_pos_lru* dst_lru = bpf_map_lookup_elem(lru_map, &pckt->flow);
    if (dst_lru) {
      dst_lru->pos = key;
      return 0;
    }
    struct real_pos_lru new_dst_lru = {};
    new_dst_lru.pos = key;
    bpf_map_update_elem(lru_map, &pckt->flow, &new_dst_lru, BPF_ANY);
  }
  return 0;
}
#endif // TCP_SERVER_ID_ROUTING

/* 
 OPENED COMMENT BEGIN 
{
  "capability": [],
  "helperCallParams": {},
  "startLine": 276,
  "endLine": 335,
  "File": "/home/sayandes/opened_extraction/examples/katran/pckt_parsing.h",
  "Funcname": "parse_quic",
  "Update_maps": [
    ""
  ],
  "Read_maps": [
    ""
  ],
  "Input": [
    "void *data",
    " void *data_end",
    " bool is_ipv6",
    " struct packet_description *pckt"
  ],
  "Output": "staticinlineint",
  "Helper": "",
  "human_func_description": [
    {
      "description": "",
      "author": "",
      "author_email": "",
      "date": ""
    }
  ],
  "AI_func_description": [
    {
      "description": "",
      "author": "",
      "author_email": "",
      "date": "",
      "params": ""
    }
  ]
}
,
 Func Description: TO BE ADDED, 
 Commentor: TO BE ADDED (<name>,<email>) 
 } 
 OPENED COMMENT END 
 */ 
__attribute__((__always_inline__)) static inline int parse_quic(
    void* data,
    void* data_end,
    bool is_ipv6,
    struct packet_description* pckt) {
  bool is_icmp = (pckt->flags & F_ICMP);
  __u64 off = calc_offset(is_ipv6, is_icmp);
  // offset points to the beginning of transport header (udp) of quic's packet
  /*                                      |QUIC PKT TYPE|           */
  if ((data + off + sizeof(struct udphdr) + sizeof(__u8)) > data_end) {
    return FURTHER_PROCESSING;
  }

  __u8* quic_data = data + off + sizeof(struct udphdr);
  __u8* pkt_type = quic_data;
  __u8* connId = NULL;
  // the position of conn id varies depending on whether the packet has a
  // long-header or short-header.
  // Once we compute the offset of conn id, just read fixed length,
  // even if the connid len can be of 0 or 4-18 bytes, since katran is only
  // concerned about the first 16 bits in Dest Conn Id
  if ((*pkt_type & QUIC_LONG_HEADER) == QUIC_LONG_HEADER) {
    // packet with long header
    if (quic_data + sizeof(struct quic_long_header) > data_end) {
      return FURTHER_PROCESSING;
    }
    if ((*pkt_type & QUIC_PACKET_TYPE_MASK) < QUIC_HANDSHAKE) {
      // for client initial and 0rtt packet - fall back to use c. hash, since
      // the connection-id is not the server-chosen one.
      return FURTHER_PROCESSING;
    }

    struct quic_long_header* long_header = (struct quic_long_header*)quic_data;
    // Post draft version 22, this byte is the conn id length of dest conn id
    if (long_header->conn_id_lens < QUIC_MIN_CONNID_LEN) {
      return FURTHER_PROCESSING;
    }
    connId = long_header->dst_connection_id;
  } else {
    // short header: just read the connId
    if (quic_data + sizeof(struct quic_short_header) > data_end) {
      return FURTHER_PROCESSING;
    }
    connId = ((struct quic_short_header*)quic_data)->connection_id;
  }
  if (!connId) {
    return FURTHER_PROCESSING;
  }
  // connId schema: if first two bits contain the right version info
  __u8 connIdVersion = (connId[0] >> 6);
  if (connIdVersion == QUIC_CONNID_VERSION_V1) {
    // extract last 16 bits from the first 18 bits:
    //            last 6 bits         +    8 bits        +   first 2 bits
    return ((connId[0] & 0x3F) << 10) | (connId[1] << 2) | (connId[2] >> 6);
  } else if (connIdVersion == QUIC_CONNID_VERSION_V2) {
    __u32 cid = (connId[1] << 16) | (connId[2] << 8) | (connId[3]);
    return cid;
  }
  return FURTHER_PROCESSING;
}

#endif // of  __PCKT_PARSING_H
