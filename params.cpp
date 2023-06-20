#include "params.h"
#include <fstream>
#include <algorithm>
#include <vector>
#include <iostream>

Params::Params( std::string in ) {
	std::string param_file( in );
	std::fstream file( param_file );
	contents = std::string( std::istreambuf_iterator<char>( file ), ( std::istreambuf_iterator<char>() ) );
}

Params::Params() : contents() {
}

Param* Params::GetParam( std::string name ) {
	std::vector<int> size = std::vector<int>( 2, 0 );

	size_t p = contents.find( name + '=' );
	if ( p == std::string::npos ) return nullptr;
	size_t end = contents.find( '\n', p );
	size_t begin = contents.rfind( '\n', p ) + 1;

	std::string line = contents.substr( begin, end - begin );

	size_t s = contents.find( ' ', p );

	int i = 0;
	if ( line.find( "list" ) != std::string::npos ) i = 1;
	if ( line.find( "matrix" ) != std::string::npos ) i = 2;

	char c;
	for ( int l = 0; l < i; l++ ) {
		c = contents[ ++s ];
		if (  c == '{' || c == '[' ) {
			s++;
			c = contents[ ++s ];
			while ( c != '}' && c != ']' ) {
				s = contents.find( ' ', s );
				size[ l ]++;
			}
			s++;
		} else {
			size_t en = contents.find( ' ', s );
			std::string size_str = contents.substr( s, en - s );
			size[ l ] = std::stoi( size_str );
			s = en + 1;
		}
	}

	std::string value = contents.substr( s, contents.find( "//", p ) - s );

	return new Param( line, name, value, size[ 0 ] ? size[ 0 ] : 1, size[ 1 ] ? size[ 1 ] : 1 );

}

void Params::AddParam( std::string np ) {
	contents += '\n' + np;
}

void Params::AddParam( std::string location, std::string type, std::string name, std::string value , std::string defaultValue , std::string lowRange , std::string highRange , std::string  comment, std::string width, std::string height ) {
	contents += '\n' + location + " " + type + " " + name + "= " + width + ( width.size() ? " " : "" ) + height + ( height.size() ? " " : "" ) + value + " " + defaultValue + " " + lowRange + " " + highRange + " // " + comment;
}

void Params::AddParam( Param param ) {
	contents += '\n' + param.line;
}


Param::Param( std::string li, std::string n, std::string v, int l = 1, int w = 1 ) : line( li ), name( n ), value( v ), length( l ), width ( w ) {
}
