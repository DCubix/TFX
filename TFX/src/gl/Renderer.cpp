#include "Renderer.h"

#include <algorithm>
#include "../core/Input.h"

namespace tfx {
	Renderer::Renderer(u32 screenWidth, u32 screenHeight)
		: m_prevVBOSize(0),
		  m_prevIBOSize(0)
	{
		m_screenWidth = screenWidth;
		m_screenHeight = screenHeight;

#define OFF(x) ((const GLvoid*)x)

		m_proj = Matrix4::ortho(0, screenWidth, screenHeight, 0, -100, 100);
		m_view = Matrix4();

		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glGenBuffers(1, &m_vbo);
		glGenBuffers(1, &m_ibo);
		glGenVertexArrays(1, &m_vao);

		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(Vertex), OFF(12));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, true, sizeof(Vertex), OFF(20));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

		glBindVertexArray(0);

		float qverts[] = {
			0, 0,
			1, 0,
			1, 1,
			0, 1
		};
		u32 qinds[] = { 0, 1, 2, 2, 3, 0 };

		m_quad_vbo = new GPUBuffer(GPUBuffer::DataArray, GPUBuffer::Static);
		m_quad_vbo->setData(qverts, 8);

		m_quad_ibo = new GPUBuffer(GPUBuffer::ElementArray, GPUBuffer::Static);
		m_quad_ibo->setData(qinds, 6);

		m_quad_vao = new GPUVertexArray();
		m_quad_vao->bindAttribute(0, GPUVertexArray::Float2, m_quad_vbo, 8);
		m_quad_vao->bindElement(m_quad_ibo);

#define DEFAULT_NMAP_SIZE 128
		byte *ndata = new byte[DEFAULT_NMAP_SIZE * DEFAULT_NMAP_SIZE * 3];
		for (int y = 0; y < DEFAULT_NMAP_SIZE; y++) {
			for (int x = 0; x < DEFAULT_NMAP_SIZE; x++) {
				int index = (x + y * DEFAULT_NMAP_SIZE) * 3;
				ndata[index + 0] = 128;
				ndata[index + 1] = 210;
				ndata[index + 2] = 255;
			}
		}
		m_defaultNormal = new GPUTexture(DEFAULT_NMAP_SIZE, DEFAULT_NMAP_SIZE, GPUTexture::RGB, true, false, ndata);

		m_geomPass = new GPUShader();
		String vs = 
#include "../shaders/ShdGeomPass.vert"
		;
		String fs =
#include "../shaders/ShdGeomPass.frag"
		;
		m_geomPass->addSource(vs, GPUShader::VertexShader);
		m_geomPass->addSource(fs, GPUShader::FragmentShader);
		m_geomPass->link();

		m_fbo_position = new GPUTexture(screenWidth, screenHeight, GPUTexture::RGBf, true, false, nullptr);
		m_fbo_normals = new GPUTexture(screenWidth, screenHeight, GPUTexture::RGBAf, true, false, nullptr);
		m_fbo_diffuse = new GPUTexture(screenWidth, screenHeight, GPUTexture::RGBA, true, false, nullptr);

		m_gbuffer = new GPUFramebuffer();
		m_gbuffer->addColorAttachment(m_fbo_position);
		m_gbuffer->addColorAttachment(m_fbo_normals);
		m_gbuffer->addColorAttachment(m_fbo_diffuse);

		m_fullScreen = new GPUShader();
		String fvs =
#include "../shaders/ShdFullScreen.vert"
			;
		String ffs =
#include "../shaders/ShdFullScreen.frag"
			;
		m_fullScreen->addSource(fvs, GPUShader::VertexShader);
		m_fullScreen->addSource(ffs, GPUShader::FragmentShader);
		m_fullScreen->link();

		m_defaultShader = new GPUShader();
		String lvs =
#include "../shaders/ShdDefault.vert"
			;
		String lfs =
#include "../shaders/ShdDefault.frag"
			;
		m_defaultShader->addSource(lvs, GPUShader::VertexShader);
		m_defaultShader->addSource(lfs, GPUShader::FragmentShader);
		m_defaultShader->link();

		m_occlusionPass = new GPUShader();
		String ovs =
#include "../shaders/ShdGeomPass.vert"
			;
		String ofs =
#include "../shaders/ShdOcclusionPass.frag"
			;
		m_occlusionPass->addSource(ovs, GPUShader::VertexShader);
		m_occlusionPass->addSource(ofs, GPUShader::FragmentShader);
		m_occlusionPass->link();

		m_fbo_occluder = new GPUTexture(TFX_LIGHT_SHADOW_SIZE, TFX_LIGHT_SHADOW_SIZE, GPUTexture::RGBAf, true, false);
		m_fbo_occluder->setFilter(GL_LINEAR, GL_LINEAR);

		m_occlusion = new GPUFramebuffer();
		m_occlusion->addColorAttachment(m_fbo_occluder);

		m_shadowPass = new GPUShader();
		String svs =
