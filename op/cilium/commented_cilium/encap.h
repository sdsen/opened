/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause) */
/* Copyright Authors of Cilium */

#ifndef __LIB_ENCAP_H_
#define __LIB_ENCAP_H_

#include "common.h"
#include "dbg.h"
#include "trace.h"
#include "l3.h"

#ifdef ENCAP_IFINDEX
/* NOT_VTEP_DST is passed to an encapsulation function when the
 * destination of the tunnel is not a VTEP.
 */
#define NOT_VTEP_DST 0
#ifdef ENABLE_IPSEC
/* 
 OPENED COMMENT BEGIN 
{
  "capabilities": [],
  "helperCallParams": {
    "redirect": [
      {
        "opVar": "NA",
        "inpVar": [
          "encap_and__nomark_ipsecstruct __ctx_buff *ctx",
          " __u32 tunnel_endpoint",
          "\t\t\t\t__u8 key",
          " __u32 seclabel"
        ]
      }
    ]
  },
  "startLine": 18,
  "endLine": 40,
  "File": "/home/palani/github/opened_extraction/examples/cilium/lib/encap.h",
  "funcName": "encap_and_redirect_nomark_ipsec",
  "updateMaps": [],
  "readMaps": [],
  "input": [
    "struct  __ctx_buff *ctx",
    " __u32 tunnel_endpoint",
    " __u8 key",
    " __u32 seclabel"
  ],
  "output": "static__always_inlineint",
  "helper": [
    "redirect"
  ],
  "compatibleHookpoints": [
    "lwt_xmit",
    "sched_cls",
    "sched_act",
    "xdp"
  ],
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
static __always_inline int
encap_and_redirect_nomark_ipsec(struct __ctx_buff *ctx, __u32 tunnel_endpoint,
				__u8 key, __u32 seclabel)
{
	/* Traffic from local host in tunnel mode will be passed to
	 * cilium_host. In non-IPSec case traffic with non-local dst
	 * will then be redirected to tunnel device. In IPSec case
	 * though we need to traverse xfrm path still. The mark +
	 * cb[4] hints will not survive a veth pair xmit to ingress
	 * however so below encap_and_redirect_ipsec will not work.
	 * Instead pass hints via cb[0], cb[4] (cb is not cleared
	 * by dev_ctx_forward) and catch hints with bpf_host
	 * prog that will populate mark/cb as expected by xfrm and 2nd
	 * traversal into bpf_host. Remember we can't use cb[0-3]
	 * in both cases because xfrm layer would overwrite them. We
	 * use cb[4] here so it doesn't need to be reset by
	 * bpf_host.
	 */
	ctx_store_meta(ctx, CB_ENCRYPT_MAGIC, or_encrypt_key(key));
	ctx_store_meta(ctx, CB_ENCRYPT_IDENTITY, seclabel);
	ctx_store_meta(ctx, CB_ENCRYPT_DST, tunnel_endpoint);
	return IPSEC_ENDPOINT;
}

/* 
 OPENED COMMENT BEGIN 
{
  "capabilities": [],
  "helperCallParams": {
    "redirect": [
      {
        "opVar": "NA",
        "inpVar": [
          "encap_and__ipsecstruct __ctx_buff *ctx",
          " __u32 tunnel_endpoint",
          "\t\t\t __u8 key",
          " __u32 seclabel"
        ]
      }
    ]
  },
  "startLine": 42,
  "endLine": 57,
  "File": "/home/palani/github/opened_extraction/examples/cilium/lib/encap.h",
  "funcName": "encap_and_redirect_ipsec",
  "updateMaps": [],
  "readMaps": [],
  "input": [
    "struct  __ctx_buff *ctx",
    " __u32 tunnel_endpoint",
    " __u8 key",
    " __u32 seclabel"
  ],
  "output": "static__always_inlineint",
  "helper": [
    "redirect"
  ],
  "compatibleHookpoints": [
    "lwt_xmit",
    "sched_cls",
    "sched_act",
    "xdp"
  ],
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
static __always_inline int
encap_and_redirect_ipsec(struct __ctx_buff *ctx, __u32 tunnel_endpoint,
			 __u8 key, __u32 seclabel)
{
	/* IPSec is performed by the stack on any packets with the
	 * MARK_MAGIC_ENCRYPT bit set. During the process though we
	 * lose the lxc context (seclabel and tunnel endpoint). The
	 * tunnel endpoint can be looked up from daddr but the sec
	 * label is stashed in the mark and extracted in bpf_host
	 * to send ctx onto tunnel for encap.
	 */
	set_encrypt_key_mark(ctx, key);
	set_identity_mark(ctx, seclabel);
	ctx_store_meta(ctx, CB_ENCRYPT_DST, tunnel_endpoint);
	return IPSEC_ENDPOINT;
}
#endif /* ENABLE_IPSEC */

