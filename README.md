# When Contributing

- Build the CmakeLists once using `make build` inside the `Shell_impl` folder. (Do this only once as this generates all the CMake dependencies)
- In the same `Shell_impl` folder run `make compile` to ONLY compile the application and check if there are no errors.
- In the same `Shell_impl` folder run `make run` to ONLY RUN the application and check for logical errors.
- Before COMMITING RUN `make clean` in the `Shell_impl` folder ensuring no temporary files are commited to github.
