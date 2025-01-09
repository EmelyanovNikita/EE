#!/usr/bin/env bash

read -p "Введите название процесса: " proc_name
read -p "Введите переодичность проверки: " time

full_path=$(ps aux|awk '{print $11}' | grep "$proc_name" | head -1)

while true 
do
		if ! pgrep -x ${proc_name} > /dev/null ;
	then
		echo "Процесс $proc_name не работает. Запускаем процесс"
		$full_path & 2>/dev/null
	else
		echo "Процесс $proc_name работает"
	fi
	sleep "$time"
	
done