/* 
 OPENED COMMENT BEGIN 
{
  "capabilities": [
    {
      "capability": "read_skb",
      "read_skb": [
        {
          "Project": "cilium",
          "Return Type": "s64",
          "Description": "Compute a checksum difference , <[ from ]>(IP: 0) the raw buffer pointed by <[ from ]>(IP: 0) , of length <[ from_size ]>(IP: 1) (that must be a multiple of 4) , towards the raw buffer pointed by <[ to ]>(IP: 2) , of size <[ to_size ]>(IP: 3) (same remark). An optional <[ seed ]>(IP: 4) can be added <[ to ]>(IP: 2) the value (this can be cascaded , the <[ seed ]>(IP: 4) may come <[ from ]>(IP: 0) a previous call <[ to ]>(IP: 2) the helper). This is flexible enough <[ to ]>(IP: 2) be used in several ways: \u00b7 With <[ from_size ]>(IP: 1) == 0 , <[ to_size ]>(IP: 3) > 0 and <[ seed ]>(IP: 4) set <[ to ]>(IP: 2) checksum , it can be used when pushing new data. \u00b7 With <[ from_size ]>(IP: 1) > 0 , <[ to_size ]>(IP: 3) == 0 and <[ seed ]>(IP: 4) set <[ to ]>(IP: 2) checksum , it can be used when removing data <[ from ]>(IP: 0) a packet. \u00b7 With <[ from_size ]>(IP: 1) > 0 , <[ to_size ]>(IP: 3) > 0 and <[ seed ]>(IP: 4) set <[ to ]>(IP: 2) 0 , it can be used <[ to ]>(IP: 2) compute a diff. Note that <[ from_size ]>(IP: 1) and <[ to_size ]>(IP: 3) do not need <[ to ]>(IP: 2) be equal. This helper can be used in combination with l3_csum_replace() and l4_csum_replace() , <[ to ]>(IP: 2) which one can feed in the difference computed with csum_diff(). ",
          "Return": " The checksum result, or a negative error code in case of failure.",
          "Function Name": "csum_diff",
          "Input Params": [
            "{Type: __be32 ,Var: *from}",
            "{Type:  u32 ,Var: from_size}",
            "{Type:  __be32 ,Var: *to}",
            "{Type:  u32 ,Var: to_size}",
            "{Type:  __wsum ,Var: seed}"
          ]
        }
      ]
    }
  ],
  "helperCallParams": {
    "csum_diff": [
      {
        "opVar": "\t\tsum ",
        "inpVar": [
          " which",
          " 16",
          " &host_ip",
          " 16",
          " 0"
        ]
      }
    ]
  },
  "startLine": 60,
  "endLine": 117,
  "File": "/home/palani/github/opened_extraction/examples/cilium/lib/encap.h",
  "funcName": "encap_remap_v6_host_address",
  "updateMaps": [],
  "readMaps": [],
  "input": [
    "struct  __ctx_buff * ctx __maybe_unused",
    " const bool egress __maybe_unused"
  ],
  "output": "static__always_inlineint",
  "helper": [
    "csum_diff"
  ],
  "compatibleHookpoints": [
    "lwt_xmit",
    "lwt_seg6local",
    "sched_cls",
    "sched_act",
    "lwt_out",
    "lwt_in",
    "xdp"
  ],
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
static __always_inline int
encap_remap_v6_host_address(struct __ctx_buff *ctx __maybe_unused,
			    const bool egress __maybe_unused)
{
#ifdef ENABLE_ENCAP_HOST_REMAP
	struct csum_offset csum = {};
	union v6addr host_ip;
	void *data, *data_end;
	struct ipv6hdr *ip6;
	union v6addr *which;
	__u8 nexthdr;
	__u16 proto;
	__be32 sum;
	__u32 noff;
	__u64 off;
	int ret;

	validate_ethertype(ctx, &proto);
	if (proto != bpf_htons(ETH_P_IPV6))
		return 0;
	if (!revalidate_data(ctx, &data, &data_end, &ip6))
		return DROP_INVALID;
	/* For requests routed via tunnel with external v6 node IP
	 * we need to remap their source address to the router address
	 * as otherwise replies are not routed via tunnel but public
	 * address instead.
	 */
	if (egress) {
		BPF_V6(host_ip, HOST_IP);
		which = (union v6addr *)&ip6->saddr;
	} else {
		BPF_V6(host_ip, ROUTER_IP);
		which = (union v6addr *)&ip6->daddr;
	}
	if (ipv6_addrcmp(which, &host_ip))
		return 0;
	nexthdr = ip6->nexthdr;
	ret = ipv6_hdrlen(ctx, &nexthdr);
	if (ret < 0)
		return ret;
	off = ((void *)ip6 - data) + ret;
	if (egress) {
		BPF_V6(host_ip, ROUTER_IP);
		noff = ETH_HLEN + offsetof(struct ipv6hdr, saddr);
	} else {
		BPF_V6(host_ip, HOST_IP);
		noff = ETH_HLEN + offsetof(struct ipv6hdr, daddr);
	}
	sum = csum_diff(which, 16, &host_ip, 16, 0);
	csum_l4_offset_and_flags(nexthdr, &csum);
	if (ctx_store_bytes(ctx, noff, &host_ip, 16, 0) < 0)
		return DROP_WRITE_ERROR;
	if (csum.offset &&
	    csum_l4_replace(ctx, off, &csum, 0, sum, BPF_F_PSEUDO_HDR) < 0)
		return DROP_CSUM_L4;
