/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause) */
/* Copyright Authors of Cilium */

#ifndef __LIB_L3_H_
#define __LIB_L3_H_

#include "common.h"
#include "ipv6.h"
#include "ipv4.h"
#include "eps.h"
#include "eth.h"
#include "dbg.h"
#include "l4.h"
#include "icmp6.h"
#include "csum.h"

#ifdef ENABLE_IPV6
/* 
 OPENED COMMENT BEGIN 
{
  "capabilities": [],
  "helperCallParams": {},
  "startLine": 18,
  "endLine": 38,
  "File": "/home/sayandes/opened_extraction/examples/cilium/lib/l3.h",
  "funcName": "ipv6_l3",
  "updateMaps": [],
  "readMaps": [],
  "input": [
    "struct  __ctx_buff *ctx",
    " int l3_off",
    " const __u8 *smac",
    " const __u8 *dmac",
    " __u8 direction"
  ],
  "output": "static__always_inlineint",
  "helper": [],
  "compatibleHookpoints": [
    "cgroup_sysctl",
    "socket_filter",
    "flow_dissector",
    "lwt_out",
    "cgroup_device",
    "raw_tracepoint",
    "cgroup_sock_addr",
    "lwt_in",
    "lwt_xmit",
    "sk_skb",
    "sock_ops",
    "sk_reuseport",
    "xdp",
    "raw_tracepoint_writable",
    "cgroup_skb",
    "lwt_seg6local",
    "tracepoint",
    "perf_event",
    "sk_msg",
    "cgroup_sock",
    "kprobe",
    "sched_cls",
    "sched_act"
  ],
  "source": [
    "static __always_inline int ipv6_l3 (struct  __ctx_buff *ctx, int l3_off, const __u8 *smac, const __u8 *dmac, __u8 direction)\n",
    "{\n",
    "    int ret;\n",
    "    ret = ipv6_dec_hoplimit (ctx, l3_off);\n",
    "    if (IS_ERR (ret))\n",
    "        return ret;\n",
    "    if (ret > 0) {\n",
    "        return icmp6_send_time_exceeded (ctx, l3_off, direction);\n",
    "    }\n",
    "    if (smac && eth_store_saddr (ctx, smac, 0) < 0)\n",
    "        return DROP_WRITE_ERROR;\n",
    "    if (dmac && eth_store_daddr (ctx, dmac, 0) < 0)\n",
    "        return DROP_WRITE_ERROR;\n",
    "    return CTX_ACT_OK;\n",
    "}\n"
  ],
  "humanFuncDescription": [
    {
      "description": "",
      "author": "",
      "authorEmail": "",
      "date": ""
    },
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
static __always_inline int ipv6_l3(struct __ctx_buff *ctx, int l3_off,
				   const __u8 *smac, const __u8 *dmac,
				   __u8 direction)
{
	int ret;

	ret = ipv6_dec_hoplimit(ctx, l3_off);
	if (IS_ERR(ret))
		return ret;
	if (ret > 0) {
		/* Hoplimit was reached */
		return icmp6_send_time_exceeded(ctx, l3_off, direction);
	}

	if (smac && eth_store_saddr(ctx, smac, 0) < 0)
		return DROP_WRITE_ERROR;
	if (dmac && eth_store_daddr(ctx, dmac, 0) < 0)
		return DROP_WRITE_ERROR;

	return CTX_ACT_OK;
}
#endif /* ENABLE_IPV6 */

