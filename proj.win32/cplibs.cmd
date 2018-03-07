set OutDir=%1
if not exist "%OutDir%" mkdir "%OutDir%"
if not exist "%OutDir%libbox2d.lib" (
	xcopy /Y /Q "%COCOS_FRAMEWORK_ROOT%\prebuilt\win32\*.*" "%OutDir%"
	copy "%OutDir%libbox2d_2013.lib" "%OutDir%libbox2d.lib"
	copy "%OutDir%libSpine_2013.lib" "%OutDir%libSpine.lib"
)