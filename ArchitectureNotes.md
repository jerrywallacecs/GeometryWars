# Assignment 2 Architecture Notes

## Progression
- Seperation of functionality into Classes
- Writing our own Vec2 class for 2D game math
- Appearance of ECS classes
- EntityManager handles all Entity data
- ECS Systems functionality in Game.h functions

## Architecture
Game Engine
> Systems
> EntityManager
	> Entity
		> Component

## Components
- Pure Data
- Stored in Entity (inside of a tuple)
- Each Component is its own class

## Types of Components
### CTransform
pos: Vec2f
velocity: Vec2f
scale: Vec2f
angle: float

### CCollision
radius: float

### CScore
score: int

### CShape
circle: sf::Circle

### CLifespan
lifespan: int
remaining: int

### CInput
up: int
down: int
left: int
right: int
shoot: int

## Entity
- Entity = Game Object
- Stores Component Tuple
- Tag = Entity 'type'
- Active = Alive or Dead
- ID = Integer identifier

### Entity Class
m_components: CTuple
m_alive: bool
m_id: int
m_tag: string
get<T>: T&
add<T>: void
has<T>: bool
id: int
isAlive: bool
tag: string&
destroy: void

## EntityManager
- Entity 'Factory' Class
- Delayed Entity Add()
	- No Iterator Invalidation
- Secondary map from tag->entity
	- Trade storage for convenience / run time
- Can do other book-keeping like memory management


### EntityManager Class
m_entities: EntityVec
m_entityMap: EntityMap
m_toAdd: EntityVec
m_totalEntities: int
init(): void
update(): void
addEntity(tag_: sp<Entity>
getEntities(): EntityVec&
getEntitites(tag): EntityVec&

## Game
- Top-level Game Object
- Holds all game data
- All game system functions
- All gameplay code via systems

### Game Class
m_window: sf::Window
m_entities: EntityManager
m_paused: bool
m_running: bool
init: void
update: void
sMovement: void
sUserInput: void
sEnemySpawner: void
sCollision: void
sRender: void

## Vec2
- 2D Math Structure
- Templated Class
- We can use 'using Vec2f = Vec2<float>' for shorthand

### Vec2 Class
x: T
y: T
operator ==: bool
operator !=: bool
operator +: Vec2
operator -: Vec2
operator *: Vec2
operator /:
normalize: void
length: float

## What can this engine do?
- Create game objects as Entity instances
- Add Component data to Entities
- Implement game play via Systems
	- Also, handle user input
- Pause game play / exit game
- Can init / load from configuration file

## Limitations
- Can only display one 'scene'
- Can NOT load texture / sound assets
- Can NOT display textured animations
- Does not have any menu

## SFMl Circles
sf::CircleShapes c(r, p);
- r = radius
- p = point count
c.setPointCount(6); // hexagon
c.setOutlineThickness(0); // no outline
c.setOutlineColor(red);
c.setFillColor(blue);
c.setOrigin({ 0, 0 }); // default
- window.draw(c) will draw the origin at c's position
c.setOrigin({ r, r });
- We want circle position to represent its center
- Collision math becomes easier