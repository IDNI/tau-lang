set(TAU_HEADER_CONTENT "")

string(REPLACE " " ";" TAU_HEADERS_LIST "${TAU_HEADERS}")

# CONTENT
foreach(header_file ${TAU_HEADERS_LIST})
        file(READ "${TAU_SOURCE_DIR}/${header_file}" HEADER_CONTENT ENCODING UTF-8)
        string(APPEND TAU_HEADER_CONTENT "\
\n\
// -----------------------------------------------------------------------------\n\
// ${header_file}\n\
\n\
${HEADER_CONTENT}\n\
\n\
\n")
endforeach()

message(STATUS "Building tau.h file ${TAU_HEADER_TEMPLATE}")

configure_file(
	${TAU_HEADER_TEMPLATE}
	${TAU_HEADER}
	@ONLY)