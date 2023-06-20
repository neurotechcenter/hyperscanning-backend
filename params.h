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
		Params();
		Param* GetParam( std::string );
		std::string contents;

		void AddParam( std::string );
		void AddParam( std::string, std::string, std::string, std::string, std::string = "%", std::string = "%", std::string = "%", std::string = "default comment", std::string = "", std::string = "" );
		void AddParam( Param );
};
