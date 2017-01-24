#ifndef UTILS_H
#define UTILS_H

#define rand_under(numerator, denominator) \
  (rand() < ((RAND_MAX / denominator) * numerator))
#define rand_range(min, max) ((rand() % (((max) + 1) - (min))) + (min))

int makedirectory(char *dir);

#endif
