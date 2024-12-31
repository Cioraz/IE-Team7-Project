# When Contributing

## Cloning and setup
- First clone the repo with `git clone`
- For ssh its `git clone git@github.com:Cioraz/IE-Team7-Project.git` (Use this preferbly, but for this have ssh key setup check it out on Youtube)
- For https its `git clone https://github.com/Cioraz/IE-Team7-Project.git`

## Building
- Build the CmakeLists once using `make build` inside the `Shell_impl` folder. (Do this only once as this generates all the CMake dependencies). Throughout the project mostly we will do this once each time we clone the project

## Compiling
- In the same `Shell_impl` folder run `make compile` to ONLY compile the application and check if there are no compile errors.

## Running
- In the same `Shell_impl` folder run `make run` to ONLY RUN the application and check for logical errors.

## Commiting
- Before COMMITING RUN `make clean` in the `Shell_impl` folder ensuring no temporary files are commited to github.
