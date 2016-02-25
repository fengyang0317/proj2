for file in *.cpp
do
	echo $file
	g++ $file -lpthread;
	./a.out -i 10000000;
done
