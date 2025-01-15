build:
	mkdir -p build && cd build && cmake .. && cd ..

compile:
	cd build && make && cd ..

run:
	cd build && ./main 

clean:
	rm -rf build
