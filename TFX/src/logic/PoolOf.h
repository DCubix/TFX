#ifndef TFX_LOGIC_POOLOF_H
#define TFX_LOGIC_POOLOF_H

#include "../core/Util.h"

#include <algorithm>

namespace tfx {
	template <typename E>
	class PoolOf {
	public:
		PoolOf() {}

		~PoolOf() {
			for (E* item : m_active) {
				SafeDelete(item);
			}
			for (E* item : m_inactive) {
				SafeDelete(item);
			}
			m_active.clear();
			m_inactive.clear();
		}

		void reset() {
			for (E* item : m_active) {
				m_inactive.push_back(item);
			}
			m_active.clear();
		}

		virtual E* create() {
			E* e = nullptr;
			if (m_inactive.empty()) {
				e = new E();
			} else {
				e = m_inactive.back();
				m_inactive.pop_back();
			}
			m_active.push_back(e);
			return e;
		}

		virtual void destroy(E* item) {
			auto pos = std::find(m_active.begin(), m_active.end(), item);
			if (pos != m_active.end()) {
				m_inactive.push_back(item);
				m_active.erase(pos);
			}
		}

		Vec<E*> getActive() const { return m_active; }
		Vec<E*> getInactive() const { return m_inactive; }

	private:
		Vec<E*> m_active, m_inactive;
	};
}

#endif // TFX_LOGIC_POOLOF_H