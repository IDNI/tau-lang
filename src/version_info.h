// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__VERSION_INFO_H__
#define __IDNI__TAU__VERSION_INFO_H__

// Defined in the version_license.cpp the build generates from
// cmake/version_license.cpp.template, VERSION and LICENSE.md.
namespace idni::tau_lang {

extern const char commit_date[];
extern const char version[];
extern const char full_version[];
extern const char license[];

}

#endif // __IDNI__TAU__VERSION_INFO_H__
