#!/usr/bin/env bash

PID=$(pgrep systemd | head -1)
ps -ef | awk '{print $3" "$8}' | grep "^$PID " | awk '{print $2}' | awk '{split($0, a, "/"); print a[length(a)]}'

