# When Contributing follow these steps

## Writing Code
### Cloning and setup
- First clone the repo with `git clone`
- For ssh its `git clone -b gemini git@github.com:Cioraz/IE-Team7-Project.git` (Use this preferbly, but for this have ssh key setup check it out on Youtube)

### Execution
- cd IE-Team7-Project/gui_shell/build
- run python3 -m venv venv to create a virtual environment
- run source venv/bin/activate
- clear contents of build folder(rm -rf *) and run cmake ..
- create converter.py file and copy the content of converter.py from github
- pip install google.generativeai
- run python3 converter.py "some text input here"
- cmake .. -DCMAKE_PREFIX_PATH=/usr/lib/qt6
- make -j$(nproc)
- ./shell_ulti

