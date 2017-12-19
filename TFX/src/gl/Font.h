#ifndef TFX_FONT_H
#define TFX_FONT_H

#include "GPUTexture.h"
#include "../core/Util.h"
#include "../core/Option.h"

namespace tfx {

	typedef struct Glyph {
		char character;
		i32 ox, oy;
		float bounds[4];
	} Glyph;

	class Font {
	public:
		Font() : m_atlas(nullptr) {}
		~Font();

		Font(const String& fileName);

		GPUTexture* getAtlas() { return m_atlas; }
		Vec<Glyph> getGlyphs() const { return m_glyphs; }

		Option<Glyph> getGlyph(char c) const;

		i32	getSpaceSize() const { return m_spaceSize; }

	private:
		GPUTexture* m_atlas;
		Vec<Glyph> m_glyphs;
		i32 m_spaceSize;
	};
}

#endif // TFX_FONT_H