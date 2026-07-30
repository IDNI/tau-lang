# BA pack selection and the generated tau_pack.h (build tree only).
#
# Each BA declares itself in src/boolean_algebras/<id>/ba.cmake; this file
# globs those manifests, resolves the ids listed in TAU_BAS into the node<>
# type, and writes tau_pack.h. Adding an in-tree BA needs no edit here.
#
# Reserved id "tau": the wrapper BA. When listed in TAU_BAS the resolver emits
# node<tau_ba<base...>, base...>, wrapper first; every other enabled id is a
# base BA contributing its manifest's TAU_BA_TYPE in TAU_BAS order.

set(TAU_BAS_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}")

if(NOT DEFINED TAU_SOURCE_ROOT)
	set(TAU_SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/src"
		CACHE INTERNAL "Tau src root for BA manifests")
endif()

set(TAU_BAS "tau,qint,qlt,nlang,bv,sbf,hsb" CACHE STRING
	"Comma-separated enabled BA ids")

# Ordered preference for the Boolean carrier — the BA whose type core builds a
# plain 0/1 in. Passed through to the compile-time fold rather than resolved
# here, so each pack in the build takes the earliest listed id it holds.
set(TAU_BOOL_CARRIERS "bv,sbf,bool" CACHE STRING
	"Comma-separated BA ids, most preferred Boolean carrier first")

set(TAU_BA_INCLUDE_DIRS "")

#
# Register an out-of-tree BA. Call before tau_resolve_ba_pack().
#
macro(tau_register_ba NAME)
	set(_one_value PATH HEADER TYPE)
	set(_multi_value LINK_LIBS REQUIRES_PACKAGES)
	cmake_parse_arguments(_reg "" "${_one_value}" "${_multi_value}" ${ARGN})
	if(NOT _reg_PATH OR NOT _reg_HEADER OR NOT _reg_TYPE)
		message(FATAL_ERROR
			"tau_register_ba(${NAME}) requires PATH, HEADER, and TYPE")
	endif()
	get_filename_component(_reg_PATH "${_reg_PATH}" ABSOLUTE)
	set(TAU_BA_${NAME}_HEADER "${_reg_HEADER}")
	set(TAU_BA_${NAME}_TYPE "${_reg_TYPE}")
	set(TAU_BA_${NAME}_SOURCES "")
	set(TAU_BA_${NAME}_LINK_LIBS "${_reg_LINK_LIBS}")
	set(TAU_BA_${NAME}_REQUIRES_PACKAGES "${_reg_REQUIRES_PACKAGES}")
	list(APPEND TAU_BA_INCLUDE_DIRS "${_reg_PATH}")
endmacro()

# A macro, not a function: the loop writes TAU_BA_<id>_* into the caller's
# scope. `return()` here would leave the calling function, so the guard is an
# explicit if().
macro(_tau_load_ba_registry)
	if(NOT _TAU_BA_REGISTRY_LOADED)
		file(GLOB _tau_manifest_files
			"${TAU_SOURCE_ROOT}/boolean_algebras/*/ba.cmake")
		foreach(_mf ${_tau_manifest_files})
			unset(TAU_BA_ID)
			unset(TAU_BA_TYPE)
			unset(TAU_BA_HEADER)
			unset(TAU_BA_SOURCES)
			unset(TAU_BA_LINK_LIBS)
			unset(TAU_BA_REQUIRES_PACKAGES)
			include(${_mf})
			if(NOT TAU_BA_ID)
				message(FATAL_ERROR "ba.cmake missing TAU_BA_ID: ${_mf}")
			endif()
			set(TAU_BA_${TAU_BA_ID}_HEADER "${TAU_BA_HEADER}")
			if(TAU_BA_TYPE)
				set(TAU_BA_${TAU_BA_ID}_TYPE "${TAU_BA_TYPE}")
			endif()
			set(TAU_BA_${TAU_BA_ID}_SOURCES "${TAU_BA_SOURCES}")
			set(TAU_BA_${TAU_BA_ID}_LINK_LIBS "${TAU_BA_LINK_LIBS}")
			set(TAU_BA_${TAU_BA_ID}_REQUIRES_PACKAGES
				"${TAU_BA_REQUIRES_PACKAGES}")
		endforeach()
		set(_TAU_BA_REGISTRY_LOADED TRUE)
	endif()
endmacro()

