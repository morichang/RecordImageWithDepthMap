@set FileName=%DATE:~-10,4%-%DATE:~-5,2%-%DATE:~-2,2%_
@cd C:/Users/%USERNAME%/Desktop
@echo �L�^�J�n���܂�
recorder.exe
@rem �L�^�I������1���ԑ҂�(�˂�̂���)
@timeout 90 /nobreak
@echo �L�^�I��
@echo bin�t�@�C���̈��k���J�n���܂�
@dir /A-D /B %FileName%*bin > temp.txt
@for /f "delims=." %%i in (temp.txt) do set ExtractingFile=%%i
@rem x64��exe�����s����
@cd 7z/x64
7za.exe a -mx=9 -m0=LZMA2 ../../%ExtractingFile%.7z ../../%ExtractingFile%.bin
@echo %ExtractingFile%.bin��7z���k�����]!
@cd ../../
@rem �ꎞ�t�@�C����temp.txt���폜����
@del temp.txt
@echo temp.txt���폜�����]!
@del %ExtractingFile%.bin
@echo �I���W�i���f�[�^:%ExtractingFile%.bin���폜�����]!
@pause
@exit