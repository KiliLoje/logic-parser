#ifndef ENCODER_H
#define ENCODER_H

#include "achievement.h"
#include <stddef.h>

char* encode_logic(struct ACHIEVEMENT_LOGIC *logic);
int encode_group(char *buffer, size_t max_size, struct GROUP *group);
int encode_condition(char *buffer, size_t max_size, struct CONDITION *cond);
int encode_numeral(char *buffer, size_t max_size, struct NUMERAL *num);

#endif // ENCODER_H