read -p "Enter name of target diractory: " target_dir

if !  find . -maxdepth 1 -type d -name "$target_dir"  -print -quit | grep -q .; then
	echo "Directory was not found"
	exit 1
fi
cd $target_dir


read -p "Введите число, сколько папок хотите создать: " num_dir
read -p "Введите шаблон имени папок: " name_dir
read -p "Введите число, сколько подпапок хотите создать: " num_dir_dir
read -p "Введите шаблон имени подпапок: " name_dir_dir
read -p "Введите число, сколько файлов хотите создать: " num_files
read -p "Введите шаблон имени файлов: " name_files
for(( i=0; i<"$num_dir"; i++))
do
        mkdir ${name_dir}${i}
	cd ${name_dir}${i}
	for((y=0; y<"$num_dir_dir"; y++)) 
        do
                mkdir ${name_dir_dir}${y}
		cd ${name_dir_dir}${y}

                for ((a=0; a<"$num_files"; a++))
                do
                        touch ${name_files}${a}
                done
                cd ..
        done
        cd ..



done

