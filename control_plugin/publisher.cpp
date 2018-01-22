    #include <string>
    #include <pybind11/pybind11.h>
    #include <pybind11/stl.h>
    #include <gazebo/gazebo_config.h>
    #include <gazebo/transport/transport.hh>
    #include <gazebo/msgs/msgs.hh>
    #include <stdlib.h>

    #include <gazebo/gazebo_client.hh>

    namespace py = pybind11;

    using namespace std;


    class Interface {
      public:
        vector<string> _vectorValue = {};
        string _name = "";
        string _jointVelSub = "";
        string _jointPosSub = "";

      Interface(string robot) {
        _name = robot;
        _jointVelSub = "~/" + _name + "/joints_vel";
        _jointPosSub = "~/" + _name + "/joints_pos";
      }

      void publishVector(string topic,  vector<string> values)
      {
        gazebo::transport::NodePtr node(new gazebo::transport::Node());
        node->Init();

        // Publish to the  velodyne topic
        gazebo::transport::PublisherPtr pub =
          node->Advertise<gazebo::msgs::GzString_V>(topic);

        // Wait for a subscriber to connect to this publisher
        pub->WaitForConnection();

        // Create a a vector3 message
        gazebo::msgs::GzString_V msg;

        for (int i = 0; i < values.size(); i++)
          msg.add_data(values[i]);

        // Send the message
        pub->Publish(msg);
      }

      vector<string> subscribeVector(string topic){
        _vectorValue = {};
        gazebo::client::setup();

        gazebo::transport::NodePtr node(new gazebo::transport::Node());
        node->Init();
        // Subscriber
        gazebo::transport::SubscriberPtr subVector = node->Subscribe(topic, &Interface::cb, this);
        // Starting delay
        gazebo::common::Time::MSleep(10);

        while (_vectorValue.empty()){
          cout << " loop \n";
          gazebo::common::Time::MSleep(1);
        }

        gazebo::client::shutdown();

        return _vectorValue;
      }

      void cb(ConstGzString_VPtr &_msg)
      {
        _vectorValue = {};
        for (int i = 0; i < _msg->data_size(); i++) 
          _vectorValue.push_back(_msg->data(i));
        cout << "received values \n";
      }

      vector<string> getJointsVel()
      {
        return subscribeVector(_jointVelSub);
      }

      vector<string> getJointsPos()
      {
        return subscribeVector(_jointPosSub);
      }

    };

    PYBIND11_MODULE(publisher, m) {
      py::class_<Interface>(m, "Interface")
        .def(py::init<const string &>())
        .def("getJointsVel", &Interface::getJointsVel)
        .def("getJointsPos", &Interface::getJointsPos);
    }