cmake_minimum_required(VERSION 3.5)

project(build-info)

# VARS
SET(BINARY_NAME ${PROJECT_NAME})
SET(LIB_NAME ${PROJECT_NAME}-lib)
SET(TEST_BINARY_NAME ${PROJECT_NAME}-test)

execute_process(COMMAND "git" "rev-parse" "--abbrev-ref" "HEAD"
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    RESULT_VARIABLE git_result
    OUTPUT_VARIABLE BRANCH_NAME)

execute_process(COMMAND "git" "rev-list" "HEAD" "--count"
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    RESULT_VARIABLE git_result
    OUTPUT_VARIABLE COMMIT_COUNT)

execute_process(COMMAND "git" "rev-parse" "--short" "HEAD"
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    RESULT_VARIABLE git_result
    OUTPUT_VARIABLE HEAD_REF)
    
execute_process(COMMAND "git" "log" "-n" "1" "--pretty=format:%cd" "--date=format:%Y-%m-%d %H:%M:%S"
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    RESULT_VARIABLE git_result
    OUTPUT_VARIABLE COMMIT_DATE)

string(STRIP ${COMMIT_COUNT} COMMIT_COUNT)
string(STRIP ${HEAD_REF} HEAD_REF)
string(STRIP ${BRANCH_NAME} BRANCH_NAME)

message("Build info:")    
message("  branch     : " ${BRANCH_NAME})    
message("  commit date: " ${COMMIT_DATE}) 
message("  commit id  : " ${HEAD_REF}) 

# CONFIGURED FILES
configure_file(build-info.h.in build-info.h)
