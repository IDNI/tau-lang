// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md
#pragma once

// --- Parse options constants ---

// default (inference + hooks enabled)
inline tau::get_options parse_opts_bf{
	.parse = { .start = tau::bf } };
inline tau::get_options parse_opts_wff{
	.parse = { .start = tau::wff } };

// no-hooks (inference enabled, hooks disabled)
inline tau::get_options parse_opts_wff_no_hooks{
	.parse = { .start = tau::wff }, .reget_with_hooks = false };

// no-infer (inference + hooks disabled)
inline tau::get_options parse_opts_bf_no_infer{
	.parse = { .start = tau::bf },
	.infer_ba_types = false, .reget_with_hooks = false };
inline tau::get_options parse_opts_wff_no_infer{
	.parse = { .start = tau::wff },
	.infer_ba_types = false, .reget_with_hooks = false };
inline tau::get_options parse_opts_cli_no_infer{
	.parse = { .start = tau::cli },
	.infer_ba_types = false, .reget_with_hooks = false };
inline tau::get_options parse_opts_no_infer{
	.parse = {},
	.infer_ba_types = false, .reget_with_hooks = false };
inline tau::get_options parse_opts_definitions_no_infer{
	.parse = { .start = tau::definitions },
	.infer_ba_types = false, .reget_with_hooks = false };
