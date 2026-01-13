#ifndef DATAORIENTEDDESIGNINGAMEDEV_UTILS_H
#define DATAORIENTEDDESIGNINGAMEDEV_UTILS_H

#include <random>

inline float randFloat(float a, const float b) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(a, b);
    return dist(rng);
}

#endif