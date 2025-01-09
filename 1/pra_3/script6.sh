#!/usr/bin/env bash

echo "Архив после создания: "
tar -cf arh.tar file_1.txt
tar -tf arh.tar

echo "Архив после добавления второго файла: "
tar -rf arh.tar file_2.txt
tar -tf arh.tar

echo "Архив после добавления 3-его файла: "
tar -rf arh.tar file_3.txt
tar -tf arh.tar

echo "Архив после удаления 2-ого файла: "
tar --delete -f arh.tar file_2.txt
tar -tf arh.tar


mkdir unbox


echo "Разархивировали архив"
tar -xf arh.tar -C unbox

