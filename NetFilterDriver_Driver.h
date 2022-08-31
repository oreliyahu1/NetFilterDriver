#ifndef NETFILTERDRIVER_DRIVER_H
#define NETFILTERDRIVER_DRIVER_H

VOID WINAPI DriverUnload(_In_ PDRIVER_OBJECT DriverObject);
NTSTATUS    DriverInit(_In_ PDRIVER_OBJECT  DriverObject,
                       _In_ PUNICODE_STRING RegistryPath);
NTSTATUS    WfpInit();

const char* const g_rgkszIPsToBlock[] = { "81.218.110.166" , "1.1.1.1" };
const char* const g_rgkszIPsToCallouts[] = { "1.2.3.4" };

constexpr size_t cIPsToBlock = sizeof(g_rgkszIPsToBlock) / sizeof(const char*);
constexpr size_t cIPsToCallouts = sizeof(g_rgkszIPsToCallouts) / sizeof(const char*);

extern HANDLE           g_hEngineHandle;
extern PDEVICE_OBJECT   g_pDeviceObject;
extern GUID             g_rgFilterKeys[cIPsToBlock];
extern GUID             g_rgFilterCalloutsKeys[cIPsToCallouts];

#endif