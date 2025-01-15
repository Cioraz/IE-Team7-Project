build:
	mkdir -p build && cd build && cmake .. && cd ..

compile:
	cd build && make && cd ..

run:
	cd build && ./shell 

clean:
	rm -rf build
