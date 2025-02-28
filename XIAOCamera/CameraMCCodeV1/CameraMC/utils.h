#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <Base64.h>

String encodeImageToBase64(const uint8_t* imageData, size_t imageSize);

#endif //UTILS_H