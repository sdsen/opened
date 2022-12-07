# OPENED Exraction Tool

LPC 2022 blurb describing the goal of the tool and an initial prototype is here: https://lpc.events/event/16/contributions/1370/ 
 
## Dependencies
 1. Works on kernel verion 5.4.0-131, Ubuntu 22:04, Intel arch x86 arch. There is a known issue with Apple Silicon based Macbooks with installing a) ``gcc-multlib`` and b) TXL.
 2. git
 3. Docker
 
## Download
 1. ``git clone --recurse-submodules git@github.com:sdsen/opened_extraction.git``
 2. ``cd opened_extraction``
 3. 
 4. To update the submodules a) ``git submodule update --remote --merge`` b) ``cd codequery; git pull``
 
## Install Process 1: Docker
 1. ``mkdir op`` To store the output of extraction phase (or any other folder name)
 2.  ``docker build . -t opened/extract:0.01``

## Install Process 2: on host
 1. **For now:** You will need to parse the Dockerfile and execute the installation steps on your host system.
 2. In future we will provide a script for on-host installation ([Issue #24](https://github.com/sdsen/opened_extraction/issues/24)).
 
## Pulling local update
 1. run ``git pull``
 2. run ``git submodule update --recursive`` 
 3. If you have docker for install, you are done. 
 4. If you have on-host install, you will need to re-install ``codequery`` by running the relevant instructions from Dockerfile.

## Extraction code and artefacts

Code extraction consists of two phases 1) Generating annotated function call graph 2) Extracting required code from source files to generate an independantly compilable module.

## Run
### Phase I: Annotated Function Call Generation

 1. Run the docker. ``docker run -it  --mount type=bind,src=<source_code_dir_on_host>,dst=/root/examples/katran --mount type=bind,src=op, dst=/root/op opened/extract:0.01``. Where ``op`` is the folder created in step Install.3 . The output is expected to be dumped in this folder, so that it is available for later processing/use in host system. 
 2. Run extraction phase 1, ``python3 extraction_runner.py -f <function_name> -s <source_folder> -o <txl_output>``, an **example is given in run1.sh**


### Phase II
1. Open the func.out file and remove the duplicate function and struct definitions. A cleaned **func.out.cleaned is shown in asset folder**. This will output an annotated function call graph in a file named func.out. Note that func.out may have duplicate function defintions. We expect the developer to disambiguate and identify the required set of functions to be extracted in Phase II.

### Phase III: Extracting Required Code
2. ``python3 function-extractor.py -o/--opdir, -c/--codequeryOutputFile, -e/--extractedFileName,  -t/--txlDir, -s/--srcdir,`` an **example is given in run2.sh**


Note that extracted.c may contain duplicate eBPF Map defintions within and ```ATTENTION``` section. We expect the developer to choose the right map definition and delete the offending defintion.


*Compilation*

Run `make` to compile the extracted code.
