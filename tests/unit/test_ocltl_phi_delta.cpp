// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Correctness cross-check and scaling measurement for phi_delta.

#include "test_init.h"
#include "test_tau_helpers.h"
#include "ltl_aba.h"
#include "ocltl_types.h"
#include "ocltl_phi_delta.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <tuple>

using namespace idni::tau_lang;

namespace {

// ── brute-force ground truth over a small tau space ─────────────────────────

bool atom_holds_direct(ocltl_type_mask tau, size_t K, const ocltl_delta_atom& atom) {
	std::vector<bool> supp = ocltl_term_support(atom.term, K);
	bool eq_zero = true;
	for (size_t A = 0; A < (size_t{1} << K); ++A)
		if (supp[A] && !((tau >> A) & 1)) { eq_zero = false; break; }
	return atom.negate ? !eq_zero : eq_zero;
}

std::set<std::tuple<ocltl_type_mask, ocltl_type_mask, size_t>>
enumerate_phi_delta(const ocltl_phi_delta_dims& dims,
	const std::vector<ocltl_delta_atom>& atoms)
{
	size_t K = dims.k();
	size_t k_sigma = dims.d_m + dims.d_x;
	std::vector<size_t> keep_sigma, keep_rho;
	for (size_t i = 0; i < k_sigma; ++i) keep_sigma.push_back(i);
	for (size_t i = k_sigma; i < K; ++i) keep_rho.push_back(i);

	std::set<std::tuple<ocltl_type_mask, ocltl_type_mask, size_t>> reachable;
	for (ocltl_type_mask tau = 0; tau < ocltl_full_mask(K); ++tau) {
		ocltl_type_mask sigma = ocltl_restrict(tau, K, keep_sigma);
		ocltl_type_mask rho = ocltl_restrict(tau, K, keep_rho);
		size_t d = 0;
		for (size_t i = 0; i < atoms.size(); ++i)
			if (atom_holds_direct(tau, K, atoms[i])) d |= (size_t{1} << i);
		reachable.emplace(sigma, rho, d);
	}
	return reachable;
}

// Builds phi_delta for (dims, atoms), then checks the projected BDD against
// a direct enumeration at every (sigma, rho, D) point.
void check_pointwise(const ocltl_phi_delta_dims& dims,
	const std::vector<ocltl_delta_atom>& atoms)
{
	auto reachable = enumerate_phi_delta(dims, atoms);
	auto result = ocltl_build_phi_delta(dims, atoms);

	const size_t sigma_n = size_t{1} << result.sigma_vars.size();
	const size_t rho_n = size_t{1} << result.rho_vars.size();
	const size_t delta_n = size_t{1} << result.delta_vars.size();

	size_t checked = 0, mismatches = 0;
	for (ocltl_type_mask sigma = 0; sigma < sigma_n; ++sigma)
	for (ocltl_type_mask rho = 0; rho < rho_n; ++rho)
	for (size_t d = 0; d < delta_n; ++d) {
		bool expected = reachable.count({ sigma, rho, d }) > 0;

		ocltl_phi_delta_bdd point =
			bdd_handle<Bool, ocltl_phi_delta_bdd_options>::htrue;
		for (size_t b = 0; b < result.sigma_vars.size(); ++b)
			point = point & bdd_handle<Bool, ocltl_phi_delta_bdd_options>::bit(
				((sigma >> b) & 1) != 0, result.sigma_vars[b]);
		for (size_t b = 0; b < result.rho_vars.size(); ++b)
			point = point & bdd_handle<Bool, ocltl_phi_delta_bdd_options>::bit(
				((rho >> b) & 1) != 0, result.rho_vars[b]);
		for (size_t b = 0; b < result.delta_vars.size(); ++b)
			point = point & bdd_handle<Bool, ocltl_phi_delta_bdd_options>::bit(
				((d >> b) & 1) != 0, result.delta_vars[b]);

		bool actual = (result.relation & point) != false;
		++checked;
		if (actual != expected) ++mismatches;
	}
	CHECK(checked == sigma_n * rho_n * delta_n);
	CHECK(mismatches == 0);
}

// ── ocltl_phi_delta_direct cross-checks ─────────────────────────────────────

std::vector<bool> to_bits(size_t mask, size_t n) {
	std::vector<bool> v(n);
	for (size_t i = 0; i < n; ++i) v[i] = (mask >> i) & 1;
	return v;
}

// Cross-checks ocltl_phi_delta_direct against a full tau-mask enumeration at every (sigma, rho, D) point.
void check_direct_against_brute(const ocltl_phi_delta_dims& dims,
	const std::vector<ocltl_delta_atom>& atoms)
{
	auto reachable = enumerate_phi_delta(dims, atoms);
	size_t k_sigma = dims.d_m + dims.d_x, k_rho = dims.d_y;
	const size_t b_count = size_t{1} << k_sigma;
	const size_t c_count = size_t{1} << k_rho;
	const size_t sigma_n = size_t{1} << b_count;
	const size_t rho_n = size_t{1} << c_count;
	const size_t delta_n = size_t{1} << atoms.size();

	size_t checked = 0, mismatches = 0;
	for (size_t sigma = 0; sigma < sigma_n; ++sigma)
	for (size_t rho = 0; rho < rho_n; ++rho)
	for (size_t d = 0; d < delta_n; ++d) {
		bool expected = reachable.count({ sigma, rho, d }) > 0;
		bool actual = ocltl_phi_delta_direct(dims, atoms,
			to_bits(sigma, b_count), to_bits(rho, c_count), d);
		++checked;
		if (actual != expected) {
			++mismatches;
			if (mismatches <= 5)
				std::cout << "MISMATCH vs brute sigma=" << sigma << " rho=" << rho
					<< " d=" << d << " expected=" << expected
					<< " actual=" << actual << std::endl;
		}
	}
	CHECK(checked == sigma_n * rho_n * delta_n);
	CHECK(mismatches == 0);
}

// Cross-checks ocltl_phi_delta_direct against phi_delta's own BDD output at every (sigma, rho, D) point.
void check_direct_against_bdd(const ocltl_phi_delta_dims& dims,
	const std::vector<ocltl_delta_atom>& atoms)
{
	auto result = ocltl_build_phi_delta(dims, atoms);
	const size_t b_count = result.sigma_vars.size();
	const size_t c_count = result.rho_vars.size();
	const size_t sigma_n = size_t{1} << b_count;
	const size_t rho_n = size_t{1} << c_count;
	const size_t delta_n = size_t{1} << result.delta_vars.size();

	size_t checked = 0, mismatches = 0;
	for (size_t sigma = 0; sigma < sigma_n; ++sigma)
	for (size_t rho = 0; rho < rho_n; ++rho)
	for (size_t d = 0; d < delta_n; ++d) {
		ocltl_phi_delta_bdd point =
			bdd_handle<Bool, ocltl_phi_delta_bdd_options>::htrue;
		for (size_t b = 0; b < b_count; ++b)
			point = point & bdd_handle<Bool, ocltl_phi_delta_bdd_options>::bit(
				((sigma >> b) & 1) != 0, result.sigma_vars[b]);
		for (size_t b = 0; b < c_count; ++b)
			point = point & bdd_handle<Bool, ocltl_phi_delta_bdd_options>::bit(
				((rho >> b) & 1) != 0, result.rho_vars[b]);
		for (size_t b = 0; b < result.delta_vars.size(); ++b)
			point = point & bdd_handle<Bool, ocltl_phi_delta_bdd_options>::bit(
				((d >> b) & 1) != 0, result.delta_vars[b]);
		bool expected = (result.relation & point) != false;

		bool actual = ocltl_phi_delta_direct(dims, atoms,
			to_bits(sigma, b_count), to_bits(rho, c_count), d);
		++checked;
		if (actual != expected) {
			++mismatches;
			if (mismatches <= 5)
				std::cout << "MISMATCH vs bdd sigma=" << sigma << " rho=" << rho
					<< " d=" << d << " expected=" << expected
					<< " actual=" << actual << std::endl;
		}
	}
	CHECK(checked == sigma_n * rho_n * delta_n);
	CHECK(mismatches == 0);
}

// A deterministic xorshift bit vector, unrelated to any real spec.
std::vector<bool> sample_bits(size_t n, uint64_t seed) {
	std::vector<bool> v(n);
	uint64_t x = seed | 1;
	for (size_t i = 0; i < n; ++i) {
		x ^= x << 13; x ^= x >> 7; x ^= x << 17;
		v[i] = (x & 1) != 0;
	}
	return v;
}

// ── packed dimensions and synthetic deltas for the scaling sweep ───────────

// d_x = s; d_m = d_y = 2*s*l for l >= 1, or s for l = 0 -- s counts streams
// per side (inputs and outputs are equal in number and share this count).
ocltl_phi_delta_dims packed_dims(size_t s, size_t l) {
	size_t d_my = (l == 0) ? s : 2 * s * l;
	return { d_my, s, d_my };
}

// A deterministic spread of coordinate-eq and coordinate-const atoms over
// [0, K), unrelated to any real spec -- only |atoms| and K matter here.
std::vector<ocltl_delta_atom> synthetic_atoms(size_t K, size_t count) {
	std::vector<ocltl_delta_atom> atoms;
	for (size_t i = 0; i < count; ++i) {
		size_t p = i % K;
		if (i % 5 == 4) {
			atoms.push_back(ocltl_atom_coordinate_const(p, (i % 10) >= 5));
			continue;
		}
		size_t q = (i * 7 + 3) % K;
		if (q == p) q = (q + 1) % K;
		atoms.push_back(ocltl_atom_coordinate_eq(p, q));
	}
	return atoms;
}

std::string fmt_dur(std::chrono::steady_clock::duration d) {
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
	std::ostringstream ss;
	ss << ms << "ms";
	return ss.str();
}

// Times ocltl_phi_delta_direct on `n_samples` pseudo-random (sigma, rho, D) triples and prints stats.
void time_direct_samples(const ocltl_phi_delta_dims& dims, size_t delta_count,
	size_t n_samples)
{
	auto atoms = synthetic_atoms(dims.k(), delta_count);
	size_t k_sigma = dims.d_m + dims.d_x, k_rho = dims.d_y;
	size_t sigma_n = size_t{1} << k_sigma, rho_n = size_t{1} << k_rho;
	size_t delta_n = size_t{1} << atoms.size();

	std::chrono::steady_clock::duration total{};
	for (size_t s = 0; s < n_samples; ++s) {
		auto sigma = sample_bits(sigma_n, 0x9e3779b97f4a7c15ull * (s + 1));
		auto rho = sample_bits(rho_n, 0xbf58476d1ce4e5b9ull * (s + 1));
		size_t d = (s * 2654435761u) % delta_n;
		auto t0 = std::chrono::steady_clock::now();
		bool r = ocltl_phi_delta_direct(dims, atoms, sigma, rho, d);
		total += std::chrono::steady_clock::now() - t0;
		(void)r;
	}
	auto avg_us = std::chrono::duration_cast<std::chrono::microseconds>(
		total / n_samples).count();
	std::cout << "phi_delta_direct K=" << dims.k() << " k_sigma=" << k_sigma
		<< " k_rho=" << k_rho << " |D|=" << delta_count
		<< " samples=" << n_samples << " avg=" << avg_us << "us total="
		<< fmt_dur(total) << std::endl;
}

// Builds and projects phi_delta for one (s, l, |atoms|) point, printing its
// stats. A timeout or ceiling hit is reported, not treated as a test
// failure; returns false when the point did not complete.
bool report_point(size_t s, size_t l, size_t delta_count,
	std::optional<bool> force_sigma_major = std::nullopt)
{
	ocltl_phi_delta_dims dims = packed_dims(s, l);
	auto atoms = synthetic_atoms(dims.k(), delta_count);
	std::cout << "phi_delta s=" << s << " l=" << l << " |D|=" << delta_count
		<< " K=" << dims.k() << " starting..." << std::endl;
	auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(120);
	auto t0 = std::chrono::steady_clock::now();
	try {
		auto result = ocltl_build_phi_delta(dims, atoms, deadline, {}, force_sigma_major);
		auto t1 = std::chrono::steady_clock::now();
		std::cout << "phi_delta s=" << s << " l=" << l << " |D|=" << delta_count
			<< " K=" << dims.k() << " tau_bits=" << result.stats.tau_bits
			<< " sigma_bits=" << result.stats.sigma_bits
			<< " rho_bits=" << result.stats.rho_bits
			<< " nodes_pre=" << result.stats.nodes_before_projection
			<< " nodes_post=" << result.stats.nodes_after_projection
			<< " time=" << fmt_dur(t1 - t0) << std::endl;
		CHECK(result.stats.tau_bits == (size_t{1} << dims.k()));
		return true;
	} catch (const ocltl_phi_delta_timeout&) {
		std::cout << "phi_delta s=" << s << " l=" << l << " |D|=" << delta_count
			<< " K=" << dims.k() << " tau_bits=" << (size_t{1} << dims.k())
			<< " TIMEOUT (>120s)" << std::endl;
		return false;
	} catch (const ocltl_phi_delta_limit_exceeded& e) {
		std::cout << "phi_delta s=" << s << " l=" << l << " |D|=" << delta_count
			<< " K=" << dims.k() << " tau_bits=" << (size_t{1} << dims.k())
			<< " CEILING " << e.ceiling << " exceeded: " << e.value
			<< " > " << e.limit << std::endl;
		return false;
	}
}

// Sweeps |Delta| upward for one (s, l), stopping the group once a point
// times out or hits a ceiling (larger |Delta| only adds more work, never
// less).
void sweep_group(size_t s, size_t l, const std::vector<size_t>& delta_counts) {
	for (size_t dc : delta_counts) {
		auto t0 = std::chrono::steady_clock::now();
		bool completed = report_point(s, l, dc);
		auto elapsed = std::chrono::steady_clock::now() - t0;
		if (!completed || elapsed >= std::chrono::seconds(120)) break;
	}
}

// Reads a tests/codegen_specs/<name> fixture, trying both the ctest working
// directory and the repo root.
std::string read_codegen_spec(const std::string& name) {
	for (const char* prefix : { "codegen_specs/", "tests/codegen_specs/" }) {
		std::ifstream f(std::string(prefix) + name);
		if (f) {
			std::ostringstream ss;
			ss << f.rdbuf();
			return ss.str();
		}
	}
	return "";
}

// The same shallow parse solve_ltl_aba's own callers use: a raw parse
// through get_nso_rr, with no normalizer pass -- extract_data_atoms sees
// exactly this tree in the real pipeline (ltl_aba_builders.tmpl.h).
tref parse_spec(const std::string& src) {
	auto nso_rr = get_nso_rr<node_t>(tau::get(src));
	if (!nso_rr.has_value()) return nullptr;
	return nso_rr.value().main->get();
}

// ── Stage-1 reconstruction: Phi_delta over sigma-mask, rho-mask and D-flag
// variables only -- never a tau-bit variable. The deleted, never-committed
// ocltl_symbolic.h has nothing to resurrect from git; this rebuilds its
// row/col/U-set construction from the already-proved ocltl_phi_delta_direct
// characterization.
namespace stage1 {

using bddT = ocltl_phi_delta_bdd;

struct result {
	bddT relation;
	std::vector<int_t> sigma_vars, rho_vars, delta_vars;
	size_t final_nodes = 0;   // nodes reachable from the finished relation (the true answer size)
	size_t table_before = 0;  // manager's cumulative node table size before this build
	size_t table_after = 0;   // ... and after -- table_after - table_before is construction churn
};

// A cell's membership in phi_delta's U-set depends only on which atoms touch
// it; fold that in plain C++ (mirroring ocltl_phi_delta_direct's own
// excluded[]/U[] arrays) and cache the resulting small D-only BDD by
// touching-set, so structurally repeated cells (the common case for
// hub-shaped atoms) share one BDD subterm instead of rebuilding it.
result build(const ocltl_phi_delta_dims& dims,
	const std::vector<ocltl_delta_atom>& atoms,
	std::optional<std::chrono::steady_clock::time_point> deadline = std::nullopt,
	size_t node_ceiling = size_t{1} << 24)
{
	const size_t K = dims.k();
	const size_t k_sigma = dims.d_m + dims.d_x, k_rho = dims.d_y;
	const size_t sigma_n = size_t{1} << k_sigma, rho_n = size_t{1} << k_rho;
	const size_t tau_n = size_t{1} << K;
	const size_t n_atoms = atoms.size();
	REQUIRE_MESSAGE(n_atoms <= 32, "touch_mask is a uint32_t");

	auto check_deadline = [&] {
		if (deadline && std::chrono::steady_clock::now() > *deadline)
			throw ocltl_phi_delta_timeout{};
	};
	auto check_ceiling = [&] {
		size_t n = ocltl_phi_delta_detail::node_table_size();
		if (n > node_ceiling)
			throw ocltl_phi_delta_limit_exceeded{ "nodes", node_ceiling, n };
	};

	// shape-tagged ids, so repeat builds at other shapes in the same process
	// never inherit each other's ids.
	std::string tag = "ocltl_stage1$" + std::to_string(k_sigma) + "_"
		+ std::to_string(k_rho) + "_" + std::to_string(n_atoms) + "$";

	std::vector<int_t> sigma_ids(sigma_n), rho_ids(rho_n), delta_ids(n_atoms);
	for (size_t B = 0; B < sigma_n; ++B) sigma_ids[B] = var_dict(tag + "sigma$" + std::to_string(B));
	for (size_t C = 0; C < rho_n; ++C) rho_ids[C] = var_dict(tag + "rho$" + std::to_string(C));
	for (size_t i = 0; i < n_atoms; ++i) delta_ids[i] = var_dict(tag + "delta$" + std::to_string(i));

	auto bit_var = [](bool v, int_t id) {
		return bdd_handle<Bool, ocltl_phi_delta_bdd_options>::bit(v, id);
	};
	bddT T = bdd_handle<Bool, ocltl_phi_delta_bdd_options>::htrue;
	bddT F = bdd_handle<Bool, ocltl_phi_delta_bdd_options>::hfalse;

	size_t table_before = ocltl_phi_delta_detail::node_table_size();

	std::vector<std::vector<bool>> supports(n_atoms);
	for (size_t i = 0; i < n_atoms; ++i)
		supports[i] = ocltl_term_support(atoms[i].term, K);

	// forces_all_set(i): D_i itself for a plain atom, ~D_i for a negated one
	// (atom.negate == "term != 0") -- same polarity convention as
	// ocltl_phi_delta_direct's forces_all_set lambda.
	std::vector<bddT> forces_all_set(n_atoms), not_forces_all_set(n_atoms);
	for (size_t i = 0; i < n_atoms; ++i) {
		bddT d = bit_var(true, delta_ids[i]);
		forces_all_set[i] = atoms[i].negate ? ~d : d;
		not_forces_all_set[i] = ~forces_all_set[i];
	}

	std::vector<bddT> not_sigma(sigma_n), not_rho(rho_n);
	for (size_t B = 0; B < sigma_n; ++B) not_sigma[B] = ~bit_var(true, sigma_ids[B]);
	for (size_t C = 0; C < rho_n; ++C) not_rho[C] = ~bit_var(true, rho_ids[C]);

	std::unordered_map<uint32_t, bddT> allowed_cache;
	auto allowed_of = [&](uint32_t touch_mask) -> const bddT& {
		if (auto it = allowed_cache.find(touch_mask); it != allowed_cache.end())
			return it->second;
		bddT r = T;
		for (size_t i = 0; i < n_atoms; ++i)
			if ((touch_mask >> i) & 1) r = r & not_forces_all_set[i];
		return allowed_cache.emplace(touch_mask, r).first->second;
	};

	std::vector<bddT> row_inner(sigma_n, F), col_inner(rho_n, F), atom_hit(n_atoms, F);

	for (size_t A = 0; A < tau_n; ++A) {
		if ((A & 0xffff) == 0) { check_deadline(); check_ceiling(); }
		size_t B = A & (sigma_n - 1), C = A >> k_sigma;
		uint32_t touch_mask = 0;
		for (size_t i = 0; i < n_atoms; ++i)
			if (supports[i][A]) touch_mask |= (uint32_t{1} << i);
		const bddT& allowed = allowed_of(touch_mask);
		row_inner[B] = row_inner[B] | (not_rho[C] & allowed);
		col_inner[C] = col_inner[C] | (not_sigma[B] & allowed);
		if (touch_mask) {
			bddT term = not_sigma[B] & not_rho[C] & allowed;
			for (size_t i = 0; i < n_atoms; ++i)
				if ((touch_mask >> i) & 1) atom_hit[i] = atom_hit[i] | term;
		}
	}
	check_ceiling();

	// Phi_delta(sigma, rho, D) <=> U != {} & every unset row/col/atom hits U
	// -- ocltl_phi_delta_direct's own characterization, folded symbolically.
	bddT any_U = F;
	for (size_t B = 0; B < sigma_n; ++B) any_U = any_U | (not_sigma[B] & row_inner[B]);

	bddT row_check = T;
	for (size_t B = 0; B < sigma_n; ++B)
		row_check = row_check & (bit_var(true, sigma_ids[B]) | row_inner[B]);

	bddT col_check = T;
	for (size_t C = 0; C < rho_n; ++C)
		col_check = col_check & (bit_var(true, rho_ids[C]) | col_inner[C]);

	bddT atom_check = T;
	for (size_t i = 0; i < n_atoms; ++i)
		atom_check = atom_check & (forces_all_set[i] | atom_hit[i]);

	bddT relation = any_U & row_check & col_check & atom_check;
	check_ceiling();

	result out;
	out.relation = relation;
	out.sigma_vars = std::move(sigma_ids);
	out.rho_vars = std::move(rho_ids);
	out.delta_vars = std::move(delta_ids);
	out.final_nodes = ocltl_bdd_node_count(relation);
	out.table_before = table_before;
	out.table_after = ocltl_phi_delta_detail::node_table_size();
	return out;
}

} // namespace stage1

// ── atomless2.tau's real 20 data atoms, packed per an (m, x, y) coordinate
// layout, generalized to arbitrary (s, l) so the same 20 templates filter
// down to smaller shapes: s=1 keeps only the stream-0 (o1/i1) templates,
// and l<2 drops templates whose lookback the shape can't carry. At s=2,
// l=2 (atomless2.tau's own shape) every template survives.
struct atom_ref { bool is_output; size_t stream; size_t lag; }; // lag: 0=[t], 1=[t-1], 2=[t-2]

struct atom_template {
	bool is_const;
	atom_ref a;
	bool const_val = false; // is_const only
	atom_ref b{};            // !is_const only
};

const std::vector<atom_template>& atomless2_templates() {
	static const std::vector<atom_template> v = {
		{ true, {true,0,0}, true },                        // o1[t]=T.
		{ true, {true,0,0}, false },                        // o1[t]=F.
		{ true, {true,1,0}, true },                         // o2[t]=T.
		{ true, {true,1,0}, false },                        // o2[t]=F.
		{ false, {true,0,0}, false, {true,1,0} },           // o1[t]=o2[t]
		{ false, {true,0,0}, false, {true,0,1} },           // o1[t]=o1[t-1]
		{ false, {true,0,0}, false, {true,0,2} },           // o1[t]=o1[t-2]
		{ false, {true,1,0}, false, {true,1,1} },           // o2[t]=o2[t-1]
		{ false, {true,1,0}, false, {true,1,2} },           // o2[t]=o2[t-2]
		{ false, {true,0,0}, false, {true,1,1} },           // o1[t]=o2[t-1]
		{ false, {true,1,0}, false, {true,0,1} },           // o2[t]=o1[t-1]
		{ false, {true,0,0}, false, {true,1,2} },           // o1[t]=o2[t-2]
		{ false, {true,1,0}, false, {true,0,2} },           // o2[t]=o1[t-2]
		{ false, {true,0,0}, false, {false,0,0} },          // o1[t]=i1[t]
		{ false, {true,0,0}, false, {false,0,1} },          // o1[t]=i1[t-1]
		{ false, {true,0,0}, false, {false,0,2} },          // o1[t]=i1[t-2]
		{ false, {true,1,0}, false, {false,1,0} },          // o2[t]=i2[t]
		{ false, {true,1,0}, false, {false,1,1} },          // o2[t]=i2[t-1]
		{ false, {true,1,0}, false, {false,1,2} },          // o2[t]=i2[t-2]
		{ false, {true,1,0}, false, {false,0,0} },          // o2[t]=i1[t]
	};
	return v;
}

// Coordinate index for `r` under packed_dims(s, l); nullopt if unrepresentable
// (stream doesn't exist, or lag deeper than what m/y can carry at this l).
// A current input ([t], lag 0) is read from d_x; every other reference is
// read from d_y when its lag fits (lag < l), else from d_m (= y[t-1], one
// step further back).
std::optional<size_t> atom_coord(const atom_ref& r, size_t s, size_t l) {
	if (r.stream >= s) return std::nullopt;
	size_t d_m = 2 * s * l, d_x = s;
	if (!r.is_output && r.lag == 0) return d_m + r.stream;
	size_t sub_off = r.is_output ? 0 : s * l;
	if (r.lag < l) return d_m + d_x + sub_off + r.stream * l + r.lag;
	if (r.lag >= 1 && r.lag <= l) return sub_off + r.stream * l + (r.lag - 1);
	return std::nullopt;
}

// atomless2.tau's real atoms, filtered to what (s, l) can represent -- s=1
// keeps 7 (stream-0 only), s=2/l=2 keeps all 20 (atomless2.tau's own shape).
std::vector<ocltl_delta_atom> real_atoms(size_t s, size_t l) {
	std::vector<ocltl_delta_atom> out;
	for (const auto& t : atomless2_templates()) {
		auto ca = atom_coord(t.a, s, l);
		if (!ca) continue;
		if (t.is_const) { out.push_back(ocltl_atom_coordinate_const(*ca, t.const_val)); continue; }
		auto cb = atom_coord(t.b, s, l);
		if (!cb) continue;
		out.push_back(ocltl_atom_coordinate_eq(*ca, *cb));
	}
	return out;
}

} // namespace

