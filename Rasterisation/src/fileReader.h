#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

class Lines
{
public:
	Lines(std::string contents) {
		std::stringstream line(contents);
		line >> i;
		line >> x;
		line >> y;
		line >> z;
	};
private:
	std::string i, x, y, z;
};

std::vector<Lines*> items;

void OBJReader(const std::string filename) {
	std::ifstream scoreFile(filename);
	if (!scoreFile) std::cerr << "Could not load file!";

	std::string lineContents;
	while (!scoreFile.eof())
	{
		getline(scoreFile, lineContents);
		items.emplace_back(new Lines(lineContents));
	}
	scoreFile.close();
}