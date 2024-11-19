set(license_file "${PROJECT_SOURCE_DIR}/LICENSE.txt")
set(output_file "${PROJECT_SOURCE_DIR}/src/version_license.h")

file(READ ${license_file} LICENSE_CONTENT ENCODING UTF-8)

# remove BOM if any. Expects word License to be the start of the text
string(FIND "${LICENSE_CONTENT}" "License" LICENSE_CONTENT_START)
string(SUBSTRING "${LICENSE_CONTENT}" ${LICENSE_CONTENT_START} -1 LICENSE_CONTENT)

string(REPLACE "\"" "\\\"" LICENSE_CONTENT "${LICENSE_CONTENT}")

execute_process(COMMAND date --iso
		OUTPUT_VARIABLE COMPILE_DATE_ISO
		OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE "\"" "\\\"" COMPILE_DATE_ISO "${COMPILE_DATE_ISO}")

configure_file(
	${PROJECT_SOURCE_DIR}/cmake/version_license.h.template
	${output_file}
	@ONLY)
