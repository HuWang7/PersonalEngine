#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

SpriteSheet::SpriteSheet(Texture& texture, IntVec2 const& simpleGridLayout)
	:m_texture(texture)
{
	Vec2 spriteSize(1.0f / simpleGridLayout.x, 1.0f / simpleGridLayout.y);
	Vec2 nudgeAmount(1.0f / 262144.0f, 1.0f / 65536.0f); // Adjustments for U and V values

	for (int y = 0; y < simpleGridLayout.y; ++y) {
		for (int x = 0; x < simpleGridLayout.x; ++x) {
			Vec2 uvAtMins(static_cast<float>(x) * spriteSize.x, 1.0f - (static_cast<float>(y + 1) * spriteSize.y));
			Vec2 uvAtMaxs(uvAtMins.x + spriteSize.x, 1.0f - static_cast<float>(y) * spriteSize.y);

			// Apply nudge to UV coordinates
			uvAtMins += nudgeAmount;
			uvAtMaxs -= nudgeAmount;

			m_spriteDefs.push_back(SpriteDefinition(*this, (int)m_spriteDefs.size(), uvAtMins, uvAtMaxs));
		}
	}
}

Texture& SpriteSheet::GetTexture() const
{
	return m_texture;
}

int SpriteSheet::GetNumSprites() const
{
	return static_cast<int>(m_spriteDefs.size());
}

SpriteDefinition const& SpriteSheet::GetSpriteDef(int spriteIndex) const
{
	return m_spriteDefs[spriteIndex];

}

void SpriteSheet::GetSpriteUVs(Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, int spriteIndex) const
{
	SpriteDefinition const& def = GetSpriteDef(spriteIndex);
	def.GetUVs(out_uvAtMins, out_uvAtMaxs);
}

AABB2 SpriteSheet::GetSpriteUVs(int spriteIndex) const
{
	Vec2 uvMins, uvMaxs;
	GetSpriteUVs(uvMins, uvMaxs, spriteIndex);
	return AABB2(uvMins, uvMaxs);
}

