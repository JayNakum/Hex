@echo off

title taskman

call .\vendors\TaskMan\venv\Scripts\activate.bat

python .\vendors\TaskMan\taskman.py

pause
