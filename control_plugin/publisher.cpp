#include <string>
#include <pybind11/pybind11.h>
#include <gazebo/gazebo_config.h>
#include <gazebo/transport/transport.hh>
#include <gazebo/msgs/msgs.hh>

#include <gazebo/gazebo_client.hh>

/////////////////////////////////////////////////
int publishVector(std::string topic,  double value)
{
  // Load gazebo as a client
  gazebo::client::setup();

  // Create our node for communication
  gazebo::transport::NodePtr node(new gazebo::transport::Node());
  node->Init();

  // Publish to the  velodyne topic
  gazebo::transport::PublisherPtr pub =
    node->Advertise<gazebo::msgs::Vector3d>(topic);//"~/my_velodyne/vel_cmd"

  // Wait for a subscriber to connect to this publisher
  pub->WaitForConnection();

  // Create a a vector3 message
  gazebo::msgs::Vector3d msg;

  // Set the velocity in the x-component
  gazebo::msgs::Set(&msg, ignition::math::Vector3d(value, 0, 0));

  // Send the message
  pub->Publish(msg);

  // Make sure to shut everything down.
  gazebo::client::shutdown();
}

PYBIND11_MODULE(publisher, m) {
    m.doc() = "Gazebo publisher"; // optional module docstring

    m.def("publishVector", &publishVector, "A function which publishes vector data to topic");
}