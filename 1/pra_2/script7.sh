#!/bin/bash


declare -A arr
arr=( ["a"]="а" ["b"]="б" ["v"]="в" ["g"]="г" ["d"]="д" ["e"]="е" ["z"]="з" ["i"]="и" ["y"]="й" ["k"]="к" ["l"]="л" ["m"]="м" ["n"]="н" ["o"]="о" ["p"]="п" ["r"]="р" ["s"]="с" ["t"]="т" ["u"]="у" ["f"]="ф" ["h"]="х" ["c"]="к" ["w"]="в" )
rus_w=""
read -p "Введите слово на английском: " word





for char in $(echo "$word" | fold -w1); do
    rus_w+="${arr[$char]:-$char}"
done

echo "$rus_w"
