from subprocess import run
from time import sleep

from multiprocessing import Process
import sys
sys.path.insert(0, './control_plugin/build')
import publisher


def moveForward():
	print("inside move forward")
	sleep(5)
	joints_velocities = publisher.getJointsVel("my_robot")
	print("joint velocities {}".format(joints_velocities))
	print("inside move forward")
	publisher.publishVector("~/my_robot/control", ["2.", "2."])
	print("inside move forward")
	sleep(5)
	joints_velocities = publisher.getJointsVel("my_robot")
	print("after 2 seconds {}".format(joints_velocities))
	publisher.publishVector("~/my_robot/control", ["0.", "0."])
	sleep(5)
	print("stopping {}".format(publisher.getJointsVel("my_robot")))

def launchGazebo():
	# launch gazebo world
	print("inside launch gazebo")
	print("./database/"+robot+".world")
	run(["gazebo", "./database/"+robot+".world"])

robot = "my_robot"
p1 = Process(target = moveForward)
p1.start()
p2 = Process(target = launchGazebo)
p2.start()
