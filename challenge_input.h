#ifndef CHALLENGE_INPUT_H
#define CHALLENGE_INPUT_H

#include <string>
#include <vector>

class challenge_input
{
public:
    challenge_input(std::string file_path);
    
private:
    access_file();
    parse_file_to_tokens();

    std::vector tokens;
};

#endif
