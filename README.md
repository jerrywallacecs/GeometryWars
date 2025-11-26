# Geometry Wars

## Player
- The player is represented by a shape which is defined in the config file
- The player must spawn in the center of the screen at the beginning of the game, and after it dies (collides with an enemy)
- The player by a speed S read from the config file using WASD
- Note: Be careful to set the speed of the player appropriately. 
If player is moving diagonally, it should not be travling at speed S in both axes but 
its total speed along the diagonal should be S. This can be accomplished by normalizing the velocity
and then multiplying by S.
- The player is confied to move only within the bounds of the window
- The player will shoot a bullet toward the mouse pointer when the left mouse button is clicked.
The speed, size, and lifespan of the bullets are read from the config file.

## Special Ability
- There must be a 'special ability' which is fired by the player when the right mouse button is clicked.
- Multiple entities (bullets etc) spawned by special weapon
- Entities have some unique graphics associated with them
- A unique game mechanic is introduced via a new component
- Note: New component must be added to ComponentTuple in Entity.hpp
- A 'cooldown timer' must be implemented for the special weapon and must be visible outside of ImGui
- The properties of the special move are not in the config file.

## Enemy
- Enemies will spawn in a random location on the screen every X frames, where X is defined in the configuration file
- Enemies must not overlap the sides of the screen at the time of spawn.
- Enemies shapes have random number of vertices, between a given minimum and maximum number, which is
specified in the config file.
- Enemy shape radius will be specified in the configuration file
- Enemies will be given a random color upon spawning
- Enemies will be given a random speed upon spawning, between a min and max value specified in the config
- When an enemy reaches the edge of the window, it should bounce off in the opposite direction at the same speed.
- When (large) enemies collide with a bullet or player, they are destroyed, and N small enemies spawn in its
place, where N is the number of vertices of the original enemy. Each small enemy must have the same number
of vertices and color of the original enemy. These small entities travel outward at angles at fixed intervals
equal to (360 / vertices). For example, if the original enemy had 6 sides, the smaller 6 enemies will travel outward
in intervals of (360/6) = 60 degrees.
- Small enemies spawned should be given a 'reasonable' speed

## Score
- Each time an enemy spawns, it is given a score component of N*100, where N is the number of vertices it has.
Small enemies get double this value
- If a player bullet kills an enemy, the game score is increased by the score component of the enemy killed.
- The score should be displayed with the font specified by the config file in the top-left corner of the screen.
- Small enemies should be given double the score of its large enemy

## Drawing
- In the render system, all entities should be given a slow rotation, which makes the game look a little nicer
- Any special effects which do not alter game play can be added for extra points
- Any Entity with a lifespan that is still alive should have its Color alpha channel set to a ration depending
on how long it has left to live. its alpha value should be set to (float)25/80 * 255. The alpha should go from
255 when it is first spawned, to 0 on the last frame it is alive.

## GUI
Must construct a GUI using ImGui with the following functionality:
- GUI must diplay options to tuyrn off each system independently with the exception of rendering and GUI systems
- For each entity in the game, the GUI must list the ID, tag, and position of that entity. must display a list of
all entities, as well as lists of entities by their tag. You must also have some way of destroying a given
entity by interacting with the UI element associated with it.
- Must be able to change the enemy spawn interval through the GUI
- Must be able to manually spawn enemies in some way through the GUI
- Must be easily usuable and clearly present

## Misc
- The 'P' key should pause the game
- The 'ESC' key should close the game

## Configuration File
The configuration file will have one line each specifying the window size, font format, player, bullet
specification, and enemy specifications. Lines will be given in that order, with the following syntax:

Window W H FL FS
- Width, Height: int
- Frame Limit: int
- Fullscreen: int (1 or 0)

Font F S R G B
- Font File: std::string (no spaces)
- Font Size: int
- RGB: (int, int, int)

Player SR CR S FR FG FB OR OG OB OT V
- Shape Radius: int
- Collision Radius: int
- Speed: float
- Fill Color: (int, int, int)
- Outline Color: (int, int, int)
- Outline Thickness: int
- Shape Vertices: int

Enemy SR CR SMIN SMAX OR OG OT VMIN VMAX L SI
- Shape Radius: int
- Collision Radius: int
- Min / Max Speed: (float, float)
- Outline Color: (int, int, int)
- Outline Thickness: int
- Min / Max Vertices: (int, int)
- Small Lifespan: int
- Spawn Interval: int

Bullet SR CR S FR FG FB OR OG OB OT V L
- Shape Radius: int
- Collision Radius: int
- Speed: float
- Fill Color: (int, int, int)
- Outline Color: (int, int, int)
- Outline Thickness: int
- Shape Vertices: int
- Lifespan: int

## Assignment Hints
Recommended approach:
0. Save the configuration fil reading until later, after Entities implemented
1. Implement the Vec2 class, which will be used for all components
2. At some point, implement EntityManager::removeDeadEntities()
3. Implement basics of the Game class:
	a. Construct a player Entitity using spawnPlayer() function
	b. Implement basic drawing of entities using the Game::sRender() function
	c. Construct some enemies using the spawnEnemy() function
	d. Construct a bullet using the spawnBullet() function
4. Implement Player Movement in Game::sUserInput and Game::sMovement
5. Implement collisions in sCollision and entity.destroy() if it's dead
6. Implement the rest of the game's functionality, including config file reading
7. Implement the GUI functionality. You may however want to implement the GUI functionality earlier to help you debug your program.
