#include "Renderer.h"

#include <algorithm>
#include "../core/Input.h"

namespace tfx {
	Renderer::Renderer(u32 screenWidth, u32 screenHeight) {
		m_screenWidth = screenWidth;
		m_screenHeight = screenHeight;

		m_proj = Matrix4::ortho(0, screenWidth, screenHeight, 0, -100, 100);
		m_view = Matrix4();

		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		m_batchVBO = new GPUBuffer(GPUBuffer::DataArray, GPUBuffer::Dynamic);
		m_batchIBO = new GPUBuffer(GPUBuffer::ElementArray, GPUBuffer::Dynamic);
		m_batchVAO = new GPUVertexArray();
		m_batchVAO->bindAttribute(0, GPUVertexArray::Float3, m_batchVBO, sizeof(Vertex));
		m_batchVAO->bindAttribute(1, GPUVertexArray::Float2, nullptr, sizeof(Vertex));
		m_batchVAO->bindAttribute(2, GPUVertexArray::Float4, nullptr, sizeof(Vertex), true);
		m_batchVAO->bindElement(m_batchIBO);
		m_batchVAO->unbind();

		float qverts[] = {
			0, 0,
			1, 0,
			1, 1,
			0, 1
		};
		u32 qinds[] = { 0, 1, 2, 2, 3, 0 };

		m_quadVBO = new GPUBuffer(GPUBuffer::DataArray, GPUBuffer::Static);
		m_quadVBO->setData(qverts, 8);

		m_quadIBO = new GPUBuffer(GPUBuffer::ElementArray, GPUBuffer::Static);
		m_quadIBO->setData(qinds, 6);

		m_quadVAO = new GPUVertexArray();
		m_quadVAO->bindAttribute(0, GPUVertexArray::Float2, m_quadVBO, 8);
		m_quadVAO->bindElement(m_quadIBO);

		byte *ndata = new byte[TFX_NMAP_SIZE * TFX_NMAP_SIZE * 3];
		for (int y = 0; y < TFX_NMAP_SIZE; y++) {
			for (int x = 0; x < TFX_NMAP_SIZE; x++) {
				int index = (x + y * TFX_NMAP_SIZE) * 3;
				ndata[index + 0] = 128;
				ndata[index + 1] = 128;
				ndata[index + 2] = 255;
			}
		}
		m_defaultNormal = new GPUTexture(TFX_NMAP_SIZE, TFX_NMAP_SIZE, GPUTexture::RGB, true, false, ndata);

		m_ambientColor = Vector3(0.12f);

		m_gbuffShader = new GPUShader();
		String vs = 
#include "../shaders/ShdGeomPass.vert"
		;
		String fs =
#include "../shaders/ShdGeomPass.frag"
		;
		m_gbuffShader->addSource(vs, GPUShader::VertexShader);
		m_gbuffShader->addSource(fs, GPUShader::FragmentShader);
		m_gbuffShader->link();

		m_fullScreenShader = new GPUShader();
		String fvs =
#include "../shaders/ShdFullScreen.vert"
			;
		String ffs =
#include "../shaders/ShdFullScreen.frag"
			;
		m_fullScreenShader->addSource(fvs, GPUShader::VertexShader);
		m_fullScreenShader->addSource(ffs, GPUShader::FragmentShader);
		m_fullScreenShader->link();

		m_defaultShader = new GPUShader();
		String dvs =
#include "../shaders/ShdDefault.vert"
			;
		String dfs =
#include "../shaders/ShdDefault.frag"
			;
		m_defaultShader->addSource(dvs, GPUShader::VertexShader);
		m_defaultShader->addSource(dfs, GPUShader::FragmentShader);
		m_defaultShader->link();

		m_occlusionShader = new GPUShader();
		String ovs =
#include "../shaders/ShdGeomPass.vert"
			;
		String ofs =
#include "../shaders/ShdOcclusionPass.frag"
			;
		m_occlusionShader->addSource(ovs, GPUShader::VertexShader);
		m_occlusionShader->addSource(ofs, GPUShader::FragmentShader);
		m_occlusionShader->link();

		m_shadowShader = new GPUShader();
		String svs =
#include "../shaders/ShdShadowPass.vert"
			;
		String sfs =
#include "../shaders/ShdShadowPass.frag"
			;
		m_shadowShader->addSource(svs, GPUShader::VertexShader);
		m_shadowShader->addSource(sfs, GPUShader::FragmentShader);
		m_shadowShader->link();

		m_fontShader = new GPUShader();
		String fntvs =
#include "../shaders/ShdGeomPass.vert"
			;
		String fntfs =
#include "../shaders/ShdFont.frag"
			;
		m_fontShader->addSource(fntvs, GPUShader::VertexShader);
		m_fontShader->addSource(fntfs, GPUShader::FragmentShader);
		m_fontShader->link();

		m_lightShader = new GPUShader();
		String livs =
#include "../shaders/ShdLight.vert"
			;
		String lifs =
#include "../shaders/ShdLight.frag"
			;
		m_lightShader->addSource(livs, GPUShader::VertexShader);
		m_lightShader->addSource(lifs, GPUShader::FragmentShader);
		m_lightShader->link();

		m_colorRT = new GPURenderTarget(screenWidth, screenHeight, GPUTexture::RGBA, true);
		m_normalsRT = new GPURenderTarget(screenWidth, screenHeight, GPUTexture::RGBf);
		m_positionRT = new GPURenderTarget(screenWidth, screenHeight, GPUTexture::RGBf);
		m_finalRT = new GPURenderTarget(screenWidth, screenHeight, GPUTexture::RGB, true);

		m_occluderRT = new GPURenderTarget(TFX_LIGHT_SIZE, TFX_LIGHT_SIZE, GPUTexture::Mono);
		m_shadowRT = new GPURenderTarget(TFX_LIGHT_SIZE, 1, GPUTexture::Mono);

		m_maskRT = new GPURenderTarget(screenWidth, screenHeight, GPUTexture::Mono);

		glViewport(0, 0, screenWidth, screenHeight);

	}

