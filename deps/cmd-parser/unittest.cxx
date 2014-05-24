#include "include/parser.hh"
#include <iostream>
#include <fstream>

template <class T, class U>
std::ostream& operator << ( std::ostream& o, const std::map<T, U> &m)
{
    for (auto &e : m)
    {
        o << e.first << ":\n" << e.second << std::endl;
    }
    return o;
}

template <class T>
std::ostream& operator << (std::ostream& o, const std::vector<T> &v)
{
    for (auto &entry : v)
    {
        o << entry << "   ";
    }
    return o;
}

int main(int argc, char const *argv[])
{

    optionparser::parser p("A test to make sure that this option parser works");


    p.add_option("--now", "-n") .help("NOWWWW");


    p.add_option("-file")   .help("pass a list of files to load.")
                            .mode(optionparser::store_mult_values)
                            .required(true)
                            .default_value("default_file.txt");

    p.add_option("-save", "-s") .help("pass a file to save.")
                                .mode(optionparser::store_value)
                                // .required(true)
                                .default_value("default.txt");
    
    p.eat_arguments(argc, argv);

    if (p.get_value("file"))
    {
        auto names = p.get_value<std::vector<std::string>>("file");

        std::cout << "Filenames" << std::endl;

        for (int i = 0; i < names.size(); ++i)
        {
            std::cout << "element " << i << ": " << names[i] << std::endl;
        }

    }

    if (p.get_value("save"))
    {
        auto names = p.get_value<std::vector<std::string>>("save");

        std::cout << "Savenames" << std::endl;

        for (int i = 0; i < names.size(); ++i)
        {
            std::cout << "element " << i << ": " << names[i] << std::endl;
        }
    }
    return 0;
}
