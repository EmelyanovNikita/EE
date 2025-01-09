#!/bin/bash

# Определяем файл для записи
LOGFILE="file.txt"


# Получаем текущее время
CURRENT_TIME=$(date +"%Y-%m-%d %H:%M:%S")

# Получаем номер записи
if [ ! -f "$LOGFILE" ]; then
    echo "$CURRENT_TIME - I run $RECORD_NUMBER time" > "$LOGFILE"
fi
RECORD_NUMBER=$(cat "$LOGFILE" | wc -l)
((RECORD_NUMBER++))
# Записываем текущее время и номер записи в файл
echo "$CURRENT_TIME - I run $RECORD_NUMBER time" >> "$LOGFILE"

# Запланируем выполнение скрипта снова через минуту
echo "$0" | at now + 1 minute 2>/dev/null
