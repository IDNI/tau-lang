// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

namespace idni::tau_lang {

	std::optional<tau<bitvector_ba>> nso_factory<bitvector_ba>::parse(const std::string& src,
		const std::string&)
	{
		return bvf.parse(src);
	}

	tau<bitvector_ba> nso_factory<bitvector_ba>::binding(const tau<bitvector_ba>& n,
		const std::string&)
	{
		return bvf.binding(n);
	}

	std::vector<std::string> nso_factory<bitvector_ba>::types() const {
		return { "sbf" };
	}

	tau<bitvector_ba> nso_factory<bitvector_ba>::splitter_one() const {
		throw std::logic_error("Unsupported");
	}

	std::string nso_factory<bitvector_ba>::default_type() const {
		return "sbf";
	}

	std::string nso_factory<bitvector_ba>::one(const std::string) const {
		return "1";
	}

	std::string nso_factory<bitvector_ba>::zero(const std::string) const {
		return "0";
	}

	std::optional<tau<bitvector_ba>> nso_factory<bitvector_ba>::unpack_tau_ba(
		const std::variant<bitvector_ba>&) const {
		// There is no tau_ba present
		return {};
	}

	nso_factory<bitvector_ba>& nso_factory<bitvector_ba>::instance() {
		static nso_factory<bitvector_ba> factory;
		return factory;
	}

	nso_factory<bitvector_ba>::nso_factory() {};


	std::optional<tau<tau_ba<bitvector_ba>, bitvector_ba>> nso_factory<tau_ba<bitvector_ba>, bitvector_ba>::parse(
		const std::string src, const std::string type_name)
	{
		if (type_name == "bit")	return bvf.parse(src);
		return tf.parse(src);
	}

	tau<tau_ba<bitvector_ba>, bitvector_ba> nso_factory<tau_ba<bitvector_ba>, bitvector_ba>::binding(
		const tau<tau_ba<bitvector_ba>, bitvector_ba>& n,
		const std::string type_name)
	{
		if (type_name == "bit") return bvf.binding(n);
		return tf.binding(n);
	}

	std::vector<std::string> nso_factory<tau_ba<bitvector_ba>, bitvector_ba>::types() const {
		return { "sbf", "tau" };
	}

	std::string nso_factory<tau_ba<bitvector_ba>, bitvector_ba>::default_type() const {
		return "tau";
	}

	std::string nso_factory<tau_ba<bitvector_ba>, bitvector_ba>::one(const std::string type_name) const {
		return type_name == "sbf" ? "1" : "T";
	}

	std::string nso_factory<tau_ba<bitvector_ba>, bitvector_ba>::zero(const std::string type_name) const {
		return type_name == "sbf" ? "0" : "F";
	}

	std::optional<tau<tau_ba<bitvector_ba>, bitvector_ba> > nso_factory<tau_ba<bitvector_ba>, bitvector_ba>::unpack_tau_ba(
			const std::variant<tau_ba<bitvector_ba>, bitvector_ba>& v) const {
		if (!std::holds_alternative<tau_ba<bitvector_ba>>(v))
			return {};
		const auto unpacked = std::get<tau_ba<bitvector_ba>>(v);
		return std::optional(unpacked.nso_rr.main);
	}

	tau<tau_ba<bitvector_ba>, bitvector_ba> nso_factory<tau_ba<bitvector_ba>, bitvector_ba>::splitter_one(
		const std::string& type_name) const
	{
		if (type_name == "sbf")
			return build_bf_constant(bvf.splitter_one());
		else return build_bf_constant(tf.splitter_one());
	}

	nso_factory<tau_ba<bitvector_ba>, bitvector_ba>&
		nso_factory<tau_ba<bitvector_ba>, bitvector_ba>::instance()
	{
		static nso_factory<tau_ba<bitvector_ba>, bitvector_ba> factory;
		return factory;
	}

	nso_factory<tau_ba<bitvector_ba>, bitvector_ba>::nso_factory() {};


	std::optional<tau<sbf_ba>> nso_factory<sbf_ba>::parse(const std::string& src,
		const std::string&) {
	return bf.parse(src);
}

tau<sbf_ba> nso_factory<sbf_ba>::binding(const tau<sbf_ba>& n,
		const std::string&){
	return bf.binding(n);
}

std::vector<std::string> nso_factory<sbf_ba>::types() const {
	return { "sbf" };
}

tau<sbf_ba> nso_factory<sbf_ba>::splitter_one() const {
	return build_bf_constant(bf.splitter_one());
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

std::optional<tau<sbf_ba> > nso_factory<sbf_ba>::unpack_tau_ba(
	const std::variant<sbf_ba>&) const {
	// There is no tau_ba present
	return {};
}

nso_factory<sbf_ba>& nso_factory<sbf_ba>::instance() {
	static nso_factory<sbf_ba> factory;
	return factory;
}

nso_factory<sbf_ba>::nso_factory() {};


std::optional<tau<tau_ba<sbf_ba>, sbf_ba>> nso_factory<tau_ba<sbf_ba>, sbf_ba>::parse(
	const std::string src, const std::string type_name)
{
	if (type_name == "sbf")	return bf.parse(src);
	return tf.parse(src);
}

tau<tau_ba<sbf_ba>, sbf_ba> nso_factory<tau_ba<sbf_ba>, sbf_ba>::binding(
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

std::optional<tau<tau_ba<sbf_ba>, sbf_ba> > nso_factory<tau_ba<sbf_ba>, sbf_ba>::unpack_tau_ba(
		const std::variant<tau_ba<sbf_ba>, sbf_ba>& v) const {
	if (!std::holds_alternative<tau_ba<sbf_ba>>(v))
		return {};
	const auto unpacked = std::get<tau_ba<sbf_ba>>(v);
	return std::optional(unpacked.nso_rr.main);
}

tau<tau_ba<sbf_ba>, sbf_ba> nso_factory<tau_ba<sbf_ba>, sbf_ba>::splitter_one(
	const std::string& type_name) const
{
	if (type_name == "sbf")
		return build_bf_constant(bf.splitter_one());
	else return build_bf_constant(tf.splitter_one());
}

nso_factory<tau_ba<sbf_ba>, sbf_ba>& nso_factory<tau_ba<sbf_ba>, sbf_ba>::instance() {
	static nso_factory<tau_ba<sbf_ba>, sbf_ba> factory;
	return factory;
}

nso_factory<tau_ba<sbf_ba>, sbf_ba>::nso_factory() {};


template<>
struct nso_factory<Bool> {

	tau<Bool> parse(const std::string&,
			const std::string&) const {
		throw std::logic_error("not implemented");
	}

	tau<Bool> binding(const tau<Bool>& n,
			const std::string& type) const {
		if (type != "bool") return n;
		return make_node<tau_sym<Bool>>(Bool(true), {});
	}

	std::vector<std::string> types() const {
		return { "bool" };
	}

	std::string default_type() const {
		return "bool";
	}

	std::string one(const std::string&) const {
		throw std::logic_error("not implemented");
	}

	std::string zero(const std::string&) const {
		throw std::logic_error("not implemented");
	}

	tau<Bool> splitter_one(const std::string&) const {
		throw std::logic_error("not implemented");
	}

	std::optional<Bool> unpack_tau_ba(
		const std::variant<Bool>&) const {
		// There is no tau_ba
		return {};
	}

	static nso_factory<Bool>& instance() {
			static nso_factory<Bool> factory;
			return factory;
		}

private:

	nso_factory() {};
};








} // namespace idni::tau_lang