# Geometry Wars

## Overview
This is a 2D arcade-style shooter game implemented in C++ using the SFML library for graphics and input handling,
and Dear ImGui for the user interface. This project is a clone inspired by the classic Geometry Wars series,
focusing on entity-component-system (ECS) architecture to manage game objects like players, enemies, and bullets.
It demonstrates core game development concepts including collision detection, entity spawning, scoring, and
configurable gameplay via a text-based configuration file.

The game features a player-controlled ship that shoots bullets at procedurally generated enemies, with mechanics
for enemy fragmentation, scoring, and a special ability on cooldown. It was developed as a learning exercise to
practice object-oriented design, game loops, and GUI integration in C++.

## Features
### Player Mechanics
- The player is represented by a configurable polygonal shape and spawns at the center of the screen.
- Movement is controlled via WASD keys at a speed defined in the configuration file, with diagonal movement normalized to maintain consistent velocity.
- The player is confined within the window bounds.
- Left mouse click fires bullets toward the cursor, with bullet properties (speed, size, lifespan) loaded from config.
- Right mouse click activates a special ability, spawning multiple entities with unique graphics and introducing a custom game mechanic via a new ECS component.
- The special ability includes a visible cooldown timer (non-ImGui) and is not configurable via the config file.

### Enemy Mechanics
- Enemies spawn at random locations every X frames, where X is defined in the config, adjusted for shape size to avoid window collision.
- Enemies have random polygonal shapes (vertex count between min/max from config), random colors, and speeds within a defined range.
- Enemies bounce off screen edges while maintaining speed.
- Upon collision with bullets or the player, large enemies fragment into smaller versions (once per vertex) that radiate outward at fixed angular intervals.
- Small enemies have double the score value of their parent and a fixed lifespan

### Scoring System
- Enemies are assigned scores based on vertex count (N * 100 for large, double for small)
- Score increases when player bullets destroy enemies and is displayed in the top-left corner.

### Rendering and Effects
- Entities rotate slowly for visual appeal.
- Lifespan-based entities fade out by reducing alpha transparency over time.

### GUI (ImGui)
- Ability to toggle individual game systems (e.g., movement, collision) on/off, excluding rendering and GUI
- Displays lists of all entities by ID, tag, and position
- Ability to desotry entities via UI interaction
- Ability to adjust enemy spawn interval dynamically
- Ability to manually spawn enemies
- Designed for usability and clear presentation
Note: The GUI may be toggled on/off using the G key

### Miscellaneous
- 'P' key pauses the game
- 'ESC' key exits the game
- Game state managed via an ECS pattern, with entity removal for dead objects

### Configuration
- Loaded from a text file with sections for window settings, font, player, enemy, and bullet properties.
- Syntax examples:
```
Window: Window W H FL FS (width, height, frame limit, fullscreen flag).
Font: Font F S R G B (file path, size, RGB color).
Player: Player SR CR S FR FG FB OR OG OB OT V (shape/collision radii, speed, colors, thickness, vertices).
Enemy: Enemy SR CR SMIN SMAX OR OG OT VMIN VMAX L SI (radii, speed range, color/thickness, vertex range, small lifespan, spawn interval).
Bullet: Bullet SR CR S FR FG FB OR OG OB OT V L (radii, speed, colors, thickness, vertices, lifespan).
```

## Technologies Used
- C++: Core language for game logic and ECS implementation
- SFML (Simple and Fast Multimedia Library): Handles window creation, graphics rendering, input, and fonts.
- Dear ImGui: For in-game debugging and configuration GUI
- Custom ECS: Lightweight entity manager with compoinents for position, velocity, shape, collision, score, etc.

## Installation and Setup
1. Clone the repository
```bash
git clone https://github.com/jerrywallacecs/GeometryWars.git
```
2. Build the project using CMAKE, as this ensures SFML and Dear ImGui are linked
3. Edit the config.txt file in the working directory
4. Run the executable

## Usage
- Launch the game executable
- Control the player with WASD, shoot with left mouse, special ability with right mouse
- Use ImGui panels for debugging and adjustments
- Pause with 'P', exit with 'ESC'

## Development Approach
The project followed a structured implementation:

- Developed a Vec2 class for vector math.
- Implemented ECS basics, including entity management and dead entity cleanup.
- Built core game loop with player spawning, rendering, enemy/bullet creation.
- Added input handling, movement, collisions, and scoring.
- Integrated config file parsing and GUI for debugging.
- Refined with special ability, fading effects, and polish.

This approach emphasized modular design and iterative testing.

## Limitations
- No audio or advanced shaders.
- Basic collision (circle-based).
- Configuration is text-based; no in-game editor beyond ImGui tweaks.
- Tested on desktop environments; not optimized for mobile or web.