#ifndef LASERHARP_h
#define LASERHARP_h

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <stdio.h>
//#include <jni.h>

#define PI 3.14159265


uint8_t findPoints();
std::string playNote(double, double);


#endif