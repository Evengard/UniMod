@echo on>list.txt
@echo on>json.c
for /R %%I in (*.lua) do @luac -s -o %%~pnI.luac %%I 
for /R %%I in (*.luac) do @echo %%I>>list.txt
for /R %%I in (*.luac) do @bin2c %%I >>json.c