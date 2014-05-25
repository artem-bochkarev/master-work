#include <vector>

template<typename INPUT_TYPE, typename OUTPUT_TYPE>
class Test
{
private:
	std::vector<INPUT_TYPE> input;
	std::vector<OUTPUT_TYPE> fixedOutput;
	std::vector<OUTPUT_TYPE> output;

public:
	Test() {}

	Test& appendInput(const INPUT_TYPE& s) {
		input.add(s);
		return *this;
	}

	Test& appendOutput(const OUTPUT_TYPE& s) {
		output.add(s);
		return *this;
	}

	Test& appendFixedOutput(const OUTPUT_TYPE& s) {
		fixedOutput.add(s);

		if (s.size() != 0) {
			for (String a : s.split(",")) {
				output.add(a.trim());
			}
		}
		return *this;
	}

	/*Test& appendEdge(Edge e) {
		Path res = this.copy();
		for (String s : e.actions) {
			res.fixedOutput.add(s);
		}
		return res;
	}*/

	int size() {
		return input.size() + output.size();
	}

	const std::vector<INPUT_TYPE>& getInput() {
		return input;
	}

	const std::vector<OUTPUT_TYPE>& getFixedOutput() {
		return fixedOutput;
	}

	const std::vector<OUTPUT_TYPE>& getOutput() {
		return output;
	}

	/*Path copy() {
		Path res = new Path();
		for (String s : input) {
			res.input.add(s);
		}
		for (String s : fixedOutput) {
			res.fixedOutput.add(s);
		}
		for (String s : output) {
			res.output.add(s);
		}
		return res;
	}*/

};