#ifndef MINIMAL_CS_H
#define MINIMAL_CS_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <ctype.h>

using namespace cv;
using namespace std;

typedef enum {WAITING, SELECTED, INITIALIZED} SelectionStatus;

void *camshift(void* coordinates);

#endif
