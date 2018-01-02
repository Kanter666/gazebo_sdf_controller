#include <string>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <gazebo/gazebo_config.h>
#include <gazebo/transport/transport.hh>
#include <gazebo/msgs/msgs.hh>

#include <gazebo/gazebo_client.hh>

/////////////////////////////////////////////////
int publishVector(std::string topic,  std::vector<std::string> values)
{
  // Load gazebo as a client
  gazebo::client::setup();

  // Create our node for communication
  gazebo::transport::NodePtr node(new gazebo::transport::Node());
  node->Init();

  // Publish to the  velodyne topic
  gazebo::transport::PublisherPtr pub =
    node->Advertise<gazebo::msgs::GzString_V>(topic);//"~/my_velodyne/vel_cmd"

  // Wait for a subscriber to connect to this publisher
  pub->WaitForConnection();

  // Create a a vector3 message
  gazebo::msgs::GzString_V msg;

  for (int i = 0; i < values.size(); i++)
    msg.add_data(values[i]);

  // Send the message
  pub->Publish(msg);

  // Make sure to shut everything down.
  gazebo::client::shutdown();
}

PYBIND11_MODULE(publisher, m) {
    m.doc() = "Gazebo publisher"; // optional module docstring

    m.def("publishVector", &publishVector, "A function which publishes vector data to topic");
}