TEST_SUITE("ocltl_phi_delta: correctness") {

	TEST_CASE("bdd_init") {
		ocltl_phi_delta_bdd_init();
	}

	TEST_CASE("projected BDD agrees pointwise with brute-force T_3 enumeration (s=1, l=0)") {
		ocltl_phi_delta_bdd_init();
		ocltl_phi_delta_dims dims{ 1, 1, 1 }; // m, x, y each a single coordinate
		std::vector<ocltl_delta_atom> atoms{
			ocltl_atom_coordinate_eq(0, 1),   // coordinate 0 (m) == coordinate 1 (x)
			ocltl_atom_coordinate_eq(1, 2),   // coordinate 1 (x) == coordinate 2 (y)
			ocltl_atom_coordinate_const(0, true), // coordinate 0 (m) == the unit
		};

		auto result = ocltl_build_phi_delta(dims, atoms);
		REQUIRE(result.sigma_vars.size() == 4);
		REQUIRE(result.rho_vars.size() == 2);
		REQUIRE(result.delta_vars.size() == 3);
		CHECK(result.stats.tau_bits == 8);

		check_pointwise(dims, atoms);
	}

	TEST_CASE("projected BDD agrees pointwise with brute-force T_3 enumeration, general terms (s=1, l=0)") {
		ocltl_phi_delta_bdd_init();
		ocltl_phi_delta_dims dims{ 1, 1, 1 }; // coordinate 0 = m, 1 = x, 2 = y
		ocltl_delta_term_ptr m = ocltl_term_coordinate(0);
		ocltl_delta_term_ptr x = ocltl_term_coordinate(1);
		ocltl_delta_term_ptr y = ocltl_term_coordinate(2);

		std::vector<ocltl_delta_atom> atoms{
			{ ocltl_term_meet(x, y), false },                    // x & y = 0
			{ ocltl_term_complement(ocltl_term_join(x, y)), false }, // x | y = 1
			{ ocltl_term_meet(ocltl_term_complement(x), y), false },  // ~x & y = 0
			{ ocltl_term_join(                                    // (m & ~x) | (x & ~y) = 0
				ocltl_term_meet(m, ocltl_term_complement(x)),
				ocltl_term_meet(x, ocltl_term_complement(y))), false },
			{ ocltl_term_meet(m, x), true },                      // m & x != 0
		};

		check_pointwise(dims, atoms);
	}

	TEST_CASE("atom-free reachable (sigma, rho) pairs are exactly valid-sigma x valid-rho (k=3)") {
		ocltl_phi_delta_dims dims{ 1, 1, 1 };
		size_t k_sigma = dims.d_m + dims.d_x, k_rho = dims.d_y;
		std::vector<ocltl_delta_atom> no_atoms;
		auto reachable = enumerate_phi_delta(dims, no_atoms);
		std::set<std::pair<ocltl_type_mask, ocltl_type_mask>> got;
		for (auto& [sigma, rho, d] : reachable) got.insert({ sigma, rho });

		size_t expected_count = 0;
		for (ocltl_type_mask sigma = 0; sigma < ocltl_type_mask{1} << (size_t{1} << k_sigma); ++sigma) {
			if (!ocltl_is_valid_type(sigma, k_sigma)) continue;
			for (ocltl_type_mask rho = 0; rho < ocltl_type_mask{1} << (size_t{1} << k_rho); ++rho) {
				if (!ocltl_is_valid_type(rho, k_rho)) continue;
				CHECK(got.count({ sigma, rho }) == 1);
				++expected_count;
			}
		}
		CHECK(got.size() == expected_count);
	}
}

