# The Problem Solver 
Have you ever wanted to play as a cowboy who has debts to pay and is in deep financial trouble, and the only way to solve these problems is to shoot at bottles until you get enough money to solve every problem in your life? No? Well, that's too bad! The Problem Solver is all about that! 

Feel like a mouse cowboy as you take out empty and emotionless bottles that did nothing wrong to you. You have debts to pay! Move fast and shoot at those bottles. Watch out! It's your old arch-nemesis! Time itself! You only have 30 seconds on the clock to solve your problems. However, if you're a good shooter, you can add to your time! Try to shoot these bottles as close to the center as possible to get a critical hit. You know when you get it since you'll hear a TING!

Move fast, boy! These problems aren't going to solve themselves.

# Build Instructions 
This project uses CMake as its build syste, so, you will need to have CMake configured on your system. The commands below, as with any CMake project, will set up the project for building. 

```
git clone https://github.com/MohamedAG2002/TheProblemSolver.git 
cd TheProblemSolver 
mkdir build && cd build 
cmake ..
```

After configuring the project with CMake, you can either use `make` on Linux to build the project or open the Visual Studio solution file CMake should have generated and build it that way if you are on Windows.

Even though this project handles all of its dependencies by building them and getting them ready for use, it sitll would be helpful to know the libraries this project uses. Below is the list of all of the libraries the engine and the game use. 

## Dependencies 
- GLFW 
- GLAD 
- MiniAudio
- stb_image
- stb_truetype
- tinyobjloader

However, as mentioned before, there is no need to handle any of these dependencies yourself. They are all--hopefully--ready for use when you clone the repository.

Before you can fully build the project, you'll also need the 'assets' directory which will be copied over to the binary directory upon building the project. You can go to the official itch page and download the game to obtain the asset directory. After which, you can easily copy the asset directory to the main project's directory and CMake will handle the rest. 

Itch: https://frodoalaska.itch.io/the-problem-solver

# Credits
Beer bottle: https://dream-search-repeat.itch.io/cups-and-bottles-little-pack

Menu them: https://opengameart.org/content/spaghetti-western-theme-orchestral

Victory: https://opengameart.org/content/victory-sting
