#pragma once
#include <unordered_map>
#include <string>
#include <HAPI_lib.h>

using namespace HAPISPACE;

namespace SOUND
{

	class SoundManager
	{
	public:
		SoundManager();
		~SoundManager();

		const std::string& LoadSound(const std::string& path);
		void PlaySound(const std::string& InstanceName, const std::string& sound, HAPI_TSoundOptions options = HAPI_TSoundOptions());

		HAPI_TSoundOptions getPlayingSoundOptions(const std::string& InstanceName);
		HAPI_MediaStatus getPlayingSoundStatus(const std::string& InstanceName);
		void setSoundOptions(const std::string& InstanceName, HAPI_TSoundOptions options);
		void stopSound(const std::string& InstanceName);
		void pauseSound(const std::string& InstanceName);
		void resumeSound(const std::string& InstanceName);
		void StopAllSounds();

	private:
		std::unordered_map<std::string, std::pair<int, HAPI_TSoundOptions>> m_PlayingSounds;
	};
};
