#include "ResourceManager.h"

namespace tfx {
	Resources::Resources() {
		m_textures = new ResourceManager<GPUTexture>();
	}

	Resources::~Resources() {
		SafeDelete(m_textures);
	}
	
	GPUTexture* Resources::getTexture(const String& fileName) {
		return m_textures->load(fileName);
	}

	void Resources::freeTexture(const String& fileName) {
		m_textures->unload(fileName);
	}

}