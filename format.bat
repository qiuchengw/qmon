@echo off

@REM set SRC_DIR=%cd%\qmon
@REM echo %SRC_DIR% 

@REM -V TAB替换为空格

@REM -U 移除括号两边不必要的空格
@REM -p 在操作符两边插入空格
@REM -f 在两行不相干代码间插入空行
for /R %%f in (*.cpp;*.c;*.h) do (astyle.exe -p -U --style=java --indent=spaces=4 "%%f")
