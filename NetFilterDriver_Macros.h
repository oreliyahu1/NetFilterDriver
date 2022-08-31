#ifndef NETFILTERDRIVER_MACROS_H
#define NETFILTERDRIVER_MACROS_H

#include <ntifs.h>
#include <ntddk.h>
#include <guiddef.h>
#include <initguid.h>
#include <devguid.h>
#include <fwpmk.h>
#include <fwpsk.h>
#include <ip2string.h>
#include <inaddr.h>

#define DRIVER_NAME_STR                        "NetFilterDriver"
#define DRIVER_NAME_WSTR                       _STR2WSTR(DRIVER_NAME_STR)
#define DRIVER_WFP_NAME_WSTR                   _STR2WSTR(DRIVER_NAME_STR" WFP Kernel Mode Session")
#define DRIVER_WFP_COMPANY_WSTR                L"Or Eliyahu"
#define DRIVER_WFP_DESCRIPTION_WSTR            _STR2WSTR(DRIVER_NAME_STR" WFP Kernel")
#define DRIVER_WFP_SUBLAYER_NAME_WSTR          _STR2WSTR(DRIVER_NAME_STR" WFP Kernel Mode SubLayer")
#define DRIVER_WFP_SUBLAYER_WEIGHT             0x0FFF
#define DRIVER_WFP_FILTER_BLOCK_IP_NAME_WSTR   _STR2WSTR(DRIVER_NAME_STR" WFP Kernel Mode Filter")
#define DRIVER_WFP_FILTER_BLOCK_IP_DESC_WSTR   _STR2WSTR(DRIVER_NAME_STR" WFP Kernel Block IP")
#define DRIVER_WFP_CALLOUT_NAME_WSTR           _STR2WSTR(DRIVER_NAME_STR" WFP Kernel Mode Callout")
#define DRIVER_WFP_CALLOUT_DESC_WSTR           _STR2WSTR(DRIVER_NAME_STR" WFP Kernel Callout")
#define DRIVER_WFP_FILTER_CALLOUT_NAME_WSTR    _STR2WSTR(DRIVER_NAME_STR" WFP Kernel Mode Filter Callout")
#define DRIVER_WFP_FILTER_CALLOUT_DESC_WSTR    _STR2WSTR(DRIVER_NAME_STR" WFP Kernel Callout")

#define NetFilterPrint(level, msg, ... )                    \
    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_##level##_LEVEL, \
    DRIVER_NAME_STR "::"#level":: "##msg##"%c", __VA_ARGS__ '\n' ))

#define EXC_ON_FAILURE(status, exc )                \
    if (!NT_SUCCESS(status)) {                      \
        exc                                         \
    }

#define RETURN_ON_FAILURE(status)                       \
    EXC_ON_FAILURE(status, return status;)

#define EXC_AND_RETURN_ON_FAILURE(status, exc)          \
    EXC_ON_FAILURE(status, exc return status;)

#define EXC_AND_JUMP_ON_FAILURE(status, label, exc)     \
    EXC_ON_FAILURE(status, exc goto label;)

#define EXC_AND_EXIT_ON_FAILURE(status, exc)           \
    EXC_AND_JUMP_ON_FAILURE(status, exit, exc)

#define PRINT_AND_RETURN_ON_FAILURE(status, ... )       \
    EXC_AND_RETURN_ON_FAILURE(status, NetFilterPrint(ERROR, __VA_ARGS__, );)

#define PRINT_AND_JUMP_ON_FAILURE(status, label, ... )  \
    EXC_AND_JUMP_ON_FAILURE(status, label, NetFilterPrint(ERROR, __VA_ARGS__, );)

#define PRINT_AND_EXIT_ON_FAILURE(status, ... )         \
    EXC_AND_EXIT_ON_FAILURE(status, NetFilterPrint(ERROR, __VA_ARGS__, );)

#define PRINT_ON_FAILURE(status, ... )                  \
    EXC_ON_FAILURE(status, NetFilterPrint(ERROR, __VA_ARGS__, );)

#endif