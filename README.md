#### Update: 8/6/2021
Experimental features of motion matching is added to the develop/motion_matching branch.
The motion matching functionality includes 
+ animation preprocessor that extracts and serializes feature vectors from animation frames 
+ an motion matching controller running matching algroithm at runtime and computing model's pose
Swich to develop/motion_matching branch to try it out. 

# InceptionEngine

This project is a independently developed game engine dedicated to character animation. It implements a simple vulkan renderer and explore more on
different techniques of doing realistic and fully controllable character animation. It also implements a FBX importer commandline tool for converting FBX
resource to engine format. A StandAlone game is provided, for the purpose of testing the ability of the engine runtime. A selected feature list
is described below. 

##### Requirement for building this program:
+ CMake 3.18 or higher.
+ A C++ compiler that supports concepts. MSVC 16.8 or higher is recommanded. 

##### How to build this program:
In CMake GUI, choose

+ Where is the source code: D:/InceptionEngine
+ Where to build the binary: D:/InceptionEngine/build_x64
+ Use default configuration.

Note that disk D: is not required. You can choose to clone this repo to other disk. However,
the file name "build_x64" is required. It won't work if you build the binary to a folder not 
naming "build_x64", since this filename is hard coded in the source code.

##### If you want to run the StandAlong game, don't forget to change the "Startup project" to StandAlone. 

##### Selected feature list
###### General 
+ Vulkan 3D renderer backend
+ ECS system for storing and ticking component
+ Nativescripting system with C++ as the scripting language
+ FBX importer commandline tool
+ Hireachical transform system with functionality such as "MoveToInDuration", "RotateInDuration"
+ Rigidbody system for simulating acceleration and speed
+ Collision detection with capsule collider
+ Adjusted and flexible third person following camera
+ Play 2D sound

###### Animation related
+ Forward kinematic to compute bone transform in animation
+ GPU linear skin
+ Skeleton socket system for weapon handling or position recording
+ Aim IK for aiming head-spine bone chain to some position
+ FABRIK for controlling finger-shoulder bone chain
+ Ball-and-socket constraint and hinge constraint in FABRIK
+ Animation State Machine
+ Event animation system (dynamically play specific animation in script)
+ Animation blending
+ Animation blending mask for playing partial animation
+ Animation Callback system. Some places where you can set callback are any specific time stamp in animation,
animation start, animation end, animation be interrupted, animation be blended out and etc.
+ Dynamic animation speed control

##### Third-party libraries used in this project
+ Vulkan
+ FBXSDK
+ glm
+ glfw
+ cereal
+ irrKlang
+ stb image




