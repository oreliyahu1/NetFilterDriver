#ifndef NETFILTERDRIVER_WFP_WRAPPER_H
#define NETFILTERDRIVER_WFP_WRAPPER_H

/*
DO NOT ALLOCATE MEMORY IN fwpm_ops FUNCTION
THERE IS NO GUARANTEE FOR MEMORY RELEASE
SINCE FwpmTransactionCommit COULD BE FAILED
WITHOUT ANY WAY TO FREE THE LAST OP RESOURCES
*/
inline NTSTATUS
WfpTransactionWrapper(NTSTATUS(*fwpm_ops)())
{
    if (!fwpm_ops) {
        return STATUS_INVALID_PARAMETER;
    }

    NTSTATUS status = FwpmTransactionBegin(g_hEngineHandle, 0);
    PRINT_AND_EXIT_ON_FAILURE(status, "operation failed, cannot begin transaction 0x%x", status);

    status = fwpm_ops();
    PRINT_AND_JUMP_ON_FAILURE(status, clean1, "operation failed 0x%x", status);

    status = FwpmTransactionCommit(g_hEngineHandle);
    PRINT_AND_JUMP_ON_FAILURE(status, clean1, "operation failed, cannot commit transaction 0x%x", status);
    goto exit;

clean1:
    FwpmTransactionAbort(g_hEngineHandle);

exit:
    return status;
}

#define WFP_OPS_WRAPPER(WfpToWrapperFunc)                \
inline NTSTATUS                                          \
WfpToWrapperFunc##()                                     \
{                                                        \
    return WfpTransactionWrapper(_##WfpToWrapperFunc##); \
}

WFP_OPS_WRAPPER(WfpAddFiltersBlockIPs);
WFP_OPS_WRAPPER(WfpAddCallouts);
WFP_OPS_WRAPPER(WfpSessionInit);
WFP_OPS_WRAPPER(WfpDeInit);

#endif
