#include "Engine/Renderer/SpriteAnimDefinition.hpp"



SpriteAnimDefinition::SpriteAnimDefinition(SpriteSheet& sheet, int startSpriteIndex, int endSpriteIndex, float durationSeconds, SpriteAnimPlaybackType playbackType /*= SpriteAnimPlaybackType::LOOP*/)
	:m_spriteSheet(sheet)
	,m_startSpriteIndex(startSpriteIndex)
	,m_endSpriteIndex(endSpriteIndex)
	,m_durationSeconds(durationSeconds)
	,m_playbackType(playbackType)
{

}

const SpriteDefinition& SpriteAnimDefinition::GetSpriteDefAtTime(float seconds) const
{
	int totalFrames = m_endSpriteIndex - m_startSpriteIndex + 1;
	float frameDuration = m_durationSeconds / totalFrames;
	int frameIndex = 0;

	switch (m_playbackType) {
	case SpriteAnimPlaybackType::ONCE:
		frameIndex = std::min(static_cast<int>(seconds / frameDuration), totalFrames - 1);
		break;
	case SpriteAnimPlaybackType::LOOP:
		frameIndex = static_cast<int>(seconds / frameDuration) % totalFrames;
		break;
	case SpriteAnimPlaybackType::PINGPONG:
	{
		int pingPongFrame = static_cast<int>(seconds / frameDuration) % (totalFrames * 2 - 2);
		frameIndex = pingPongFrame < totalFrames ? pingPongFrame : totalFrames * 2 - 2 - pingPongFrame;
	}
	break;
	}

	return m_spriteSheet.GetSpriteDef(m_startSpriteIndex + frameIndex);
}

float SpriteAnimDefinition::GetSecondsPerFrame() const
{
	return m_durationSeconds;
}

int SpriteAnimDefinition::GetNumOfFrameHaveToPlay() const
{
	return (m_endSpriteIndex - m_startSpriteIndex) + 1;
}