/* 
 OPENED COMMENT BEGIN 
{
  "capabilities": [],
  "helperCallParams": {},
  "startLine": 41,
  "endLine": 56,
  "File": "/home/sayandes/opened_extraction/examples/cilium/lib/l3.h",
  "funcName": "ipv4_l3",
  "updateMaps": [],
  "readMaps": [],
  "input": [
    "struct  __ctx_buff *ctx",
    " int l3_off",
    " const __u8 *smac",
    " const __u8 *dmac",
    " struct iphdr *ip4"
  ],
  "output": "static__always_inlineint",
  "helper": [],
  "compatibleHookpoints": [
    "cgroup_sysctl",
    "socket_filter",
    "flow_dissector",
    "lwt_out",
    "cgroup_device",
    "raw_tracepoint",
    "cgroup_sock_addr",
    "lwt_in",
    "lwt_xmit",
    "sk_skb",
    "sock_ops",
    "sk_reuseport",
    "xdp",
    "raw_tracepoint_writable",
    "cgroup_skb",
    "lwt_seg6local",
    "tracepoint",
    "perf_event",
    "sk_msg",
    "cgroup_sock",
    "kprobe",
    "sched_cls",
    "sched_act"
  ],
  "source": [
    "static __always_inline int ipv4_l3 (struct  __ctx_buff *ctx, int l3_off, const __u8 *smac, const __u8 *dmac, struct iphdr *ip4)\n",
    "{\n",
    "    if (ipv4_dec_ttl (ctx, l3_off, ip4)) {\n",
    "        return DROP_INVALID;\n",
    "    }\n",
    "    if (smac && eth_store_saddr (ctx, smac, 0) < 0)\n",
    "        return DROP_WRITE_ERROR;\n",
    "    if (dmac && eth_store_daddr (ctx, dmac, 0) < 0)\n",
    "        return DROP_WRITE_ERROR;\n",
    "    return CTX_ACT_OK;\n",
    "}\n"
  ],
  "humanFuncDescription": [
    {
      "description": "",
      "author": "",
      "authorEmail": "",
      "date": ""
    },
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
static __always_inline int ipv4_l3(struct __ctx_buff *ctx, int l3_off,
				   const __u8 *smac, const __u8 *dmac,
				   struct iphdr *ip4)
{
	if (ipv4_dec_ttl(ctx, l3_off, ip4)) {
		/* FIXME: Send ICMP TTL */
		return DROP_INVALID;
	}

	if (smac && eth_store_saddr(ctx, smac, 0) < 0)
		return DROP_WRITE_ERROR;
	if (dmac && eth_store_daddr(ctx, dmac, 0) < 0)
		return DROP_WRITE_ERROR;

	return CTX_ACT_OK;
}

#ifndef SKIP_POLICY_MAP
#ifdef ENABLE_IPV6
/* Performs IPv6 L2/L3 handling and delivers the packet to the destination pod
 * on the same node, either via the stack or via a redirect call.
 * Depending on the configuration, it may also enforce ingress policies for the
 * destination pod via a tail call.
 */
/* 
 OPENED COMMENT BEGIN 
{
  "capabilities": [],
  "helperCallParams": {
    "redirect": [
      {
        "opVar": "NA",
        "inpVar": [
          "\treturn _epctx",
          " ep->ifindex",
          " from_host"
        ]
      }
    ],
    "tail_call": [
      {
        "opVar": "NA",
        "inpVar": [
          "\t_dynamicctx",
          " &POLICY_CALL_MAP",
          " ep->lxc_id"
        ]
      }
    ]
  },
  "startLine": 65,
  "endLine": 106,
  "File": "/home/sayandes/opened_extraction/examples/cilium/lib/l3.h",
  "funcName": "ipv6_local_delivery",
  "updateMaps": [],
  "readMaps": [],
  "input": [
    "struct  __ctx_buff *ctx",
    " int l3_off",
    " __u32 seclabel",
    " const struct endpoint_info *ep",
    " __u8 direction",
    " bool from_host __maybe_unused"
  ],
  "output": "static__always_inlineint",
  "helper": [
    "tail_call",
    "redirect"
  ],
  "compatibleHookpoints": [
    "sched_cls",
    "sched_act",
    "lwt_xmit",
    "xdp"
  ],
  "source": [
    "static __always_inline int ipv6_local_delivery (struct  __ctx_buff *ctx, int l3_off, __u32 seclabel, const struct endpoint_info *ep, __u8 direction, bool from_host __maybe_unused)\n",
    "{\n",
    "    mac_t router_mac = ep->node_mac;\n",
    "    mac_t lxc_mac = ep->mac;\n",
    "    int ret;\n",
    "    cilium_dbg (ctx, DBG_LOCAL_DELIVERY, ep->lxc_id, seclabel);\n",
    "    ret = ipv6_l3 (ctx, l3_off, (__u8 *) & router_mac, (__u8 *) & lxc_mac, direction);\n",
    "    if (ret != CTX_ACT_OK)\n",
    "        return ret;\n",
    "\n",
    "#ifdef LOCAL_DELIVERY_METRICS\n",
    "    update_metrics (ctx_full_len (ctx), direction, REASON_FORWARDED);\n",
    "\n",
    "#endif\n",
    "\n",
    "#if defined(USE_BPF_PROG_FOR_INGRESS_POLICY) && \\\n",
    "\t!defined(FORCE_LOCAL_POLICY_EVAL_AT_SOURCE)\n",
    "    ctx->mark |= MARK_MAGIC_IDENTITY;\n",
    "    set_identity_mark (ctx, seclabel);\n",
    "    return redirect_ep (ctx, ep->ifindex, from_host);\n",
    "\n",
    "#else\n",
    "    ctx_store_meta (ctx, CB_SRC_LABEL, seclabel);\n",
    "    ctx_store_meta (ctx, CB_IFINDEX, ep->ifindex);\n",
    "    ctx_store_meta (ctx, CB_FROM_HOST, from_host ? 1 : 0);\n",
    "    tail_call_dynamic (ctx, &POLICY_CALL_MAP, ep->lxc_id);\n",
    "    return DROP_MISSED_TAIL_CALL;\n",
    "\n",
    "#endif\n",
    "}\n"
  ],
  "humanFuncDescription": [
    {
      "description": "",
      "author": "",
      "authorEmail": "",
      "date": ""
    },
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
static __always_inline int ipv6_local_delivery(struct __ctx_buff *ctx, int l3_off,
					       __u32 seclabel,
					       const struct endpoint_info *ep,
					       __u8 direction,
					       bool from_host __maybe_unused)
{
	mac_t router_mac = ep->node_mac;
	mac_t lxc_mac = ep->mac;
	int ret;

	cilium_dbg(ctx, DBG_LOCAL_DELIVERY, ep->lxc_id, seclabel);

	/* This will invalidate the size check */
	ret = ipv6_l3(ctx, l3_off, (__u8 *) &router_mac, (__u8 *) &lxc_mac, direction);
	if (ret != CTX_ACT_OK)
		return ret;

