#ifndef SRC_HW_IMP_HPP_
#define SRC_HW_IMP_HPP_

#include "xaxidma.h"
#include "xtime_l.h"

#include "utility.hpp"

void hw_load_weights(u16* weights);
void hw_rand_weights();

void hw_predict(u8* features, u8* results, u16 nb_ex);
float hw_train(u8* features, u8* labels, u16 nb_ex, u16 nb_iter);


#endif
