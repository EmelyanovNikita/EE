#!/usr/bin/env bash

read -p "Введите название процесса: " proc_name
read -p "Введите переодичность проверки: " time

while true
do
    if pgrep -x "$proc_name" > /dev/null; then
        echo "Процесс $proc_name работает"
    else
        echo "Процесс $proc_name не работает"
    fi
    sleep "$time"
done

