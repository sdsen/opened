/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause) */
/* Copyright Authors of Cilium */

#ifndef __HASH_H_
#define __HASH_H_

#include "common.h"
#include "jhash.h"

/* The daddr is explicitly excluded from the hash here in order to allow for
 * backend selection to choose the same backend even on different service VIPs.
 */
/* 
 OPENED COMMENT BEGIN 
{
  "capabilities": [],
  "helperCallParams": {},
  "startLine": 13,
  "endLine": 18,
  "File": "/home/sayandes/opened_extraction/examples/cilium/lib/hash.h",
  "funcName": "hash_from_tuple_v4",
  "updateMaps": [],
  "readMaps": [],
  "input": [
    "const struct ipv4_ct_tuple *tuple"
  ],
  "output": "static__always_inline__u32",
  "helper": [],
  "compatibleHookpoints": [
    "cgroup_device",
    "sched_cls",
    "perf_event",
    "sched_act",
    "cgroup_sock",
    "raw_tracepoint",
    "sk_msg",
    "cgroup_skb",
    "lwt_seg6local",
    "lwt_xmit",
    "cgroup_sock_addr",
    "tracepoint",
    "cgroup_sysctl",
    "lwt_out",
    "raw_tracepoint_writable",
    "xdp",
    "sk_reuseport",
    "sock_ops",
    "flow_dissector",
    "sk_skb",
    "kprobe",
    "socket_filter",
    "lwt_in"
  ],
  "source": [
    "static __always_inline __u32 hash_from_tuple_v4 (const struct ipv4_ct_tuple *tuple)\n",
    "{\n",
    "    return jhash_3words (tuple->saddr, ((__u32) tuple->dport << 16) | tuple->sport, tuple->nexthdr, HASH_INIT4_SEED);\n",
    "}\n"
  ],
  "called_function_list": [
    "jhash_3words"
  ],
  "call_depth": -1,
  "humanFuncDescription": [
    null
  ],
  "AI_func_description": [
    {
      "description": "",
      "author": "",
      "authorEmail": "",
      "date": "",
      "invocationParameters": ""
    }
  ]
} 
 OPENED COMMENT END 
 */ 
static __always_inline __u32 hash_from_tuple_v4(const struct ipv4_ct_tuple *tuple)
{
	return jhash_3words(tuple->saddr,
			    ((__u32)tuple->dport << 16) | tuple->sport,
			    tuple->nexthdr, HASH_INIT4_SEED);
}

/* 
 OPENED COMMENT BEGIN 
{
  "capabilities": [],
  "helperCallParams": {},
  "startLine": 20,
  "endLine": 35,
  "File": "/home/sayandes/opened_extraction/examples/cilium/lib/hash.h",
  "funcName": "hash_from_tuple_v6",
  "updateMaps": [],
  "readMaps": [],
  "input": [
    "const struct ipv6_ct_tuple *tuple"
  ],
  "output": "static__always_inline__u32",
  "helper": [],
  "compatibleHookpoints": [
    "cgroup_device",
    "sched_cls",
    "perf_event",
    "sched_act",
    "cgroup_sock",
    "raw_tracepoint",
    "sk_msg",
    "cgroup_skb",
    "lwt_seg6local",
    "lwt_xmit",
    "cgroup_sock_addr",
    "tracepoint",
    "cgroup_sysctl",
    "lwt_out",
    "raw_tracepoint_writable",
    "xdp",
    "sk_reuseport",
    "sock_ops",
    "flow_dissector",
    "sk_skb",
    "kprobe",
    "socket_filter",
    "lwt_in"
  ],
  "source": [
    "static __always_inline __u32 hash_from_tuple_v6 (const struct ipv6_ct_tuple *tuple)\n",
    "{\n",
    "    __u32 a, b, c;\n",
    "    a = tuple->saddr.p1;\n",
    "    b = tuple->saddr.p2;\n",
    "    c = tuple->saddr.p3;\n",
    "    __jhash_mix (a, b, c);\n",
    "    a += tuple->saddr.p4;\n",
    "    b += ((__u32) tuple->dport << 16) | tuple->sport;\n",
    "    c += tuple->nexthdr;\n",
    "    __jhash_mix (a, b, c);\n",
    "    a += HASH_INIT6_SEED;\n",
    "    __jhash_final (a, b, c);\n",
    "    return c;\n",
    "}\n"
  ],
  "called_function_list": [
    "__jhash_mix",
    "__jhash_final"
  ],
  "call_depth": -1,
  "humanFuncDescription": [
    null
  ],
  "AI_func_description": [
    {
      "description": "",
      "author": "",
      "authorEmail": "",
      "date": "",
      "invocationParameters": ""
    }
  ]
} 
 OPENED COMMENT END 
 */ 
static __always_inline __u32 hash_from_tuple_v6(const struct ipv6_ct_tuple *tuple)
{
	__u32 a, b, c;

	a = tuple->saddr.p1;
	b = tuple->saddr.p2;
	c = tuple->saddr.p3;
	__jhash_mix(a, b, c);
	a += tuple->saddr.p4;
	b += ((__u32)tuple->dport << 16) | tuple->sport;
	c += tuple->nexthdr;
	__jhash_mix(a, b, c);
	a += HASH_INIT6_SEED;
	__jhash_final(a, b, c);
	return c;
}

#endif /* __HASH_H_ */
