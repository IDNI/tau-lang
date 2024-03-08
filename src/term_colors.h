// class term_colors TC. constructor takes argument (bool colors_enabled = true);
// use: os << TC.BLUE() << "blue text" << TC.CLEAR();
// or:  os << TC(BRIGHT, WHITE) << "bright and white text" << TC.CLEAR();
#include <vector>
#include <string>

// TODO (HIGH) move this file to parser and remove these definitions there
// undef parser's color macros
#undef COLOR
#undef BRIGHT
#undef UNDERLINED
#undef FLASHING
#undef AND
#undef CLEAR
#undef BLACK
#undef RED
#undef GREEN
#undef YELLOW
#undef BROWN
#undef BLUE
#undef PURPLE
#undef CYAN
#undef WHITE
#undef GRAY

namespace idni::term {

using color = enum {
	COLOR,
	CLEAR, BLACK, RED, GREEN,
	YELLOW, BROWN, BLUE, PURPLE,
	CYAN, WHITE, GRAY,
	AND, BRIGHT, UNDERLINED, FLASHING
};

struct colors {
	colors(bool colors_enabled = true) : enabled(colors_enabled) {}
	bool enable() { return enabled = true; }
	bool disable() { return enabled = false; }
	bool toggle() { return enabled = !enabled; }
	bool set(bool e) { return enabled = e; }
	bool enabled = true;
	std::string operator()(color c) { return get(c); }
	std::string operator()(color c, color c2) {
		return get(c) + get(color::AND) + get(c2); }
	std::string COLOR()       { return get(color::COLOR); }
	std::string AND()         { return get(color::AND); }
	std::string BRIGHT()      { return get(color::BRIGHT); }
	std::string UNDERLINED()  { return get(color::UNDERLINED); }
	std::string FLASHING()    { return get(color::FLASHING); }
	std::string CLEAR()       { return getc(color::CLEAR); }
	std::string BLACK()       { return getc(color::BLACK); }
	std::string RED()         { return getc(color::RED); }
	std::string GREEN()       { return getc(color::GREEN); }
	std::string YELLOW()      { return getc(color::YELLOW); }
	std::string BROWN()       { return getc(color::BROWN); }
	std::string BLUE()        { return getc(color::BLUE); }
	std::string PURPLE()      { return getc(color::PURPLE); }
	std::string CYAN()        { return getc(color::CYAN); }
	std::string WHITE()       { return getc(color::WHITE); }
	std::string GRAY()        { return getc(color::GRAY); }

	const std::string& get(color c) {
		static const std::vector<std::string> color_code{
			"\x1b[",
			"0m", "30m", "31m", "32m",
			"33m", "33m", "34m", "35m",
			"36m", "37m", "37m",
			";", "1", "4", "5"
		};
		static const std::string no_code{};
		return enabled ? color_code[c] : no_code;
	}
	std::string getc(color c) { return COLOR() + get(c); }
};

} // idni::term namespace