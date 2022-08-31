#include "NetFilterDriver_Include.h"

NTSTATUS
_WfpDeInit()
{
    NTSTATUS status = STATUS_SUCCESS;

    if (g_hEngineHandle) {
        for (int iIP = 0; iIP < cIPsToCallouts; ++iIP) {
            if (RtlIsZeroMemory(&g_rgFilterCalloutsKeys[iIP], sizeof(g_rgFilterCalloutsKeys[iIP]))) {
                break;
            }

            status = FwpmFilterDeleteByKey(g_hEngineHandle, &g_rgFilterCalloutsKeys[iIP]);
            PRINT_ON_FAILURE(status, "wfp deinit - failed, cannot delete callout filter 0x%x", status);
        }
        RtlZeroMemory(&g_rgFilterCalloutsKeys, sizeof(g_rgFilterCalloutsKeys));

        status = FwpmCalloutDeleteByKey(g_hEngineHandle, &NETFILTERDRIVER_GUID_WFP_CALLOUT);
        PRINT_ON_FAILURE(status, "wfp deinit - failed, cannot delete callout 0x%x", status);

        for (int iIP = 0; iIP < cIPsToBlock; ++iIP) {
            if (RtlIsZeroMemory(&g_rgFilterKeys[iIP], sizeof(g_rgFilterKeys[iIP]))) {
                break;
            }

            status = FwpmFilterDeleteByKey(g_hEngineHandle, &g_rgFilterKeys[iIP]);
            PRINT_ON_FAILURE(status, "wfp deinit - failed, cannot delete filter 0x%x", status);
        }
        RtlZeroMemory(&g_rgFilterKeys, sizeof(g_rgFilterKeys));

        status = FwpmSubLayerDeleteByKey(g_hEngineHandle, &NETFILTERDRIVER_GUID_WFP_SUBLAYER);
        PRINT_ON_FAILURE(status, "wfp deinit - failed, cannot delete sublayer 0x%x", status);

        status = FwpmProviderDeleteByKey(g_hEngineHandle, &NETFILTERDRIVER_GUID_WFP_PROVIDER);
        PRINT_ON_FAILURE(status, "wfp deinit - failed, cannot delete provider 0x%x", status);

    }

    return STATUS_SUCCESS;
}

NTSTATUS
_WfpSessionInit()
{
    NTSTATUS       status = STATUS_SUCCESS;
    FWPM_PROVIDER  *pProvider = NULL;
    FWPM_SUBLAYER  *pSubLayer = NULL;
    FWPM_PROVIDER  provider = { 0 };
    FWPM_SUBLAYER  subLayer = { 0 };

    status = FwpmProviderGetByKey(g_hEngineHandle, &NETFILTERDRIVER_GUID_WFP_PROVIDER, &pProvider);
    if (status == STATUS_FWP_PROVIDER_NOT_FOUND) {
        provider.providerKey = NETFILTERDRIVER_GUID_WFP_PROVIDER;
        provider.displayData.name = DRIVER_WFP_COMPANY_WSTR;
        provider.displayData.description = DRIVER_WFP_DESCRIPTION_WSTR;
        //provider.flags = FWPM_PROVIDER_FLAG_PERSISTENT;
        status = FwpmProviderAdd(g_hEngineHandle, &provider, NULL);

        if (status == STATUS_FWP_ALREADY_EXISTS) {
            NetFilterPrint(INFO, "Provider already exists");
        } else {
            PRINT_AND_EXIT_ON_FAILURE(status, "Provider add failed 0x%x", status);
        }
    } else {
        FwpmFreeMemory((VOID**)&pProvider);
    }

    status = FwpmSubLayerGetByKey(g_hEngineHandle, &NETFILTERDRIVER_GUID_WFP_SUBLAYER, &pSubLayer);
    if (status == STATUS_FWP_SUBLAYER_NOT_FOUND) {
        subLayer.providerKey = (GUID*)&NETFILTERDRIVER_GUID_WFP_PROVIDER;
        subLayer.subLayerKey = NETFILTERDRIVER_GUID_WFP_SUBLAYER;
        subLayer.displayData.name = DRIVER_WFP_SUBLAYER_NAME_WSTR;
        subLayer.weight = DRIVER_WFP_SUBLAYER_WEIGHT;
        //subLayer.flags = FWPM_SUBLAYER_FLAG_PERSISTENT;
        status = FwpmSubLayerAdd(g_hEngineHandle, &subLayer, NULL);

        if (status == STATUS_FWP_ALREADY_EXISTS) {
            NetFilterPrint(INFO, "SubLayer already exists");
        } else {
            PRINT_AND_JUMP_ON_FAILURE(status, clean1, "SubLayer add failed 0x%x", status);
        }
    } else {
        FwpmFreeMemory((VOID**)&pSubLayer);
    }

clean1:
    FwpmProviderDeleteByKey(g_hEngineHandle, &NETFILTERDRIVER_GUID_WFP_PROVIDER);

exit:
    return status;
}

