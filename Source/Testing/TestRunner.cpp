#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

TEST_CASE("Unit Test Works") {
	REQUIRE(1 == 1);
}

int main(int argc, char* argv[]) {
	int result = Catch::Session().run(argc, argv);
	system("pause");
	return result;
}