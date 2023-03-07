#define EXIT_REASON 18
BPF_HASH(start, u8, u8);
/* 
 OPENED COMMENT BEGIN 
{
  "capabilities": [],
  "helperCallParams": {
    "bpf_trace_printk": [
      {
        "opVar": "NA",
        "inpVar": [
          "\t\t\"KVM_EXIT exit_reason : %d\\\\n\"",
          " args->exit_reason"
        ]
      }
    ]
  },
  "startLine": 3,
  "endLine": 11,
  "File": "/home/sayandes/opened_extraction/examples/bcc/kvm_hypercall.c",
  "funcName": "TRACEPOINT_PROBE",
  "updateMaps": [],
  "readMaps": [],
  "input": [
    "kvm",
    " kvm_exit"
  ],
  "output": "NA",
  "helper": [
    "bpf_trace_printk"
  ],
  "compatibleHookpoints": [
    "sk_msg",
    "lwt_xmit",
    "kprobe",
    "cgroup_device",
    "cgroup_skb",
    "cgroup_sock_addr",
    "sk_skb",
    "raw_tracepoint",
    "xdp",
    "lwt_in",
    "sock_ops",
    "socket_filter",
    "raw_tracepoint_writable",
    "flow_dissector",
    "perf_event",
    "cgroup_sysctl",
    "sched_cls",
    "lwt_out",
    "lwt_seg6local",
    "sk_reuseport",
    "cgroup_sock",
    "sched_act",
    "tracepoint"
  ],
  "source": [
    "TRACEPOINT_PROBE (kvm, kvm_exit)\n",
    "{\n",
    "    u8 e = EXIT_REASON;\n",
    "    u8 one = 1;\n",
    "    if (args->exit_reason == EXIT_REASON) {\n",
    "        bpf_trace_printk (\"KVM_EXIT exit_reason : %d\\\\n\", args->exit_reason);\n",
    "        start.update (&e, &one);\n",
    "    }\n",
    "    return 0;\n",
    "}\n"
  ],
  "humanFuncDescription": [
    {
      "description": "",
      "author": "",
      "authorEmail": "",
      "date": ""
    },
    {}
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
TRACEPOINT_PROBE(kvm, kvm_exit) {
	u8 e = EXIT_REASON;
	u8 one = 1;
	if (args->exit_reason == EXIT_REASON) {
		bpf_trace_printk("KVM_EXIT exit_reason : %d\\n", args->exit_reason);
		start.update(&e, &one);
	}
	return 0;
}
/* 
 OPENED COMMENT BEGIN 
{
  "capabilities": [],
  "helperCallParams": {
    "bpf_trace_printk": [
      {
        "opVar": "NA",
        "inpVar": [
          "\t\t\"KVM_ENTRY vcpu_id : %u\\\\n\"",
          " args->vcpu_id"
        ]
      }
    ]
  },
  "startLine": 12,
  "endLine": 21,
  "File": "/home/sayandes/opened_extraction/examples/bcc/kvm_hypercall.c",
  "funcName": "TRACEPOINT_PROBE",
  "updateMaps": [],
  "readMaps": [],
  "input": [
    "kvm",
    " kvm_entry"
  ],
  "output": "NA",
  "helper": [
    "bpf_trace_printk"
  ],
  "compatibleHookpoints": [
    "sk_msg",
    "lwt_xmit",
    "kprobe",
    "cgroup_device",
    "cgroup_skb",
    "cgroup_sock_addr",
    "sk_skb",
    "raw_tracepoint",
    "xdp",
    "lwt_in",
    "sock_ops",
    "socket_filter",
    "raw_tracepoint_writable",
    "flow_dissector",
    "perf_event",
    "cgroup_sysctl",
    "sched_cls",
    "lwt_out",
    "lwt_seg6local",
    "sk_reuseport",
    "cgroup_sock",
    "sched_act",
    "tracepoint"
  ],
  "source": [
    "TRACEPOINT_PROBE (kvm, kvm_entry)\n",
    "{\n",
    "    u8 e = EXIT_REASON;\n",
    "    u8 zero = 0;\n",
    "    u8 *s = start.lookup (&e);\n",
    "    if (s != NULL && *s == 1) {\n",
    "        bpf_trace_printk (\"KVM_ENTRY vcpu_id : %u\\\\n\", args->vcpu_id);\n",
    "        start.update (&e, &zero);\n",
    "    }\n",
    "    return 0;\n",
    "}\n"
  ],
  "humanFuncDescription": [
    {
      "description": "",
      "author": "",
      "authorEmail": "",
      "date": ""
    },
    {}
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
TRACEPOINT_PROBE(kvm, kvm_entry) {
	u8 e = EXIT_REASON;
	u8 zero = 0;
	u8 *s = start.lookup(&e);
	if (s != NULL && *s == 1) {
		bpf_trace_printk("KVM_ENTRY vcpu_id : %u\\n", args->vcpu_id);
		start.update(&e, &zero);
	}
	return 0;
}
/* 
 OPENED COMMENT BEGIN 
{
  "capabilities": [],
  "helperCallParams": {
    "bpf_trace_printk": [
      {
        "opVar": "NA",
        "inpVar": [
          "\t\t\"HYPERCALL nr : %d\\\\n\"",
          " args->nr"
        ]
      }
    ]
  },
  "startLine": 22,
  "endLine": 30,
  "File": "/home/sayandes/opened_extraction/examples/bcc/kvm_hypercall.c",
  "funcName": "TRACEPOINT_PROBE",
  "updateMaps": [],
  "readMaps": [],
  "input": [
    "kvm",
    " kvm_hypercall"
  ],
  "output": "NA",
  "helper": [
    "bpf_trace_printk"
  ],
  "compatibleHookpoints": [
    "sk_msg",
    "lwt_xmit",
    "kprobe",
    "cgroup_device",
    "cgroup_skb",
    "cgroup_sock_addr",
    "sk_skb",
    "raw_tracepoint",
    "xdp",
    "lwt_in",
    "sock_ops",
    "socket_filter",
    "raw_tracepoint_writable",
    "flow_dissector",
    "perf_event",
    "cgroup_sysctl",
    "sched_cls",
    "lwt_out",
    "lwt_seg6local",
    "sk_reuseport",
    "cgroup_sock",
    "sched_act",
    "tracepoint"
  ],
  "source": [
    "TRACEPOINT_PROBE (kvm, kvm_hypercall)\n",
    "{\n",
    "    u8 e = EXIT_REASON;\n",
    "    u8 zero = 0;\n",
    "    u8 *s = start.lookup (&e);\n",
    "    if (s != NULL && *s == 1) {\n",
    "        bpf_trace_printk (\"HYPERCALL nr : %d\\\\n\", args->nr);\n",
    "    }\n",
    "    return 0;\n",
    "}\n"
  ],
  "humanFuncDescription": [
    {
      "description": "",
      "author": "",
      "authorEmail": "",
      "date": ""
    },
    {}
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
TRACEPOINT_PROBE(kvm, kvm_hypercall) {
	u8 e = EXIT_REASON;
	u8 zero = 0;
	u8 *s = start.lookup(&e);
	if (s != NULL && *s == 1) {
		bpf_trace_printk("HYPERCALL nr : %d\\n", args->nr);
	}
	return 0;
};