NTSTATUS
_WfpAddFiltersBlockIPs()
{
    NTSTATUS status = STATUS_SUCCESS;

    for (int iIP = 0; iIP < cIPsToBlock; ++iIP) {
        FWPM_FILTER_CONDITION aFilterCondition = { 0 };
        FWPM_FILTER aFilter = { 0 };
        IN_ADDR aIPv4 = { 0 };
        UINT16 u16Port = 0;
        FWP_V4_ADDR_AND_MASK aIpv4AddrMask = { 0 };

        aFilterCondition.fieldKey = FWPM_CONDITION_IP_REMOTE_ADDRESS;
        aFilterCondition.matchType = FWP_MATCH_EQUAL;
        aFilterCondition.conditionValue.type = FWP_V4_ADDR_MASK;
        status = RtlIpv4StringToAddressEx(g_rgkszIPsToBlock[iIP], TRUE, &aIPv4, &u16Port);
        PRINT_AND_EXIT_ON_FAILURE(status, "operation failed, cannot convert ipv4 to uint32 0x%x", status);
        aIpv4AddrMask.addr = RtlUlongByteSwap(aIPv4.S_un.S_addr);
        aIpv4AddrMask.mask = MAXUINT32;
        aFilterCondition.conditionValue.v4AddrMask = &aIpv4AddrMask;

        status = ExUuidCreate(&g_rgFilterKeys[iIP]);
        PRINT_AND_JUMP_ON_FAILURE(status, clean1, "operation failed, cannot create GUID 0x%x", status);

        RtlCopyMemory(&aFilter.filterKey, &g_rgFilterKeys[iIP], sizeof(g_rgFilterKeys[iIP]));
        aFilter.displayData.name = DRIVER_WFP_FILTER_BLOCK_IP_NAME_WSTR;
        aFilter.displayData.description = DRIVER_WFP_FILTER_BLOCK_IP_DESC_WSTR;
        //aFilter.flags = FWPM_FILTER_FLAG_PERSISTENT;
        aFilter.providerKey = (GUID*)&NETFILTERDRIVER_GUID_WFP_PROVIDER;
        aFilter.layerKey = FWPM_LAYER_OUTBOUND_IPPACKET_V4;
        aFilter.subLayerKey = NETFILTERDRIVER_GUID_WFP_SUBLAYER;
        aFilter.weight.type = FWP_UINT8;
        aFilter.weight.int8 = 0xf;
        aFilter.numFilterConditions = 1;
        aFilter.filterCondition = &aFilterCondition;
        aFilter.action.type = FWP_ACTION_BLOCK;

        status = FwpmFilterAdd(g_hEngineHandle, &aFilter, NULL, NULL);
        PRINT_AND_JUMP_ON_FAILURE(status, clean1, "operation failed, cannot add filter 0x%x", status);
    }

    goto exit;

clean1:
    RtlZeroMemory(&g_rgFilterKeys, sizeof(g_rgFilterKeys));

exit:
    return status;
}

