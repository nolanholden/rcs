#!/usr/bin/env bash

readonly PROJECT_ROOT=.

readonly DOXY_CONFIG_FILE=${PROJECT_ROOT}/doxygen.config

readonly DOXY_OUTPUT_ROOT=${PROJECT_ROOT}/documentation
readonly DOXY_OUTPUT_HTML=${DOXY_OUTPUT_ROOT}/html
readonly DOXY_OUTPUT_HTML_INDEX=${DOXY_OUTPUT_HTML}/index.html

# Clear the old report.
rm -rf ${DOXY_OUTPUT_ROOT}
mkdir ${DOXY_OUTPUT_ROOT}

# Generate.
doxygen ${DOXY_CONFIG_FILE}

# Open it. (without forking)
#open ${INDEX_PATH}                     # linux
start ${DOXY_OUTPUT_HTML_INDEX} # windows
