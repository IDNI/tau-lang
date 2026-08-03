set(TAU_HEADER_CONTENT "")

string(REPLACE " " ";" TAU_HEADERS_LIST "${TAU_HEADERS}")

# CONTENT
foreach(header_file ${TAU_HEADERS_LIST})
        # a generated header arrives as an absolute build-tree path; a source
        # one is relative to TAU_SOURCE_DIR
        if(IS_ABSOLUTE "${header_file}")
                set(_path "${header_file}")
        else()
                set(_path "${TAU_SOURCE_DIR}/${header_file}")
        endif()
        file(READ "${_path}" HEADER_CONTENT ENCODING UTF-8)
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