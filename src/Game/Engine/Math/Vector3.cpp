#include "Vector3.h"

// Apparently for math libraries like Vector3 it's faster to just have the code defined in a header file
// It does take longer to recompile Vector3 though if I do change it and it is included in many files
// This file will only define some of the constants
Vector3 Vector3::ZERO = Vector3();
Vector3 Vector3::ONE = Vector3(1, 1, 1);
Vector3 Vector3::UP = Vector3(0, 1, 0);
Vector3 Vector3::DOWN = Vector3(0, -1, 0);
Vector3 Vector3::RIGHT = Vector3(1, 0, 0);
Vector3 Vector3::LEFT = Vector3(-1, 0, 0);
Vector3 Vector3::FORWARD = Vector3(0, 0, 1);
Vector3 Vector3::BACKWARD = Vector3(0, 0, -1);
