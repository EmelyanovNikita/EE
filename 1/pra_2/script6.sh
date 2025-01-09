#!/bin/sh
read -p "Введите файл, который нужно резервно вскопировать, и путь к нему: " file
read -p "Введите паппку, куда скопировать файл: " dir
read -p "Введите дату, когда нужно сделать копирование: " date

echo "$(tar --create --file  ${dir}/$(basename $file).tar $file)" |  at $date 2> /dev/null
