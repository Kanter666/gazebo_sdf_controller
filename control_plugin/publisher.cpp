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
      private: 
        gazebo::transport::NodePtr _node;
        string _name = "";
        string _jointVelPub = "";
        string _jointVelSub = "";
        string _jointPosSub = "";

      public:
        vector<string> _vectorValue = {};

      Interface(string robot) {
        _name = robot;
        _jointVelSub = "~/" + _name + "/joints_vel";
        _jointPosSub = "~/" + _name + "/joints_pos";
        _jointVelPub = "~/" + _name + "/control_vel";

        gazebo::client::setup();

        _node = gazebo::transport::NodePtr(new gazebo::transport::Node());
        _node->Init();
      }

      void publishVector(string topic,  vector<string> values)
      {

        // Publish to the  velodyne topic
        gazebo::transport::PublisherPtr pub =
          _node->Advertise<gazebo::msgs::GzString_V>(topic);

        cerr << " waiting for connection \n ";
        // Wait for a subscriber to connect to this publisher
        pub->WaitForConnection();

        // Create a a vector3 message
        gazebo::msgs::GzString_V msg;

        for (int i = 0; i < values.size(); i++)
          msg.add_data(values[i]);

        cerr << " publishing data \n ";
        // Send the message
        pub->Publish(msg);
      }

      int setJointsVel(vector<string> velocities)
      {
        cerr << "inside set joint " << velocities[0];
        publishVector(_jointVelPub, velocities);
        cerr << " after ";

        return 1;
      }

      vector<string> subscribeVector(string topic){
        _vectorValue = {};
        
        // Subscriber
        gazebo::transport::SubscriberPtr subVector = _node->Subscribe(topic, &Interface::callback, this);
        // Starting delay
        gazebo::common::Time::MSleep(10);

        while (_vectorValue.empty()){
          gazebo::common::Time::MSleep(1);
        }

        subVector->Unsubscribe();

        return _vectorValue;
      }

      void callback(ConstGzString_VPtr &_msg)
      {
        vector<string> tempVector = {};
        for (int i = 0; i < _msg->data_size(); i++) 
          tempVector.push_back(_msg->data(i));
        _vectorValue = tempVector;
      }

      void poseCallback(ConstPosesStampedPtr &posesStamped)
      {
        vector<string> tempVector = {};

        for (int i =0; i < posesStamped->pose_size(); ++i)
        {
          const ::gazebo::msgs::Pose &pose = posesStamped->pose(i);
          std::string name = pose.name();
          if (name == _name)
          {
            tempVector.push_back(boost::lexical_cast<std::string>(pose.position().x()));
            tempVector.push_back(boost::lexical_cast<std::string>(pose.position().y()));
            tempVector.push_back(boost::lexical_cast<std::string>(pose.position().z()));
            tempVector.push_back(boost::lexical_cast<std::string>(pose.orientation().x()));
            tempVector.push_back(boost::lexical_cast<std::string>(pose.orientation().y()));
            tempVector.push_back(boost::lexical_cast<std::string>(pose.orientation().z()));
            _vectorValue = tempVector;
          }
        }
      }

      vector<string> getJointsVel()
      {
        return subscribeVector(_jointVelSub);
      }

      vector<string> getJointsPos()
      {
        return subscribeVector(_jointPosSub);
      }

      vector<string> getModelPose()
      {
        _vectorValue = {};
        
        // Subscriber
        gazebo::transport::SubscriberPtr subVector = _node->Subscribe("~/pose/info", &Interface::poseCallback, this);
        // Starting delay
        gazebo::common::Time::MSleep(15);

        while (_vectorValue.empty()){
          gazebo::common::Time::MSleep(1);
        }

        subVector->Unsubscribe();

        return _vectorValue;
      }

    };

    PYBIND11_MODULE(publisher, m) {
      py::class_<Interface>(m, "Interface")
        .def(py::init<const string &>())
        .def("setJointsVel", &Interface::setJointsVel)
        .def("getJointsVel", &Interface::getJointsVel)
        .def("getJointsPos", &Interface::getJointsPos)
        .def("getModelPose", &Interface::getModelPose);
    }