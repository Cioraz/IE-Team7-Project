# When Contributing follow these steps

## Writing Code
### Cloning and setup
- First clone the repo with `git clone`
- For ssh its `git clone -b model_testing git@github.com:Cioraz/IE-Team7-Project.git` (Use this preferbly, but for this have ssh key setup check it out on Youtube)

### Execution
- cd IE-Team7-Project/gui_shell/build
- clear contents of build folder except checkpoint-1040 and converter.py
- run cmake ..
- python3 -m venv venv
- source venv/bin/activate
- pip install transformers torch
- pip install peft
- python3 converter.py "some text"
- cmake .. -DCMAKE_PREFIX_PATH=/usr/lib/qt6
- make -j$(nproc)
- ./shell_ulti

## Pushing to Github
## Branching
- First create a new branch using `git switch -c <branch_name>`. Give a suitable branch name according to what new feature is being added.
- Then follow commands like,
  - `git add .` : To add code to the Staging area (Google this if you dont understand)
  - `git commit -m <Please give a good commit message here>` : The commit which will be shown on github
  - `git push` : To push the code onto github.
  - Then come over to github and create a Pull Request (Google about this).