	Renderer::~Renderer() {
		delete m_quadIBO;
		delete m_quadVBO;
		delete m_quadVAO;

		delete m_defaultNormal;
		
		delete m_gbuffShader;
		delete m_fullScreenShader;
		delete m_defaultShader;
		delete m_fontShader;
		delete m_ambientShader;
		delete m_occlusionShader;
		delete m_shadowShader;
		delete m_lightShader;

		delete m_colorRT;
		delete m_normalsRT;
		delete m_positionRT;
		delete m_finalRT;
		delete m_occluderRT;
		delete m_shadowRT;
		delete m_maskRT;

		delete m_batchVBO;
		delete m_batchIBO;
		delete m_batchVAO;
	}

	i32 Renderer::draw(Font* font, const String& str) {
		if (font == nullptr) {
			return 0;
		}
		int sx = m_state.x;
		int sy = m_state.y;
		float fx = sx;
		float fy = sy;
		
		m_state.rotation = 0;
		bool occ = m_state.occluder;
		m_state.occluder = false;

		float yp = 0.75f / (float) font->getAtlas()->getHeight();
		for (char c : str.str()) {
			auto glyph = font->getGlyph(c);
			if (glyph.ok()) {
				Glyph g = glyph.unwrap();
				float ch = float(g.bounds[3] * (float) font->getAtlas()->getHeight()) * m_state.scale.y;
				m_state.clipRect = Vector4(
					g.bounds[0], g.bounds[1]+yp,
					g.bounds[2], g.bounds[3]-yp
				);
				float oy = (float(g.oy) * m_state.scale.y);
				m_state.x = int(floor(fx));
				m_state.y = int(floor(fy - (ch - oy)));

				if (c == '\n') {
					fx = sx;
					fy += font->getSpaceSize() + ch;
				} else if (c == ' ') {
					fx += glyph.unwrap().ox * m_state.scale.x;
				} else {
					draw(font->getAtlas(), nullptr, true);
					fx += glyph.unwrap().ox * m_state.scale.x;
				}
			} else {
				if (c == ' ') {
					fx += font->getSpaceSize() * m_state.scale.x;
				} else if (c == '\n') {
					fx = sx;
					fy += (font->getSpaceSize() * 2.0f) * m_state.scale.y;
				}
			}
			
		}

		m_state.clipRect = Vector4(0, 0, 1, 1);
		m_state.x = 0;
		m_state.y = 0;
		m_state.occluder = occ;

		return (i32) fx;
	}

