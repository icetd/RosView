#ifndef _ROS_mbf_msgs_GetPathGoal_h
#define _ROS_mbf_msgs_GetPathGoal_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "geometry_msgs/PoseStamped.h"

namespace mbf_msgs
{

  class GetPathGoal : public ros::Msg
  {
    public:
      typedef bool _use_start_pose_type;
      _use_start_pose_type use_start_pose;
      typedef geometry_msgs::PoseStamped _start_pose_type;
      _start_pose_type start_pose;
      typedef geometry_msgs::PoseStamped _target_pose_type;
      _target_pose_type target_pose;
      typedef double _tolerance_type;
      _tolerance_type tolerance;
      typedef const char* _planner_type;
      _planner_type planner;
      typedef uint8_t _concurrency_slot_type;
      _concurrency_slot_type concurrency_slot;

    GetPathGoal():
      use_start_pose(0),
      start_pose(),
      target_pose(),
      tolerance(0),
      planner(""),
      concurrency_slot(0)
    {
    }

    virtual int serialize(unsigned char *outbuffer) const override
    {
      int offset = 0;
      union {
        bool real;
        uint8_t base;
      } u_use_start_pose;
      u_use_start_pose.real = this->use_start_pose;
      *(outbuffer + offset + 0) = (u_use_start_pose.base >> (8 * 0)) & 0xFF;
      offset += sizeof(this->use_start_pose);
      offset += this->start_pose.serialize(outbuffer + offset);
      offset += this->target_pose.serialize(outbuffer + offset);
      union {
        double real;
        uint64_t base;
      } u_tolerance;
      u_tolerance.real = this->tolerance;
      *(outbuffer + offset + 0) = (u_tolerance.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_tolerance.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_tolerance.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_tolerance.base >> (8 * 3)) & 0xFF;
      *(outbuffer + offset + 4) = (u_tolerance.base >> (8 * 4)) & 0xFF;
      *(outbuffer + offset + 5) = (u_tolerance.base >> (8 * 5)) & 0xFF;
      *(outbuffer + offset + 6) = (u_tolerance.base >> (8 * 6)) & 0xFF;
      *(outbuffer + offset + 7) = (u_tolerance.base >> (8 * 7)) & 0xFF;
      offset += sizeof(this->tolerance);
      uint32_t length_planner = strlen(this->planner);
      varToArr(outbuffer + offset, length_planner);
      offset += 4;
      memcpy(outbuffer + offset, this->planner, length_planner);
      offset += length_planner;
      *(outbuffer + offset + 0) = (this->concurrency_slot >> (8 * 0)) & 0xFF;
      offset += sizeof(this->concurrency_slot);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer) override
    {
      int offset = 0;
      union {
        bool real;
        uint8_t base;
      } u_use_start_pose;
      u_use_start_pose.base = 0;
      u_use_start_pose.base |= ((uint8_t) (*(inbuffer + offset + 0))) << (8 * 0);
      this->use_start_pose = u_use_start_pose.real;
      offset += sizeof(this->use_start_pose);
      offset += this->start_pose.deserialize(inbuffer + offset);
      offset += this->target_pose.deserialize(inbuffer + offset);
      union {
        double real;
        uint64_t base;
      } u_tolerance;
      u_tolerance.base = 0;
      u_tolerance.base |= ((uint64_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_tolerance.base |= ((uint64_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_tolerance.base |= ((uint64_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_tolerance.base |= ((uint64_t) (*(inbuffer + offset + 3))) << (8 * 3);
      u_tolerance.base |= ((uint64_t) (*(inbuffer + offset + 4))) << (8 * 4);
      u_tolerance.base |= ((uint64_t) (*(inbuffer + offset + 5))) << (8 * 5);
      u_tolerance.base |= ((uint64_t) (*(inbuffer + offset + 6))) << (8 * 6);
      u_tolerance.base |= ((uint64_t) (*(inbuffer + offset + 7))) << (8 * 7);
      this->tolerance = u_tolerance.real;
      offset += sizeof(this->tolerance);
      uint32_t length_planner;
      arrToVar(length_planner, (inbuffer + offset));
      offset += 4;
      for(unsigned int k= offset; k< offset+length_planner; ++k){
          inbuffer[k-1]=inbuffer[k];
      }
      inbuffer[offset+length_planner-1]=0;
      this->planner = (char *)(inbuffer + offset-1);
      offset += length_planner;
      this->concurrency_slot =  ((uint8_t) (*(inbuffer + offset)));
      offset += sizeof(this->concurrency_slot);
     return offset;
    }

    virtual const char * getType() override { return "mbf_msgs/GetPathGoal"; };
    virtual const char * getMD5() override { return "301d9f5ec2f8f08d1d4e16663a6d2c5a"; };

  };

}
#endif