TEST_SUITE("ocltl_phi_delta: direct predicate vs brute force") {

	TEST_CASE("atom-free (k_sigma=2, k_rho=1)") {
		ocltl_phi_delta_dims dims{ 1, 1, 1 };
		std::vector<ocltl_delta_atom> no_atoms;
		check_direct_against_brute(dims, no_atoms);
	}

	TEST_CASE("coordinate atoms (k_sigma=2, k_rho=1)") {
		ocltl_phi_delta_dims dims{ 1, 1, 1 };
		std::vector<ocltl_delta_atom> atoms{
			ocltl_atom_coordinate_eq(0, 1),
			ocltl_atom_coordinate_eq(1, 2),
			ocltl_atom_coordinate_const(0, true),
		};
		check_direct_against_brute(dims, atoms);
	}

	TEST_CASE("general term atoms (k_sigma=2, k_rho=1)") {
		ocltl_phi_delta_dims dims{ 1, 1, 1 };
		ocltl_delta_term_ptr m = ocltl_term_coordinate(0);
		ocltl_delta_term_ptr x = ocltl_term_coordinate(1);
		ocltl_delta_term_ptr y = ocltl_term_coordinate(2);
		std::vector<ocltl_delta_atom> atoms{
			{ ocltl_term_meet(x, y), false },
			{ ocltl_term_complement(ocltl_term_join(x, y)), false },
			{ ocltl_term_meet(ocltl_term_complement(x), y), false },
			{ ocltl_term_join(
				ocltl_term_meet(m, ocltl_term_complement(x)),
				ocltl_term_meet(x, ocltl_term_complement(y))), false },
			{ ocltl_term_meet(m, x), true },
		};
		check_direct_against_brute(dims, atoms);
	}

	TEST_CASE("asymmetric split k_sigma=1, k_rho=2, atom-free") {
		ocltl_phi_delta_dims dims{ 1, 0, 2 }; // d_m=1, d_x=0 -> k_sigma=1; d_y=2 -> k_rho=2
		std::vector<ocltl_delta_atom> no_atoms;
		check_direct_against_brute(dims, no_atoms);
	}

	TEST_CASE("asymmetric split k_sigma=1, k_rho=2, coordinate atoms") {
		ocltl_phi_delta_dims dims{ 1, 0, 2 };
		std::vector<ocltl_delta_atom> atoms{
			ocltl_atom_coordinate_eq(0, 1),
			ocltl_atom_coordinate_eq(1, 2),
			ocltl_atom_coordinate_const(2, false),
		};
		check_direct_against_brute(dims, atoms);
	}

	TEST_CASE("k_sigma=2, k_rho=2, multiple atoms with mixed D") {
		ocltl_phi_delta_dims dims{ 2, 0, 2 }; // d_m=2, d_x=0 -> k_sigma=2; d_y=2 -> k_rho=2
		std::vector<ocltl_delta_atom> atoms{
			ocltl_atom_coordinate_eq(0, 2),
			ocltl_atom_coordinate_eq(1, 3),
			ocltl_atom_coordinate_const(0, true),
			ocltl_atom_coordinate_const(3, false),
		};
		check_direct_against_brute(dims, atoms);
	}

	TEST_CASE("k_sigma=3, k_rho=1, multiple atoms with mixed D") {
		ocltl_phi_delta_dims dims{ 2, 1, 1 }; // k_sigma=3, k_rho=1
		std::vector<ocltl_delta_atom> atoms{
			ocltl_atom_coordinate_eq(0, 1),
			ocltl_atom_coordinate_eq(1, 2),
			ocltl_atom_coordinate_eq(2, 3),
			ocltl_atom_coordinate_const(0, true),
		};
		check_direct_against_brute(dims, atoms);
	}
}

