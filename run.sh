make
# ./create_fs sample_tests/sample_test_3/disk1
# ./create_fs sample_tests/sample_test_3/disk2
./create_fs sample_tests/sample_test_4/clean_disk
cd sample_tests/sample_test_4/
# ./../../fs input1
./../../fs trivial-input
# cd consistency-check
# ./../fs consistency-input
# valgrind --tool=memcheck --leak-check=yes ./fs sample_tests/sample_test_1/input1
# ./fs disk0