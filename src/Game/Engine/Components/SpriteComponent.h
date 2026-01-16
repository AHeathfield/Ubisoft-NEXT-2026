#pragma once

#include "../../../ContestAPI/app.h"
#include "../Core/Utility.h"
#include <unordered_map>

// API provides a sprite which contains:
// texture, animation, and an animation player for sprites
// It has a method called Draw() which is needed to draw the sprite
// Update(dt) is purely meant for animations, the sprite can also store multiple animations 
// They have efficient texture handling already so no need to do it myself (1 texture for many sprites)

// Basically this will integrate it into the ECS and
// Im going to add custom animation play options since currently it's a little confusing
struct SpriteComponent
{
    CSimpleSprite* sprite = nullptr;
    int renderLayer = 0;
    float animationSpeed = 1.f; // Will be multiplied to the deltaTime in Update() method so animations can vary in speed
    bool isHidden = false;
    bool isPlaying = false;

    // So I can link animations to strings instead (like godot)
    std::unordered_map<std::string, unsigned int> animations;

    SpriteComponent() = default;
    SpriteComponent(const std::string& file_path, const int columns, const int rows, const int render_layer = 0, bool hidden = false)
	: renderLayer(render_layer),
	isHidden(hidden)
    {
	sprite = App::CreateSprite(file_path.c_str(), columns, rows);
	if (sprite == nullptr)
	{
	    utility::log("ERROR: Cannot load sprite: " + file_path);
	}
    }

    // This will internally create 2 animations
    // Normal animation and a reversed animation
    void CreateAnimation(const std::string& name, const std::vector<int>& frames)
    {
	if (frames.size() < 1)
	{
	    utility::log("ERROR: The animation " + name + " cannot be less than 1 frame.");
	    return;
	}

	// Normal animation
	animations.insert({name, mId});
	sprite->CreateAnimation(mId, 1.f, frames);
	mId++;

	// Reversed animation
	animations.insert({name + "_REV", mId});
	std::vector<int> reversed = std::vector<int>(frames.rbegin(), frames.rend());
	sprite->CreateAnimation(mId, 1.f, reversed);
    }

    
    void Play(const std::string& animation_name, const float speed = 1.f, const bool playFromBeginning = true)
    {
	if (utility::hasKey(animations, animation_name))
	{
	    animationSpeed = speed;
	    sprite->SetAnimation(animations[animation_name], playFromBeginning);
	    isPlaying = true;
	}
	else
	{
	    utility::log("ERROR: Cannot play the animation " + animation_name + ", since it does not exist.");
	}
    }

    void PlayReverse(const std::string& animation_name, const float speed = 1.f, const bool playFromBeginning = true)
    {
	Play(animation_name + "_REV", speed, playFromBeginning);
    }

    void Stop()
    {
	isPlaying = false;
    }


private:
    unsigned int mId = 0;
};
