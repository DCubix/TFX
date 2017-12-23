#ifndef TFX_CORE_RESOURCEMANAGER_H
#define TFX_CORE_RESOURCEMANAGER_H

#include "Util.h"
#include "Logger.h"

#include <unordered_map>

#include "../gl/GPUTexture.h"

namespace tfx {

	template <typename T>
	class Resource {
		template <typename S> friend class ResourceManager;
	public:
		Resource() : m_refs(0), m_data(nullptr) {}
		Resource(T* data) : m_refs(0), m_data(data) {}
		~Resource() {
			SafeDelete(m_data);
		}

		T* get() { return m_data; }
		u32 refs() const { return m_references; }

	protected:
		u32 m_refs;

	private:
		Resource(const Resource<T>& c) {}
		Resource<T>& operator =(const Resource<T>& c) { return *this; }

		T* m_data;
	};

	template <typename T>
	class ResourceManager {
	public:
		ResourceManager() {

		}

		~ResourceManager() {
			auto it = m_resources.begin();
			while (it != m_resources.end()) {
				delete (*it).second;
				it = m_resources.erase(it);
			}
		}

		T* load(const String& fileName) {
			if (fileName.length() == 0) {
				LogError("File Name cannot be empty.");
				return nullptr;
			}

			auto pos = m_resources.find(fileName.str());
			if (pos != m_resources.end()) {
				(*pos).second->m_refs++;
				return (*pos).second->get();
			}

			Vec<byte> fileData = Util::loadFile(fileName);
			Resource<T>* res = new Resource<T>(new T(fileData.data(), fileData.size()));
			res->m_refs++;

			m_resources.insert({ fileName.str(), res });

			return res->get();
		}

		void unload(const String& fileName) {
			if (fileName.length() == 0) {
				LogError("File Name cannot be empty.");
				return;
			}

			auto pos = m_resources.find(fileName.str());
			if (pos != m_resources.end()) {
				if (--(*pos).second->m_refs <= 0) {
					delete (*pos).second;
					m_resources.erase(pos);
				}
			} else {
				LogError(String("Resource not found: ") + fileName);
			}
		}

	private:
		ResourceManager(const ResourceManager<T>& c) {}
		ResourceManager<T>& operator =(const ResourceManager<T>& c) { return *this; }

		std::unordered_map<std::string, Resource<T>*> m_resources;
	};

	class Resources {
	public:
		Resources();
		~Resources();

		GPUTexture* getTexture(const String& fileName);
		void freeTexture(const String& fileName);

	private:
		ResourceManager<GPUTexture>* m_textures;
	};
}

#endif // TFX_CORE_RESOURCEMANAGER_H