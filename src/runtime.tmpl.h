// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

namespace idni::tau_lang {


	std::optional<tau<sbf_ba>> nso_factory<sbf_ba>::parse(const std::string& src,
		const std::string&)
	{
		return bf.parse(src);
	}

	tau<sbf_ba> nso_factory<sbf_ba>::binding(const tau<sbf_ba>& n,
		const std::string&)
	{
		return bf.binding(n);
	}

	std::vector<std::string> nso_factory<sbf_ba>::types() const {
		return { "sbf" };
	}
	tau<sbf_ba> nso_factory<sbf_ba>::splitter_one() const {
		return build_bf_constant(bf.splitter_one(), build_type<sbf_ba>("sbf"));
	}

	std::string nso_factory<sbf_ba>::default_type() const {
		return "sbf";
	}

	std::string nso_factory<sbf_ba>::one(const std::string) const {
		return "1";
	}

	std::string nso_factory<sbf_ba>::zero(const std::string) const {
		return "0";
	}

	
	nso_factory<sbf_ba>& nso_factory<sbf_ba>::instance() {
		static nso_factory<sbf_ba> factory;
		return factory;
	}

	nso_factory<sbf_ba>::nso_factory() {};


	std::optional<tau_nso<sbf_ba>> nso_factory<tau_ba<sbf_ba>, sbf_ba>::parse(
		const std::string src, const std::string type_name)
	{
		if (type_name == "sbf")	return bf.parse(src);
		else if (type_name.starts_with("bv")) {
			return bvf.parse(src);
		}
		return tf.parse(src);
	}

	tau_nso<sbf_ba> nso_factory<tau_ba<sbf_ba>, sbf_ba>::binding(
		const tau<tau_ba<sbf_ba>, sbf_ba>& n,
		const std::string type_name)
	{
		if (type_name == "sbf") return bf.binding(n);
		return tf.binding(n);
	}

	std::vector<std::string> nso_factory<tau_ba<sbf_ba>, sbf_ba>::types() const {
		return { "sbf", "tau" };
	}

	std::string nso_factory<tau_ba<sbf_ba>, sbf_ba>::default_type() const {
		return "tau";
	}

	std::string nso_factory<tau_ba<sbf_ba>, sbf_ba>::one(const std::string type_name) const {
		return type_name == "sbf" ? "1" : "T";
	}

	std::string nso_factory<tau_ba<sbf_ba>, sbf_ba>::zero(const std::string type_name) const {
		return type_name == "sbf" ? "0" : "F";
	}

	std::optional<tau_nso<sbf_ba> > nso_factory<tau_ba<sbf_ba>, sbf_ba>::unpack_tau_ba(
			const std::variant<tau_ba<sbf_ba>, sbf_ba>& v) const {
		if (!std::holds_alternative<tau_ba<sbf_ba>>(v))
			return {};
		const auto unpacked = std::get<tau_ba<sbf_ba>>(v);
		return std::optional(unpacked.nso_rr.main);
	}

	inline std::variant<tau_ba<sbf_ba>, sbf_ba> nso_factory<tau_ba<sbf_ba>,	sbf_ba>::pack_tau_ba(
			const tau<tau_ba<sbf_ba>, sbf_ba>& c) const {
		tau_ba<sbf_ba> t {c};
		return {t};
	}

	tau_nso<sbf_ba> nso_factory<tau_ba<sbf_ba>, sbf_ba>::splitter_one(
		const std::string& type_name) const
	{
		if (type_name == "sbf")
			return build_bf_constant(bf.splitter_one(), build_type<tau_ba<sbf_ba>, sbf_ba>("sbf"));
		else return build_bf_constant(tf.splitter_one(), build_type<tau_ba<sbf_ba>, sbf_ba>("tau"));
	}

	nso_factory<tau_ba<sbf_ba>, sbf_ba>&
		nso_factory<tau_ba<sbf_ba>, sbf_ba>::instance()
	{
		static nso_factory<tau_ba<sbf_ba>, sbf_ba> factory;
		return factory;
	}

	nso_factory<tau_ba<sbf_ba>, sbf_ba>::nso_factory() {};

} // namespace idni::tau_lang