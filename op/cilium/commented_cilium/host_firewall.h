/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause) */
/* Copyright Authors of Cilium */

#ifndef __LIB_HOST_FIREWALL_H_
#define __LIB_HOST_FIREWALL_H_

/* Only compile in if host firewall is enabled and file is included from
 * bpf_host.
 */
#if defined(ENABLE_HOST_FIREWALL) && defined(IS_BPF_HOST)

# include "policy.h"
# include "policy_log.h"
# include "trace.h"

# ifdef ENABLE_IPV6
/* 
 OPENED COMMENT BEGIN 
{
  "capabilities": [
    {
      "capability": "pkt_go_to_next_module",
      "pkt_go_to_next_module": [
        {
          "Project": "cilium",
          "Return Type": "int",
          "Input Params": [],
          "Function Name": "TC_ACT_OK",
          "Return": 0,
          "Description": "will terminate the packet processing pipeline and allows the packet to proceed. Pass the skb onwards either to upper layers of the stack on ingress or down to the networking device driver for transmission on egress, respectively. TC_ACT_OK sets skb->tc_index based on the classid the tc BPF program set. The latter is set out of the tc BPF program itself through skb->tc_classid from the BPF context.",
          "compatible_hookpoints": [
            "xdp",
            "sched_cls",
            "sched_act"
          ],
          "capabilities": [
            "pkt_go_to_next_module"
          ]
        }
      ]
    }
  ],
  "helperCallParams": {},
  "startLine": 17,
  "endLine": 100,
  "File": "/home/sayandes/opened_extraction/examples/cilium/lib/host_firewall.h",
  "funcName": "ipv6_host_policy_egress",
  "developer_inline_comments": [
    {
      "start_line": 1,
      "end_line": 1,
      "text": "/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause) */"
    },
    {
      "start_line": 2,
      "end_line": 2,
      "text": "/* Copyright Authors of Cilium */"
    },
    {
      "start_line": 7,
      "end_line": 9,
      "text": "/* Only compile in if host firewall is enabled and file is included from\n * bpf_host.\n */"
    },
    {
      "start_line": 32,
      "end_line": 32,
      "text": "/* Only enforce host policies for packets from host IPs. */"
    },
    {
      "start_line": 39,
      "end_line": 39,
      "text": "/* Lookup connection in conntrack map. */"
    },
    {
      "start_line": 55,
      "end_line": 55,
      "text": "/* Retrieve destination identity. */"
    },
    {
      "start_line": 62,
      "end_line": 62,
      "text": "/* Perform policy lookup. */"
    },
    {
      "start_line": 66,
      "end_line": 66,
      "text": "/* Reply traffic and related are allowed regardless of policy verdict. */"
    }
  ],
  "updateMaps": [],
  "readMaps": [],
  "input": [
    "struct  __ctx_buff *ctx",
    " __u32 src_id",
    " struct trace_ctx *trace"
  ],
  "output": "static__always_inlineint",
  "helper": [
    "CTX_ACT_OK"
  ],
  "compatibleHookpoints": [
    "sched_cls",
    "sched_act",
    "xdp"
  ],
  "source": [
    "static __always_inline int ipv6_host_policy_egress (struct  __ctx_buff *ctx, __u32 src_id, struct trace_ctx *trace)\n",
    "{\n",
    "    int ret, verdict, l3_off = ETH_HLEN, l4_off, hdrlen;\n",
    "    struct ct_state ct_state_new = {}, ct_state = {};\n",
    "    __u8 policy_match_type = POLICY_MATCH_NONE;\n",
    "    __u8 audited = 0;\n",
    "    struct remote_endpoint_info *info;\n",
    "    struct ipv6_ct_tuple tuple = {}\n",
    "    ;\n",
    "    __u32 dst_id = 0;\n",
    "    union v6addr orig_dip;\n",
    "    void *data, *data_end;\n",
    "    struct ipv6hdr *ip6;\n",
    "    if (src_id != HOST_ID)\n",
    "        return CTX_ACT_OK;\n",
    "    if (!revalidate_data (ctx, &data, &data_end, &ip6))\n",
    "        return DROP_INVALID;\n",
    "    tuple.nexthdr = ip6->nexthdr;\n",
    "    ipv6_addr_copy (&tuple.saddr, (union v6addr *) &ip6->saddr);\n",
    "    ipv6_addr_copy (&tuple.daddr, (union v6addr *) &ip6->daddr);\n",
    "    ipv6_addr_copy (&orig_dip, (union v6addr *) &ip6->daddr);\n",
    "    hdrlen = ipv6_hdrlen (ctx, & tuple.nexthdr);\n",
    "    if (hdrlen < 0)\n",
    "        return hdrlen;\n",
    "    l4_off = l3_off + hdrlen;\n",
    "    ret = ct_lookup6 (get_ct_map6 (& tuple), & tuple, ctx, l4_off, CT_EGRESS, & ct_state, & trace -> monitor);\n",
    "    if (ret < 0)\n",
    "        return ret;\n",
    "    trace->reason = (enum trace_reason) ret;\n",
    "    info = lookup_ip6_remote_endpoint (& orig_dip);\n",
    "    if (info && info->sec_label)\n",
    "        dst_id = info->sec_label;\n",
    "    cilium_dbg (ctx, info ? DBG_IP_ID_MAP_SUCCEED6 : DBG_IP_ID_MAP_FAILED6, orig_dip.p4, dst_id);\n",
    "    verdict = policy_can_egress6 (ctx, & tuple, src_id, dst_id, & policy_match_type, & audited);\n",
    "    if (ret != CT_REPLY && ret != CT_RELATED && verdict < 0) {\n",
    "        send_policy_verdict_notify (ctx, dst_id, tuple.dport, tuple.nexthdr, POLICY_EGRESS, 1, verdict, policy_match_type, audited);\n",
    "        return verdict;\n",
    "    }\n",
    "    switch (ret) {\n",
    "    case CT_NEW :\n",
    "        send_policy_verdict_notify (ctx, dst_id, tuple.dport, tuple.nexthdr, POLICY_EGRESS, 1, verdict, policy_match_type, audited);\n",
    "        ct_state_new.src_sec_id = HOST_ID;\n",
    "        ret = ct_create6 (get_ct_map6 (& tuple), & CT_MAP_ANY6, & tuple, ctx, CT_EGRESS, & ct_state_new, verdict > 0, false);\n",
    "        if (IS_ERR (ret))\n",
    "            return ret;\n",
    "        break;\n",
    "    case CT_REOPENED :\n",
    "        send_policy_verdict_notify (ctx, dst_id, tuple.dport, tuple.nexthdr, POLICY_EGRESS, 1, verdict, policy_match_type, audited);\n",
    "    case CT_ESTABLISHED :\n",
    "    case CT_RELATED :\n",
    "    case CT_REPLY :\n",
    "        break;\n",
    "    default :\n",
    "        return DROP_UNKNOWN_CT;\n",
    "    }\n",
    "    return CTX_ACT_OK;\n",
    "}\n"
  ],
  "called_function_list": [
    "lookup_ip6_remote_endpoint",
    "get_ct_map6",
    "revalidate_data",
    "ipv6_hdrlen",
    "send_policy_verdict_notify",
    "policy_can_egress6",
    "IS_ERR",
    "ipv6_addr_copy",
    "ct_create6",
    "cilium_dbg",
    "ct_lookup6"
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
static __always_inline int
ipv6_host_policy_egress(struct __ctx_buff *ctx, __u32 src_id,
			struct trace_ctx *trace)
{
	int ret, verdict, l3_off = ETH_HLEN, l4_off, hdrlen;
	struct ct_state ct_state_new = {}, ct_state = {};
	__u8 policy_match_type = POLICY_MATCH_NONE;
	__u8 audited = 0;
	struct remote_endpoint_info *info;
	struct ipv6_ct_tuple tuple = {};
	__u32 dst_id = 0;
	union v6addr orig_dip;
	void *data, *data_end;
	struct ipv6hdr *ip6;

	/* Only enforce host policies for packets from host IPs. */
	if (src_id != HOST_ID)
		return CTX_ACT_OK;

	if (!revalidate_data(ctx, &data, &data_end, &ip6))
		return DROP_INVALID;

	/* Lookup connection in conntrack map. */
	tuple.nexthdr = ip6->nexthdr;
	ipv6_addr_copy(&tuple.saddr, (union v6addr *)&ip6->saddr);
	ipv6_addr_copy(&tuple.daddr, (union v6addr *)&ip6->daddr);
	ipv6_addr_copy(&orig_dip, (union v6addr *)&ip6->daddr);
	hdrlen = ipv6_hdrlen(ctx, &tuple.nexthdr);
	if (hdrlen < 0)
		return hdrlen;
	l4_off = l3_off + hdrlen;
	ret = ct_lookup6(get_ct_map6(&tuple), &tuple, ctx, l4_off, CT_EGRESS,
			 &ct_state, &trace->monitor);
	if (ret < 0)
		return ret;

	trace->reason = (enum trace_reason)ret;

	/* Retrieve destination identity. */
	info = lookup_ip6_remote_endpoint(&orig_dip);
	if (info && info->sec_label)
		dst_id = info->sec_label;
	cilium_dbg(ctx, info ? DBG_IP_ID_MAP_SUCCEED6 : DBG_IP_ID_MAP_FAILED6,
		   orig_dip.p4, dst_id);

	/* Perform policy lookup. */
	verdict = policy_can_egress6(ctx, &tuple, src_id, dst_id,
				     &policy_match_type, &audited);

	/* Reply traffic and related are allowed regardless of policy verdict. */
	if (ret != CT_REPLY && ret != CT_RELATED && verdict < 0) {
		send_policy_verdict_notify(ctx, dst_id, tuple.dport,
					   tuple.nexthdr, POLICY_EGRESS, 1,
					   verdict, policy_match_type, audited);
		return verdict;
	}

	switch (ret) {
	case CT_NEW:
		send_policy_verdict_notify(ctx, dst_id, tuple.dport,
					   tuple.nexthdr, POLICY_EGRESS, 1,
					   verdict, policy_match_type, audited);
		ct_state_new.src_sec_id = HOST_ID;
		ret = ct_create6(get_ct_map6(&tuple), &CT_MAP_ANY6, &tuple,
				 ctx, CT_EGRESS, &ct_state_new, verdict > 0, false);
		if (IS_ERR(ret))
			return ret;
		break;

	case CT_REOPENED:
		send_policy_verdict_notify(ctx, dst_id, tuple.dport,
					   tuple.nexthdr, POLICY_EGRESS, 1,
					   verdict, policy_match_type, audited);
	case CT_ESTABLISHED:
	case CT_RELATED:
	case CT_REPLY:
		break;

	default:
		return DROP_UNKNOWN_CT;
	}

	return CTX_ACT_OK;
}

/* 
 OPENED COMMENT BEGIN 
{
  "capabilities": [
    {
      "capability": "pkt_go_to_next_module",
      "pkt_go_to_next_module": [
        {
          "Project": "cilium",
          "Return Type": "int",
          "Input Params": [],
          "Function Name": "TC_ACT_OK",
          "Return": 0,
          "Description": "will terminate the packet processing pipeline and allows the packet to proceed. Pass the skb onwards either to upper layers of the stack on ingress or down to the networking device driver for transmission on egress, respectively. TC_ACT_OK sets skb->tc_index based on the classid the tc BPF program set. The latter is set out of the tc BPF program itself through skb->tc_classid from the BPF context.",
          "compatible_hookpoints": [
            "xdp",
            "sched_cls",
            "sched_act"
          ],
          "capabilities": [
            "pkt_go_to_next_module"
          ]
        }
      ]
    }
  ],
  "helperCallParams": {},
  "startLine": 102,
  "endLine": 199,
  "File": "/home/sayandes/opened_extraction/examples/cilium/lib/host_firewall.h",
  "funcName": "ipv6_host_policy_ingress",
  "developer_inline_comments": [
    {
      "start_line": 120,
      "end_line": 120,
      "text": "/* Retrieve destination identity. */"
    },
    {
      "start_line": 128,
      "end_line": 128,
      "text": "/* Only enforce host policies for packets to host IPs. */"
    },
    {
      "start_line": 132,
      "end_line": 132,
      "text": "/* Lookup connection in conntrack map. */"
    },
    {
      "start_line": 147,
      "end_line": 147,
      "text": "/* Retrieve source identity. */"
    },
    {
      "start_line": 154,
      "end_line": 154,
      "text": "/* Perform policy lookup */"
    },
    {
      "start_line": 159,
      "end_line": 159,
      "text": "/* Reply traffic and related are allowed regardless of policy verdict. */"
    },
    {
      "start_line": 173,
      "end_line": 173,
      "text": "/* Create new entry for connection in conntrack map. */"
    },
    {
      "start_line": 194,
      "end_line": 196,
      "text": "/* This change is necessary for packets redirected from the lxc device to\n\t * the host device.\n\t */"
    }
  ],
  "updateMaps": [],
  "readMaps": [],
  "input": [
    "struct  __ctx_buff *ctx",
    " __u32 *src_id",
    " struct trace_ctx *trace"
  ],
  "output": "static__always_inlineint",
  "helper": [
    "CTX_ACT_OK"
  ],
  "compatibleHookpoints": [
    "sched_cls",
    "sched_act",
    "xdp"
  ],
  "source": [
    "static __always_inline int ipv6_host_policy_ingress (struct  __ctx_buff *ctx, __u32 *src_id, struct trace_ctx *trace)\n",
    "{\n",
    "    struct ct_state ct_state_new = {}, ct_state = {};\n",
    "    __u8 policy_match_type = POLICY_MATCH_NONE;\n",
    "    __u8 audited = 0;\n",
    "    __u32 dst_id = WORLD_ID;\n",
    "    struct remote_endpoint_info *info;\n",
    "    int ret, verdict, l4_off, hdrlen;\n",
    "    struct ipv6_ct_tuple tuple = {}\n",
    "    ;\n",
    "    union v6addr orig_sip;\n",
    "    void *data, *data_end;\n",
    "    struct ipv6hdr *ip6;\n",
    "    if (!revalidate_data (ctx, &data, &data_end, &ip6))\n",
    "        return DROP_INVALID;\n",
    "    ipv6_addr_copy (&tuple.daddr, (union v6addr *) &ip6->daddr);\n",
    "    info = lookup_ip6_remote_endpoint (& tuple.daddr);\n",
    "    if (info && info->sec_label)\n",
    "        dst_id = info->sec_label;\n",
    "    cilium_dbg (ctx, info ? DBG_IP_ID_MAP_SUCCEED6 : DBG_IP_ID_MAP_FAILED6, tuple.daddr.p4, dst_id);\n",
    "    if (dst_id != HOST_ID)\n",
    "        return CTX_ACT_OK;\n",
    "    tuple.nexthdr = ip6->nexthdr;\n",
    "    ipv6_addr_copy (&tuple.saddr, (union v6addr *) &ip6->saddr);\n",
    "    ipv6_addr_copy (&orig_sip, (union v6addr *) &ip6->saddr);\n",
    "    hdrlen = ipv6_hdrlen (ctx, & tuple.nexthdr);\n",
    "    if (hdrlen < 0)\n",
    "        return hdrlen;\n",
    "    l4_off = ETH_HLEN + hdrlen;\n",
    "    ret = ct_lookup6 (get_ct_map6 (& tuple), & tuple, ctx, l4_off, CT_INGRESS, & ct_state, & trace -> monitor);\n",
    "    if (ret < 0)\n",
    "        return ret;\n",
    "    trace->reason = (enum trace_reason) ret;\n",
    "    info = lookup_ip6_remote_endpoint (& orig_sip);\n",
    "    if (info && info->sec_label)\n",
    "        *src_id = info->sec_label;\n",
    "    cilium_dbg (ctx, info ? DBG_IP_ID_MAP_SUCCEED6 : DBG_IP_ID_MAP_FAILED6, orig_sip.p4, *src_id);\n",
    "    verdict = policy_can_access_ingress (ctx, * src_id, dst_id, tuple.dport, tuple.nexthdr, false, & policy_match_type, & audited);\n",
    "    if (ret != CT_REPLY && ret != CT_RELATED && verdict < 0) {\n",
    "        send_policy_verdict_notify (ctx, *src_id, tuple.dport, tuple.nexthdr, POLICY_INGRESS, 1, verdict, policy_match_type, audited);\n",
    "        return verdict;\n",
    "    }\n",
    "    switch (ret) {\n",
    "    case CT_NEW :\n",
    "        send_policy_verdict_notify (ctx, *src_id, tuple.dport, tuple.nexthdr, POLICY_INGRESS, 1, verdict, policy_match_type, audited);\n",
    "        ct_state_new.src_sec_id = *src_id;\n",
    "        ct_state_new.node_port = ct_state.node_port;\n",
    "        ret = ct_create6 (get_ct_map6 (& tuple), & CT_MAP_ANY6, & tuple, ctx, CT_INGRESS, & ct_state_new, verdict > 0, false);\n",
    "        if (IS_ERR (ret))\n",
    "            return ret;\n",
    "    case CT_REOPENED :\n",
    "        send_policy_verdict_notify (ctx, *src_id, tuple.dport, tuple.nexthdr, POLICY_INGRESS, 1, verdict, policy_match_type, audited);\n",
    "    case CT_ESTABLISHED :\n",
    "    case CT_RELATED :\n",
    "    case CT_REPLY :\n",
    "        break;\n",
    "    default :\n",
    "        return DROP_UNKNOWN_CT;\n",
    "    }\n",
    "    ctx_change_type (ctx, PACKET_HOST);\n",
    "    return CTX_ACT_OK;\n",
    "}\n"
  ],
  "called_function_list": [
    "lookup_ip6_remote_endpoint",
    "get_ct_map6",
    "revalidate_data",
    "ipv6_hdrlen",
    "send_policy_verdict_notify",
    "ctx_change_type",
    "policy_can_access_ingress",
    "IS_ERR",
    "ipv6_addr_copy",
    "ct_create6",
    "cilium_dbg",
    "ct_lookup6"
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
static __always_inline int
ipv6_host_policy_ingress(struct __ctx_buff *ctx, __u32 *src_id,
			 struct trace_ctx *trace)
{
	struct ct_state ct_state_new = {}, ct_state = {};
	__u8 policy_match_type = POLICY_MATCH_NONE;
	__u8 audited = 0;
	__u32 dst_id = WORLD_ID;
	struct remote_endpoint_info *info;
	int ret, verdict, l4_off, hdrlen;
	struct ipv6_ct_tuple tuple = {};
	union v6addr orig_sip;
	void *data, *data_end;
	struct ipv6hdr *ip6;

	if (!revalidate_data(ctx, &data, &data_end, &ip6))
		return DROP_INVALID;

	/* Retrieve destination identity. */
	ipv6_addr_copy(&tuple.daddr, (union v6addr *)&ip6->daddr);
	info = lookup_ip6_remote_endpoint(&tuple.daddr);
	if (info && info->sec_label)
		dst_id = info->sec_label;
	cilium_dbg(ctx, info ? DBG_IP_ID_MAP_SUCCEED6 : DBG_IP_ID_MAP_FAILED6,
		   tuple.daddr.p4, dst_id);

	/* Only enforce host policies for packets to host IPs. */
	if (dst_id != HOST_ID)
		return CTX_ACT_OK;

	/* Lookup connection in conntrack map. */
	tuple.nexthdr = ip6->nexthdr;
	ipv6_addr_copy(&tuple.saddr, (union v6addr *)&ip6->saddr);
	ipv6_addr_copy(&orig_sip, (union v6addr *)&ip6->saddr);
	hdrlen = ipv6_hdrlen(ctx, &tuple.nexthdr);
	if (hdrlen < 0)
		return hdrlen;
	l4_off = ETH_HLEN + hdrlen;
	ret = ct_lookup6(get_ct_map6(&tuple), &tuple, ctx, l4_off, CT_INGRESS,
			 &ct_state, &trace->monitor);
	if (ret < 0)
		return ret;

	trace->reason = (enum trace_reason)ret;

	/* Retrieve source identity. */
	info = lookup_ip6_remote_endpoint(&orig_sip);
	if (info && info->sec_label)
		*src_id = info->sec_label;
	cilium_dbg(ctx, info ? DBG_IP_ID_MAP_SUCCEED6 : DBG_IP_ID_MAP_FAILED6,
		   orig_sip.p4, *src_id);

	/* Perform policy lookup */
	verdict = policy_can_access_ingress(ctx, *src_id, dst_id, tuple.dport,
					    tuple.nexthdr, false,
					    &policy_match_type, &audited);

	/* Reply traffic and related are allowed regardless of policy verdict. */
	if (ret != CT_REPLY && ret != CT_RELATED && verdict < 0) {
		send_policy_verdict_notify(ctx, *src_id, tuple.dport,
					   tuple.nexthdr, POLICY_INGRESS, 1,
					   verdict, policy_match_type, audited);
		return verdict;
	}

	switch (ret) {
	case CT_NEW:
		send_policy_verdict_notify(ctx, *src_id, tuple.dport,
					   tuple.nexthdr, POLICY_INGRESS, 1,
					   verdict, policy_match_type, audited);

		/* Create new entry for connection in conntrack map. */
		ct_state_new.src_sec_id = *src_id;
		ct_state_new.node_port = ct_state.node_port;
		ret = ct_create6(get_ct_map6(&tuple), &CT_MAP_ANY6, &tuple,
				 ctx, CT_INGRESS, &ct_state_new, verdict > 0, false);
		if (IS_ERR(ret))
			return ret;

	case CT_REOPENED:
		send_policy_verdict_notify(ctx, *src_id, tuple.dport,
					   tuple.nexthdr, POLICY_INGRESS, 1,
					   verdict, policy_match_type, audited);
	case CT_ESTABLISHED:
	case CT_RELATED:
	case CT_REPLY:
		break;

	default:
		return DROP_UNKNOWN_CT;
	}

	/* This change is necessary for packets redirected from the lxc device to
	 * the host device.
	 */
	ctx_change_type(ctx, PACKET_HOST);
	return CTX_ACT_OK;
}
# endif /* ENABLE_IPV6 */

# ifdef ENABLE_IPV4
#  ifndef ENABLE_MASQUERADE
/* 
 OPENED COMMENT BEGIN 
{
  "capabilities": [
    {
      "capability": "pkt_go_to_next_module",
      "pkt_go_to_next_module": [
        {
          "Project": "cilium",
          "Return Type": "int",
          "Input Params": [],
          "Function Name": "TC_ACT_OK",
          "Return": 0,
          "Description": "will terminate the packet processing pipeline and allows the packet to proceed. Pass the skb onwards either to upper layers of the stack on ingress or down to the networking device driver for transmission on egress, respectively. TC_ACT_OK sets skb->tc_index based on the classid the tc BPF program set. The latter is set out of the tc BPF program itself through skb->tc_classid from the BPF context.",
          "compatible_hookpoints": [
            "xdp",
            "sched_cls",
            "sched_act"
          ],
          "capabilities": [
            "pkt_go_to_next_module"
          ]
        }
      ]
    }
  ],
  "helperCallParams": {},
  "startLine": 204,
  "endLine": 249,
  "File": "/home/sayandes/opened_extraction/examples/cilium/lib/host_firewall.h",
  "funcName": "whitelist_snated_egress_connections",
  "developer_inline_comments": [
    {
      "start_line": 214,
      "end_line": 223,
      "text": "/* If kube-proxy is in use (no BPF-based masquerading), packets from\n\t * pods may be SNATed. The response packet will therefore have a host\n\t * IP as the destination IP.\n\t * To avoid enforcing host policies for response packets to pods, we\n\t * need to create a CT entry for the forward, SNATed packet from the\n\t * pod. Response packets will thus match this CT entry and bypass host\n\t * policies.\n\t * We know the packet is a SNATed packet if the srcid from ipcache is\n\t * HOST_ID, but the actual srcid (derived from the packet mark) isn't.\n\t */"
    }
  ],
  "updateMaps": [],
  "readMaps": [],
  "input": [
    "struct  __ctx_buff *ctx",
    " __u32 ipcache_srcid",
    " struct trace_ctx *trace"
  ],
  "output": "static__always_inlineint",
  "helper": [
    "CTX_ACT_OK"
  ],
  "compatibleHookpoints": [
    "sched_cls",
    "sched_act",
    "xdp"
  ],
  "source": [
    "static __always_inline int whitelist_snated_egress_connections (struct  __ctx_buff *ctx, __u32 ipcache_srcid, struct trace_ctx *trace)\n",
    "{\n",
    "    struct ct_state ct_state_new = {}, ct_state = {};\n",
    "    struct ipv4_ct_tuple tuple = {}\n",
    "    ;\n",
    "    void *data, *data_end;\n",
    "    struct iphdr *ip4;\n",
    "    int ret, l4_off;\n",
    "    if (ipcache_srcid == HOST_ID) {\n",
    "        if (!revalidate_data (ctx, &data, &data_end, &ip4))\n",
    "            return DROP_INVALID;\n",
    "        tuple.nexthdr = ip4->protocol;\n",
    "        tuple.daddr = ip4->daddr;\n",
    "        tuple.saddr = ip4->saddr;\n",
    "        l4_off = ETH_HLEN + ipv4_hdrlen (ip4);\n",
    "        ret = ct_lookup4 (get_ct_map4 (& tuple), & tuple, ctx, l4_off, CT_EGRESS, & ct_state, & trace -> monitor);\n",
    "        if (ret < 0)\n",
    "            return ret;\n",
    "        trace->reason = (enum trace_reason) ret;\n",
    "        if (ret == CT_NEW) {\n",
    "            ret = ct_create4 (get_ct_map4 (& tuple), & CT_MAP_ANY4, & tuple, ctx, CT_EGRESS, & ct_state_new, false, false);\n",
    "            if (IS_ERR (ret))\n",
    "                return ret;\n",
    "        }\n",
    "    }\n",
    "    return CTX_ACT_OK;\n",
    "}\n"
  ],
  "called_function_list": [
    "revalidate_data",
    "ipv4_hdrlen",
    "ct_create4",
    "get_ct_map4",
    "IS_ERR",
    "ct_lookup4"
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
static __always_inline int
whitelist_snated_egress_connections(struct __ctx_buff *ctx, __u32 ipcache_srcid,
				    struct trace_ctx *trace)
{
	struct ct_state ct_state_new = {}, ct_state = {};
	struct ipv4_ct_tuple tuple = {};
	void *data, *data_end;
	struct iphdr *ip4;
	int ret, l4_off;

	/* If kube-proxy is in use (no BPF-based masquerading), packets from
	 * pods may be SNATed. The response packet will therefore have a host
	 * IP as the destination IP.
	 * To avoid enforcing host policies for response packets to pods, we
	 * need to create a CT entry for the forward, SNATed packet from the
	 * pod. Response packets will thus match this CT entry and bypass host
	 * policies.
	 * We know the packet is a SNATed packet if the srcid from ipcache is
	 * HOST_ID, but the actual srcid (derived from the packet mark) isn't.
	 */
	if (ipcache_srcid == HOST_ID) {
		if (!revalidate_data(ctx, &data, &data_end, &ip4))
			return DROP_INVALID;

		tuple.nexthdr = ip4->protocol;
		tuple.daddr = ip4->daddr;
		tuple.saddr = ip4->saddr;
		l4_off = ETH_HLEN + ipv4_hdrlen(ip4);
		ret = ct_lookup4(get_ct_map4(&tuple), &tuple, ctx, l4_off,
				 CT_EGRESS, &ct_state, &trace->monitor);
		if (ret < 0)
			return ret;

		trace->reason = (enum trace_reason)ret;

		if (ret == CT_NEW) {
			ret = ct_create4(get_ct_map4(&tuple), &CT_MAP_ANY4,
					 &tuple, ctx, CT_EGRESS, &ct_state_new,
					 false, false);
			if (IS_ERR(ret))
				return ret;
		}
	}

	return CTX_ACT_OK;
}
#   endif

/* 
 OPENED COMMENT BEGIN 
{
  "capabilities": [
    {
      "capability": "pkt_go_to_next_module",
      "pkt_go_to_next_module": [
        {
          "Project": "cilium",
          "Return Type": "int",
          "Input Params": [],
          "Function Name": "TC_ACT_OK",
          "Return": 0,
          "Description": "will terminate the packet processing pipeline and allows the packet to proceed. Pass the skb onwards either to upper layers of the stack on ingress or down to the networking device driver for transmission on egress, respectively. TC_ACT_OK sets skb->tc_index based on the classid the tc BPF program set. The latter is set out of the tc BPF program itself through skb->tc_classid from the BPF context.",
          "compatible_hookpoints": [
            "xdp",
            "sched_cls",
            "sched_act"
          ],
          "capabilities": [
            "pkt_go_to_next_module"
          ]
        }
      ]
    }
  ],
  "helperCallParams": {},
  "startLine": 252,
  "endLine": 337,
  "File": "/home/sayandes/opened_extraction/examples/cilium/lib/host_firewall.h",
  "funcName": "ipv4_host_policy_egress",
  "developer_inline_comments": [
    {
      "start_line": 272,
      "end_line": 272,
      "text": "/* Only enforce host policies for packets from host IPs. */"
    },
    {
      "start_line": 280,
      "end_line": 280,
      "text": "/* Lookup connection in conntrack map. */"
    },
    {
      "start_line": 292,
      "end_line": 292,
      "text": "/* Retrieve destination identity. */"
    },
    {
      "start_line": 299,
      "end_line": 299,
      "text": "/* Perform policy lookup. */"
    },
    {
      "start_line": 303,
      "end_line": 303,
      "text": "/* Reply traffic and related are allowed regardless of policy verdict. */"
    }
  ],
  "updateMaps": [],
  "readMaps": [],
  "input": [
    "struct  __ctx_buff *ctx",
    " __u32 src_id",
    " __u32 ipcache_srcid __maybe_unused",
    " struct trace_ctx *trace"
  ],
  "output": "static__always_inlineint",
  "helper": [
    "CTX_ACT_OK"
  ],
  "compatibleHookpoints": [
    "sched_cls",
    "sched_act",
    "xdp"
  ],
  "source": [
    "static __always_inline int ipv4_host_policy_egress (struct  __ctx_buff *ctx, __u32 src_id, __u32 ipcache_srcid __maybe_unused, struct trace_ctx *trace)\n",
    "{\n",
    "    struct ct_state ct_state_new = {}, ct_state = {};\n",
    "    int ret, verdict, l4_off, l3_off = ETH_HLEN;\n",
    "    __u8 policy_match_type = POLICY_MATCH_NONE;\n",
    "    __u8 audited = 0;\n",
    "    struct remote_endpoint_info *info;\n",
    "    struct ipv4_ct_tuple tuple = {}\n",
    "    ;\n",
    "    __u32 dst_id = 0;\n",
    "    void *data, *data_end;\n",
    "    struct iphdr *ip4;\n",
    "    if (src_id != HOST_ID) {\n",
    "\n",
    "#  ifndef ENABLE_MASQUERADE\n",
    "        return whitelist_snated_egress_connections (ctx, ipcache_srcid, trace);\n",
    "\n",
    "#  else\n",
    "        return CTX_ACT_OK;\n",
    "\n",
    "#  endif\n",
    "    }\n",
    "    if (!revalidate_data (ctx, &data, &data_end, &ip4))\n",
    "        return DROP_INVALID;\n",
    "    tuple.nexthdr = ip4->protocol;\n",
    "    tuple.daddr = ip4->daddr;\n",
    "    tuple.saddr = ip4->saddr;\n",
    "    l4_off = l3_off + ipv4_hdrlen (ip4);\n",
    "    ret = ct_lookup4 (get_ct_map4 (& tuple), & tuple, ctx, l4_off, CT_EGRESS, & ct_state, & trace -> monitor);\n",
    "    if (ret < 0)\n",
    "        return ret;\n",
    "    trace->reason = (enum trace_reason) ret;\n",
    "    info = lookup_ip4_remote_endpoint (ip4 -> daddr);\n",
    "    if (info && info->sec_label)\n",
    "        dst_id = info->sec_label;\n",
    "    cilium_dbg (ctx, info ? DBG_IP_ID_MAP_SUCCEED4 : DBG_IP_ID_MAP_FAILED4, ip4->daddr, dst_id);\n",
    "    verdict = policy_can_egress4 (ctx, & tuple, src_id, dst_id, & policy_match_type, & audited);\n",
    "    if (ret != CT_REPLY && ret != CT_RELATED && verdict < 0) {\n",
    "        send_policy_verdict_notify (ctx, dst_id, tuple.dport, tuple.nexthdr, POLICY_EGRESS, 0, verdict, policy_match_type, audited);\n",
    "        return verdict;\n",
    "    }\n",
    "    switch (ret) {\n",
    "    case CT_NEW :\n",
    "        send_policy_verdict_notify (ctx, dst_id, tuple.dport, tuple.nexthdr, POLICY_EGRESS, 0, verdict, policy_match_type, audited);\n",
    "        ct_state_new.src_sec_id = HOST_ID;\n",
    "        ret = ct_create4 (get_ct_map4 (& tuple), & CT_MAP_ANY4, & tuple, ctx, CT_EGRESS, & ct_state_new, verdict > 0, false);\n",
    "        if (IS_ERR (ret))\n",
    "            return ret;\n",
    "        break;\n",
    "    case CT_REOPENED :\n",
    "        send_policy_verdict_notify (ctx, dst_id, tuple.dport, tuple.nexthdr, POLICY_EGRESS, 0, verdict, policy_match_type, audited);\n",
    "    case CT_ESTABLISHED :\n",
    "    case CT_RELATED :\n",
    "    case CT_REPLY :\n",
    "        break;\n",
    "    default :\n",
    "        return DROP_UNKNOWN_CT;\n",
    "    }\n",
    "    return CTX_ACT_OK;\n",
    "}\n"
  ],
  "called_function_list": [
    "revalidate_data",
    "send_policy_verdict_notify",
    "ipv4_hdrlen",
    "ct_create4",
    "get_ct_map4",
    "policy_can_egress4",
    "IS_ERR",
    "lookup_ip4_remote_endpoint",
    "ct_lookup4",
    "cilium_dbg",
    "whitelist_snated_egress_connections"
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
static __always_inline int
ipv4_host_policy_egress(struct __ctx_buff *ctx, __u32 src_id,
			__u32 ipcache_srcid __maybe_unused,
			struct trace_ctx *trace)
{
	struct ct_state ct_state_new = {}, ct_state = {};
	int ret, verdict, l4_off, l3_off = ETH_HLEN;
	__u8 policy_match_type = POLICY_MATCH_NONE;
	__u8 audited = 0;
	struct remote_endpoint_info *info;
	struct ipv4_ct_tuple tuple = {};
	__u32 dst_id = 0;
	void *data, *data_end;
	struct iphdr *ip4;

	if (src_id != HOST_ID) {
#  ifndef ENABLE_MASQUERADE
		return whitelist_snated_egress_connections(ctx, ipcache_srcid,
							   trace);
#  else
		/* Only enforce host policies for packets from host IPs. */
		return CTX_ACT_OK;
#  endif
	}

	if (!revalidate_data(ctx, &data, &data_end, &ip4))
		return DROP_INVALID;

	/* Lookup connection in conntrack map. */
	tuple.nexthdr = ip4->protocol;
	tuple.daddr = ip4->daddr;
	tuple.saddr = ip4->saddr;
	l4_off = l3_off + ipv4_hdrlen(ip4);
	ret = ct_lookup4(get_ct_map4(&tuple), &tuple, ctx, l4_off, CT_EGRESS,
			 &ct_state, &trace->monitor);
	if (ret < 0)
		return ret;

	trace->reason = (enum trace_reason)ret;

	/* Retrieve destination identity. */
	info = lookup_ip4_remote_endpoint(ip4->daddr);
	if (info && info->sec_label)
		dst_id = info->sec_label;
	cilium_dbg(ctx, info ? DBG_IP_ID_MAP_SUCCEED4 : DBG_IP_ID_MAP_FAILED4,
		   ip4->daddr, dst_id);

	/* Perform policy lookup. */
	verdict = policy_can_egress4(ctx, &tuple, src_id, dst_id,
				     &policy_match_type, &audited);

	/* Reply traffic and related are allowed regardless of policy verdict. */
	if (ret != CT_REPLY && ret != CT_RELATED && verdict < 0) {
		send_policy_verdict_notify(ctx, dst_id, tuple.dport,
					   tuple.nexthdr, POLICY_EGRESS, 0,
					   verdict, policy_match_type, audited);
		return verdict;
	}

	switch (ret) {
	case CT_NEW:
		send_policy_verdict_notify(ctx, dst_id, tuple.dport,
					   tuple.nexthdr, POLICY_EGRESS, 0,
					   verdict, policy_match_type, audited);
		ct_state_new.src_sec_id = HOST_ID;
		ret = ct_create4(get_ct_map4(&tuple), &CT_MAP_ANY4, &tuple,
				 ctx, CT_EGRESS, &ct_state_new, verdict > 0, false);
		if (IS_ERR(ret))
			return ret;
		break;

	case CT_REOPENED:
		send_policy_verdict_notify(ctx, dst_id, tuple.dport,
					   tuple.nexthdr, POLICY_EGRESS, 0,
					   verdict, policy_match_type, audited);
	case CT_ESTABLISHED:
	case CT_RELATED:
	case CT_REPLY:
		break;

	default:
		return DROP_UNKNOWN_CT;
	}

	return CTX_ACT_OK;
}

/* 
 OPENED COMMENT BEGIN 
{
  "capabilities": [
    {
      "capability": "pkt_go_to_next_module",
      "pkt_go_to_next_module": [
        {
          "Project": "cilium",
          "Return Type": "int",
          "Input Params": [],
          "Function Name": "TC_ACT_OK",
          "Return": 0,
          "Description": "will terminate the packet processing pipeline and allows the packet to proceed. Pass the skb onwards either to upper layers of the stack on ingress or down to the networking device driver for transmission on egress, respectively. TC_ACT_OK sets skb->tc_index based on the classid the tc BPF program set. The latter is set out of the tc BPF program itself through skb->tc_classid from the BPF context.",
          "compatible_hookpoints": [
            "xdp",
            "sched_cls",
            "sched_act"
          ],
          "capabilities": [
            "pkt_go_to_next_module"
          ]
        }
      ]
    }
  ],
  "helperCallParams": {},
  "startLine": 339,
  "endLine": 439,
  "File": "/home/sayandes/opened_extraction/examples/cilium/lib/host_firewall.h",
  "funcName": "ipv4_host_policy_ingress",
  "developer_inline_comments": [
    {
      "start_line": 357,
      "end_line": 357,
      "text": "/* Retrieve destination identity. */"
    },
    {
      "start_line": 364,
      "end_line": 364,
      "text": "/* Only enforce host policies for packets to host IPs. */"
    },
    {
      "start_line": 368,
      "end_line": 368,
      "text": "/* Lookup connection in conntrack map. */"
    },
    {
      "start_line": 374,
      "end_line": 376,
      "text": "/* Indicate that this is a datagram fragment for which we cannot\n\t * retrieve L4 ports. Do not set flag if we support fragmentation.\n\t */"
    },
    {
      "start_line": 386,
      "end_line": 386,
      "text": "/* Retrieve source identity. */"
    },
    {
      "start_line": 393,
      "end_line": 393,
      "text": "/* Perform policy lookup */"
    },
    {
      "start_line": 399,
      "end_line": 399,
      "text": "/* Reply traffic and related are allowed regardless of policy verdict. */"
    },
    {
      "start_line": 413,
      "end_line": 413,
      "text": "/* Create new entry for connection in conntrack map. */"
    },
    {
      "start_line": 434,
      "end_line": 436,
      "text": "/* This change is necessary for packets redirected from the lxc device to\n\t * the host device.\n\t */"
    }
  ],
  "updateMaps": [],
  "readMaps": [],
  "input": [
    "struct  __ctx_buff *ctx",
    " __u32 *src_id",
    " struct trace_ctx *trace"
  ],
  "output": "static__always_inlineint",
  "helper": [
    "CTX_ACT_OK"
  ],
  "compatibleHookpoints": [
    "sched_cls",
    "sched_act",
    "xdp"
  ],
  "source": [
    "static __always_inline int ipv4_host_policy_ingress (struct  __ctx_buff *ctx, __u32 *src_id, struct trace_ctx *trace)\n",
    "{\n",
    "    struct ct_state ct_state_new = {}, ct_state = {};\n",
    "    int ret, verdict, l4_off, l3_off = ETH_HLEN;\n",
    "    __u8 policy_match_type = POLICY_MATCH_NONE;\n",
    "    __u8 audited = 0;\n",
    "    __u32 dst_id = WORLD_ID;\n",
    "    struct remote_endpoint_info *info;\n",
    "    struct ipv4_ct_tuple tuple = {}\n",
    "    ;\n",
    "    bool is_untracked_fragment = false;\n",
    "    void *data, *data_end;\n",
    "    struct iphdr *ip4;\n",
    "    if (!revalidate_data (ctx, &data, &data_end, &ip4))\n",
    "        return DROP_INVALID;\n",
    "    info = lookup_ip4_remote_endpoint (ip4 -> daddr);\n",
    "    if (info && info->sec_label)\n",
    "        dst_id = info->sec_label;\n",
    "    cilium_dbg (ctx, info ? DBG_IP_ID_MAP_SUCCEED4 : DBG_IP_ID_MAP_FAILED4, ip4->daddr, dst_id);\n",
    "    if (dst_id != HOST_ID)\n",
    "        return CTX_ACT_OK;\n",
    "    tuple.nexthdr = ip4->protocol;\n",
    "    tuple.daddr = ip4->daddr;\n",
    "    tuple.saddr = ip4->saddr;\n",
    "    l4_off = l3_off + ipv4_hdrlen (ip4);\n",
    "\n",
    "#  ifndef ENABLE_IPV4_FRAGMENTS\n",
    "    is_untracked_fragment = ipv4_is_fragment (ip4);\n",
    "\n",
    "#  endif\n",
    "    ret = ct_lookup4 (get_ct_map4 (& tuple), & tuple, ctx, l4_off, CT_INGRESS, & ct_state, & trace -> monitor);\n",
    "    if (ret < 0)\n",
    "        return ret;\n",
    "    trace->reason = (enum trace_reason) ret;\n",
    "    info = lookup_ip4_remote_endpoint (ip4 -> saddr);\n",
    "    if (info && info->sec_label)\n",
    "        *src_id = info->sec_label;\n",
    "    cilium_dbg (ctx, info ? DBG_IP_ID_MAP_SUCCEED4 : DBG_IP_ID_MAP_FAILED4, ip4->saddr, *src_id);\n",
    "    verdict = policy_can_access_ingress (ctx, * src_id, dst_id, tuple.dport, tuple.nexthdr, is_untracked_fragment, & policy_match_type, & audited);\n",
    "    if (ret != CT_REPLY && ret != CT_RELATED && verdict < 0) {\n",
    "        send_policy_verdict_notify (ctx, *src_id, tuple.dport, tuple.nexthdr, POLICY_INGRESS, 0, verdict, policy_match_type, audited);\n",
    "        return verdict;\n",
    "    }\n",
    "    switch (ret) {\n",
    "    case CT_NEW :\n",
    "        send_policy_verdict_notify (ctx, *src_id, tuple.dport, tuple.nexthdr, POLICY_INGRESS, 0, verdict, policy_match_type, audited);\n",
    "        ct_state_new.src_sec_id = *src_id;\n",
    "        ct_state_new.node_port = ct_state.node_port;\n",
    "        ret = ct_create4 (get_ct_map4 (& tuple), & CT_MAP_ANY4, & tuple, ctx, CT_INGRESS, & ct_state_new, verdict > 0, false);\n",
    "        if (IS_ERR (ret))\n",
    "            return ret;\n",
    "    case CT_REOPENED :\n",
    "        send_policy_verdict_notify (ctx, *src_id, tuple.dport, tuple.nexthdr, POLICY_INGRESS, 0, verdict, policy_match_type, audited);\n",
    "    case CT_ESTABLISHED :\n",
    "    case CT_RELATED :\n",
    "    case CT_REPLY :\n",
    "        break;\n",
    "    default :\n",
    "        return DROP_UNKNOWN_CT;\n",
    "    }\n",
    "    ctx_change_type (ctx, PACKET_HOST);\n",
    "    return CTX_ACT_OK;\n",
    "}\n"
  ],
  "called_function_list": [
    "revalidate_data",
    "send_policy_verdict_notify",
    "ipv4_hdrlen",
    "ct_create4",
    "ctx_change_type",
    "get_ct_map4",
    "policy_can_access_ingress",
    "IS_ERR",
    "lookup_ip4_remote_endpoint",
    "ipv4_is_fragment",
    "ct_lookup4",
    "cilium_dbg"
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
static __always_inline int
ipv4_host_policy_ingress(struct __ctx_buff *ctx, __u32 *src_id,
			 struct trace_ctx *trace)
{
	struct ct_state ct_state_new = {}, ct_state = {};
	int ret, verdict, l4_off, l3_off = ETH_HLEN;
	__u8 policy_match_type = POLICY_MATCH_NONE;
	__u8 audited = 0;
	__u32 dst_id = WORLD_ID;
	struct remote_endpoint_info *info;
	struct ipv4_ct_tuple tuple = {};
	bool is_untracked_fragment = false;
	void *data, *data_end;
	struct iphdr *ip4;

	if (!revalidate_data(ctx, &data, &data_end, &ip4))
		return DROP_INVALID;

	/* Retrieve destination identity. */
	info = lookup_ip4_remote_endpoint(ip4->daddr);
	if (info && info->sec_label)
		dst_id = info->sec_label;
	cilium_dbg(ctx, info ? DBG_IP_ID_MAP_SUCCEED4 : DBG_IP_ID_MAP_FAILED4,
		   ip4->daddr, dst_id);

	/* Only enforce host policies for packets to host IPs. */
	if (dst_id != HOST_ID)
		return CTX_ACT_OK;

	/* Lookup connection in conntrack map. */
	tuple.nexthdr = ip4->protocol;
	tuple.daddr = ip4->daddr;
	tuple.saddr = ip4->saddr;
	l4_off = l3_off + ipv4_hdrlen(ip4);
#  ifndef ENABLE_IPV4_FRAGMENTS
	/* Indicate that this is a datagram fragment for which we cannot
	 * retrieve L4 ports. Do not set flag if we support fragmentation.
	 */
	is_untracked_fragment = ipv4_is_fragment(ip4);
#  endif
	ret = ct_lookup4(get_ct_map4(&tuple), &tuple, ctx, l4_off, CT_INGRESS,
			 &ct_state, &trace->monitor);
	if (ret < 0)
		return ret;

	trace->reason = (enum trace_reason)ret;

	/* Retrieve source identity. */
	info = lookup_ip4_remote_endpoint(ip4->saddr);
	if (info && info->sec_label)
		*src_id = info->sec_label;
	cilium_dbg(ctx, info ? DBG_IP_ID_MAP_SUCCEED4 : DBG_IP_ID_MAP_FAILED4,
		   ip4->saddr, *src_id);

	/* Perform policy lookup */
	verdict = policy_can_access_ingress(ctx, *src_id, dst_id, tuple.dport,
					    tuple.nexthdr,
					    is_untracked_fragment,
					    &policy_match_type, &audited);

	/* Reply traffic and related are allowed regardless of policy verdict. */
	if (ret != CT_REPLY && ret != CT_RELATED && verdict < 0) {
		send_policy_verdict_notify(ctx, *src_id, tuple.dport,
					   tuple.nexthdr, POLICY_INGRESS, 0,
					   verdict, policy_match_type, audited);
		return verdict;
	}

	switch (ret) {
	case CT_NEW:
		send_policy_verdict_notify(ctx, *src_id, tuple.dport,
					   tuple.nexthdr, POLICY_INGRESS, 0,
					   verdict, policy_match_type, audited);

		/* Create new entry for connection in conntrack map. */
		ct_state_new.src_sec_id = *src_id;
		ct_state_new.node_port = ct_state.node_port;
		ret = ct_create4(get_ct_map4(&tuple), &CT_MAP_ANY4, &tuple,
				 ctx, CT_INGRESS, &ct_state_new, verdict > 0, false);
		if (IS_ERR(ret))
			return ret;

	case CT_REOPENED:
		send_policy_verdict_notify(ctx, *src_id, tuple.dport,
					   tuple.nexthdr, POLICY_INGRESS, 0,
					   verdict, policy_match_type, audited);
	case CT_ESTABLISHED:
	case CT_RELATED:
	case CT_REPLY:
		break;

	default:
		return DROP_UNKNOWN_CT;
	}

	/* This change is necessary for packets redirected from the lxc device to
	 * the host device.
	 */
	ctx_change_type(ctx, PACKET_HOST);
	return CTX_ACT_OK;
}
# endif /* ENABLE_IPV4 */
#endif /* ENABLE_HOST_FIREWALL && IS_BPF_HOST */
#endif /* __LIB_HOST_FIREWALL_H_ */
