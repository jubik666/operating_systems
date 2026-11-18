#include "../include/lib.h"
#include <math.h>

float cos_derivative(float a, float dx) {
  float f = cosf(a + dx) - cosf(a - dx);
  float d = 2 * dx;
  return f / d;
}

int gcd(int a, int b) {
  int res = 1;
  for (int i = 1; i <= a && i <= b; ++i) {
    if (a % i == 0 && b % i == 0) {
      res = i;
    }
  }
  return res;
}