	void Renderer::draw(GPUTexture* diffuse, GPUTexture* normal, bool defer) {
		if (diffuse == nullptr) return;
		
		i32 x = m_state.x;
		i32 y = m_state.y;
		i32 z = m_state.z;
		float sx = m_state.scale.x;
		float sy = m_state.scale.y;
		float ox = m_state.origin.x;
		float oy = m_state.origin.y;
		Vector4 uv = m_state.clipRect;
		float rotation = m_state.rotation;
		Vector4 color = m_state.color;
		bool additive = m_state.additive;
		bool occluder = m_state.occluder;

		Vector4 dst = Vector4(
			x, y,
			sx * float(diffuse->getWidth()),
			sy * float(diffuse->getHeight())
		);

		float width = dst.z * uv.z;
		float height = dst.w * uv.w;

		float cx = ox * width;
		float cy = oy * height;

		Vector2 tl(-cx, -cy);
		Vector2 tr(width - cx, -cy);
		Vector2 br(width - cx, height - cy);
		Vector2 bl(-cx, height - cy);

		Vector2 pos(dst.x, dst.y);
		Vector2 tlr = tl.rotate(rotation) + pos;
		Vector2 trr = tr.rotate(rotation) + pos;
		Vector2 brr = br.rotate(rotation) + pos;
		Vector2 blr = bl.rotate(rotation) + pos;

		float u1 = uv.x;
		float v1 = uv.y;
		float u2 = uv.x + uv.z;
		float v2 = uv.y + uv.w;

		Vertex TL;
		TL.position = tlr.extend(z);
		TL.uv = Vector2(u1, v1);
		TL.color = color;

		Vertex TR;
		TR.position = trr.extend(z);
		TR.uv = Vector2(u2, v1);
		TR.color = color;

		Vertex BR;
		BR.position = brr.extend(z);
		BR.uv = Vector2(u2, v2);
		BR.color = color;

		Vertex BL;
		BL.position = blr.extend(z);
		BL.uv = Vector2(u1, v2);
		BL.color = color;

		Drawable* d = new Drawable();
		d->additive = additive;
		d->tl = TL;
		d->tr = TR;
		d->bl = BL;
		d->br = BR;
		d->diffuse = diffuse;
		d->normal = normal == nullptr ? m_defaultNormal : normal;
		d->occluder = occluder;
		d->defer = defer;

		m_drawables.push_back(d);
	}

	void Renderer::drawLight(Vector3 color, float radius, float intensity, bool shadow) {
		if (m_lights.size() < TFX_MAX_LIGHTS) {
			Light l;
			l.color = color;
			l.intensity = intensity;
			l.radius = radius;
			l.shadow = shadow;
			l.x = m_state.x;
			l.y = m_state.y;
			l.z = m_state.z;
			m_lights.push_back(l);
		} else {
			Light& l = m_lights.front();
			l.color = color;
			l.intensity = intensity;
			l.radius = radius;
			l.shadow = shadow;
			l.x = m_state.x;
			l.y = m_state.y;
			l.z = m_state.z;
		}
	}

	void Renderer::begin() {
		for (Drawable* d : m_drawables) {
			delete d;
		}
		m_drawables.clear();
		m_batches.clear();
		m_lights.clear();
	}

