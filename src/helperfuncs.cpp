#include "Header.h"

#define PI 3.14159265

double f(double t)
{
    return 2 * sin(2 * PI * t) + 3 * sin(2 * PI * 4 * t) + sin(2 * PI * 16 * t);
}