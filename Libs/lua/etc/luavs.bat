@rem Script to build LuaJIT under "Visual Studio .NET Command Prompt".
@rem Do not run from this directory; run it from the toplevel: etc\luavs.bat .
@rem It creates lua51.dll, lua51.lib and luajit.exe in src.
@rem (contributed by David Manura and Mike Pall)

@setlocal
@set MYCOMPILE=cl /nologo /MD /O2 /W3 /c /D_CRT_SECURE_NO_DEPRECATE /I . /I ..\dynasm
@set MYLINK=link /nologo
@set MYMT=mt /nologo

cd src
%MYCOMPILE% l*.c
del lua.obj luac.obj
@REM %MYLINK% /out:lua51.lib l*.obj
lib *.obj -OUT:testlib.lib
cd ..