TEST_SUITE("ocltl_phi_delta: direct predicate vs BDD") {

	TEST_CASE("bdd_init") {
		ocltl_phi_delta_bdd_init();
	}

	TEST_CASE("k_sigma=3, k_rho=2 (K=5), atom-free") {
		ocltl_phi_delta_bdd_init();
		ocltl_phi_delta_dims dims{ 2, 1, 2 }; // k_sigma=3, k_rho=2
		std::vector<ocltl_delta_atom> no_atoms;
		check_direct_against_bdd(dims, no_atoms);
	}

	TEST_CASE("k_sigma=3, k_rho=2 (K=5), multiple atoms mixed D") {
		ocltl_phi_delta_bdd_init();
		ocltl_phi_delta_dims dims{ 2, 1, 2 };
		std::vector<ocltl_delta_atom> atoms{
			ocltl_atom_coordinate_eq(0, 3),
			ocltl_atom_coordinate_eq(1, 4),
			ocltl_atom_coordinate_const(2, true),
			ocltl_atom_coordinate_const(4, false),
		};
		check_direct_against_bdd(dims, atoms);
	}

	TEST_CASE("k_sigma=2, k_rho=3 (K=5), atom-free") {
		ocltl_phi_delta_bdd_init();
		ocltl_phi_delta_dims dims{ 1, 1, 3 }; // k_sigma=2, k_rho=3
		std::vector<ocltl_delta_atom> no_atoms;
		check_direct_against_bdd(dims, no_atoms);
	}

	TEST_CASE("k_sigma=2, k_rho=3 (K=5), multiple atoms mixed D") {
		ocltl_phi_delta_bdd_init();
		ocltl_phi_delta_dims dims{ 1, 1, 3 };
		std::vector<ocltl_delta_atom> atoms{
			ocltl_atom_coordinate_eq(0, 2),
			ocltl_atom_coordinate_eq(1, 3),
			ocltl_atom_coordinate_const(4, true),
			ocltl_atom_coordinate_const(3, false),
		};
		check_direct_against_bdd(dims, atoms);
	}

	// K=8 already exceeds the BDD build's default node ceiling, even atom-free.
	TEST_CASE("k_sigma=5, k_rho=3 (K=8), atom-free, does not build under default limits") {
		ocltl_phi_delta_bdd_init();
		ocltl_phi_delta_dims dims{ 4, 1, 3 }; // k_sigma=5, k_rho=3
		std::vector<ocltl_delta_atom> no_atoms;
		CHECK_THROWS_AS(ocltl_build_phi_delta(dims, no_atoms),
			const ocltl_phi_delta_limit_exceeded&);
	}
}