NTSTATUS
_WfpAddCallouts()
{
    NTSTATUS                status = STATUS_SUCCESS;
    FWPM_CALLOUT            *pMCallout = NULL;
    FWPS_CALLOUT            aSCallOut = { 0 };
    FWPM_CALLOUT            aMCallout = { 0 };
    FWPM_FILTER_CONDITION   aFilterCondition = { 0 };
    FWPM_FILTER             aFilter = { 0 };
    IN_ADDR                 aIPv4 = { 0 };
    UINT16                  u16Port = 0;
    FWP_V4_ADDR_AND_MASK    aIpv4AddrMask = { 0 };

    aSCallOut.calloutKey = NETFILTERDRIVER_GUID_WFP_CALLOUT;
    aSCallOut.flags = FWP_CALLOUT_FLAG_ALLOW_OFFLOAD;
    aSCallOut.notifyFn = FwpsCalloutNotifyFn;
    aSCallOut.classifyFn = FwpsCalloutClassifyFn;
    aSCallOut.flowDeleteFn = NULL;
    status = FwpsCalloutRegister(g_pDeviceObject, &aSCallOut, NULL);
    PRINT_AND_EXIT_ON_FAILURE(status, "operation failed, cannot register callout 0x%x", status);

    status = FwpmCalloutGetByKey(g_hEngineHandle, &NETFILTERDRIVER_GUID_WFP_SUBLAYER, &pMCallout);
    if (status == STATUS_FWP_CALLOUT_NOT_FOUND) {
        aMCallout.calloutKey = NETFILTERDRIVER_GUID_WFP_CALLOUT;
        aMCallout.displayData.name = DRIVER_WFP_CALLOUT_NAME_WSTR;
        aMCallout.displayData.description = DRIVER_WFP_CALLOUT_DESC_WSTR;
        //aMCallout.flags = FWPM_CALLOUT_FLAG_PERSISTENT;
        aMCallout.providerKey = (GUID*)&NETFILTERDRIVER_GUID_WFP_PROVIDER;
        aMCallout.applicableLayer = FWPM_LAYER_OUTBOUND_IPPACKET_V4;
        status = FwpmCalloutAdd(g_hEngineHandle, &aMCallout, NULL, NULL);
        PRINT_AND_JUMP_ON_FAILURE(status, clean3, "operation failed, cannot add callout 0x%x", status);

        if (status == STATUS_FWP_ALREADY_EXISTS) {
            NetFilterPrint(INFO, "Callout already exists");
        } else {
            PRINT_AND_EXIT_ON_FAILURE(status, "Callout add failed 0x%x", status);
        }
    } else {
        FwpmFreeMemory((VOID**)&pMCallout);
    }

    aFilterCondition.fieldKey = FWPM_CONDITION_IP_REMOTE_ADDRESS;
    aFilterCondition.matchType = FWP_MATCH_EQUAL;
    aFilterCondition.conditionValue.type = FWP_V4_ADDR_MASK;
    status = RtlIpv4StringToAddressEx(g_rgkszIPsToCallouts[0], TRUE, &aIPv4, &u16Port);
    PRINT_AND_JUMP_ON_FAILURE(status, clean2, "operation failed, cannot convert ipv4 to uint32 0x%x", status);
    aIpv4AddrMask.addr = RtlUlongByteSwap(aIPv4.S_un.S_addr);
    aIpv4AddrMask.mask = MAXUINT32;
    aFilterCondition.conditionValue.v4AddrMask = &aIpv4AddrMask;

    status = ExUuidCreate(&g_rgFilterCalloutsKeys[0]);
    PRINT_AND_JUMP_ON_FAILURE(status, clean1, "operation failed, cannot create GUID 0x%x", status);

    RtlCopyMemory(&aFilter.filterKey, &g_rgFilterCalloutsKeys[0], sizeof(g_rgFilterCalloutsKeys[0]));
    aFilter.displayData.name = DRIVER_WFP_FILTER_CALLOUT_NAME_WSTR;
    aFilter.displayData.description = DRIVER_WFP_FILTER_CALLOUT_DESC_WSTR;
    //aFilter.flags = FWPM_FILTER_FLAG_PERSISTENT;
    aFilter.providerKey = (GUID*)&NETFILTERDRIVER_GUID_WFP_PROVIDER;
    aFilter.layerKey = FWPM_LAYER_OUTBOUND_IPPACKET_V4;
    aFilter.subLayerKey = NETFILTERDRIVER_GUID_WFP_SUBLAYER;
    aFilter.weight.type = FWP_UINT8;
    aFilter.weight.int8 = 0xf;
    aFilter.numFilterConditions = 1;
    aFilter.filterCondition = &aFilterCondition;
    aFilter.action.type = FWP_ACTION_CALLOUT_UNKNOWN;
    aFilter.action.calloutKey = NETFILTERDRIVER_GUID_WFP_CALLOUT;

    status = FwpmFilterAdd(g_hEngineHandle, &aFilter, NULL, NULL);
    PRINT_AND_JUMP_ON_FAILURE(status, clean1, "operation failed, cannot add filter 0x%x", status);

    goto exit;

clean1:
    RtlZeroMemory(&g_rgFilterCalloutsKeys, sizeof(g_rgFilterCalloutsKeys));

clean2:
    FwpmCalloutDeleteByKey(g_hEngineHandle, &NETFILTERDRIVER_GUID_WFP_CALLOUT);

clean3:
    FwpsCalloutUnregisterByKey(&NETFILTERDRIVER_GUID_WFP_CALLOUT);

exit:
    return status;
}