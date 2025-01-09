#!/usr/bin/env bash

read -p "Введите имя процесса: " proc_name
read -p "Ввеведите дату время начала поиска логов " time_beg
read -p "Кконца поиска логов: " time_end
dir_proc_name=$(ps aux|awk '{print $11}' | grep "$proc_name" | head -1) 

journalctl --since "$time_beg" --until "$time_end" | grep "$dir_proc_name"
