set DriverName=NetFilterDriver
set DriverBinPath=%CD%\%DriverName%.sys

sc query %DriverName%
if %ERRORLEVEL% NEQ 0 (goto Error)

:ContinueToPreInstall

sc stop %DriverName%
sc delete %DriverName%
echo "Done"
exit /b 0

:Error

echo "Error: " %ERRORLEVEL%
exit /b %ERRORLEVEL%
