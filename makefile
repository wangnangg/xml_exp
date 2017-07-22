curr_dir:=${shell pwd}
compiler:=clang++
gtest_dir:=${curr_dir}/googletest/googletest
src_dir:=${curr_dir}/src
test_dir:=${curr_dir}/test
build_dir:=${curr_dir}/build
test_build_dir:=${curr_dir}/build/test
utest_exe:=${build_dir}/utest


cpp_files:=${wildcard ${src_dir}/*.cpp}
obj_files:=${addprefix ${build_dir}/,${notdir ${cpp_files:.cpp=.o}}}
test_cpp_files:=${wildcard ${test_dir}/*.cpp}
test_obj_files:=${addprefix ${test_build_dir}/,${notdir ${test_cpp_files:.cpp=.o}}}

deps=${obj_files:.o=.d} ${test_obj_files:.o=.d}

gtest_obj:=${test_build_dir}/gtest-all.o

gtest_flags:=-isystem ${gtest_dir}/include -I${gtest_dir}
flags:=-I${src_dir} -std=c++14 -Werror -MMD -g
link_flags:=-lstdc++ -pthread

.PHONY: run_utest
run_utest: ${utest_exe}
	cd ${test_dir} && ${utest_exe} ${utest_arg}

${utest_exe}: ${obj_files} ${test_obj_files} ${gtest_obj}
	${compiler} $^ ${link_flags} -o $@

${gtest_obj}: ${gtest_dir}/src/gtest-all.cc | ${test_build_dir}
	${compiler} ${gtest_flags} -c $^ -o $@

.PHONY: build
build: ${obj_files} ${test_obj_files} ${gtest_obj}
	${compiler} $^ ${link_flags} -o /dev/null

${build_dir}:
	mkdir -p $@

${test_build_dir}:
	mkdir -p $@

.PHONY: clean
clean:
	rm -rf build

${test_build_dir}/%.o: ${test_dir}/%.cpp | ${test_build_dir}
	${compiler} ${gtest_flags} ${flags} -c $< -o $@

${build_dir}/%.o: src/%.cpp | ${build_dir}
	${compiler} ${flags} -c $< -o $@

-include ${deps}
