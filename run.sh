make
cd sample_tests/sample_test_1/
./../../fs input1
# valgrind --tool=memcheck --leak-check=yes ./fs sample_tests/sample_test_1/input1
# ./fs disk0