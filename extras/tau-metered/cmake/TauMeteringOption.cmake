include_guard(GLOBAL)
option(TAU_BUILD_METERED_NATIVE "Build source-only, opt-in metered Tau host (experimental)" OFF)
if(NOT TAU_BUILD_METERED_NATIVE)
  return()
endif()
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include("${CMAKE_CURRENT_LIST_DIR}/TauMeteredSourceDeps.cmake")
add_library(tau_metered_native STATIC "${CMAKE_CURRENT_LIST_DIR}/../src/metered_tau.cpp")
add_library(TAU::METERED ALIAS tau_metered_native)
set_target_properties(tau_metered_native PROPERTIES EXPORT_NAME METERED)
target_compile_features(tau_metered_native PUBLIC cxx_std_20 PRIVATE cxx_std_23)
target_include_directories(tau_metered_native PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../src>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/tau-metered>")
target_link_libraries(tau_metered_native PRIVATE OpenSSL::Crypto Threads::Threads TauMeteredSourceDeps::Wasmtime)
set(config_dir "${CMAKE_INSTALL_LIBDIR}/cmake/TauMeteredSource")
configure_package_config_file("${CMAKE_CURRENT_LIST_DIR}/TauMeteredSourceConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/TauMeteredSourceConfig.cmake" INSTALL_DESTINATION "${config_dir}")
install(TARGETS tau_metered_native EXPORT TauMeteredSourceTargets ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT MeteredSDK)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../src/metered_tau.h" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/tau-metered" COMPONENT MeteredSDK)
install(EXPORT TauMeteredSourceTargets NAMESPACE TAU:: DESTINATION "${config_dir}" COMPONENT MeteredSDK)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/TauMeteredSourceDeps.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/TauMeteredSourceConfig.cmake" DESTINATION "${config_dir}" COMPONENT MeteredSDK)
