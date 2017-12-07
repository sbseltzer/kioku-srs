set build_dir=%CD%
IF "%build_conf%" == "" (
  set build_conf=Release
)
cd build
set CTEST_OUTPUT_ON_FAILURE=1
ctest -C %build_conf% --timeout 5
set result=%errorlevel%
if not "%result%"=="0" (
  type Testing\Temporary\LastTest.log
)
cd %build_dir%
exit /b %result%