	void Renderer::end(int w, int h) {
		if (w == 0) w = m_screenWidth;
		if (h == 0) h = m_screenHeight;
		updateBufferData();

		std::sort(m_batches.begin(), m_batches.end(), [](Batch const& a, Batch const& b) -> bool {
			return a.z < b.z;
		});

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		pass1(); // Fill GBuffer

		pass2(); // Lighting

		pass3(w, h); // Defer
	}

	RendererState Renderer::lock() {
		RendererState cpy = m_state.copy();
		m_state = RendererState();
		return cpy;
	}

	void Renderer::unlock(const RendererState& state) {
		m_state = state;
	}

	void Renderer::setPosition(i32 x, i32 y, i32 z) {
		m_state.x = x;
		m_state.y = y;
		m_state.z = z;
	}

	void Renderer::setScale(const Vector2& scale) {
		m_state.scale = scale;
	}

	void Renderer::setRotation(float rotation) {
		m_state.rotation = rotation;
	}

	void Renderer::translate(i32 x, i32 y, i32 z) {
		m_state.x += x;
		m_state.y += y;
		m_state.z += z;
	}

	void Renderer::scale(const Vector2& scale) {
		m_state.scale = m_state.scale + scale;
	}

	void Renderer::setOrigin(const Vector2& origin) {
		m_state.origin = origin;
	}

	void Renderer::rotate(float rotation) {
		m_state.rotation += rotation;
	}

	void Renderer::clip(const Vector4& clipRect) {
		m_state.clipRect = clipRect;
	}

	void Renderer::unclip() {
		m_state.clipRect = Vector4(0, 0, 1, 1);
	}

	void Renderer::setColor(const Vector4& color) {
		m_state.color = color;
	}

	void Renderer::setAdditive(bool v) {
		m_state.additive = v;
	}

	void Renderer::setOccluder(bool v) {
		m_state.occluder = v;
	}

	void Renderer::updateBufferData() {
		if (m_drawables.empty()) return;

		std::sort(m_drawables.begin(), m_drawables.end(), [](Drawable *a, Drawable *b) -> bool {
			return a->diffuse->getBindCode() < b->diffuse->getBindCode();
		});

		Vec<Vertex> vertices;
		vertices.reserve(m_drawables.size() * 4);
		Vec<u32> indices;
		indices.reserve(m_drawables.size() * 6);

		vertices.insert(vertices.end(), { m_drawables[0]->tl, m_drawables[0]->tr, m_drawables[0]->br, m_drawables[0]->bl });
		indices.insert(indices.end(), { 0, 1, 2, 2, 3, 0 });

		m_batches.emplace_back(
			0,
			6,
			m_drawables[0]->additive,
			m_drawables[0]->occluder,
			m_drawables[0]->diffuse,
			m_drawables[0]->normal,
			m_drawables[0]->tl.position.z,
			m_drawables[0]->defer
		);

		u32 io = 4;
		u32 off = 0;
		for (u32 i = 1; i < m_drawables.size(); ++i) {
			Drawable* current = m_drawables[i];
			Drawable* prev = m_drawables[i - 1];
			if (current->diffuse->getBindCode() != prev->diffuse->getBindCode() ||
				current->normal->getBindCode() != prev->normal->getBindCode() ||
				current->occluder != prev->occluder ||
				current->tl.position.z != prev->tl.position.z ||
				current->defer != prev->defer) {
				off += m_batches.back().indices;
				m_batches.emplace_back(
					off,
					6,
					current->additive,
					current->occluder,
					current->diffuse,
					current->normal,
					current->tl.position.z,
					current->defer
				);
			} else {
				m_batches.back().indices += 6;
			}

			vertices.insert(vertices.end(), { current->tl, current->tr, current->br, current->bl });
			indices.insert(indices.end(), { 0 + io, 1 + io, 2 + io, 2 + io, 3 + io, 0 + io });

			io += 4;
		}

		m_batchVBO->setData<Vertex>(vertices.data(), vertices.size());
		m_batchIBO->setData<u32>(indices.data(), indices.size());
	}

