#ifndef _ROS_manager_msgs_Plan_h
#define _ROS_manager_msgs_Plan_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "std_msgs/Header.h"
#include "manager_msgs/Pose.h"
#include "manager_msgs/Type.h"

namespace manager_msgs
{

  class Plan : public ros::Msg
  {
    public:
      typedef std_msgs::Header _header_type;
      _header_type header;
      typedef int64_t _id_type;
      _id_type id;
      typedef manager_msgs::Pose _pose_type;
      _pose_type pose;
      typedef manager_msgs::Type _type_type;
      _type_type type;
      typedef int64_t _action_id_type;
      _action_id_type action_id;
      typedef int64_t _needle_capacity_type;
      _needle_capacity_type needle_capacity;
      typedef int64_t _alignment_offset_type;
      _alignment_offset_type alignment_offset;

    Plan():
      header(),
      id(0),
      pose(),
      type(),
      action_id(0),
      needle_capacity(0),
      alignment_offset(0)
    {
    }

    virtual int serialize(unsigned char *outbuffer) const override
    {
      int offset = 0;
      offset += this->header.serialize(outbuffer + offset);
      union {
        int64_t real;
        uint64_t base;
      } u_id;
      u_id.real = this->id;
      *(outbuffer + offset + 0) = (u_id.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_id.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_id.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_id.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_id.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_id.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_id.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_id.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->id);
      offset += this->pose.serialize(outbuffer + offset);
      offset += this->type.serialize(outbuffer + offset);
      union {
        int64_t real;
        uint64_t base;
      } u_action_id;
      u_action_id.real = this->action_id;
      *(outbuffer + offset + 0) = (u_action_id.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_action_id.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_action_id.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_action_id.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_action_id.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_action_id.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_action_id.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_action_id.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->action_id);
      union {
        int64_t real;
        uint64_t base;
      } u_needle_capacity;
      u_needle_capacity.real = this->needle_capacity;
      *(outbuffer + offset + 0) = (u_needle_capacity.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_needle_capacity.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_needle_capacity.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_needle_capacity.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_needle_capacity.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_needle_capacity.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_needle_capacity.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_needle_capacity.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->needle_capacity);
      union {
        int64_t real;
        uint64_t base;
      } u_alignment_offset;
      u_alignment_offset.real = this->alignment_offset;
      *(outbuffer + offset + 0) = (u_alignment_offset.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_alignment_offset.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_alignment_offset.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_alignment_offset.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_alignment_offset.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_alignment_offset.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_alignment_offset.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_alignment_offset.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->alignment_offset);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer) override
    {
      int offset = 0;
      offset += this->header.deserialize(inbuffer + offset);
      union {
        int64_t real;
        uint64_t base;
      } u_id;
      u_id.base = 0;
      u_id.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_id.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_id.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_id.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_id.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_id.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_id.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_id.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->id = u_id.real;
      offset += sizeof(this->id);
      offset += this->pose.deserialize(inbuffer + offset);
      offset += this->type.deserialize(inbuffer + offset);
      union {
        int64_t real;
        uint64_t base;
      } u_action_id;
      u_action_id.base = 0;
      u_action_id.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_action_id.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_action_id.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_action_id.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_action_id.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_action_id.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_action_id.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_action_id.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->action_id = u_action_id.real;
      offset += sizeof(this->action_id);
      union {
        int64_t real;
        uint64_t base;
      } u_needle_capacity;
      u_needle_capacity.base = 0;
      u_needle_capacity.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_needle_capacity.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_needle_capacity.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_needle_capacity.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_needle_capacity.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_needle_capacity.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_needle_capacity.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_needle_capacity.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->needle_capacity = u_needle_capacity.real;
      offset += sizeof(this->needle_capacity);
      union {
        int64_t real;
        uint64_t base;
      } u_alignment_offset;
      u_alignment_offset.base = 0;
      u_alignment_offset.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_alignment_offset.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_alignment_offset.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_alignment_offset.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_alignment_offset.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_alignment_offset.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_alignment_offset.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_alignment_offset.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->alignment_offset = u_alignment_offset.real;
      offset += sizeof(this->alignment_offset);
     return offset;
    }

    virtual const char * getType() override { return "manager_msgs/Plan"; };
    virtual const char * getMD5() override { return "0aa697337325b79a4a38dd5b6c083116"; };

  };

}
#endif
