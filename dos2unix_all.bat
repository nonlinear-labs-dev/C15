%~d0
cd %~dp0
for /R %%f in (*.bat *.ac *.c *.cal .clang-format *.cpp *.css *.dot *.editorconfig .git* *.h *.hpp *.html *.in *.java *.js *.json *.ld *.md *.mk *.mount *.sfdisk *.service *.sh *.svg *.tex *.txt *.xml) do dos2unix.exe "%%f"
cd build-tools\epc\hook
for /R %%f in (*.*) do dos2unix.exe "%%f"
cd build-tools\epc\install
for /R %%f in (*.*) do dos2unix.exe "%%f"
pause
