# NetFilterDriver
NetFilterDriver is a sample of Windows kernel driver that based WFP (Windows Filtering Platform) for filtering/interacting with packet processing

* constant list for ip's to block and to triger a callout - manged by NetFilterDriver_Driver.h::g_rgkszIPsToBlock | g_rgkszIPsToCallouts
* filters have the same weights - it could create a raise between checking the filters in case of permit with block on the same sublayer
(not recommended, but for blocking its ok)
* Tested on Win 11
* Windows 11 21H2 SDK/WDK (22000), Windows 11 22H2 SDK/WDK (22621)

###  install and uninstall by batch file - Install.bat / Uninstall.bat

### see DBG prints
- Open up the registry
- Navigate to Computer\HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\Debug Print Filter
- if Debug Print Filter doesn't exist - create it
- Add the following value IHVDRIVER : REG_DWORD : 0x8
- Reboot

### References
- https://docs.microsoft.com/en-us/windows-hardware/drivers/kernel/
- https://docs.microsoft.com/en-us/windows-hardware/drivers/network/roadmap-for-developing-wfp-callout-drivers
