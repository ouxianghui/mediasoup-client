#include "participant.h"
#include "api/media_stream_interface.h"

namespace vi {

	Participant::Participant(const std::string& id, const std::string& displayName)
		: _id(id)
		, _displayName(displayName)
	{

	}

	std::string Participant::id()
	{
		return _id;
	}

	std::string Participant::displayName()
	{
		return _displayName;
	}

	bool Participant::isActive()
	{
		return _isActive;
	}

    bool Participant::hasAudio()
	{
        if (_audioTracks.empty()) {
            return false;
        }
        return true;
	}

	bool Participant::isAudioMuted()
	{
		return _isAudioMuted;
	}

    bool Participant::hasVideo()
	{
        if (_videoTracks.empty()) {
            return false;
        }
        return true;
	}

	bool Participant::isVideoMuted() {
		return _isVideoMuted;
	}

    int32_t Participant::speakingVolume()
    {
        return _volume;
    }

	std::unordered_map<std::string, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>> Participant::getVideoTracks()
	{
		return _videoTracks;
	}

	void Participant::setId(const std::string& id)
	{
		_id = id;
	}

	void Participant::setDisplayName(const std::string& name)
	{
		_displayName = name;
	}

	void Participant::setActive(bool isActive)
	{
		_isActive = isActive;
	}

	void Participant::setAudioMuted(bool muted)
	{
		_isAudioMuted = muted;
	}

	void Participant::setVideoMuted(bool muted)
	{
		_isVideoMuted = muted;
	}

    void Participant::setSpeakingVolume(int32_t volume)
    {
        _volume = volume;
    }

    void Participant::setAudioTracks(const std::unordered_map<std::string, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>>& tracks)
    {
        _audioTracks = tracks;
    }

	void Participant::setVideoTracks(const std::unordered_map<std::string, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>>& tracks)
	{
		_videoTracks = tracks;
	}
}
