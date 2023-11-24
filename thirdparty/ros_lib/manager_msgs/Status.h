#ifndef _ROS_manager_msgs_Status_h
#define _ROS_manager_msgs_Status_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "std_msgs/Header.h"

namespace manager_msgs
{

  class Status : public ros::Msg
  {
    public:
      typedef std_msgs::Header _header_type;
      _header_type header;
      typedef uint8_t _status_type;
      _status_type status;
      enum { STOP =  0 };
      enum { START =  1 };
      enum { CANCEL =  2 };
      enum { CONTINUE =  3 };
      enum { DELETEALL =  4 };
      enum { MAPSAVE =  5 };

    Status():
      header(),
      status(0)
    {
    }

    virtual int serialize(unsigned char *outbuffer) const override
    {
      int offset = 0;
      offset += this->header.serialize(outbuffer + offset);
      *(outbuffer + offset + 0) = (this->status >> (8 * 0)) & 0xFF;
      offset += sizeof(this->status);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer) override
    {
      int offset = 0;
      offset += this->header.deserialize(inbuffer + offset);
      this->status =  ((uint8_t) (*(inbuffer + offset)));
      offset += sizeof(this->status);
     return offset;
    }

    virtual const char * getType() override { return "manager_msgs/Status"; };
    virtual const char * getMD5() override { return "593a64654818a9bdd4d99c681cbdf720"; };

  };

}
#endif
