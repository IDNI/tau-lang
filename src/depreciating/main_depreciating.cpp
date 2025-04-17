// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "utility/cli.h"
#include "depreciating/boolean_algebras/sbf_ba_depreciating.h"
#include "depreciating/repl_evaluator_depreciating.h"

using namespace std;
using namespace idni;
using namespace idni::tau_lang_depreciating;

void welcome();

int run_repl_depreciating(cli::options& opts, bool charvar,
	boost::log::trivial::severity_level sev, bool exp)
{
	repl_evaluator<sbf_ba> re(
	{
		.status = opts["status"].get<bool>(),
		.colors = opts["color"].get<bool>(),
		.charvar = charvar,
#ifdef DEBUG
		.debug_repl = opts["debug"].get<bool>(),
#endif // DEBUG
		.severity = sev,
		.experimental = exp
	});
	string e = opts["evaluate"].get<string>();
	if (e.size()) return re.eval(e), 0;
	repl<decltype(re)> r(re, "tau> ", ".tau_history");
	welcome();
	re.prompt();
	return r.run();
}