	void Renderer::renderBatches(GPUShader* shader,
		const std::function<bool(const Batch&)>& cb,
		const std::function<void(GPUShader*)>& setunicb)
	{
		m_batchVAO->bind();

		GPUShader* shd = shader;
		if (shd == nullptr) { return; }
		shd->bind();

		for (Batch b : m_batches) {
			if (cb && !cb(b)) { continue; }

			if (setunicb) {
				setunicb(shd);
			}

			b.diffuse->bind(0);
			b.normal->bind(1);

			if (b.additive) glBlendFunc(GL_ONE, GL_ONE);

			glDrawElements(GL_TRIANGLES, b.indices, GL_UNSIGNED_INT, (void*) (b.offset * sizeof(u32)));
		}

		shd->unbind();
		m_batchVAO->unbind();
	}

	void Renderer::pass1() {
		// Render color
		m_colorRT->bind();
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		renderBatches(m_gbuffShader,
			[](const Batch& b) {
			return !b.defer;
			},
				[&](GPUShader* shd) {
				shd->getUniform("MAT_View").set(m_view);
				shd->getUniform("MAT_Projection").set(m_proj);
				shd->getUniform("TEX_Diffuse").set(0);
				shd->getUniform("TEX_Normal").set(1);
				shd->getUniform("GBP_Mode").set(0);
			}
		);
		m_colorRT->unbind();

		// Render positions
		m_positionRT->bind();
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		renderBatches(m_gbuffShader,
			[](const Batch& b) {
				return !b.defer;
			},
			[&](GPUShader* shd) {
				shd->getUniform("MAT_View").set(m_view);
				shd->getUniform("MAT_Projection").set(m_proj);
				shd->getUniform("TEX_Diffuse").set(0);
				shd->getUniform("TEX_Normal").set(1);
				shd->getUniform("GBP_Mode").set(1);
			}
		);
		m_positionRT->unbind();

		// Render normals
		m_normalsRT->bind();
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		renderBatches(m_gbuffShader,
			[](const Batch& b) {
			return !b.defer;
			},
				[&](GPUShader* shd) {
				shd->getUniform("MAT_View").set(m_view);
				shd->getUniform("MAT_Projection").set(m_proj);
				shd->getUniform("TEX_Diffuse").set(0);
				shd->getUniform("TEX_Normal").set(1);
				shd->getUniform("GBP_Mode").set(2);
			}
		);
		m_normalsRT->unbind();

		// Render mask
		m_maskRT->bind();
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		renderBatches(m_gbuffShader,
			[](const Batch& b) {
			return b.occluder;
		},
			[&](GPUShader* shd) {
			shd->getUniform("MAT_View").set(m_view);
			shd->getUniform("MAT_Projection").set(m_proj);
			shd->getUniform("TEX_Diffuse").set(0);
			shd->getUniform("TEX_Normal").set(1);
			shd->getUniform("GBP_Mode").set(3);
		}
		);
		m_maskRT->unbind();
	}

