%~d0
cd %~dp0
for /R %%f in (*.c *.cpp *.h) do clang-format.exe --verbose -i -style=file "%%f"
pause
