#include <string>

class Param {
	public:
		Param( std::string, std::string, std::string, int, int );
		int length;
		int width;
		std::string line;
		std::string name;
		std::string value;
};

class Params {
	public:
		Params( std::string );
		Param* GetParam( std::string );
		std::string contents;

		void AddParam( std::string );
};
