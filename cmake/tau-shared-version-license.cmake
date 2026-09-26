# Function version_license(template_file, output_file)
# takes a template file and replaces the following placeholders:
#   @VERSION@          with content of VERSION file
#   @LICENSE_CONTENT@  with content of LICENSE.md file
#   @COMMIT_DATE_ISO@  with the commit date in ISO format
# and writes the result to the output file
function(version_license template_file output_file)
	# @VERSION@
	set(version_file "${PROJECT_SOURCE_DIR}/VERSION")
	file(READ ${version_file} VERSION ENCODING UTF-8)
	string(STRIP "${VERSION}" VERSION)
	string(REPLACE "\"" "\\\"" VERSION "${VERSION}")

	# @LICENSE_CONTENT@
	set(license_file "${PROJECT_SOURCE_DIR}/LICENSE.md")
	if(NOT EXISTS ${license_file})
		set(license_file "${PROJECT_SOURCE_DIR}/LICENSE.txt")
	endif()
	file(READ ${license_file} LICENSE_CONTENT ENCODING UTF-8)
	# strip the UTF-8 BOM; the template's raw string literal needs no escaping
	string(ASCII 239 187 191 UTF8_BOM)
	string(REGEX REPLACE "^${UTF8_BOM}" "" LICENSE_CONTENT "${LICENSE_CONTENT}")

	# @COMMIT_DATE_ISO@
	execute_process(COMMAND git log -1 --format=%cs
			WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
			OUTPUT_VARIABLE COMMIT_DATE_ISO
			OUTPUT_STRIP_TRAILING_WHITESPACE
			ERROR_QUIET)
	if(NOT COMMIT_DATE_ISO)
		string(TIMESTAMP COMMIT_DATE_ISO "%Y-%m-%d" UTC)
	endif()
	string(REPLACE "\"" "\\\"" COMMIT_DATE_ISO "${COMMIT_DATE_ISO}")

	configure_file(${template_file} ${output_file} @ONLY)
endfunction()
