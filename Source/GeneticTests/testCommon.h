#include <string>
typedef float FITNES_TYPE;
typedef int INPUT_TYPE;
typedef int OUTPUT_TYPE;

typedef int TEST_AUTOMAT;

typedef std::string FORMULA_TYPE;

#include "Test.hpp"
typedef Test<INPUT_TYPE, OUTPUT_TYPE> TEST;
typedef TEST TEST_TYPE;

static const int max_transitions = 5;
static const int input_any = -1;
static const int input_null = -2;