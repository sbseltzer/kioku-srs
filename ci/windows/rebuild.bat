IF "%build_conf%" == "" (
  set build_conf=Release
)
cmake --build build --config %build_conf%
set result=%errorlevel%
exit /b %result%