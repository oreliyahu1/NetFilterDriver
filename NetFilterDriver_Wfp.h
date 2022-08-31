#ifndef NETFILTERDRIVER_WFP_H
#define NETFILTERDRIVER_WFP_H

// fwpm ops to wrapper only!
NTSTATUS _WfpAddFiltersBlockIPs();
NTSTATUS _WfpAddCallouts();
NTSTATUS _WfpSessionInit();
NTSTATUS _WfpDeInit();

#endif