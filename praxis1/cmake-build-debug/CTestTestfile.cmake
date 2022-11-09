# CMake generated Testfile for 
# Source directory: /home/caro/Documents/RN_22_23/praxis1
# Build directory: /home/caro/Documents/RN_22_23/praxis1/cmake-build-debug
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(Execute "/home/caro/Documents/RN_22_23/praxis1/test/execute.py" "/home/caro/Documents/RN_22_23/praxis1/cmake-build-debug/webserver")
set_tests_properties(Execute PROPERTIES  _BACKTRACE_TRIPLES "/home/caro/Documents/RN_22_23/praxis1/CMakeLists.txt;14;add_test;/home/caro/Documents/RN_22_23/praxis1/CMakeLists.txt;0;")
add_test(Listen "/home/caro/Documents/RN_22_23/praxis1/test/listen.py" "/home/caro/Documents/RN_22_23/praxis1/cmake-build-debug/webserver")
set_tests_properties(Listen PROPERTIES  _BACKTRACE_TRIPLES "/home/caro/Documents/RN_22_23/praxis1/CMakeLists.txt;15;add_test;/home/caro/Documents/RN_22_23/praxis1/CMakeLists.txt;0;")
add_test(Reply "/home/caro/Documents/RN_22_23/praxis1/test/reply.py" "/home/caro/Documents/RN_22_23/praxis1/cmake-build-debug/webserver")
set_tests_properties(Reply PROPERTIES  _BACKTRACE_TRIPLES "/home/caro/Documents/RN_22_23/praxis1/CMakeLists.txt;16;add_test;/home/caro/Documents/RN_22_23/praxis1/CMakeLists.txt;0;")
add_test(Packets "/home/caro/Documents/RN_22_23/praxis1/test/packets.py" "/home/caro/Documents/RN_22_23/praxis1/cmake-build-debug/webserver")
set_tests_properties(Packets PROPERTIES  _BACKTRACE_TRIPLES "/home/caro/Documents/RN_22_23/praxis1/CMakeLists.txt;17;add_test;/home/caro/Documents/RN_22_23/praxis1/CMakeLists.txt;0;")
add_test(HTTPReply "/home/caro/Documents/RN_22_23/praxis1/test/httpreply.py" "/home/caro/Documents/RN_22_23/praxis1/cmake-build-debug/webserver")
set_tests_properties(HTTPReply PROPERTIES  _BACKTRACE_TRIPLES "/home/caro/Documents/RN_22_23/praxis1/CMakeLists.txt;18;add_test;/home/caro/Documents/RN_22_23/praxis1/CMakeLists.txt;0;")
add_test(HTTPReplies "/home/caro/Documents/RN_22_23/praxis1/test/httpreplies.py" "/home/caro/Documents/RN_22_23/praxis1/cmake-build-debug/webserver")
set_tests_properties(HTTPReplies PROPERTIES  _BACKTRACE_TRIPLES "/home/caro/Documents/RN_22_23/praxis1/CMakeLists.txt;19;add_test;/home/caro/Documents/RN_22_23/praxis1/CMakeLists.txt;0;")
add_test(StaticContent "/home/caro/Documents/RN_22_23/praxis1/test/static_content.py" "/home/caro/Documents/RN_22_23/praxis1/cmake-build-debug/webserver")
set_tests_properties(StaticContent PROPERTIES  _BACKTRACE_TRIPLES "/home/caro/Documents/RN_22_23/praxis1/CMakeLists.txt;20;add_test;/home/caro/Documents/RN_22_23/praxis1/CMakeLists.txt;0;")
add_test(DynamicContent "/home/caro/Documents/RN_22_23/praxis1/test/dynamic_content.py" "/home/caro/Documents/RN_22_23/praxis1/cmake-build-debug/webserver")
set_tests_properties(DynamicContent PROPERTIES  _BACKTRACE_TRIPLES "/home/caro/Documents/RN_22_23/praxis1/CMakeLists.txt;21;add_test;/home/caro/Documents/RN_22_23/praxis1/CMakeLists.txt;0;")