TEST_SUITE("ocltl_phi_delta: direct predicate scaling") {

	TEST_CASE("bdd_init") {
		ocltl_phi_delta_bdd_init();
	}

	TEST_CASE("K=9 (k_sigma=5, k_rho=4)") {
		time_direct_samples({ 4, 1, 4 }, 8, 20);
	}

	TEST_CASE("K=10 (k_sigma=6, k_rho=4)") {
		time_direct_samples({ 5, 1, 4 }, 8, 20);
	}

	TEST_CASE("K=18 (s=2, l=2, atomless2.tau's shape)") {
		time_direct_samples(packed_dims(2, 2), 20, 10);
	}
}

TEST_SUITE("ocltl_phi_delta: assumption checks against a real spec") {

	TEST_CASE("atomless2.tau data atoms are quantifier-free and its (s, l) match the packing formula") {
		std::string src = read_codegen_spec("atomless2.tau");
		REQUIRE_MESSAGE(!src.empty(), "tests/codegen_specs/atomless2.tau not found");

		tref fm = parse_spec(src);
		REQUIRE(fm != nullptr);

		auto atoms = extract_data_atoms<node_t>(fm);
		CHECK(atoms.size() == 20);

		std::set<std::string> input_names, output_names;
		int_t max_shift = 0;
		for (auto& [atom, name] : atoms) {
			// Every data atom is a bf-level comparison; the bf grammar has no
			// quantifier production, so none of these can contain wff_ex/wff_all.
			CHECK(tau::get(atom).find_top([](tref n) {
				const auto& t = tree<node_t>::get(n);
				return t.is(tau::wff_ex) || t.is(tau::wff_all);
			}) == nullptr);

			auto io_vars = tau::get(atom).select_top(is_child<node_t, tau::io_var>);
			for (tref v : io_vars) {
				const std::string& nm = get_var_name<node_t>(v);
				// select_top's is_child<> returns the "variable" wrapper, one
				// level above the io_var node io_var_direction expects.
				size_t dir = io_var_direction<node_t>(tau::trim(v));
				if (dir == 1) input_names.insert(nm);
				else if (dir == 2) output_names.insert(nm);
				max_shift = std::max(max_shift, get_io_var_shift<node_t>(v));
			}
		}

		// s counts streams per side: inputs and outputs are equal in number.
		CHECK(input_names.size() == output_names.size());
		size_t s = input_names.size();
		size_t l = static_cast<size_t>(max_shift);
		ocltl_phi_delta_dims dims = packed_dims(s, l);
		std::cout << "atomless2.tau: " << input_names.size() << " input stream(s), "
			<< output_names.size() << " output stream(s), s=" << s
			<< ", l=" << l << ", d_tau=" << dims.k()
			<< ", tau_bits=2^" << dims.k() << "\n";

		// The spec's own vocabulary: i1, i2, o1, o2.
		CHECK(input_names.size() == 2);
		CHECK(output_names.size() == 2);
		CHECK(s == 2);
		CHECK(max_shift == 2);
		CHECK(dims.k() == 18);
	}
}

