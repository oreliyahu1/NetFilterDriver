
set DriverName=NetFilterDriver
set DriverBinPath=%CD%\%DriverName%.sys

sc query %DriverName%

::Error code: (Win32) 0x424 (1060) - The specified service does not exist as an installed service.
if %ERRORLEVEL% EQU 1060 (goto ContinueToInstall)
if %ERRORLEVEL% EQU 0 (goto ContinueToPreInstall)
goto Error

:ContinueToPreInstall

sc stop %DriverName%
sc delete %DriverName%


:ContinueToInstall

if not exist %DriverBinPath% (
    echo "Error: Driver doesn't exist"
    goto Error
)

sc create %DriverName% type=kernel binPath=%DriverBinPath%
sc start %DriverName%
echo "Done"
exit /b 0

:Error

echo "Error: " %ERRORLEVEL%
exit /b %ERRORLEVEL%
