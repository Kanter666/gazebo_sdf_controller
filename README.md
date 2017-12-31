# gazebo_sdf_controller
## About
I coudn't find universall controller for robot joints in gazebo so I decided to create my own. Current version needs to have different controllers for different number of joints. Every robot will also need own .world file. Future version will have one controller for any number of joints, however I am not sure if I can have one .world file. I am also planning to add subscriber for data about robot, and extend control with position (current version controls by applying force).

Controller is wrapped in python using pybind11.

## Installation/Running
You will need to build control plugin and launch plugin inside their folders with:
```
mkdir build
cd build
cmake ..
make
```

Then you launch world with robot connected to launch plugin (my_robot). Afterwards run python in build folder of control plugin, then import publisher and use publishVector function:
```
import publisher

publisher.publishVector("~/my_robot/vel_cmd1", 2.1)
publisher.publishVector("~/my_robot/vel_cmd2", 4.1)
```
Wheels will start moving with different increase in speeds.
