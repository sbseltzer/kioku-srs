set build_dir=%CD%
IF "%build_conf%" == "" (
  set build_conf=Release
)
cd build
rm -rf test-sandbox
mkdir test-sandbox
set CTEST_OUTPUT_ON_FAILURE=1
ctest -C %build_conf% --build-run-dir test-sandbox
set result=%errorlevel%
if not "%result%"=="0" (
  type Testing\Temporary\LastTest.log
)
cd %build_dir%
exit /b %result%