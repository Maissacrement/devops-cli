
#!make
SOURCE_FILE=main
OUTPUT=devops
LIBRARY=-lboost_program_options -lpcap
html_file="src/html/index.html" # Replace with your data file name
output_file="src/html/indexHTML.hpp" # Replace with the desired header file name

CURRENT_DIR := $(shell pwd)
ARCH := $(shell uname -m)

# Définissez votre variable en fonction de l'architecture
ifeq ($(ARCH), aarch64)
    LIBRARY := -lboost_program_options -lpcap -g -I ~/boost-build/boost_1_81_0 -Wall
endif

define binaryToCppFile
    @echo "unsigned char $1[] = {" > ${CURRENT_DIR}/$2
	@hexdump -v -e '16/1 "0x%02x, " "\n"' ${CURRENT_DIR}/$3 | sed 's/0x[[:space:]]*,//g' >> ${CURRENT_DIR}/$2
	@echo "};" >> ${CURRENT_DIR}/$2
	@echo "unsigned int $1_len = sizeof($1);" >> ${CURRENT_DIR}/$2
	@echo "Header file $2 generated."
endef

clean-src:
	@rm -rvf ${CURRENT_DIR}/src

clean:
	@rm -rvf ${CURRENT_DIR}/bin

install:
	@mv ${CURRENT_DIR}/bin/${OUTPUT} /usr/bin/

build: clean
	$(call binaryToCppFile,src_html_index_html,$(output_file),$(html_file))
	$(call binaryToCppFile,env,src/html/env.hpp,src/html/.env)
	
	@mkdir -p ${CURRENT_DIR}/bin
	@g++ -Wall ${CURRENT_DIR}/src/${SOURCE_FILE}.cpp -o ${CURRENT_DIR}/bin/${OUTPUT} ${LIBRARY}

run:
	@${CURRENT_DIR}/bin/${OUTPUT}

${OUTPUT}: build install