#endif /* ENABLE_ENCAP_HOST_REMAP */
	return 0;
}

/* 
 OPENED COMMENT BEGIN 
{
  "capabilities": [],
  "helperCallParams": {},
  "startLine": 119,
  "endLine": 154,
  "File": "/home/palani/github/opened_extraction/examples/cilium/lib/encap.h",
  "funcName": "__encap_with_nodeid",
  "updateMaps": [],
  "readMaps": [],
  "input": [
    "struct  __ctx_buff *ctx",
    " __u32 tunnel_endpoint",
    " __u32 seclabel",
    " __u32 vni __maybe_unused",
    " enum trace_reason ct_reason",
    " __u32 monitor"
  ],
  "output": "static__always_inlineint",
  "helper": [],
  "compatibleHookpoints": [
    "sk_msg",
    "lwt_xmit",
    "cgroup_sock_addr",
    "sock_ops",
    "sk_skb",
    "lwt_seg6local",
    "perf_event",
    "cgroup_sysctl",
    "cgroup_sock",
    "xdp",
    "socket_filter",
    "cgroup_skb",
    "kprobe",
    "raw_tracepoint_writable",
    "sk_reuseport",
    "sched_cls",
    "sched_act",
    "raw_tracepoint",
    "flow_dissector",
    "lwt_out",
    "lwt_in",
    "cgroup_device",
    "tracepoint"
  ],
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
static __always_inline int
__encap_with_nodeid(struct __ctx_buff *ctx, __u32 tunnel_endpoint,
		    __u32 seclabel, __u32 vni __maybe_unused,
		    enum trace_reason ct_reason, __u32 monitor)
{
	struct bpf_tunnel_key key = {};
	__u32 node_id;
	int ret;

	/* When encapsulating, a packet originating from the local host is
	 * being considered as a packet from a remote node as it is being
	 * received.
	 */
	if (seclabel == HOST_ID)
		seclabel = LOCAL_NODE_ID;

	node_id = bpf_htonl(tunnel_endpoint);
#ifdef ENABLE_VTEP
	if (vni != NOT_VTEP_DST)
		key.tunnel_id = vni;
	else
