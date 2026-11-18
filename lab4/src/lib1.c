#include "../include/lib.h"
#include <math.h>

float cos_derivative(float a, float dx) {
  float f = cosf(a + dx) - cosf(a);
  return f / dx;
}

int gcd(int a, int b) {
  while (b != 0) { 
    int c = a % b;
    a = b;
    b = c;
  }
  return a;
}