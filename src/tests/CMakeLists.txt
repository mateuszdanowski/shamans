add_executable(bottomlessBagTest.cpp)
add_executable(sandArrangementTest.cpp)
add_executable(crystalSelectionTest.cpp)


target_link_libraries( bottomlessBagTest pthread )

target_link_libraries( sandArrangementTest pthread )

target_link_libraries( crystalSelectionTest pthread )