TEST_SUITE("ocltl_phi_delta: id ordering comparison") {

	TEST_CASE("bdd_init") {
		ocltl_phi_delta_bdd_init();
	}

	// k_sigma=3 > k_rho=2: rho-major (the smaller dimension) is compared against sigma-major.
	TEST_CASE("sigma-major tau-bit ids (s=1, l=1, k_sigma=3, k_rho=2)") {
		ocltl_phi_delta_bdd_init();
		for (size_t dc : std::vector<size_t>{ 1, 2, 4, 8, 16, 20, 24 })
			if (!report_point(1, 1, dc, true)) break;
	}

	TEST_CASE("rho-major tau-bit ids (s=1, l=1, k_sigma=3, k_rho=2)") {
		ocltl_phi_delta_bdd_init();
		for (size_t dc : std::vector<size_t>{ 1, 2, 4, 8, 16, 20, 24 })
			if (!report_point(1, 1, dc, false)) break;
	}

	// k_sigma=2 < k_rho=5: the winning axis should flip to sigma.
	TEST_CASE("sigma-major tau-bit ids on swapped dims (k_sigma=2, k_rho=5)") {
		ocltl_phi_delta_bdd_init();
		ocltl_phi_delta_dims dims{ 1, 1, 5 };
		for (size_t dc : std::vector<size_t>{ 1, 2, 4, 8 }) {
			auto atoms = synthetic_atoms(dims.k(), dc);
			try {
				auto result = ocltl_build_phi_delta(dims, atoms, std::nullopt, {}, true);
				std::cout << "sigma-major |D|=" << dc
					<< " nodes_pre=" << result.stats.nodes_before_projection
					<< " nodes_post=" << result.stats.nodes_after_projection << std::endl;
			} catch (const ocltl_phi_delta_limit_exceeded& e) {
				std::cout << "sigma-major |D|=" << dc << " CEILING " << e.ceiling
					<< " exceeded: " << e.value << " > " << e.limit << std::endl;
				break;
			}
		}
	}

	TEST_CASE("rho-major tau-bit ids on swapped dims (k_sigma=2, k_rho=5)") {
		ocltl_phi_delta_bdd_init();
		ocltl_phi_delta_dims dims{ 1, 1, 5 };
		for (size_t dc : std::vector<size_t>{ 1, 2, 4, 8 }) {
			auto atoms = synthetic_atoms(dims.k(), dc);
			try {
				auto result = ocltl_build_phi_delta(dims, atoms, std::nullopt, {}, false);
				std::cout << "rho-major |D|=" << dc
					<< " nodes_pre=" << result.stats.nodes_before_projection
					<< " nodes_post=" << result.stats.nodes_after_projection << std::endl;
			} catch (const ocltl_phi_delta_limit_exceeded& e) {
				std::cout << "rho-major |D|=" << dc << " CEILING " << e.ceiling
					<< " exceeded: " << e.value << " > " << e.limit << std::endl;
				break;
			}
		}
	}
}

// One dims/atom-count shape per process: the shared BDD namespace remembers ids across shapes built in the same run.
void probe_point(const ocltl_phi_delta_dims& dims, size_t delta_count) {
	auto atoms = synthetic_atoms(dims.k(), delta_count);
	std::cout << "probe K=" << dims.k() << " k_sigma=" << (dims.d_m + dims.d_x)
		<< " k_rho=" << dims.d_y << " |D|=" << delta_count << " starting..." << std::endl;
	try {
		auto result = ocltl_build_phi_delta(dims, atoms);
		std::cout << "probe K=" << dims.k() << " |D|=" << delta_count
			<< " nodes_pre=" << result.stats.nodes_before_projection
			<< " nodes_post=" << result.stats.nodes_after_projection << std::endl;
	} catch (const ocltl_phi_delta_limit_exceeded& e) {
		std::cout << "probe K=" << dims.k() << " |D|=" << delta_count << " CEILING "
			<< e.ceiling << " exceeded: " << e.value << " > " << e.limit << std::endl;
	}
}

