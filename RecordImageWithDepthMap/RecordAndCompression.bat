@set FileName=%DATE:~-10,4%-%DATE:~-5,2%-%DATE:~-2,2%_
@cd C:/Users/%USERNAME%/Desktop
@echo 記録開始します
recorder.exe
@rem 記録終了から1分間待つ(ねんのため)
@timeout 90 /nobreak
@echo 記録終了
@echo binファイルの圧縮を開始します
@dir /A-D /B %FileName%*bin > temp.txt
@for /f "delims=." %%i in (temp.txt) do set ExtractingFile=%%i
@rem x64のexeを実行する
@cd 7z/x64
7za.exe a -mx=9 -m0=LZMA2 ../../%ExtractingFile%.7z ../../%ExtractingFile%.bin
@echo %ExtractingFile%.binを7z圧縮したゾ!
@cd ../../
@rem 一時ファイルのtemp.txtを削除する
@del temp.txt
@echo temp.txtを削除したゾ!
@del %ExtractingFile%.bin
@echo オリジナルデータ:%ExtractingFile%.binを削除したゾ!
@pause
@exit