// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

namespace idni::tau_lang {

template <typename...BAs>
std::optional<tau<BAs...>> bool_ba_factory<BAs...>::parse(
	const std::string& /*src*/)
{
	// TODO implement
	return {};
}

template <typename...BAs>
tau<BAs...> bool_ba_factory<BAs...>::binding(const tau<BAs...>& /*sn*/) {
	// TODO implement
	return nullptr;
}

template <typename...BAs>
std::variant<BAs...> bool_ba_factory<BAs...>::splitter_one() const {
	throw std::logic_error("Unsupported");
}

} // namespace idni::tau_lang