TEST_SUITE("ocltl_phi_delta: boundary probe") {

	TEST_CASE("bdd_init") {
		ocltl_phi_delta_bdd_init();
	}

	TEST_CASE("atom-free K=6 (k_sigma=4, k_rho=2)") {
		ocltl_phi_delta_bdd_init();
		probe_point({ 3, 1, 2 }, 0);
	}
	TEST_CASE("atom-free K=7 (k_sigma=4, k_rho=3)") {
		ocltl_phi_delta_bdd_init();
		probe_point({ 3, 1, 3 }, 0);
	}
	TEST_CASE("atom-free K=8 (k_sigma=5, k_rho=3)") {
		ocltl_phi_delta_bdd_init();
		probe_point({ 4, 1, 3 }, 0);
	}
	TEST_CASE("atom-free K=9 (k_sigma=5, k_rho=4)") {
		ocltl_phi_delta_bdd_init();
		probe_point({ 4, 1, 4 }, 0);
	}
	TEST_CASE("atom-free K=10 (k_sigma=6, k_rho=4)") {
		ocltl_phi_delta_bdd_init();
		probe_point({ 5, 1, 4 }, 0);
	}
	TEST_CASE("k_rho fixed at 3, k_sigma=6 (K=9)") {
		ocltl_phi_delta_bdd_init();
		probe_point({ 5, 1, 3 }, 0);
	}
	TEST_CASE("k_rho fixed at 3, k_sigma=9 (K=12)") {
		ocltl_phi_delta_bdd_init();
		probe_point({ 8, 1, 3 }, 0);
	}
	TEST_CASE("one-atom K=6 (k_sigma=4, k_rho=2)") {
		ocltl_phi_delta_bdd_init();
		probe_point({ 3, 1, 2 }, 1);
	}
	TEST_CASE("one-atom K=7 (k_sigma=4, k_rho=3)") {
		ocltl_phi_delta_bdd_init();
		probe_point({ 3, 1, 3 }, 1);
	}
	TEST_CASE("one-atom K=8 (k_sigma=5, k_rho=3)") {
		ocltl_phi_delta_bdd_init();
		probe_point({ 4, 1, 3 }, 1);
	}
	TEST_CASE("one-atom K=9 (k_sigma=5, k_rho=4)") {
		ocltl_phi_delta_bdd_init();
		probe_point({ 4, 1, 4 }, 1);
	}
}

TEST_SUITE("ocltl_phi_delta: scaling measurement") {

	TEST_CASE("bdd_init") {
		ocltl_phi_delta_bdd_init();
	}

	TEST_CASE("s=1, l=0 (tau_bits=8)") {
		ocltl_phi_delta_bdd_init();
		sweep_group(1, 0, { 1, 2, 4, 8 });
	}

	TEST_CASE("s=1, l=1 (tau_bits=32)") {
		ocltl_phi_delta_bdd_init();
		sweep_group(1, 1, { 1, 2, 4, 8, 16 });
	}

	TEST_CASE("s=1, l=2 (tau_bits=512)") {
		ocltl_phi_delta_bdd_init();
		sweep_group(1, 2, { 1, 2, 4, 8, 16 });
	}

	TEST_CASE("s=2, l=1 (tau_bits=1024)") {
		ocltl_phi_delta_bdd_init();
		sweep_group(2, 1, { 1, 2, 4, 8, 16, 20 });
	}

	TEST_CASE("s=2, l=2 (tau_bits=262144)") {
		ocltl_phi_delta_bdd_init();
		sweep_group(2, 2, { 1, 2, 4, 8, 16, 20 });
	}
}

// Spot-checks stage1's relation against ocltl_phi_delta_direct at n_samples
// pseudo-random (sigma, rho, D) points. ocltl_phi_delta_direct's sigma/rho
// parameters are full type masks, one bit per minterm of the sigma/rho
// sub-tuple, so an exhaustive sweep is only feasible at the smallest
// k_sigma/k_rho; this samples random masks of the right length instead,
// reused at every stage1 vs. direct comparison.
void spot_check_stage1_vs_direct(const ocltl_phi_delta_dims& dims,
	const std::vector<ocltl_delta_atom>& atoms, const stage1::result& result,
	size_t n_samples)
{
	const size_t k_sigma = dims.d_m + dims.d_x, k_rho = dims.d_y;
	const size_t sigma_n = size_t{1} << k_sigma, rho_n = size_t{1} << k_rho;
	const size_t delta_n = size_t{1} << atoms.size();
	size_t mismatches = 0;
	for (size_t s = 0; s < n_samples; ++s) {
		auto sigma_bits = sample_bits(sigma_n, 0x9e3779b97f4a7c15ull * (s + 1));
		auto rho_bits = sample_bits(rho_n, 0xbf58476d1ce4e5b9ull * (s + 1));
		size_t d = (s * 2654435761u) % delta_n;
		bool expected = ocltl_phi_delta_direct(dims, atoms, sigma_bits, rho_bits, d);
		stage1::bddT point = bdd_handle<Bool, ocltl_phi_delta_bdd_options>::htrue;
		for (size_t B = 0; B < sigma_n; ++B)
			point = point & bdd_handle<Bool, ocltl_phi_delta_bdd_options>::bit(
				sigma_bits[B], result.sigma_vars[B]);
		for (size_t C = 0; C < rho_n; ++C)
			point = point & bdd_handle<Bool, ocltl_phi_delta_bdd_options>::bit(
				rho_bits[C], result.rho_vars[C]);
		for (size_t b = 0; b < result.delta_vars.size(); ++b)
			point = point & bdd_handle<Bool, ocltl_phi_delta_bdd_options>::bit(
				((d >> b) & 1) != 0, result.delta_vars[b]);
		bool actual = (result.relation & point) != false;
		if (actual != expected) {
			++mismatches;
			std::cout << "SPOT-CHECK MISMATCH sample=" << s << " expected=" << expected
				<< " actual=" << actual << std::endl;
		}
	}
	std::cout << "spot-check: " << n_samples << " samples, " << mismatches << " mismatches" << std::endl;
	CHECK(mismatches == 0);
}

