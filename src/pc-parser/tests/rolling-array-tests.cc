#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "../src/rolling-array.hh"

TEST_CASE("RollingArray can be initialized with the max array size.", "[RollingArray]" ) {
    RollingArray<double> test_array(100);
    for(int i = 0;i<100;i++){
        test_array.append((double)i);
    }
    REQUIRE(test_array.getSize()==100);

    SECTION("RollingArray has an index operator."){
        REQUIRE(test_array[50] == 50.f);
    }
    
    SECTION("RollingArray shifts data to the right when appended with new data."){
        REQUIRE(test_array[0]==0.f);
        test_array.append(101.f);
        REQUIRE(test_array[0]==1.f);
    }

}

