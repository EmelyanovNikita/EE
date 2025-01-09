#!/bin/bash

source <(grep = file_for_script5.ini | sed 's/ *= */=/g')

proc_name_search=$(ps aux | awk '{print $11}' | grep -m 1 "${proc_name}")

if [ -z "$proc_name_search" ]; then
    echo "Process not found."
    exit 1
fi

journalctl --since "${time_beg}" --until "${time_end}" | grep "${proc_name_search}"