#ifdef LOCAL_DELIVERY_METRICS
	/*
	 * Special LXC case for updating egress forwarding metrics.
	 * Note that the packet could still be dropped but it would show up
	 * as an ingress drop counter in metrics.
	 */
	update_metrics(ctx_full_len(ctx), direction, REASON_FORWARDED);
#endif

#if defined(USE_BPF_PROG_FOR_INGRESS_POLICY) && \
	!defined(FORCE_LOCAL_POLICY_EVAL_AT_SOURCE)
	ctx->mark |= MARK_MAGIC_IDENTITY;
	set_identity_mark(ctx, seclabel);

	return redirect_ep(ctx, ep->ifindex, from_host);
#else
	/* Jumps to destination pod's BPF program to enforce ingress policies. */
	ctx_store_meta(ctx, CB_SRC_LABEL, seclabel);
	ctx_store_meta(ctx, CB_IFINDEX, ep->ifindex);
	ctx_store_meta(ctx, CB_FROM_HOST, from_host ? 1 : 0);

	tail_call_dynamic(ctx, &POLICY_CALL_MAP, ep->lxc_id);
	return DROP_MISSED_TAIL_CALL;
#endif
}
#endif /* ENABLE_IPV6 */

/* Performs IPv4 L2/L3 handling and delivers the packet to the destination pod
 * on the same node, either via the stack or via a redirect call.
 * Depending on the configuration, it may also enforce ingress policies for the
 * destination pod via a tail call.
 */
