#ifndef _ROS_SERVICE_QueryHardwareVersion_h
#define _ROS_SERVICE_QueryHardwareVersion_h
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"

namespace ltme_node
{

static const char QUERYHARDWAREVERSION[] = "ltme_node/QueryHardwareVersion";

  class QueryHardwareVersionRequest : public ros::Msg
  {
    public:

    QueryHardwareVersionRequest()
    {
    }

    virtual int serialize(unsigned char *outbuffer) const override
    {
      int offset = 0;
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer) override
    {
      int offset = 0;
     return offset;
    }

    virtual const char * getType() override { return QUERYHARDWAREVERSION; };
    virtual const char * getMD5() override { return "d41d8cd98f00b204e9800998ecf8427e"; };

  };

  class QueryHardwareVersionResponse : public ros::Msg
  {
    public:
      typedef const char* _hardware_version_type;
      _hardware_version_type hardware_version;

    QueryHardwareVersionResponse():
      hardware_version("")
    {
    }

    virtual int serialize(unsigned char *outbuffer) const override
    {
      int offset = 0;
      uint32_t length_hardware_version = strlen(this->hardware_version);
      varToArr(outbuffer + offset, length_hardware_version);
      offset += 4;
      memcpy(outbuffer + offset, this->hardware_version, length_hardware_version);
      offset += length_hardware_version;
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer) override
    {
      int offset = 0;
      uint32_t length_hardware_version;
      arrToVar(length_hardware_version, (inbuffer + offset));
      offset += 4;
      for(unsigned int k= offset; k< offset+length_hardware_version; ++k){
          inbuffer[k-1]=inbuffer[k];
      }
      inbuffer[offset+length_hardware_version-1]=0;
      this->hardware_version = (char *)(inbuffer + offset-1);
      offset += length_hardware_version;
     return offset;
    }

    virtual const char * getType() override { return QUERYHARDWAREVERSION; };
    virtual const char * getMD5() override { return "77f2d4a0768554de8b833ceceda9c80a"; };

  };

  class QueryHardwareVersion {
    public:
    typedef QueryHardwareVersionRequest Request;
    typedef QueryHardwareVersionResponse Response;
  };

}
#endif
