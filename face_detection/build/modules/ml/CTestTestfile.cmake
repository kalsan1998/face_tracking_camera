# CMake generated Testfile for 
# Source directory: /home/kalsan/src/face_detection/opencv-master/modules/ml
# Build directory: /home/kalsan/src/face_detection/build/modules/ml
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(opencv_test_ml "/home/kalsan/src/face_detection/build/bin/opencv_test_ml" "--gtest_output=xml:opencv_test_ml.xml")
set_tests_properties(opencv_test_ml PROPERTIES  LABELS "Main;opencv_ml;Accuracy" WORKING_DIRECTORY "/home/kalsan/src/face_detection/build/test-reports/accuracy")
