#ifndef NETFILTERDRIVER_WFP_CALLOUTS_H
#define NETFILTERDRIVER_WFP_CALLOUTS_H

VOID NTAPI
FwpsCalloutClassifyFn(_In_        const FWPS_INCOMING_VALUES0          *inFixedValues,
                      _In_        const FWPS_INCOMING_METADATA_VALUES0 *inMetaValues,
                      _Inout_opt_ void                                 *layerData,
                      _In_opt_    const void                           *classifyContext,
                      _In_        const FWPS_FILTER3                   *filter,
                      _In_        UINT64                               flowContext,
                      _Inout_     FWPS_CLASSIFY_OUT0                   *classifyOut
);

NTSTATUS NTAPI
FwpsCalloutNotifyFn(_In_    FWPS_CALLOUT_NOTIFY_TYPE notifyType,
                    _In_    const GUID               *filterKey,
                    _Inout_ FWPS_FILTER3             *filter
);

#endif