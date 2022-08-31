#include "NetFilterDriver_Include.h"

VOID NTAPI
FwpsCalloutClassifyFn(_In_        const FWPS_INCOMING_VALUES0          *inFixedValues,
                      _In_        const FWPS_INCOMING_METADATA_VALUES0 *inMetaValues,
                      _Inout_opt_ void                                 *layerData,
                      _In_opt_    const void                           *classifyContext,
                      _In_        const FWPS_FILTER3                   *filter,
                      _In_        UINT64                               flowContext,
                      _Inout_     FWPS_CLASSIFY_OUT0                   *classifyOut
)
{
    UNREFERENCED_PARAMETER(inFixedValues);
    UNREFERENCED_PARAMETER(inMetaValues);
    UNREFERENCED_PARAMETER(layerData);
    UNREFERENCED_PARAMETER(classifyContext);
    UNREFERENCED_PARAMETER(filter);
    UNREFERENCED_PARAMETER(flowContext);
    UNREFERENCED_PARAMETER(classifyOut);

    NetFilterPrint(INFO, "FwpsCalloutClassifyFn is called");
}

NTSTATUS NTAPI
FwpsCalloutNotifyFn(_In_    FWPS_CALLOUT_NOTIFY_TYPE notifyType,
                    _In_    const GUID               *filterKey,
                    _Inout_ FWPS_FILTER3             *filter
)
{
    UNREFERENCED_PARAMETER(notifyType);
    UNREFERENCED_PARAMETER(filterKey);
    UNREFERENCED_PARAMETER(filter);

    NetFilterPrint(INFO, "FwpsCalloutNotifyFn is called");

    return STATUS_SUCCESS;
}