#endif /* ENABLE_VTEP */
		key.tunnel_id = seclabel;
	key.remote_ipv4 = node_id;
	key.tunnel_ttl = 64;

	cilium_dbg(ctx, DBG_ENCAP, node_id, seclabel);

	ret = ctx_set_tunnel_key(ctx, &key, sizeof(key), BPF_F_ZERO_CSUM_TX);
	if (unlikely(ret < 0))
		return DROP_WRITE_ERROR;

	send_trace_notify(ctx, TRACE_TO_OVERLAY, seclabel, 0, 0, ENCAP_IFINDEX,
			  ct_reason, monitor);
	return 0;
}

/* 
 OPENED COMMENT BEGIN 
{
  "capabilities": [],
  "helperCallParams": {
    "redirect": [
      {
        "opVar": "NA",
        "inpVar": [
          "__encap_and__with_nodeidstruct __ctx_buff *ctx",
          " __u32 tunnel_endpoint",
          "\t\t\t\t __u32 seclabel",
          " __u32 vni",
          "\t\t\t\t const struct trace_ctx *trace"
        ]
      },
      {
        "opVar": "NA",
        "inpVar": [
          "\treturn ctx_ctx",
          " ENCAP_IFINDEX",
          " 0"
        ]
      }
    ]
  },
  "startLine": 156,
  "endLine": 167,
  "File": "/home/palani/github/opened_extraction/examples/cilium/lib/encap.h",
  "funcName": "__encap_and_redirect_with_nodeid",
  "updateMaps": [],
  "readMaps": [],
  "input": [
    "struct  __ctx_buff *ctx",
    " __u32 tunnel_endpoint",
    " __u32 seclabel",
    " __u32 vni",
    " const struct trace_ctx *trace"
  ],
  "output": "static__always_inlineint",
  "helper": [
    "redirect"
  ],
  "compatibleHookpoints": [
    "lwt_xmit",
    "sched_cls",
    "sched_act",
    "xdp"
  ],
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
static __always_inline int
__encap_and_redirect_with_nodeid(struct __ctx_buff *ctx, __u32 tunnel_endpoint,
				 __u32 seclabel, __u32 vni,
				 const struct trace_ctx *trace)
{
	int ret = __encap_with_nodeid(ctx, tunnel_endpoint, seclabel,
				      vni, trace->reason, trace->monitor);
	if (ret != 0)
		return ret;

	return ctx_redirect(ctx, ENCAP_IFINDEX, 0);
}

/* encap_and_redirect_with_nodeid returns IPSEC_ENDPOINT after ctx meta-data is
 * set when IPSec is enabled. Caller should pass the ctx to the stack at this
 * point. Otherwise returns CTX_ACT_TX on successful redirect to tunnel device.
 * On error returns CTX_ACT_DROP, DROP_NO_TUNNEL_ENDPOINT or DROP_WRITE_ERROR.
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
          "encap_and__with_nodeidstruct __ctx_buff *ctx",
          " __u32 tunnel_endpoint",
          "\t\t\t       __u8 key __maybe_unused",
          " __u32 seclabel",
          "\t\t\t       const struct trace_ctx *trace"
        ]
      },
      {
        "opVar": "NA",
        "inpVar": [
          "#ifdef ENABLE_IPSEC\tif key\t\treturn encap_and__nomark_ipsecctx",
          " tunnel_endpoint",
          " key",
          " seclabel"
        ]
      },
      {
        "opVar": "NA",
        "inpVar": [
          "#endif\treturn __encap_and__with_nodeidctx",
          " tunnel_endpoint",
          " seclabel",
          " NOT_VTEP_DST",
          "\t\t\t\t\t\ttrace"
        ]
      }
    ]
  },
  "startLine": 174,
  "endLine": 185,
  "File": "/home/palani/github/opened_extraction/examples/cilium/lib/encap.h",
  "funcName": "encap_and_redirect_with_nodeid",
  "updateMaps": [],
  "readMaps": [],
  "input": [
    "struct  __ctx_buff *ctx",
    " __u32 tunnel_endpoint",
    " __u8 key __maybe_unused",
    " __u32 seclabel",
    " const struct trace_ctx *trace"
  ],
  "output": "static__always_inlineint",
  "helper": [
    "redirect"
  ],
  "compatibleHookpoints": [
    "lwt_xmit",
    "sched_cls",
    "sched_act",
    "xdp"
  ],
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
static __always_inline int
encap_and_redirect_with_nodeid(struct __ctx_buff *ctx, __u32 tunnel_endpoint,
			       __u8 key __maybe_unused, __u32 seclabel,
			       const struct trace_ctx *trace)
{
#ifdef ENABLE_IPSEC
	if (key)
		return encap_and_redirect_nomark_ipsec(ctx, tunnel_endpoint, key, seclabel);
#endif
	return __encap_and_redirect_with_nodeid(ctx, tunnel_endpoint, seclabel, NOT_VTEP_DST,
						trace);
}

/* encap_and_redirect based on ENABLE_IPSEC flag and from_host bool will decide
 * which version of code to call. With IPSec enabled and from_host set use the
 * IPSec branch which configures metadata for IPSec kernel stack. Otherwise
 * packet is redirected to output tunnel device and ctx will not be seen by
 * IP stack.
 *
 * Returns IPSEC_ENDPOINT when ctx needs to be handed to IP stack for IPSec
 * handling, CTX_ACT_DROP, DROP_NO_TUNNEL_ENDPOINT or DROP_WRITE_ERROR on error,
 * and finally on successful redirect returns CTX_ACT_TX.
 */
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
    "redirect": [
      {
        "opVar": "NA",
        "inpVar": [
          "encap_and__lxcstruct __ctx_buff *ctx",
          " __u32 tunnel_endpoint",
          "\t\t       __u8 encrypt_key __maybe_unused",
          "\t\t       struct endpoint_key *key",
          " __u32 seclabel",
          "\t\t       const struct trace_ctx *trace"
        ]
      },
      {
        "opVar": "NA",
        "inpVar": [
          "#ifdef ENABLE_IPSEC\t\tif encrypt_key\t\t\treturn encap_and__ipsecctx",
          " tunnel_endpoint",
          "\t\t\t\t\t\t\tencrypt_key",
          " seclabel"
        ]
      },
      {
        "opVar": "NA",
        "inpVar": [
          "#else\t\treturn __encap_and__with_nodeidctx",
          " tunnel_endpoint",
          "\t\t\t\t\t\t\tseclabel",
          " NOT_VTEP_DST",
          " trace"
        ]
      },
      {
        "opVar": "NA",
        "inpVar": [
          "\t\treturn encap_and__ipsecctx",
          " tunnel->ip4",
          "\t\t\t\t\t\tmin_encrypt_key",
          "\t\t\t\t\t\tseclabel"
        ]
      },
      {
        "opVar": "NA",
        "inpVar": [
          "\t#endif\treturn __encap_and__with_nodeidctx",
          " tunnel->ip4",
          " seclabel",
          "\t\t\t\t\t\tNOT_VTEP_DST",
          " trace"
        ]
      }
    ],
    "map_lookup_elem": [
      {
        "opVar": "#endif \t\ttunnel ",
        "inpVar": [
          " &TUNNEL_MAP",
          " key"
        ]
      }
    ]
  },
  "startLine": 197,
  "endLine": 242,
  "File": "/home/palani/github/opened_extraction/examples/cilium/lib/encap.h",
  "funcName": "encap_and_redirect_lxc",
  "updateMaps": [],
  "readMaps": [
    "  TUNNEL_MAP"
  ],
  "input": [
    "struct  __ctx_buff *ctx",
    " __u32 tunnel_endpoint",
    " __u8 encrypt_key __maybe_unused",
    " struct endpoint_key *key",
    " __u32 seclabel",
    " const struct trace_ctx *trace"
  ],
  "output": "static__always_inlineint",
  "helper": [
    "map_lookup_elem",
    "redirect"
  ],
  "compatibleHookpoints": [
    "sched_cls",
    "lwt_xmit",
    "sched_act",
    "xdp"
  ],
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
static __always_inline int
encap_and_redirect_lxc(struct __ctx_buff *ctx, __u32 tunnel_endpoint,
		       __u8 encrypt_key __maybe_unused,
		       struct endpoint_key *key, __u32 seclabel,
		       const struct trace_ctx *trace)
{
	struct endpoint_key *tunnel;

	if (tunnel_endpoint) {
#ifdef ENABLE_IPSEC
		if (encrypt_key)
			return encap_and_redirect_ipsec(ctx, tunnel_endpoint,
							encrypt_key, seclabel);
#endif
#if !defined(ENABLE_NODEPORT) && (defined(ENABLE_IPSEC) || defined(ENABLE_HOST_FIREWALL))
		/* For IPSec and the host firewall, traffic from a pod to a remote node
		 * is sent through the tunnel. In the case of node --> VIP@remote pod,
		 * packets may be DNATed when they enter the remote node. If kube-proxy
		 * is used, the response needs to go through the stack on the way to
		 * the tunnel, to apply the correct reverse DNAT.
		 * See #14674 for details.
		 */
		return __encap_with_nodeid(ctx, tunnel_endpoint, seclabel, NOT_VTEP_DST,
					   trace->reason, trace->monitor);
#else
		return __encap_and_redirect_with_nodeid(ctx, tunnel_endpoint,
							seclabel, NOT_VTEP_DST, trace);
#endif /* !ENABLE_NODEPORT && (ENABLE_IPSEC || ENABLE_HOST_FIREWALL) */
	}

	tunnel = map_lookup_elem(&TUNNEL_MAP, key);
	if (!tunnel)
		return DROP_NO_TUNNEL_ENDPOINT;

#ifdef ENABLE_IPSEC
	if (tunnel->key) {
		__u8 min_encrypt_key = get_min_encrypt_key(tunnel->key);

		return encap_and_redirect_ipsec(ctx, tunnel->ip4,
						min_encrypt_key,
						seclabel);
	}
#endif
	return __encap_and_redirect_with_nodeid(ctx, tunnel->ip4, seclabel,
						NOT_VTEP_DST, trace);
}

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
    "redirect": [
      {
        "opVar": "NA",
        "inpVar": [
          "encap_and__netdevstruct __ctx_buff *ctx",
          " struct endpoint_key *k",
          "\t\t\t  __u32 seclabel",
          " const struct trace_ctx *trace"
        ]
      },
      {
        "opVar": "NA",
        "inpVar": [
          "\t\treturn encap_and__nomark_ipsecctx",
          " tunnel->ip4",
          "\t\t\t\t\t\t       key",
          " seclabel"
        ]
      },
      {
        "opVar": "NA",
        "inpVar": [
          "\t#endif\treturn __encap_and__with_nodeidctx",
          " tunnel->ip4",
          " seclabel",
          "\t\t\t\t\t\tNOT_VTEP_DST",
          " trace"
        ]
      }
    ],
    "map_lookup_elem": [
      {
        "opVar": "\ttunnel ",
        "inpVar": [
          " &TUNNEL_MAP",
          " k"
        ]
      }
    ]
  },
  "startLine": 244,
  "endLine": 264,
  "File": "/home/palani/github/opened_extraction/examples/cilium/lib/encap.h",
  "funcName": "encap_and_redirect_netdev",
  "updateMaps": [],
  "readMaps": [
    "  TUNNEL_MAP"
  ],
  "input": [
    "struct  __ctx_buff *ctx",
    " struct endpoint_key *k",
    " __u32 seclabel",
    " const struct trace_ctx *trace"
  ],
  "output": "static__always_inlineint",
  "helper": [
    "map_lookup_elem",
    "redirect"
  ],
  "compatibleHookpoints": [
    "sched_cls",
    "lwt_xmit",
    "sched_act",
    "xdp"
  ],
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
static __always_inline int
encap_and_redirect_netdev(struct __ctx_buff *ctx, struct endpoint_key *k,
			  __u32 seclabel, const struct trace_ctx *trace)
{
	struct endpoint_key *tunnel;

	tunnel = map_lookup_elem(&TUNNEL_MAP, k);
	if (!tunnel)
		return DROP_NO_TUNNEL_ENDPOINT;

#ifdef ENABLE_IPSEC
	if (tunnel->key) {
		__u8 key = get_min_encrypt_key(tunnel->key);

		return encap_and_redirect_nomark_ipsec(ctx, tunnel->ip4,
						       key, seclabel);
	}
#endif
	return __encap_and_redirect_with_nodeid(ctx, tunnel->ip4, seclabel,
						NOT_VTEP_DST, trace);
}
#endif /* ENCAP_IFINDEX */
#endif /* __LIB_ENCAP_H_ */
