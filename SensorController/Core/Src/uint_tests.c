/**
 * @file uint_tests.c
 * @author Matthew Cockburn 
 * @brief 
 * @version 1
 * @date 2023-09-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifdef UNIT_TESTING

const bool *test_func(void)[]=
{
    {TEST_PRESSURE},
    {TEST_ROSIF},
    {TEST_IMU},

}

void TEST_Task(void){

    uint8_t results = 0;

    for(uint8_t i = 0; i < NUM_TESTS; ++i){
        if(test_func[i]()){
            results++;
        }
    }



}


#ifdef ROS_IF_TEST

#endif

#ifdef IMU_TEST

#endif

#ifdef PRESSURE

#endif

#endif