/* 
 OPENED COMMENT BEGIN 
{
  "capabilities": [],
  "helperCallParams": {
    "redirect": [
      {
        "opVar": "NA",
        "inpVar": [
          "\treturn _epctx",
          " ep->ifindex",
          " from_host"
        ]
      }
    ],
    "tail_call": [
      {
        "opVar": "NA",
        "inpVar": [
          "\t_dynamicctx",
          " &POLICY_CALL_MAP",
          " ep->lxc_id"
        ]
      }
    ]
  },
  "startLine": 114,
  "endLine": 154,
  "File": "/home/sayandes/opened_extraction/examples/cilium/lib/l3.h",
  "funcName": "ipv4_local_delivery",
  "updateMaps": [],
  "readMaps": [],
  "input": [
    "struct  __ctx_buff *ctx",
    " int l3_off",
    " __u32 seclabel",
    " struct iphdr *ip4",
    " const struct endpoint_info *ep",
    " __u8 direction __maybe_unused",
    " bool from_host __maybe_unused"
  ],
  "output": "static__always_inlineint",
  "helper": [
    "tail_call",
    "redirect"
  ],
  "compatibleHookpoints": [
    "sched_cls",
    "sched_act",
    "lwt_xmit",
    "xdp"
  ],
  "source": [
    "static __always_inline int ipv4_local_delivery (struct  __ctx_buff *ctx, int l3_off, __u32 seclabel, struct iphdr *ip4, const struct endpoint_info *ep, __u8 direction __maybe_unused, bool from_host __maybe_unused)\n",
    "{\n",
    "    mac_t router_mac = ep->node_mac;\n",
    "    mac_t lxc_mac = ep->mac;\n",
    "    int ret;\n",
    "    cilium_dbg (ctx, DBG_LOCAL_DELIVERY, ep->lxc_id, seclabel);\n",
    "    ret = ipv4_l3 (ctx, l3_off, (__u8 *) & router_mac, (__u8 *) & lxc_mac, ip4);\n",
    "    if (ret != CTX_ACT_OK)\n",
    "        return ret;\n",
    "\n",
    "#ifdef LOCAL_DELIVERY_METRICS\n",
    "    update_metrics (ctx_full_len (ctx), direction, REASON_FORWARDED);\n",
    "\n",
    "#endif\n",
    "\n",
    "#if defined(USE_BPF_PROG_FOR_INGRESS_POLICY) && \\\n",
    "\t!defined(FORCE_LOCAL_POLICY_EVAL_AT_SOURCE)\n",
    "    ctx->mark |= MARK_MAGIC_IDENTITY;\n",
    "    set_identity_mark (ctx, seclabel);\n",
    "    return redirect_ep (ctx, ep->ifindex, from_host);\n",
    "\n",
    "#else\n",
    "    ctx_store_meta (ctx, CB_SRC_LABEL, seclabel);\n",
    "    ctx_store_meta (ctx, CB_IFINDEX, ep->ifindex);\n",
    "    ctx_store_meta (ctx, CB_FROM_HOST, from_host ? 1 : 0);\n",
    "    tail_call_dynamic (ctx, &POLICY_CALL_MAP, ep->lxc_id);\n",
    "    return DROP_MISSED_TAIL_CALL;\n",
    "\n",
    "#endif\n",
    "}\n"
  ],
  "humanFuncDescription": [
    {
      "description": "",
      "author": "",
      "authorEmail": "",
      "date": ""
    },
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
static __always_inline int ipv4_local_delivery(struct __ctx_buff *ctx, int l3_off,
					       __u32 seclabel, struct iphdr *ip4,
					       const struct endpoint_info *ep,
					       __u8 direction __maybe_unused,
					       bool from_host __maybe_unused)
{
	mac_t router_mac = ep->node_mac;
	mac_t lxc_mac = ep->mac;
	int ret;

	cilium_dbg(ctx, DBG_LOCAL_DELIVERY, ep->lxc_id, seclabel);

	ret = ipv4_l3(ctx, l3_off, (__u8 *) &router_mac, (__u8 *) &lxc_mac, ip4);
	if (ret != CTX_ACT_OK)
		return ret;

#ifdef LOCAL_DELIVERY_METRICS
	/*
	 * Special LXC case for updating egress forwarding metrics.
	 * Note that the packet could still be dropped but it would show up
	 * as an ingress drop counter in metrics.
	 */
	update_metrics(ctx_full_len(ctx), direction, REASON_FORWARDED);
#endif

#if defined(USE_BPF_PROG_FOR_INGRESS_POLICY) && \
	!defined(FORCE_LOCAL_POLICY_EVAL_AT_SOURCE)
	ctx->mark |= MARK_MAGIC_IDENTITY;
	set_identity_mark(ctx, seclabel);

	return redirect_ep(ctx, ep->ifindex, from_host);
#else
	/* Jumps to destination pod's BPF program to enforce ingress policies. */
	ctx_store_meta(ctx, CB_SRC_LABEL, seclabel);
	ctx_store_meta(ctx, CB_IFINDEX, ep->ifindex);
	ctx_store_meta(ctx, CB_FROM_HOST, from_host ? 1 : 0);

	tail_call_dynamic(ctx, &POLICY_CALL_MAP, ep->lxc_id);
	return DROP_MISSED_TAIL_CALL;