	void Renderer::pass2() {
		m_finalRT->bind();
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glBlendFunc(GL_ONE, GL_ONE);

		// Ambient
		m_fullScreenShader->bind();
		m_fullScreenShader->getUniform("TEX_Screen").set(0);
		m_fullScreenShader->getUniform("COL_Tint").set(m_ambientColor);

		m_colorRT->getTexture()->bind(0);

		m_quadVAO->bind();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		m_quadVAO->unbind();

		m_fullScreenShader->unbind();
		
		m_finalRT->unbind();

		// Lights
		float ls = float(TFX_LIGHT_SIZE);
		for (Light light : m_lights) {
			Matrix4 m = Matrix4::uniformScaling(ls / 2.0f) *
				Matrix4::translation(light.x, light.y, 0);
			// Shadow
			if (light.shadow) {
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				// Occluder
				m_occluderRT->bind();
				glClearColor(0, 0, 0, 1);
				glClear(GL_COLOR_BUFFER_BIT);

				renderBatches(m_occlusionShader,
					[](const Batch& b) {
						return b.occluder;
					},
						[&](GPUShader* shd) {
						shd->getUniform("MAT_Projection").set(
							Matrix4::ortho(0, ls, ls, 0, -100, 100)
						);
						shd->getUniform("MAT_View").set(
							Matrix4::translation(-(float) light.x + ls / 2.0f, -(float) light.y + ls / 2.0f, 0)
						);
						shd->getUniform("TEX_Diffuse").set(0);
					}
				);
				m_occluderRT->unbind();

				// Distance shadow map
				m_shadowRT->bind();
				glClearColor(1, 1, 1, 1);
				glClear(GL_COLOR_BUFFER_BIT);

				m_shadowShader->bind();

				m_shadowShader->getUniform("TEX_Occlusion").set(0);
				m_shadowShader->getUniform("TEX_ShadowSize").set(ls);

				m_occluderRT->getTexture()->bind(0);

				m_quadVAO->bind();
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
				m_quadVAO->unbind();

				m_shadowRT->unbind();
				glBlendFunc(GL_ONE, GL_ONE);
			}

			passLight(light);
		}
	}

	void Renderer::pass3(int w, int h) {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		m_finalRT->bind(GPURenderTarget::Read);
		glBlitFramebuffer(
		0, 0, m_screenWidth, m_screenHeight, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST
		);
		m_finalRT->unbind();

		renderBatches(m_fontShader,
			[](const Batch& b) {
			return b.defer;
			},
				[&](GPUShader* shd) {
				shd->getUniform("MAT_View").set(m_view);
				shd->getUniform("MAT_Projection").set(m_proj);
				shd->getUniform("TEX_Diffuse").set(0);
			}
		);
	}

	void Renderer::passLight(const Light& light) {
		float ls = float(TFX_LIGHT_SIZE);
		Matrix4 m = Matrix4::uniformScaling(ls / 2.0f) * Matrix4::translation(light.x, light.y, 0);
		
		m_finalRT->bind();

		// Light
		m_lightShader->bind();
		m_lightShader->getUniform("TEX_Position").set(0);
		m_lightShader->getUniform("TEX_Normal").set(1);
		m_lightShader->getUniform("TEX_Diffuse").set(2);

		m_lightShader->getUniform("TEX_ShadowEnabled").set(light.shadow ? 1 : 0);

		m_lightShader->getUniform("sresolution").set(Vector2(m_screenWidth, m_screenHeight));

		if (light.shadow) {
			m_lightShader->getUniform("TEX_Shadow").set(3);
			m_lightShader->getUniform("TEX_Mask").set(4);
			m_lightShader->getUniform("TEX_ShadowSize").set(ls);
		}

		m_lightShader->getUniform("MAT_Projection").set(m_proj);
		m_lightShader->getUniform("MAT_Model").set(m);

		m_lightShader->getUniform("LIT_position").set(Vector3(light.x, light.y, light.z));
		m_lightShader->getUniform("LIT_radius").set(light.radius > ls / 2.0f ? ls / 2.0f : light.radius);
		m_lightShader->getUniform("LIT_intensity").set(light.intensity);
		m_lightShader->getUniform("LIT_color").set(light.color);

		m_positionRT->getTexture()->bind(0);
		m_normalsRT->getTexture()->bind(1);
		m_colorRT->getTexture()->bind(2);

		if (light.shadow) {
			m_shadowRT->getTexture()->bind(3);
			m_maskRT->getTexture()->bind(4);
		}

		m_quadVAO->bind();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		m_quadVAO->unbind();

		m_lightShader->unbind();
		m_finalRT->unbind();
	}

}