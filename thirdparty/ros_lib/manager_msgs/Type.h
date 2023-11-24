#ifndef _ROS_manager_msgs_Type_h
#define _ROS_manager_msgs_Type_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"

namespace manager_msgs
{

  class Type : public ros::Msg
  {
    public:
      typedef uint8_t _status_type;
      _status_type status;
      enum { STOP =  0 };
      enum { PAUSE =  1 };
      enum { MOVE =  2 };
      enum { ACTION =  3 };

    Type():
      status(0)
    {
    }

    virtual int serialize(unsigned char *outbuffer) const override
    {
      int offset = 0;
      *(outbuffer + offset + 0) = (this->status >> (8 * 0)) & 0xFF;
      offset += sizeof(this->status);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer) override
    {
      int offset = 0;
      this->status =  ((uint8_t) (*(inbuffer + offset)));
      offset += sizeof(this->status);
     return offset;
    }

    virtual const char * getType() override { return "manager_msgs/Type"; };
    virtual const char * getMD5() override { return "41aa323be3c944320ef6a43a04b1d136"; };

  };

}
#endif
