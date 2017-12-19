#ifndef _TWS_OPTION_
#define _TWS_OPTION_

#include <cstdio>

namespace tfx {
	template <typename T>
	class Option {
	public:
		Option() : m_ok(false) {}
		Option(T value, bool some) : m_value(value), m_ok(some) {}

		bool ok() const { return m_ok; }

		T& unwrap() {
			if (m_ok) {
				return m_value;
			}
			std::abort();
		}

		T& unwrap_or(T t) {
			if (m_ok) {
				return m_value;
			}
			return t;
		}

	protected:
		T m_value;
		bool m_ok;
	};

	template <typename T>
	Option<T> Some(T t) { return Option<T>(t, true); }

	typedef struct None_t {
		template <typename T>
		operator Option<T>() { return Option<T>(); }
	} None;
}

#endif // _TWS_OPTION_