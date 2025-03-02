# Centipede
## Description
Atari's iconic arcade game Centipede recreated using C++ and SFML 2.4.2. You can learn more about the game itself <a href=https://en.wikipedia.org/wiki/Centipede_%28video_game%29>here</a>.

This is a WIP. It was my first semester final project for the course Programming Fundamentals.

![image](https://github.com/user-attachments/assets/f3186046-91d2-4b4d-bed2-b0f00b098f92)


## Controls

- Use **arrow keys** to control player movement (it cannot go outside the player area)
- Use the **spacebar** to shoot bullets

## Gameplay 

1. The centipede spawns randomly across the grid and moves left or right, turning when hitting a mushroom or screen boundary.
2. Shooting a mushroom twice destroys it and earns 1 point.
3. Hitting a centipede body segment splits the centipede at that point, and the new segment reverses direction. The score increases by 10 for every hit to the body.
4. The centipede head must be hit to fully eliminate a segment, awarding 20 points. All centipede segment heads must be hit to win the game.
5. If the centipede reaches the player area, it loops continuously until defeated.
6. The first centipede hit spawns a poisoned mushroom in the player area.
7. Touching a poisoned mushroom kills the player.
8. If the centipede touches the player, they die instantly.
9. The score resets upon death.

## Compilation
### 1. Install Dependencies
### On Linux (Ubuntu/Debian): 

``` sh
sudo apt update
sudo apt install g++ libsfml-dev
```

### Windows (MinGW & SFML)
1. Install MinGW (for g++) and add it to the system PATH.
2. Download SFML 2.4.2 from <a href= https://www.sfml-dev.org/download/sfml/>SFML's official site.</a>

### 2. Compile the Game

``` sh
g++ -c Centipede.cpp
g++ Centipede.o -o sfml-app -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system
```

### 3. Run the Game

``` sh
./sfml-app
```

![image](https://github.com/user-attachments/assets/c5dcde5d-5419-4221-b5df-db75cf7519b8)

