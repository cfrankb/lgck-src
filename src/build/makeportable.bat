rem del files\portable.txt
echo "This is a portable version of LGCK builder." > files\portable.txt
cd files
del ..\setup\lgck-portable.zip 
7z a ..\setup\lgck-portable.zip *.* licenses\*.* tutorial\*.* plugins\platforms\*.*
del portable.txt
pause