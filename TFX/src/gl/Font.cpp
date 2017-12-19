#include "Font.h"

#include "glad.h"

namespace tfx {
	Font::~Font() {
		if (m_atlas) delete m_atlas;
	}

	Font::Font(const String& fileName) {
		tpImage img = tpLoadPNG(fileName);
		size_t bpos = 0;
		// Read char data
		/*
		* The font info is as follows:
		*		HEADER
		*		=================================
		*		FONT_SIZE			|	int (4 bytes)
		*		MAP_WIDTH			|	int (4 bytes)
		*		MAP_HEIGHT			|	int (4 bytes)
		*		CHAR COUNT			|	short (2 bytes)
		*
		*		CHAR DATA (array)
		*		====================================
		*		CHAR CODE		|	1 byte
		*		ADVANCE			|	int (4 bytes)
		*		Y-OFFSET		|	int (4 bytes)
		*		BOUNDS X		|	int	(4 bytes)
		* 		BOUNDS Y		|	int	(4 bytes)
		* 		BOUNDS W		|	int	(4 bytes)
		* 		BOUNDS H		|	int	(4 bytes)
		*/

		int sz = img.w * img.h;

		// Extract GREEN channel (char data)
		byte* G = new byte[sz];
		for (int i = 0; i < sz; i++) {
			G[i] = img.pix[i].g;
		}

		// Skip font size
		bpos += 4;
		
		union {
			u8 b[4];
			i32 v;
		} mapWidth;
		mapWidth.b[3] = G[bpos++];
		mapWidth.b[2] = G[bpos++];
		mapWidth.b[1] = G[bpos++];
		mapWidth.b[0] = G[bpos++];

		union {
			u8 b[4];
			i32 v;
		} mapHeight;
		mapHeight.b[3] = G[bpos++];
		mapHeight.b[2] = G[bpos++];
		mapHeight.b[1] = G[bpos++];
		mapHeight.b[0] = G[bpos++];

		union {
			u8 b[4]; i32 v;
		} spaceSize;
		spaceSize.b[3] = G[bpos++];
		spaceSize.b[2] = G[bpos++];
		spaceSize.b[1] = G[bpos++];
		spaceSize.b[0] = G[bpos++];
		m_spaceSize = spaceSize.v;
		
		union {
			u8 b[2]; u16 v;
		} charCount;
		charCount.b[1] = G[bpos++];
		charCount.b[0] = G[bpos++];

		for (u16 i = 0; i < charCount.v; i++) {
			Glyph chr;
			chr.character = (char) G[bpos++];

			union {
				u8 b[4]; i32 v;
			} xo;
			xo.b[3] = G[bpos++];
			xo.b[2] = G[bpos++];
			xo.b[1] = G[bpos++];
			xo.b[0] = G[bpos++];
			chr.ox = xo.v;

			union {
				u8 b[4]; i32 v;
			} yo;
			yo.b[3] = G[bpos++];
			yo.b[2] = G[bpos++];
			yo.b[1] = G[bpos++];
			yo.b[0] = G[bpos++];
			chr.oy = yo.v;

			for (int j = 0; j < 4; j++) {
				union {
					u8 b[4]; i32 v;
				} bcomp;
				bcomp.b[3] = G[bpos++];
				bcomp.b[2] = G[bpos++];
				bcomp.b[1] = G[bpos++];
				bcomp.b[0] = G[bpos++];
				if (j % 2 == 0) {
					chr.bounds[j] = (float) bcomp.v / (float) mapWidth.v;
				} else {
					chr.bounds[j] = (float) bcomp.v / (float) mapHeight.v;
				}
			}
			m_glyphs.push_back(chr);
		}

		// Load the texture Data
		// Extract BLUE channel (font map)
		byte* B = new byte[img.w * img.h * 4];
		for (int y = 0; y < img.h; y++) {
			for (int x = 0; x < img.w; x++) {
				int k = (x + y * img.w);
				int i = k * 4;
				B[i + 0] = img.pix[k].b;
				B[i + 1] = img.pix[k].b;
				B[i + 2] = img.pix[k].b;
				B[i + 3] = img.pix[k].b;
			}
		}

		m_atlas = new GPUTexture(
			mapWidth.v, mapHeight.v,
			GPUTexture::RGBA,
			true, true,
			B
		);

		delete[] G;
		delete[] B;
		free(img.pix);
	}

	Option<Glyph> Font::getGlyph(char c) const {
		for (Glyph g : m_glyphs) {
			if (g.character == c)
				return Some(g);
		}
		return None();
	}
}