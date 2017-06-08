#include "challenge_input.h"

#include <boost/tokenizer.hpp>

challenge_input::challenge_input()
{
}

void challenge_input::access_file()
{
}

void challenge_input::parse_file_to_tokens()
{
    std::string equation = "green = 3 + red + blue + 2";

    boost::char_separator< char > seperator( " " );
    boost::tokenizer< boost::char_separator< char > > tokens( equation, seperator );
    for ( const auto & t : tokens )
    {
        std::cout << t << std::endl;
    }
}
