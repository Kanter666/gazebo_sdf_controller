import os
import threading
import sys
sys.path.insert(0, './control_plugin/build')
from publisher import Interface
from time import sleep
	

def run_gazebo():
	print("Starting gazebo")
	os.system("gazebo ./database/"+robot_name+".world")

thread1 = threading.Thread(target=run_gazebo)
thread1.daemon = True
thread1.start()
robot_name = "my_robot"

print("Waiting for gazebo to load")
sleep(10)

print("Starting robot interface")
robot = Interface(robot_name)

print("joint velocities: {}  model position: {}"
	.format(robot.getJointsVel(), robot.getModelPose())
	)

print("Setting joints velocities to [1., 1.], and waiting 5 seconds")
robot.setJointsVel(["1.", "1."])
sleep(5)

print("joint velocities: {}  model position: {}"
	.format(robot.getJointsVel(), robot.getModelPose())
	)

print("Setting joints velocities to [0., 0.], and waiting 5 seconds")
robot.setJointsVel(["0.", "0."])
sleep(5)

print("joint velocities: {}  model position: {}"
	.format(robot.getJointsVel(), robot.getModelPose())
	)

print("Example run finished - press ENTER to end")	
input()
