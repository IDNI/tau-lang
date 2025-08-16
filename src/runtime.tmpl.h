// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

namespace idni::tau_lang {

// ---------------------------------------------------------------------

/*std::optional<constant_with_type<sbf_ba>> nso_factory<sbf_ba>::parse(
	const std::string& src, const std::string)
{
	return sbf_ba_factory<sbf_ba>::parse(src);
}*/

std::vector<std::string> nso_factory<sbf_ba>::types() {
	return { "sbf" };
}

std::string nso_factory<sbf_ba>::default_type() {
	return "sbf";
}

std::string nso_factory<sbf_ba>::one(const std::string) {
	return "1";
}

std::string nso_factory<sbf_ba>::zero(const std::string) {
	return "0";
}

tref nso_factory<sbf_ba>::splitter_one() {
	using tau = tree<node<sbf_ba>>;
	return tau::get(tau::bf, tau::get_ba_constant(sbf_ba_factory<sbf_ba>::splitter_one(), "sbf"));
}

tref nso_factory<sbf_ba>::unpack_tau_ba(const std::variant<sbf_ba>&) {
	return nullptr; // There is no tau_ba present
}

std::variant<sbf_ba> nso_factory<sbf_ba>::pack_tau_ba(
	tref) {
	// There is no tau_ba
	return {};
}





std::vector<std::string> nso_factory<bv, sbf_ba>::types() {
	return { "sbf" };
}

std::string nso_factory<bv, sbf_ba>::default_type() {
	return "sbf";
}

std::string nso_factory<bv, sbf_ba>::one(const std::string) {
	return "1";
}

std::string nso_factory<bv, sbf_ba>::zero(const std::string) {
	return "0";
}

tref nso_factory<bv, sbf_ba>::splitter_one() {
	using tau = tree<node<bv, sbf_ba>>;
	return tau::get(tau::bf, tau::get_ba_constant(sbf_ba_factory<bv, sbf_ba>::splitter_one(), "sbf"));
}

tref nso_factory<bv, sbf_ba>::unpack_tau_ba(const std::variant<bv, sbf_ba>&) {
	return nullptr; // There is no tau_ba present
}

std::variant<bv, sbf_ba> nso_factory<bv, sbf_ba>::pack_tau_ba(
	tref) {
	// There is no tau_ba
	return {};
}

/*std::optional<constant_with_type<tau_ba<sbf_ba>, sbf_ba>>
	nso_factory<tau_ba<sbf_ba>, sbf_ba>::parse(
		const std::string& constant_source,
		const std::string type_name)
{
	return type_name == "sbf" ? sbf_ba_factory<tau_ba<sbf_ba>, sbf_ba>::parse(constant_source)
				  : tau_ba_factory<sbf_ba>::parse(constant_source);
}*/

std::vector<std::string> nso_factory<tau_ba<sbf_ba>, sbf_ba>::types() {
	return { "sbf", "tau" };
}

std::string nso_factory<tau_ba<sbf_ba>, sbf_ba>::default_type() {
	return "tau";
}

std::string nso_factory<tau_ba<sbf_ba>, sbf_ba>::one(
	const std::string type_name)
{
	return type_name == "sbf" ? "1" : "T";
}

std::string nso_factory<tau_ba<sbf_ba>, sbf_ba>::zero(
	const std::string type_name)
{
	return type_name == "sbf" ? "0" : "F";
}

tref nso_factory<tau_ba<sbf_ba>, sbf_ba>::splitter_one(
		const std::string type_name)
{
	using tau = tree<node<tau_ba<sbf_ba>, sbf_ba>>;
	return (type_name == "sbf")
		? tau::get(tau::bf,
			tau::get_ba_constant(
				sbf_ba_factory<tau_ba<sbf_ba>, sbf_ba>::splitter_one(), "sbf"))
		: tau::get(tau::bf,
			tau::get_ba_constant(
				tau_ba_factory<sbf_ba>::splitter_one(), "tau"));
}

tref nso_factory<tau_ba<sbf_ba>, sbf_ba>::unpack_tau_ba(
		const std::variant<tau_ba<sbf_ba>, sbf_ba>& v)
{
	if (!std::holds_alternative<tau_ba<sbf_ba>>(v)) return {};
	const auto unpacked = std::get<tau_ba<sbf_ba>>(v);
	return unpacked.nso_rr.main->get();
}

inline std::variant<tau_ba<sbf_ba>, sbf_ba> nso_factory<tau_ba<sbf_ba>,
	sbf_ba>::pack_tau_ba(tref c) {
	tau_ba<sbf_ba> t {c};
	return {t};
}








std::vector<std::string> nso_factory<tau_ba<bv, sbf_ba>, bv, sbf_ba>::types() {
	return { "sbf", "tau" };
}

std::string nso_factory<tau_ba<bv, sbf_ba>, bv, sbf_ba>::default_type() {
	return "tau";
}

std::string nso_factory<tau_ba<bv, sbf_ba>, bv, sbf_ba>::one(
	const std::string type_name)
{
	return type_name == "sbf" ? "1" : "T";
}

std::string nso_factory<tau_ba<bv, sbf_ba>, bv, sbf_ba>::zero(
	const std::string type_name)
{
	return type_name == "sbf" ? "0" : "F";
}

tref nso_factory<tau_ba<bv, sbf_ba>, bv, sbf_ba>::splitter_one(
		const std::string type_name)
{
	using tau = tree<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>;
	return (type_name == "sbf")
		? tau::get(tau::bf,
			tau::get_ba_constant(
				sbf_ba_factory<tau_ba<bv, sbf_ba>, bv, sbf_ba>::splitter_one(), "sbf"))
		: tau::get(tau::bf,
			tau::get_ba_constant(
				tau_ba_factory<bv, sbf_ba>::splitter_one(), "tau"));
}

tref nso_factory<tau_ba<bv, sbf_ba>, bv, sbf_ba>::unpack_tau_ba(
		const std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>& v)
{
	if (!std::holds_alternative<tau_ba<bv, sbf_ba>>(v)) return {};
	const auto unpacked = std::get<tau_ba<bv, sbf_ba>>(v);
	return unpacked.nso_rr.main->get();
}

inline std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba> nso_factory<tau_ba<bv, sbf_ba>,
	bv, sbf_ba>::pack_tau_ba(tref c) {
	tau_ba<bv, sbf_ba> t {c};
	return {t};
}


} // namespace idni::tau_lang