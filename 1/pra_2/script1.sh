read -p "Enter name of diractory: " dir

cd $dir
for(( i=0; i<3; i++))
do 
	mkdir new_dir$i
	cd new_dir$i
	for((y=0; y<3; y++))
	do
		mkdir new_dir$y
		cd new_dir$y
		for ((a=0; a<3; a++))
		do 
			touch new_file$a
		done
		cd ..
	done
	cd ..



done
