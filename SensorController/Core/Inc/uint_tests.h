/**
 * @file uint_tests.h
 * @author Matthew Cockburn
 * @brief 
 * @version 1
 * @date 2023-09-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "build.h"

#ifdef UNIT_TESTING

#define ROS_IF_TEST
#define IMU_TEST
#define PRESSURE_TEST

#define NUM_TESTS 3

void TEST_Task(void);

#ifdef ROS_IF_TEST

#endif

#ifdef IMU_TEST

#endif

#ifdef PRESSURE

#endif

#endif