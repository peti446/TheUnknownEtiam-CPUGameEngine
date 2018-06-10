#include "SoundManager.h"

namespace SOUND
{
	SoundManager::SoundManager()
	{
	}

	SoundManager::~SoundManager()
	{
	}

	const std::string& SoundManager::LoadSound(const std::string& path)
	{
		HAPI.LoadSound(path);
		return path;
	}

	void SoundManager::PlaySound(const std::string& InstanceName, const std::string& sound, HAPI_TSoundOptions options)
	{
		int i = 0;
		HAPI.PlaySound(sound, options, i);
		if (this->m_PlayingSounds.find(InstanceName) != this->m_PlayingSounds.end())
		{
			HAPI_MediaStatus status = this->getPlayingSoundStatus(InstanceName);
			if (status != HAPI_MediaStatus::eError && status != HAPI_MediaStatus::eStopped)
			{
				std::cerr << "Sond with the instance name " << InstanceName << " is arleady running. Pls stop it first to use the name again" << std::endl;
				return;
			}
		}
		this->m_PlayingSounds[InstanceName] = std::make_pair(i, options);
	}

	HAPI_TSoundOptions SoundManager::getPlayingSoundOptions(const std::string & InstanceName)
	{
		HAPI_TSoundOptions options = HAPI_TSoundOptions();
		if (this->m_PlayingSounds.find(InstanceName) != this->m_PlayingSounds.end())
		{
			options = this->m_PlayingSounds.at(InstanceName).second;
		}
		else
		{
			std::cerr << "Sound with instance name: " << InstanceName << " has not been found. Returning default options." << std::endl;
		}
		return options;
	}

	HAPI_MediaStatus SoundManager::getPlayingSoundStatus(const std::string & InstanceName)
	{
		if (this->m_PlayingSounds.find(InstanceName) != this->m_PlayingSounds.end())
		{
			float toDelete = 0;
			return HAPI.GetSoundStatus(this->m_PlayingSounds.at(InstanceName).first, toDelete);
		}
		std::cerr << "Sound with instance name: " << InstanceName << " has not been found." << std::endl;
		return HAPI_MediaStatus::eError;
	}

	void SoundManager::setSoundOptions(const std::string & InstanceName, HAPI_TSoundOptions options)
	{
		if (this->m_PlayingSounds.find(InstanceName) != this->m_PlayingSounds.end())
		{
			HAPI.ChangePlayingSoundOptions(this->m_PlayingSounds.at(InstanceName).first, options);
			return;
		}
		std::cerr << "Sound with instance name: " << InstanceName << " has not been found." << std::endl;
	}

	void SoundManager::stopSound(const std::string & InstanceName)
	{
		if (this->m_PlayingSounds.find(InstanceName) != this->m_PlayingSounds.end())
		{
			if (this->getPlayingSoundStatus(InstanceName) == HAPI_MediaStatus::ePlaying)
			{
				HAPI.StopSound(this->m_PlayingSounds.at(InstanceName).first);
			}
			return;
		}
		std::cerr << "Sound with instance name: " << InstanceName << " has not been found." << std::endl;
	}

	void SoundManager::pauseSound(const std::string & InstanceName)
	{
		if (this->m_PlayingSounds.find(InstanceName) != this->m_PlayingSounds.end())
		{
			if (this->getPlayingSoundStatus(InstanceName) == HAPI_MediaStatus::ePlaying)
			{
				HAPI.StopSound(this->m_PlayingSounds.at(InstanceName).first, true);
			}
			return;
		}
		std::cerr << "Sound with instance name: " << InstanceName << " has not been found." << std::endl;
	}

	void SoundManager::resumeSound(const std::string & InstanceName)
	{
		if (this->m_PlayingSounds.find(InstanceName) != this->m_PlayingSounds.end())
		{
			if (this->getPlayingSoundStatus(InstanceName) == HAPI_MediaStatus::ePaused)
			{
				HAPI.StopSound(this->m_PlayingSounds.at(InstanceName).first, true);
			}
			return;
		}
		std::cerr << "Sound with instance name: " << InstanceName << " has not been found." << std::endl;
	}

	void SoundManager::StopAllSounds()
	{
		for (std::unordered_map<std::string, std::pair<int, HAPI_TSoundOptions>>::iterator i = this->m_PlayingSounds.begin(); i != this->m_PlayingSounds.end(); i++)
		{
			this->stopSound(i->first);
		}
	}

};
