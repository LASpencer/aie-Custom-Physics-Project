Physics Project by Andrew Spencer 2018

This solution includes both the PhysicsEngine library, which runs a physics engine, and a demo 
application showing how it works. 


The demo opens on a title screen, allowing one of four demos to be chosen by pressing the relevant number 
key.

1. Pool Game
This is a game of eight-ball pool for two players. Each player tries to sink every ball of their colour 
and then the black ball to win the game.

This demonstrates collisions between spheres and boxes, and the use of CollisionObservers to influence the 
game when collisions occur (to detect foul hits from the cue ball, and when a ball is pocketed)

Controls:
ESC			Return to title screen
R			Rerack balls. Counts as a loss if game ongoing
Click and drag		Shoot cueball
Spacebar		Cancel taking shot/rerack balls at end of game

2. Slug Game

This is a game where a slug has to get through an obstacle course to reach a green square of food.

This demonstrates collisions, kinematic and static objects, gravity, springs, and softbodies being used 
and changing their strength at runtime

Controls:
ESC					Return to title screen
WASD					Move slug's head
Left Shift				Relax slug's body
Direction Keys				Move Camera
Right Control/Middle Mouse Click	Center camera on slug
R					Reset game

3. Bouncing Balls

This shows several objects moving around a scene. 

ESC				Return to title screen
Click and drag			Apply impulse to an object
G				Toggle gravity

4. Rope Bridge

This shows a rope connected between two fixed points.

ESC				Return to title screen
Click and drag			Pull on a particle
W				Increase rope tightness
S				Decrease rope tightness


See the Docs folder for a Class Diagram, Activity Diagram, User Manual and Reference document.