#
# Resolve TAU_BAS into the node<> arguments and the per-pack data.
#
function(tau_resolve_ba_pack)
	_tau_load_ba_registry()

	string(REPLACE " " "" _bas_nospace "${TAU_BAS}")
	string(REPLACE "," ";" _ba_ids "${_bas_nospace}")

	set(_headers "")
	set(_sources_extra "")
	set(_link_libs "")
	set(_base_types "")
	set(_has_tau FALSE)

	foreach(_id ${_ba_ids})
		if(_id STREQUAL "")
			continue()
		endif()
		if(NOT DEFINED TAU_BA_${_id}_HEADER)
			message(FATAL_ERROR
				"Unknown BA id '${_id}' in TAU_BAS=${TAU_BAS}")
		endif()
		list(APPEND _headers "${TAU_BA_${_id}_HEADER}")
		if(TAU_BA_${_id}_SOURCES)
			list(APPEND _sources_extra ${TAU_BA_${_id}_SOURCES})
		endif()
		if(TAU_BA_${_id}_LINK_LIBS)
			list(APPEND _link_libs ${TAU_BA_${_id}_LINK_LIBS})
		endif()
		if(_id STREQUAL "tau")
			set(_has_tau TRUE)
		else()
			if(NOT DEFINED TAU_BA_${_id}_TYPE)
				message(FATAL_ERROR
					"BA '${_id}' has no TAU_BA_TYPE in its manifest")
			endif()
			list(APPEND _base_types "${TAU_BA_${_id}_TYPE}")
		endif()
	endforeach()

	if(_base_types STREQUAL "")
		message(FATAL_ERROR "TAU_BAS must enable at least one base BA")
	endif()

	list(JOIN _base_types ", " _base_types_str)

	if(_has_tau)
		set(_node_args "tau_ba<${_base_types_str}>, ${_base_types_str}")
	else()
		set(_node_args "${_base_types_str}")
	endif()

	list(REMOVE_DUPLICATES _link_libs)

	set(_required_packages "")
	foreach(_id ${_ba_ids})
		if(NOT _id STREQUAL "" AND TAU_BA_${_id}_REQUIRES_PACKAGES)
			list(APPEND _required_packages
				${TAU_BA_${_id}_REQUIRES_PACKAGES})
		endif()
	endforeach()
	list(REMOVE_DUPLICATES _required_packages)

	set(_type_name_literals "")
	foreach(_id ${_ba_ids})
		if(NOT _id STREQUAL "")
			list(APPEND _type_name_literals "\"${_id}\"")
		endif()
	endforeach()
	list(JOIN _type_name_literals ", " _type_names_str)

	set(TAU_BA_HEADERS "${_headers}" PARENT_SCOPE)
	set(TAU_BA_SOURCES_EXTRA "${_sources_extra}" PARENT_SCOPE)
	set(TAU_BA_LINK_LIBS "${_link_libs}" PARENT_SCOPE)
	set(TAU_BA_REQUIRED_PACKAGES "${_required_packages}" PARENT_SCOPE)
	set(TAU_PACK_NODE_ARGS "${_node_args}" PARENT_SCOPE)
	set(TAU_PACK_TYPE_NAMES "${_type_names_str}" PARENT_SCOPE)
	set(TAU_PACK_HAS_TAU "${_has_tau}" PARENT_SCOPE)
	set(TAU_PACK_BASE_BAS "${_base_types_str}" PARENT_SCOPE)
	set(TAU_PACK_FULL_BAS "${_node_args}" PARENT_SCOPE)
endfunction()

#
# Write tau_pack.h into the build tree.
#
function(tau_generate_pack_header)
	set(TAU_PACK_INCLUDE_DIR
		"${CMAKE_BINARY_DIR}/generated/include" CACHE INTERNAL
		"Directory containing the generated tau_pack.h")
	set(TAU_PACK_HEADER "${TAU_PACK_INCLUDE_DIR}/tau_pack.h")

	file(MAKE_DIRECTORY "${TAU_PACK_INCLUDE_DIR}")

	set(TAU_PACK_INCLUDES "")
	foreach(_h ${TAU_BA_HEADERS})
		string(APPEND TAU_PACK_INCLUDES "#include \"${_h}\"\n")
	endforeach()

	set(TAU_PACK_BA_DEFINES "")
	string(REPLACE " " "" _bas_nospace "${TAU_BAS}")
	string(REPLACE "," ";" _ba_ids "${_bas_nospace}")
	foreach(_id ${_ba_ids})
		if(_id STREQUAL "")
			continue()
		endif()
		string(MAKE_C_IDENTIFIER "${_id}" _id_ident)
		string(TOUPPER "${_id_ident}" _id_upper)
		string(APPEND TAU_PACK_BA_DEFINES
			"#define TAU_PACK_HAS_BA_${_id_upper} 1\n")
	endforeach()

	if(TAU_PACK_HAS_TAU)
		set(TAU_PACK_HAS_TAU_CPP "true")
	else()
		set(TAU_PACK_HAS_TAU_CPP "false")
	endif()

	# The carrier order reaches the fold as one macro rather than through
	# tau_pack.h: ba_pack_traits.h cannot include the generated header, which
	# includes tau_tree.h and so the traits themselves.
	string(REPLACE " " "" _carriers "${TAU_BOOL_CARRIERS}")
	if(_carriers STREQUAL "")
		message(FATAL_ERROR "TAU_BOOL_CARRIERS must name at least one BA id")
	endif()
	add_compile_definitions(TAU_PACK_BOOL_CARRIERS="${_carriers}")
	string(REPLACE "," ";" _carrier_ids "${_carriers}")
	set(_carrier_in_pack FALSE)
	foreach(_id ${_carrier_ids})
		if(_id IN_LIST _ba_ids)
			set(_carrier_in_pack TRUE)
		endif()
	endforeach()
	if(NOT _carrier_in_pack)
		message(WARNING
			"none of TAU_BOOL_CARRIERS='${TAU_BOOL_CARRIERS}' is in "
			"TAU_BAS='${TAU_BAS}'; each pack falls back to its first BA "
			"declaring can_host_bool")
	endif()

	configure_file(
		"${TAU_BAS_CMAKE_DIR}/tau_pack.h.in"
		"${TAU_PACK_HEADER}"
		@ONLY)

	add_custom_target(tau_pack_header ALL DEPENDS "${TAU_PACK_HEADER}")
endfunction()
