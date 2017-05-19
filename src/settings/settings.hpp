#ifndef __REGISTRY__SETTINGS_HPP__
#define __REGISTRY__SETTINGS_HPP__

#include <string>
#include <map>

namespace cygextreg
{

class Settings
{
public:
	/**
	 * Behaviour when script exists.
	 * CLOSE:		Close window after script exits
	 * ON_ERROR:	Keep window open if script exits with an error
	 * ALWAYS:		Always keep window open after script exits
	 */
	enum class ExitBehaviour { CLOSE, ON_ERROR, ALWAYS };

	/**
	 * Constructor.
	 */
	Settings() :
		_exitBehaviour(ExitBehaviour::ON_ERROR) {
	}

	/**
	 * Whether exit behaviour is supported.
	 *
	 * @param const std::string& str Behaviour name
	 * @return bool
	 */
	static bool isSupportedExitBehaviour(const std::string& str);

	/**
	 * Get settings with exit behaviour set.
	 *
	 * @param const std::string& str Behaviour name
	 * @return Settings
	 */
	Settings withExitBehaviourStr(const std::string& str) const;

	/**
	 * Get the exit behaviour.
	 *
	 * @return ExitBehaviour
	 */
	ExitBehaviour exitBehaviour() const {
		return _exitBehaviour;
	}

	/**
	 * Get the exit behaviour name.
	 *
	 * @return std::string
	 */
	std::string exitBehaviourStr() const;

private:
	ExitBehaviour _exitBehaviour;

	typedef std::map<std::string, Settings::ExitBehaviour> exitBehaviourMap_t;

	/**
	 * Mapping from behaviour name to enumeration.
	 */
	static exitBehaviourMap_t _exitBehaviourMap;
};

}

#endif
