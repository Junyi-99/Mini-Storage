mmap_test: func_.o mmap_test.o
	g++ -std=c++11 func_.o mmap_test.o -o mmap_test 
func_.o: func_.h
	g++ -std=c++11 -c func_.cpp
mmap_test.o: func_.h
	g++ -std=c++11 -c mmap_test.cpp

.PHONY:clean
clean:
	rm -f *.o
	rm mmap_test