TEST_SUITE("ocltl_phi_delta: stage1 symbolic (sigma/rho/D only) correctness") {

	TEST_CASE("bdd_init") {
		ocltl_phi_delta_bdd_init();
	}

	// pointwise cross-check helper, mirroring check_pointwise / check_direct_against_bdd above.
	void check_stage1_pointwise(const ocltl_phi_delta_dims& dims,
		const std::vector<ocltl_delta_atom>& atoms)
	{
		auto reachable = enumerate_phi_delta(dims, atoms);
		auto result = stage1::build(dims, atoms);
		const size_t sigma_n = size_t{1} << result.sigma_vars.size();
		const size_t rho_n = size_t{1} << result.rho_vars.size();
		const size_t delta_n = size_t{1} << result.delta_vars.size();
		size_t checked = 0, mismatches = 0;
		for (size_t sigma = 0; sigma < sigma_n; ++sigma)
		for (size_t rho = 0; rho < rho_n; ++rho)
		for (size_t d = 0; d < delta_n; ++d) {
			bool expected = reachable.count({ sigma, rho, d }) > 0;
			stage1::bddT point = bdd_handle<Bool, ocltl_phi_delta_bdd_options>::htrue;
			for (size_t b = 0; b < result.sigma_vars.size(); ++b)
				point = point & bdd_handle<Bool, ocltl_phi_delta_bdd_options>::bit(
					((sigma >> b) & 1) != 0, result.sigma_vars[b]);
			for (size_t b = 0; b < result.rho_vars.size(); ++b)
				point = point & bdd_handle<Bool, ocltl_phi_delta_bdd_options>::bit(
					((rho >> b) & 1) != 0, result.rho_vars[b]);
			for (size_t b = 0; b < result.delta_vars.size(); ++b)
				point = point & bdd_handle<Bool, ocltl_phi_delta_bdd_options>::bit(
					((d >> b) & 1) != 0, result.delta_vars[b]);
			bool actual = (result.relation & point) != false;
			++checked;
			if (actual != expected) {
				++mismatches;
				if (mismatches <= 5)
					std::cout << "STAGE1 MISMATCH sigma=" << sigma << " rho=" << rho
						<< " d=" << d << " expected=" << expected
						<< " actual=" << actual << std::endl;
			}
		}
		CHECK(checked == sigma_n * rho_n * delta_n);
		CHECK(mismatches == 0);
	}

	TEST_CASE("agrees with brute-force T_3 enumeration (s=1, l=0)") {
		ocltl_phi_delta_bdd_init();
		ocltl_phi_delta_dims dims{ 1, 1, 1 };
		std::vector<ocltl_delta_atom> atoms{
			ocltl_atom_coordinate_eq(0, 1),
			ocltl_atom_coordinate_eq(1, 2),
			ocltl_atom_coordinate_const(0, true),
		};
		check_stage1_pointwise(dims, atoms);
	}

	TEST_CASE("agrees with brute-force T_3 enumeration, atom-free (k_sigma=2, k_rho=1)") {
		ocltl_phi_delta_bdd_init();
		ocltl_phi_delta_dims dims{ 1, 1, 1 };
		std::vector<ocltl_delta_atom> no_atoms;
		check_stage1_pointwise(dims, no_atoms);
	}

	TEST_CASE("agrees with brute-force T_3 enumeration, general terms (s=1, l=0)") {
		ocltl_phi_delta_bdd_init();
		ocltl_phi_delta_dims dims{ 1, 1, 1 };
		ocltl_delta_term_ptr m = ocltl_term_coordinate(0);
		ocltl_delta_term_ptr x = ocltl_term_coordinate(1);
		ocltl_delta_term_ptr y = ocltl_term_coordinate(2);
		std::vector<ocltl_delta_atom> atoms{
			{ ocltl_term_meet(x, y), false },
			{ ocltl_term_complement(ocltl_term_join(x, y)), false },
			{ ocltl_term_meet(ocltl_term_complement(x), y), false },
			{ ocltl_term_join(
				ocltl_term_meet(m, ocltl_term_complement(x)),
				ocltl_term_meet(x, ocltl_term_complement(y))), false },
			{ ocltl_term_meet(m, x), true },
		};
		check_stage1_pointwise(dims, atoms);
	}

	TEST_CASE("agrees with brute-force T_3 enumeration, asymmetric split (k_sigma=1, k_rho=2)") {
		ocltl_phi_delta_bdd_init();
		ocltl_phi_delta_dims dims{ 1, 0, 2 };
		std::vector<ocltl_delta_atom> atoms{
			ocltl_atom_coordinate_eq(0, 1),
			ocltl_atom_coordinate_eq(1, 2),
			ocltl_atom_coordinate_const(2, false),
		};
		check_stage1_pointwise(dims, atoms);
	}

	TEST_CASE("agrees with brute-force T_3 enumeration, mixed D (k_sigma=3, k_rho=1)") {
		ocltl_phi_delta_bdd_init();
		ocltl_phi_delta_dims dims{ 2, 1, 1 };
		std::vector<ocltl_delta_atom> atoms{
			ocltl_atom_coordinate_eq(0, 1),
			ocltl_atom_coordinate_eq(1, 2),
			ocltl_atom_coordinate_eq(2, 3),
			ocltl_atom_coordinate_const(0, true),
		};
		check_stage1_pointwise(dims, atoms);
	}

	TEST_CASE("agrees with ocltl_phi_delta_direct at real atomless2-shaped atoms, s=1 l=2 (K=9)") {
		ocltl_phi_delta_bdd_init();
		auto dims = packed_dims(1, 2);
		auto atoms = real_atoms(1, 2);
		REQUIRE(atoms.size() == 7);
		auto result = stage1::build(dims, atoms);
		spot_check_stage1_vs_direct(dims, atoms, result, 2000);
	}

	TEST_CASE("real atomless2.tau atom templates reproduce the K=18 assumption check (s=2, l=2)") {
		auto dims = packed_dims(2, 2);
		auto atoms = real_atoms(2, 2);
		CHECK(dims.k() == 18);
		CHECK(atoms.size() == 20);
	}
}

// Opt-in: TAU_OCLTL_RUN_STAGE1=1. Off by default -- the K=18/20-atom point
// is a bounded but potentially long, memory-heavy run, not a default ctest cost.
bool run_stage1() {
	const char* v = std::getenv("TAU_OCLTL_RUN_STAGE1");
	return v && *v && std::string(v) != "0";
}

void run_stage1_point(const std::string& label, size_t s, size_t l,
	size_t timeout_s = 900, size_t spot_samples = 200)
{
	if (!run_stage1()) {
		MESSAGE("TAU_OCLTL_RUN_STAGE1 not set; skipping " << label);
		return;
	}
	ocltl_phi_delta_bdd_init();
	auto dims = packed_dims(s, l);
	auto atoms = real_atoms(s, l);
	std::cout << "stage1 " << label << " s=" << s << " l=" << l << " K=" << dims.k()
		<< " |atoms|=" << atoms.size() << " table_before="
		<< ocltl_phi_delta_detail::node_table_size() << " starting..." << std::endl;
	auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(timeout_s);
	auto t0 = std::chrono::steady_clock::now();
	try {
		auto result = stage1::build(dims, atoms, deadline);
		auto t1 = std::chrono::steady_clock::now();
		std::cout << "stage1 " << label << " K=" << dims.k() << " |atoms|=" << atoms.size()
			<< " final_nodes=" << result.final_nodes
			<< " table_before=" << result.table_before
			<< " table_after=" << result.table_after
			<< " cumulative_delta=" << (result.table_after - result.table_before)
			<< " time=" << fmt_dur(t1 - t0) << std::endl;
		spot_check_stage1_vs_direct(dims, atoms, result, spot_samples);
	} catch (const ocltl_phi_delta_timeout&) {
		std::cout << "stage1 " << label << " K=" << dims.k() << " |atoms|=" << atoms.size()
			<< " TIMEOUT (>" << timeout_s << "s)" << std::endl;
	} catch (const ocltl_phi_delta_limit_exceeded& e) {
		std::cout << "stage1 " << label << " K=" << dims.k() << " |atoms|=" << atoms.size()
			<< " CEILING " << e.ceiling << " exceeded: " << e.value << " > " << e.limit << std::endl;
	}
}

TEST_SUITE("ocltl_phi_delta: stage1 measurement (opt-in, TAU_OCLTL_RUN_STAGE1=1)") {

	TEST_CASE("bdd_init") {
		ocltl_phi_delta_bdd_init();
	}

	TEST_CASE("K=9 (s=1, l=2) atomless2.tau's real single-stream atoms") {
		run_stage1_point("K9-1stream", 1, 2);
	}

	TEST_CASE("K=10 (s=2, l=1) atomless2.tau's real atoms, 1-step lookback only") {
		run_stage1_point("K10-2stream-l1", 2, 1);
	}

	TEST_CASE("K=15 (s=3, l=1) same atoms padded to a third (unused) stream") {
		run_stage1_point("K15-3stream-l1", 3, 1);
	}

	TEST_CASE("K=18 (s=2, l=2) atomless2.tau's full real 20 atoms -- headline point") {
		run_stage1_point("K18-full", 2, 2, 900, 500);
	}
}
