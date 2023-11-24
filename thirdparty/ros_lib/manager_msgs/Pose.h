#ifndef _ROS_manager_msgs_Pose_h
#define _ROS_manager_msgs_Pose_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "manager_msgs/Point.h"
#include "manager_msgs/Quaternion.h"

namespace manager_msgs
{

  class Pose : public ros::Msg
  {
    public:
      typedef manager_msgs::Point _position_type;
      _position_type position;
      typedef manager_msgs::Quaternion _orientation_type;
      _orientation_type orientation;

    Pose():
      position(),
      orientation()
    {
    }

    virtual int serialize(unsigned char *outbuffer) const override
    {
      int offset = 0;
      offset += this->position.serialize(outbuffer + offset);
      offset += this->orientation.serialize(outbuffer + offset);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer) override
    {
      int offset = 0;
      offset += this->position.deserialize(inbuffer + offset);
      offset += this->orientation.deserialize(inbuffer + offset);
     return offset;
    }

    virtual const char * getType() override { return "manager_msgs/Pose"; };
    virtual const char * getMD5() override { return "e45d45a5a1ce597b249e23fb30fc871f"; };

  };

}
#endif
