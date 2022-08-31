#include "NetFilterDriver_Include.h"

HANDLE         g_hEngineHandle = NULL;
PDEVICE_OBJECT g_pDeviceObject = NULL;
GUID           g_rgFilterKeys[cIPsToBlock] = { 0 };
GUID           g_rgFilterCalloutsKeys[cIPsToCallouts] = { 0 };

extern "C"
NTSTATUS
DriverEntry(_In_ PDRIVER_OBJECT  DriverObject,
            _In_ PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(RegistryPath);

    NTSTATUS status = STATUS_SUCCESS;
    NetFilterPrint(INFO, "loading driver");

    status = DriverInit(DriverObject, RegistryPath);
    PRINT_AND_EXIT_ON_FAILURE(status, "loading driver - failed, cannot initialize 0x%x", status);

    NetFilterPrint(INFO, "loading driver - completed");

exit:
    return status;
}

VOID WINAPI
DriverUnload(_In_ PDRIVER_OBJECT DriverObject)
{
    NetFilterPrint(INFO, "unloading driver");

    if (g_hEngineHandle) {
        NTSTATUS status = WfpDeInit();
        PRINT_ON_FAILURE(status, "unloading driver - failed, wfp DeInit failed 0x%x", status);

        status = FwpsCalloutUnregisterByKey(&NETFILTERDRIVER_GUID_WFP_CALLOUT);
        PRINT_ON_FAILURE(status, "unloading driver - failed, cannot unregister callout 0x%x", status);

        status = FwpmEngineClose(g_hEngineHandle);
        PRINT_ON_FAILURE(status, "unloading driver - failed, cannot close engine 0x%x", status);
        g_hEngineHandle = NULL;
    }

    if (g_pDeviceObject) {
        IoDeleteDevice(g_pDeviceObject);
        g_pDeviceObject = NULL;
    }

    if (DriverObject->DriverName.Buffer) {
        RtlFreeUnicodeString(&DriverObject->DriverName);
    }

    NetFilterPrint(INFO, "unloading driver - completed");
}

NTSTATUS
DriverInit(_In_ PDRIVER_OBJECT  DriverObject,
           _In_ PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(RegistryPath);

    NTSTATUS status = STATUS_SUCCESS;
    NetFilterPrint(INFO, "initializing driver");

    if (!RtlCreateUnicodeString(&DriverObject->DriverName, DRIVER_NAME_WSTR)) {
        status = STATUS_NO_MEMORY;
        PRINT_AND_EXIT_ON_FAILURE(status, "initializing driver - failed, cannot create driver name string");
    }

    DriverObject->DriverUnload = DriverUnload;

    status = IoCreateDevice(DriverObject, 0, NULL, FILE_DEVICE_UNKNOWN, 0, FALSE, &g_pDeviceObject);
    PRINT_AND_JUMP_ON_FAILURE(status, clean5, "initializing driver - failed, cannot create device 0x%x", status);

    status = WfpInit();
    PRINT_AND_JUMP_ON_FAILURE(status, clean4, "initializing driver - failed, cannot initialize wfp 0x%x", status);

    status = WfpSessionInit();
    PRINT_AND_JUMP_ON_FAILURE(status, clean3, "initializing driver - failed, cannot initialize wfp session 0x%x", status);

    status = WfpAddFiltersBlockIPs();
    PRINT_AND_JUMP_ON_FAILURE(status, clean2, "initializing driver - failed - cannot add filters for ip's block 0x%x", status);

    status = WfpAddCallouts();
    PRINT_AND_JUMP_ON_FAILURE(status, clean1, "initializing driver - failed - cannot add callouts 0x%x", status);

    NetFilterPrint(INFO, "initializing driver - completed");
    goto exit;

clean1:
    for (int iIP = 0; iIP < cIPsToBlock; ++iIP) {
        if (RtlIsZeroMemory(&g_rgFilterKeys[iIP], sizeof(g_rgFilterKeys[iIP]))) {
            break;
        }

        FwpmFilterDeleteByKey(g_hEngineHandle, &g_rgFilterKeys[iIP]);
    }
    RtlZeroMemory(&g_rgFilterKeys, sizeof(g_rgFilterKeys));

clean2:
    FwpmSubLayerDeleteByKey(g_hEngineHandle, &NETFILTERDRIVER_GUID_WFP_SUBLAYER);
    FwpmProviderDeleteByKey(g_hEngineHandle, &NETFILTERDRIVER_GUID_WFP_PROVIDER);

clean3:
    FwpmEngineClose(g_hEngineHandle);
    g_hEngineHandle = NULL;

clean4:
    IoDeleteDevice(g_pDeviceObject);
    g_pDeviceObject = NULL;

clean5:
    RtlFreeUnicodeString(&DriverObject->DriverName);

exit:
    return status;
}

NTSTATUS
WfpInit()
{
    NTSTATUS        status = STATUS_SUCCESS;
    FWPM_SESSION    session = { 0 };
    g_hEngineHandle = NULL;

    session.displayData.name = DRIVER_WFP_NAME_WSTR;
    session.flags = FWPM_SESSION_FLAG_DYNAMIC;
    status = FwpmEngineOpen(NULL, RPC_C_AUTHN_WINNT, NULL, &session, &g_hEngineHandle);
    PRINT_ON_FAILURE(status, "initializing driver - failed, cannot open session for filter engine 0x%x", status);

    return status;
}