#endif
}
#endif /* SKIP_POLICY_MAP */

/* 
 OPENED COMMENT BEGIN 
{
  "capabilities": [
    {
      "capability": "map_read",
      "map_read": [
        {
          "Project": "cilium",
          "Return Type": "void*",
          "Description": "Perform a lookup in <[ map ]>(IP: 0) for an entry associated to key. ",
          "Return": " Map value associated to key, or NULL if no entry was found.",
          "Function Name": "map_lookup_elem",
          "Input Params": [
            "{Type: struct map ,Var: *map}",
            "{Type:  const void ,Var: *key}"
          ]
        }
      ]
    }
  ],
  "helperCallParams": {
    "map_lookup_elem": [
      {
        "opVar": "\tcfg ",
        "inpVar": [
          " &ENCRYPT_MAP",
          " &encrypt_key"
        ]
      }
    ]
  },
  "startLine": 157,
  "endLine": 185,
  "File": "/home/sayandes/opened_extraction/examples/cilium/lib/l3.h",
  "funcName": "get_min_encrypt_key",
  "updateMaps": [],
  "readMaps": [
    "  ENCRYPT_MAP"
  ],
  "input": [
    "__u8 peer_key __maybe_unused"
  ],
  "output": "static__always_inline__u8",
  "helper": [
    "map_lookup_elem"
  ],
  "compatibleHookpoints": [
    "cgroup_sysctl",
    "socket_filter",
    "flow_dissector",
    "lwt_out",
    "cgroup_device",
    "raw_tracepoint",
    "cgroup_sock_addr",
    "lwt_in",
    "lwt_xmit",
    "sk_skb",
    "sock_ops",
    "sk_reuseport",
    "xdp",
    "raw_tracepoint_writable",
    "cgroup_skb",
    "lwt_seg6local",
    "tracepoint",
    "perf_event",
    "sk_msg",
    "cgroup_sock",
    "kprobe",
    "sched_cls",
    "sched_act"
  ],
  "source": [
    "static __always_inline __u8 get_min_encrypt_key (__u8 peer_key __maybe_unused)\n",
    "{\n",
    "\n",
    "#ifdef ENABLE_IPSEC\n",
    "    __u8 local_key = 0;\n",
    "    __u32 encrypt_key = 0;\n",
    "    struct encrypt_config *cfg;\n",
    "    cfg = map_lookup_elem (& ENCRYPT_MAP, & encrypt_key);\n",
    "    if (cfg)\n",
    "        local_key = cfg->encrypt_key;\n",
    "    if (peer_key == MAX_KEY_INDEX)\n",
    "        return local_key == 1 ? peer_key : local_key;\n",
    "    if (local_key == MAX_KEY_INDEX)\n",
    "        return peer_key == 1 ? local_key : peer_key;\n",
    "    return local_key < peer_key ? local_key : peer_key;\n",
    "\n",
    "#else\n",
    "    return 0;\n",
    "\n",
    "#endif /* ENABLE_IPSEC */\n",
    "}\n"
  ],
  "humanFuncDescription": [
    {
      "description": "",
      "author": "",
      "authorEmail": "",
      "date": ""
    },
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
static __always_inline __u8 get_min_encrypt_key(__u8 peer_key __maybe_unused)
{
#ifdef ENABLE_IPSEC
	__u8 local_key = 0;
	__u32 encrypt_key = 0;
	struct encrypt_config *cfg;

	cfg = map_lookup_elem(&ENCRYPT_MAP, &encrypt_key);
	/* Having no key info for a context is the same as no encryption */
	if (cfg)
		local_key = cfg->encrypt_key;

	/* If both ends can encrypt/decrypt use smaller of the two this
	 * way both ends will have keys installed assuming key IDs are
	 * always increasing. However, we have to handle roll-over case
	 * and to do this safely we assume keys are no more than one ahead.
	 * We expect user/control-place to accomplish this. Notice zero
	 * will always be returned if either local or peer have the zero
	 * key indicating no encryption.
	 */
	if (peer_key == MAX_KEY_INDEX)
		return local_key == 1 ? peer_key : local_key;
	if (local_key == MAX_KEY_INDEX)
		return peer_key == 1 ? local_key : peer_key;
	return local_key < peer_key ? local_key : peer_key;
#else
	return 0;
#endif /* ENABLE_IPSEC */
}

#endif
