#pragma once

#include "../Math/Vector3.h"
#include "../../../ContestAPI/AppSettings.h"
#include <cmath>
#include <cstdint>

// Based on Godots CharacterBody https://docs.godotengine.org/en/stable/classes/class_characterbody3d.html
// A Character body is a physics body thats not affected by physics at all.
// Other physics bodies (RigidBody) are affected by this body.
// Going to try to make this a high level API with at least Wall detection and Hopefully Slope detection
struct CharacterBodyComponent
{
    Vector3 velocity = Vector3(0, 0, 0);
    Vector3 upDirection = Vector3(0.f, -1.f, 0.f);

    // number of times it will "slide" each frame
    // slide = move to wall, if still more movement, just slide along wall (or any surface)
    uint8_t maxSlides = 6;
    float safeMargin = 0.001f;

    // Detection properties
    // This is the dot product of a vector and the UP vector (basically the angle but calculations are faster)
    float floorThreshold = 0.7f; // ~ 45 deg
    float floorSnapDistance = 0.2f;

    // States 
    bool isOnFloor = false;
    bool isOnCeiling = false;
    bool isOnWall = false;
    bool justJumped = false;

    CharacterBodyComponent() = default;
    CharacterBodyComponent(const Vector3& up_direction, const uint8_t max_slides, const float safe_margin, const float floor_angle_deg, const float floor_snap_distance)
        : maxSlides(max_slides),
        safeMargin(safe_margin),
        floorSnapDistance(floor_snap_distance)
    {
        upDirection = up_direction.GetNormal();
        floorThreshold = std::cosf(floor_angle_deg * PI/ 180.f);
    }
};
