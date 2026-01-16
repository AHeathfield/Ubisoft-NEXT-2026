#pragma once

#include "../../../ContestAPI/app.h"
#include "../Core/Utility.h"
#include <unordered_map>

// I like how the API handles audio, so this is just to connect Audio to an entity
// There will be no AudioSystem since the API does it already
// Formatted to be similar usage to the SpriteComponent
class AudioComponent
{
    // Key = Name you give for audio
    // Value = Filepath
    std::unordered_map<std::string, std::string> mAudios;

public:
    // Add audio
    void AddAudio(const std::string& name, const std::string& file_path)
    {
	mAudios[name] = file_path;
    }

    void RemoveAudio(const std::string& name)
    {
	mAudios.erase(name);
    }

    // Play the audio, wont restart if already playing
    void Play(const std::string& name, const bool isLooping = false)
    {
	if (IsAudioPlaying(name) == false)
	{
	    if (utility::hasKey(mAudios, name))
	    {
		App::PlayAudio(mAudios[name].c_str(), isLooping);
	    }
	    else
	    {
		utility::log("The audio: " + name + ", does not exist make sure to add it with AddAudio().");
	    }
	}
    }

    void Restart(const std::string& name, const bool isLooping = false)
    {
	if (utility::hasKey(mAudios, name))
	{
	    App::PlayAudio(mAudios[name].c_str(), isLooping);
	}
	else
	{
	    utility::log("The audio: " + name + ", does not exist make sure to add it with AddAudio().");
	}
    }

    void Stop(const std::string& name)
    {
	if (utility::hasKey(mAudios, name))
	{
	    App::StopAudio(mAudios[name].c_str());
	}
	else
	{
	    utility::log("The audio: " + name + ", does not exist make sure to add it with AddAudio().");
	}
    }

    bool IsAudioPlaying(const std::string& name)
    {
	if (utility::hasKey(mAudios, name))
	{
	    return App::IsSoundPlaying(mAudios[name].c_str());
	}
	else
	{
	    utility::log("The audio: " + name + ", does not exist make sure to add it with AddAudio().");
	    return false;
	}
    }
};
