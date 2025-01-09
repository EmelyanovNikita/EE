#!/usr/bin/env bash 


read -p "Введите время запуска будльника: " time 

echo "mplayer mus_script1.mp3" | at $time 2> /dev/null


