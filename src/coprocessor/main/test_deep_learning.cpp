#include <stdio.h>
#include <human_face_detect_mnp01.hpp>
#include <human_face_detect_msr01.hpp>

extern "C" bool test_deep_learning(uint8_t **pixels) {

    HumanFaceDetectMSR01 s1(0.1F, 0.5F, 10, 0.2F);
    HumanFaceDetectMNP01 s2(0.5F, 0.3F, 5);

    std::list<dl::detect::result_t> &candidates
        = s1.infer((uint8_t *)(*pixels), {320, 240, 3});
    std::list<dl::detect::result_t> &results
        = s2.infer((uint8_t *)(*pixels), {320, 240, 3}, candidates);

    int i = 0;
    for (std::list<dl::detect::result_t>::iterator prediction = results.begin(); prediction != results.end(); prediction++, i++)
    {
        printf("[%d] score: %f\n", i, prediction->score);
        if(prediction->score >= 0.9){
            return true;
        }
    }
    return false;
}