#include "../shaders/ShdShadowPass.vert"
			;
		String sfs =
#include "../shaders/ShdShadowPass.frag"
			;
		m_shadowPass->addSource(svs, GPUShader::VertexShader);
		m_shadowPass->addSource(sfs, GPUShader::FragmentShader);
		m_shadowPass->link();

		m_fbo_shadow = new GPUTexture(TFX_LIGHT_SHADOW_SIZE, 1, GPUTexture::RGB, true, false);
		m_fbo_shadow->setFilter(GL_LINEAR, GL_LINEAR);

		m_shadow = new GPUFramebuffer();
		m_shadow->addColorAttachment(m_fbo_shadow);

		m_ambientColor = Vector3(0.12f);

		m_finalPass = new GPUShader();
		String fivs =
#include "../shaders/ShdFullScreen.vert"
			;
		String fifs =
#include "../shaders/ShdFinalPass.frag"
			;
		m_finalPass->addSource(fivs, GPUShader::VertexShader);
		m_finalPass->addSource(fifs, GPUShader::FragmentShader);
		m_finalPass->link();

		m_fbo_final = new GPUTexture(screenWidth, screenHeight, GPUTexture::RGBA, true, false);

		m_final = new GPUFramebuffer();
		m_final->addColorAttachment(m_fbo_final);

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

		glViewport(0, 0, screenWidth, screenHeight);
	}

	Renderer::~Renderer() {
		glDeleteBuffers(1, &m_vbo);
		glDeleteBuffers(1, &m_ibo);
		glDeleteVertexArrays(1, &m_vao);
		delete m_quad_ibo;
		delete m_quad_vbo;
		delete m_quad_vao;
		delete m_defaultNormal;
		delete m_geomPass;
		delete m_gbuffer;
		delete m_fbo_diffuse;
		delete m_fbo_position;
		delete m_fullScreen;
		delete m_fbo_normals;
		
		delete m_occlusionPass;
		delete m_occlusion;
		delete m_fbo_occluder;
		delete m_shadow;
		delete m_shadowPass;
		delete m_fbo_shadow;
		delete m_finalPass;
		delete m_fbo_final;
		delete m_final;

		delete m_defaultShader;
		delete m_fontShader;
		delete m_ambientShader;
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

		// Lighting
		m_final->bind();
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		pass2();
		m_final->unbind();

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_final->getBindCode());
		glBlitFramebuffer(
			0, 0, m_screenWidth, m_screenHeight, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST
		);

		pass3();
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

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		if (vertices.size() > m_prevVBOSize) {
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);
			m_prevVBOSize = vertices.size();
		}
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		if (indices.size() > m_prevIBOSize) {
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(u32), indices.data(), GL_DYNAMIC_DRAW);
			m_prevIBOSize = indices.size();
		}
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(Vertex), indices.data());
	}

	void Renderer::renderBatches(GPUShader* shader,
		const std::function<bool(const Batch&)>& cb,
		const std::function<void(GPUShader*)>& setunicb)
	{
		glBindVertexArray(m_vao);

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
		glBindVertexArray(0);
	}

	void Renderer::pass1() {
		m_gbuffer->bind();
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		renderBatches(m_geomPass,
			[](const Batch& b) {
				return !b.defer;
			},
			[&](GPUShader* shd) {
				shd->getUniform("MAT_View").set(m_view);
				shd->getUniform("MAT_Projection").set(m_proj);
				shd->getUniform("TEX_Diffuse").set(0);
				shd->getUniform("TEX_Normal").set(1);
			}
		);

		m_gbuffer->unbind();
	}

	void Renderer::pass2() {
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		glBlendFunc(GL_ONE, GL_ONE);

		// Ambient
		m_fullScreen->bind();
		m_fullScreen->getUniform("TEX_Screen").set(0);
		m_fullScreen->getUniform("COL_Tint").set(m_ambientColor);

		m_fbo_diffuse->bind(0);

		m_quad_vao->bind();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		m_quad_vao->unbind();

		m_fullScreen->unbind();

		// Lights
		m_defaultShader->bind();
		m_defaultShader->getUniform("TEX_Position").set(0);
		m_defaultShader->getUniform("TEX_Normal").set(1);
		m_defaultShader->getUniform("TEX_Diffuse").set(2);

		m_fbo_position->bind(0);
		m_fbo_normals->bind(1);
		m_fbo_diffuse->bind(2);

		for (Light light : m_lights) {
			m_defaultShader->getUniform("LIT_position").set(Vector3(light.x, light.y, light.z));
			m_defaultShader->getUniform("LIT_radius").set(light.radius);
			m_defaultShader->getUniform("LIT_intensity").set(light.intensity);
			m_defaultShader->getUniform("LIT_color").set(light.color);

			m_quad_vao->bind();
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			m_quad_vao->unbind();
		}
		m_defaultShader->unbind();
	}

	void Renderer::pass3() {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

}