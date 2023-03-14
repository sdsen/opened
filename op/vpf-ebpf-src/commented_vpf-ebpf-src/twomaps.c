// Copyright (c) Prevail Verifier contributors.
// SPDX-License-Identifier: MIT
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;

struct ebpf_map {
    uint32_t type;
    uint32_t key_size;
    uint32_t value_size;
    uint32_t max_entries;
    uint32_t map_flags;
    uint32_t inner_map_idx;
    uint32_t numa_node;
};
#define BPF_MAP_TYPE_ARRAY 2

__attribute__((section("maps"), used))
struct ebpf_map map1 =
    {.type = BPF_MAP_TYPE_ARRAY,
     .key_size = sizeof(int),
     .value_size = sizeof(uint64_t),
     .max_entries = 1};

__attribute__((section("maps"), used))
struct ebpf_map map2 =
    {.type = BPF_MAP_TYPE_ARRAY,
     .key_size = sizeof(int),
     .value_size = sizeof(uint64_t),
     .max_entries = 2};

static void* (*bpf_map_lookup_elem)(struct ebpf_map* map, const void* key) = (void*) 1;
static int (*get_prandom_u32)() = (void*)7;

struct ctx;

/* 
 OPENED COMMENT BEGIN 
{
  "capabilities": [
    {
      "capability": "read_sys_info",
      "read_sys_info": [
        {
          "Project": "cilium",
          "Return Type": "u32",
          "Description": "Get a pseudo-random number. From a security point of view , this helper uses its own pseudo-random internal state , and cannot be used to infer the seed of other random functions in the kernel. However , it is essential to note that the generator used by the helper is not cryptographically secure. ",
          "Return": " A random 32-bit unsigned value.",
          "Function Name": "get_prandom_u32",
          "Input Params": [
            "{Type: voi ,Var: void}"
          ],
          "compatible_hookpoints": [
            "socket_filter",
            "kprobe",
            "sched_cls",
            "sched_act",
            "tracepoint",
            "xdp",
            "perf_event",
            "cgroup_skb",
            "cgroup_sock",
            "lwt_in",
            "lwt_out",
            "lwt_xmit",
            "sock_ops",
            "sk_skb",
            "sk_msg",
            "raw_tracepoint",
            "cgroup_sock_addr",
            "lwt_seg6local",
            "sk_reuseport",
            "flow_dissector",
            "raw_tracepoint_writable"
          ],
          "capabilities": [
            "read_sys_info"
          ]
        }
      ]
    }
  ],
  "helperCallParams": {},
  "startLine": 36,
  "endLine": 48,
  "File": "/home/sayandes/opened_extraction/examples/vpf-ebpf-src/twomaps.c",
  "funcName": "func",
  "updateMaps": [],
  "readMaps": [
    " map"
  ],
  "input": [
    "struct ctx *ctx"
  ],
  "output": "int",
  "helper": [
    "get_prandom_u32",
    "bpf_map_lookup_elem"
  ],
  "compatibleHookpoints": [
    "cgroup_skb",
    "cgroup_sock_addr",
    "cgroup_sock",
    "tracepoint",
    "kprobe",
    "sk_msg",
    "flow_dissector",
    "lwt_seg6local",
    "sk_reuseport",
    "sched_cls",
    "lwt_out",
    "lwt_xmit",
    "raw_tracepoint",
    "sock_ops",
    "raw_tracepoint_writable",
    "socket_filter",
    "perf_event",
    "sched_act",
    "lwt_in",
    "sk_skb",
    "xdp"
  ],
  "source": [
    "int func (struct ctx *ctx)\n",
    "{\n",
    "    uint32_t rand32 = get_prandom_u32 ();\n",
    "    struct ebpf_map *map = (rand32 & 1) ? &map1 : &map2;\n",
    "    int key = 10;\n",
    "    uint64_t *value = (uint64_t *) bpf_map_lookup_elem (map, &key);\n",
    "    if (value == 0)\n",
    "        return 0;\n",
    "    return (int) *value;\n",
    "}\n"
  ],
  "called_function_list": [
    "ebpf_map_update_elem",
    "ebpf_get_current_comm"
  ],
  "call_depth": -1,
  "humanFuncDescription": [
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
int func(struct ctx* ctx)
{
    uint32_t rand32 = get_prandom_u32();
    struct ebpf_map* map = (rand32 & 1) ? &map1 : &map2;

    int key = 10;
    uint64_t* value = (uint64_t*)bpf_map_lookup_elem(map, &key);
    if (value == 0)
        return 0;

    // The following is safe since both maps have the same value size.
    return (int)*value;
}
