#include "settings.hpp"

namespace cygextreg
{

Settings::exitBehaviourMap_t Settings::_exitBehaviourMap = {
	{ "close", ExitBehaviour::CLOSE },
	{ "error", ExitBehaviour::ON_ERROR },
	{ "keep", ExitBehaviour::ALWAYS }
};

bool Settings::isSupportedExitBehaviour(const std::string& str) {
	return _exitBehaviourMap.count(str) > 0;
}

Settings Settings::withExitBehaviourStr(const std::string& str) const {
	Settings s = *this;
	if (_exitBehaviourMap.count(str)) {
		s._exitBehaviour = _exitBehaviourMap[str];
	}
	return s;
}

std::string Settings::exitBehaviourStr() const {
	for (auto p : _exitBehaviourMap) {
		if (p.second == _exitBehaviour) {
			return p.first;
		}
	}
	return std::string();
}

}
