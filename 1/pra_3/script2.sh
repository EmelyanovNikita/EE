#!/usr/bin/env bash


echo "Количество букв: "
grep -o '[а-яА-ЯёЁ]' alice.txt | wc -l
