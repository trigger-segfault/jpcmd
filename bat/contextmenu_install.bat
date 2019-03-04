@echo off

set "search=C:\\Programs\\Tools\\Jpcmd"
set "replace=%CD:\=\\%"

cd reg
if exist contextmenu_install.reg (
	del contextmenu_install.reg
)
cd ..

setLocal EnableDelayedExpansion
for /f "tokens=* delims= " %%a in (reg/contextmenu_install_TEMPLATE.reg) do (
	set str=%%a
	set str=!str:%search%=%replace%!
	echo !str!
	echo !str!>>reg/contextmenu_install.reg
)
endLocal

start reg/contextmenu_install.reg