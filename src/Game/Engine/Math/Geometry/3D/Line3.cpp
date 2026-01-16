#include "Line3.h"

float Line3::GetLength() const
{
    return (start - end).GetMagnitude();
}

// This is faster ;)
float Line3::GetLengthSquared() const
{
    return (start - end).GetMagnitudeSquared();
}
