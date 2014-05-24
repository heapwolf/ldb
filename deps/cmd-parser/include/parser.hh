//-----------------------------------------------------------------------------
//  parser.hh -- A Header-Only commandline argument parser
//  Author: Luke de Oliveira <luke.deoliveira@yale.edu>
//-----------------------------------------------------------------------------


//----------------------------------------------------------------------------
#ifndef PARSER__HH
#define PARSER__HH 

#include <map>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <stdlib.h>
#include <unordered_set>


namespace optionparser
{
//----------------------------------------------------------------------------
enum storage_mode { store_true, store_value, store_mult_values };
//----------------------------------------------------------------------------
struct dict_entry
{
    unsigned int pos;
    std::string name;
};
//----------------------------------------------------------------------------
struct option
{
    option(){}
    void help_doc()
    {
        std::string h = "    ";
        if (m_long_flag != "")
        {
            h += m_long_flag;
            if (m_short_flag != "")
            {
                h += ", ";
            }
        }
        if (m_short_flag != "")
        {
            h+= m_short_flag;
        }

        printf("%-25s%s\n", h.c_str(), m_help.c_str());
    }

    bool &found() 
    {
        return m_found;
    }

    option &required(bool req) 
    {
        m_required = req;
        return *this;
    }
    bool &required() 
    {
        return m_required;
    }

    std::string &short_flag() 
    {
        return m_short_flag;
    }
    
    std::string &long_flag() 
    {
        return m_long_flag;
    }
    
    std::string m_short_flag = "", m_long_flag = "";

    storage_mode &mode() 
    {
        return m_mode;
    }
    
    option &mode(const storage_mode &mode)
    {
        m_mode = mode;
        return *this;
    }

    std::string &help() 
    {
        return m_help;
    }
    
    option &help(const std::string &help)
    {
        m_help = help;
        return *this;
    }

    std::string &dest() 
    {
        return m_dest;
    }
    
    option &dest(const std::string &dest)
    {
        m_dest = dest;
        return *this;
    }

    std::string &default_value() 
    {
        return m_default_value;
    }
    
    option &default_value(const std::string &default_value)
    {
        m_default_value = default_value;
        return *this;
    }

    option &default_value(const char* default_value)
    {
        m_default_value = std::string(default_value);
        return *this;
    }

    template <typename T>
    option &default_value(const T &default_value)
    {
        m_default_value = std::to_string(default_value);
        return *this;
    }

    //     option &default_value(const int &default_value)
    // {
    //     m_default_value = std::to_string(default_value);
    //     return *this;
    // }
    // option &default_value(const double &default_value)
    // {
    //     m_default_value = std::to_string(default_value);
    //     return *this;
    // }
    // option &default_value(const unsigned int &default_value)
    // {
    //     m_default_value = std::to_string(default_value);
    //     return *this;
    // }
    // option &default_value(const float &default_value)
    // {
    //     m_default_value = std::to_string(default_value);
    //     return *this;
    // }


    bool m_found = false;
    bool m_required = false;
    storage_mode m_mode = store_true;
    std::string m_help = "";
    std::string m_dest = "";
    std::string m_default_value = "";
};
//----------------------------------------------------------------------------
inline std::string remove_character(std::string str, char c = ' ');

typedef std::map<std::string, std::vector<std::string>> Archive;
typedef std::map<std::string, dict_entry> Dictionary;

//-----------------------------------------------------------------------------
//  Parser Class
//-----------------------------------------------------------------------------
class parser
{
public:
    parser(std::string description = "", bool create_help = true) 
    : m_options(0), m_description(description)
    {
        if (create_help)
        {
            add_option("--help", "-h")
            .help("Display this help message and exit.");
        }
    }

    ~parser() = default;

    void eat_arguments(int argc, char const *argv[]);

    option &add_option(std::string longoption, std::string shortoption);
    option &add_option(std::string opt);

    template <class T = bool>
    T get_value(std::string key);

    void help();

private:
     option &add_option_internal(std::string longoption, 
        std::string shortoption);

    

    void error(const std::string &e);

    Archive m_values;
    std::vector<option> m_options;
    std::string m_prog_name, m_description;
    std::unordered_set<std::string> with_val, without_val;
    Dictionary m_opt_map;
    std::map<std::string, unsigned int> idx;
};
//----------------------------------------------------------------------------
option &parser::add_option(std::string longoption, std::string shortoption)
{
    return add_option_internal(longoption, shortoption);
}
//----------------------------------------------------------------------------
option &parser::add_option(std::string opt)
{
    if (opt[0] == '-')
    {
        if (opt[1] == '-')
        {
            return add_option_internal(opt, "");
        }
        if (opt.size() > 2)
        {
            return add_option_internal(opt, "");
        }
        else
        {
            return add_option_internal("", opt);
        }
    }
    error("Positional arguments such as '" + opt + "' aren't yet supported.");
    return add_option_internal(opt, "");
}
//----------------------------------------------------------------------------
option &parser::add_option_internal(std::string longoption, 
    std::string shortoption)
{
    m_options.resize(m_options.size() + 1);

    option &opt = m_options.back();

    if (longoption != "")
    {
        if (opt.m_dest == "")
        {
            opt.m_dest = remove_character(longoption, '-');   
        }
    }
    else
    {
        if (opt.m_dest == "")
        {
            opt.m_dest = remove_character(shortoption, '-') + "_option";   
        }
    }

    opt.long_flag() = longoption;
    opt.short_flag() = shortoption;
    auto k = remove_character(shortoption, '-');
    m_opt_map[k].pos = m_options.size() - 1;
    m_opt_map[k].name = opt.m_dest;
    return opt;
}
//----------------------------------------------------------------------------
void parser::eat_arguments(int argc, char const *argv[])
{
    unsigned int idx_ctr = 0;
    for (auto &opt : m_options)
    {
        idx[opt.m_dest] = idx_ctr;
        if (opt.short_flag() != "")
        { 
            auto shortflag = opt.short_flag();
            if (opt.mode() != store_true)
            {
                with_val.insert(remove_character(shortflag, '-'));
            }
            else
            {
                without_val.insert(remove_character(shortflag, '-'));
            }
            m_opt_map[remove_character(shortflag, '-')].name = opt.dest();
        }
        ++idx_ctr;
    }
    m_prog_name = argv[0];
    std::vector<std::string> arguments;
    for (int i = 1; i < argc; ++i)
    {
        arguments.push_back(argv[i]);
    }

    // for each argument cluster
    
    for (int arg = 0; arg < arguments.size(); ++arg)
    {
        auto argument = arguments[arg];
        bool match_found = false;


        for (auto &option : m_options) // for each option set
        {
            auto opt = option;

            if (opt.long_flag() == "")
            {
                continue;
            }

            if (argument[0] == '-')
            {
                if (argument.size() == 1)
                {
                    error("A flag needs a letter...");
                }
                if (argument.find(opt.long_flag()) == 0)
                {
                    if (opt.mode() == store_true)
                    {
                        option.found() = true;
                        match_found = true;
                        break;
                    }
                    
                    if(argument.size() > opt.long_flag().size())
                    {
                        option.found() = false;
                        if (opt.mode() != store_true)
                        {
                            auto search_pt = argument.find_first_of('=');
                            if (search_pt == std::string::npos)
                            {
                                std::string e = "Error, long options (";
                                e += opt.long_flag();
                                e +=") require a '=' or space before a value.";
                                error(e);
                            }
                            std::string val = argument.substr(search_pt + 1);
                            if (val == "")
                            {
                                std::string e = "Error, argument needed";
                                error(e + " after '='.");
                            }
                            if (opt.mode() == store_value)
                            {
                                m_values[opt.dest()].clear();
                                m_values[opt.dest()].push_back(val);
                            }
                            else
                            {
                                m_values[opt.dest()].push_back(val);
                                if (++arg < arguments.size())
                                {
                                    auto next_arg = arguments[arg];
                                    while (next_arg[0] != '-')
                                    {
                                        m_values[opt.dest()].push_back(
                                            next_arg);

                                        if (++arg >= arguments.size())
                                        {
                                            break;
                                        }
                                        next_arg = arguments[arg];
                                    }
                                    arg--;
                                }
                            }
                            option.found() = true;
                            match_found = true;
                            break;
                        }
                        else
                        {
                            std::string e = "Error, value passed to flag '";
                            e += opt.long_flag();
                            error(e + "', which takes no values.");
                        }              
                    }

                    if ((opt.mode() == store_value) && 
                        (option.found() == false))
                    {
                        bool skip = false;
                        if (++arg >= arguments.size())
                        {
                            if (opt.default_value() == "")
                            {
                                std::string e("error, flag '" + 
                                    opt.long_flag() + "' requires an argument.");
                                error(e);
                            }
                            skip = true;
                            if (m_values[opt.dest()].size() == 0)
                            {
                                m_values[opt.dest()].push_back(opt.default_value());
                            }
                        }
                        std::string next_arg;
                        if (!skip)
                        {
                            next_arg = arguments[arg];
                            if (next_arg[0] != '-')
                            {
                                m_values[opt.dest()].clear();
                                m_values[opt.dest()].push_back(next_arg);
                            }
                            else
                            {
                                if (m_values[opt.dest()].size() == 0)
                                {
                                    m_values[opt.dest()].push_back(opt.default_value());
                                }
                            }
                        }
                        option.found() = true;
                        match_found = true;
                        break;
                    }
                    if ((opt.mode() == store_mult_values) && 
                        (option.found() == false))
                    {
                        bool skip = false;
                        if (++arg >= arguments.size())
                        {
                            if (opt.default_value() == "")
                            {
                                std::string e("error, flag '" + 
                                    opt.long_flag() + "' requires an argument.");
                                error(e);
                            }
                            skip = true;
                            if (m_values[opt.dest()].size() == 0)
                            {
                                m_values[opt.dest()].push_back(opt.default_value());
                            }
                        }
                        if (!skip)
                        {
                            auto next_arg = arguments[arg];
                            if (next_arg[0] == '-')
                            {
                                if ((opt.default_value() == "") && 
                                    (m_values[opt.dest()].size() == 0))
                                {
                                    std::string e("error, flag '" + opt.long_flag() 
                                        + "' requires at least one argument.");
                                    error(e);
                                }
                                if((opt.default_value() != "") && 
                                    (m_values[opt.dest()].size() == 0))
                                {
                                    m_values[opt.dest()].push_back(opt.default_value());
                                }
                                
                            }
                            while (next_arg[0] != '-')
                            {

                                m_values[opt.dest()].push_back(next_arg);
                                if (++arg >= arguments.size())
                                {
                                    break;
                                }
                                next_arg = arguments[arg];
                            }
                            arg--;
                            option.found() = true;
                            match_found = true;
                            break;
                        } 
                    }
                }
            }
        }
        if ((argument[0] == '-') && 
            !(match_found) &&
            (argument[1] != '-'))
        {
            bool have_value = false;
            for (int i = 1; i < argument.size(); ++i)
            {
                bool valid_flag = false;
                std::string key(1, argument[i]);
                bool value_flag = (with_val.count(key) > 0);
                if (value_flag)
                {
                    auto crit_pos = argument.find(key);
                    if (argument.size() > (crit_pos + 1))
                    {
                        if(m_options[m_opt_map[key].pos].mode() == store_value)
                        {
                            m_values[m_opt_map[key].name].clear();
                            m_values[m_opt_map[key].name].push_back(
                                argument.substr(i + 1));

                            m_options[m_opt_map[key].pos].found() = true;
                            break;
                        }
                        else
                        {
                            m_values[m_opt_map[key].name].push_back(
                                argument.substr(i + 1));

                            m_options[m_opt_map[key].pos].found() = true;
                            if (++arg < arguments.size())
                            {
                                auto next_arg = arguments[arg];
                                while (next_arg[0] != '-')
                                {
                                    m_values[m_opt_map[key].name].push_back(
                                        next_arg);

                                    if (++arg >= arguments.size())
                                    {
                                        break;
                                    }
                                    next_arg = arguments[arg];
                                }
                                arg--;
                            }
                            break;
                        }
                    }
                    else // case where theres a space until the next args.
                    {
                        if (++arg >= arguments.size())
                        {
                            std::string e("error, flag '-" + key + 
                                "' requires at least one argument.");
                            error(e);
                        }
                        auto next_arg = arguments[arg];
                        if (next_arg[0] == '-')
                        {
                            std::string e("error, flag '-" + key + 
                                "' requires an argument.");
                            error(e);
                        }
                        if (m_options[m_opt_map[key].pos].mode() 
                            == store_value)
                        {
                            m_values[m_opt_map[key].name].clear();
                            m_values[m_opt_map[key].name].push_back(next_arg);
                            m_options[m_opt_map[key].pos].found() = true;
                        }
                        else
                        {
                            while (next_arg[0] != '-')
                            {

                                m_values[m_opt_map[key].name].push_back(
                                    next_arg);
                                if (++arg >= arguments.size())
                                {
                                    break;
                                }
                                next_arg = arguments[arg];
                            }
                            m_options[m_opt_map[key].pos].found() = true;
                            arg--;
                        }

                    }
                    break;
                }

                bool bool_flag = (without_val.count(key) > 0);
                if (bool_flag)
                {
                    m_options[m_opt_map[key].pos].found() = true;
                }
                else
                {
                    error("Invalid flag '-" + key + "'");
                }
            }
            match_found = true;
        }
        if(!match_found && argument[0] == '-')
        {
            error("Unrecognized flag/option '" + argument + "'");
        }

    }
    if (get_value("help"))
    {
        help();
    }
    std::vector<std::string> missing;
    for (auto &opt : m_options)
    {    
        if ((opt.required()) && (!opt.found()))
        {
            missing.push_back(opt.dest());
        }
        else if ((opt.default_value() != "") && (!opt.found()))
        {
            m_values[opt.dest()].push_back(opt.default_value());
            opt.found() = true;
        }
    }
    if (missing.size() > 0)
    {
        std::string e = "Missing required flags: ";
        e += missing.at(0);
        for (int i = 1; i < missing.size(); ++i)
        {
            e += ", ";
            e += missing.at(i);
        }
        error(e + ".");
    }
}
//----------------------------------------------------------------------------
void parser::error(const std::string &e)
{
    std::cerr << "In excecutable \'";
    std::cerr << m_prog_name << "\':\n" << e << std::endl;
    exit(1);
}

void parser::help()
{
    auto split = m_prog_name.find_last_of('/');
    std::string stripped_name = m_prog_name.substr(split + 1);
    std::cout << "usage: " << stripped_name << " [-h] ";
    for (auto &option : m_options)
    {
        if (option.required())
        {
            if (option.short_flag() != "")
            {
                std::cout << option.short_flag();
            }
            else
            {
                std::cout << option.long_flag();
            }
            
            if (option.mode() == store_value)
            {
                std::cout << " ARG ";
            }
            if (option.mode() == store_mult_values)
            {
                std::cout << " ARG1 [ARG2 ...] ";
            }
        }
    }
    std::cout << "[options]\n";

    if (m_description != "")
    {
        std::cout << "\n" << m_description << "\n" << std::endl;
    }
    for (auto &option : m_options)
    {
        option.help_doc();
    }
    exit(0);
}

//----------------------------------------------------------------------------
template <class T>
T parser::get_value(std::string key)
{
    try
    {
        return m_options[idx.at(key)].found();
    }
    catch(std::out_of_range &err)
    {
        std::string e("Tried to access value for field '");
        e += key;
        e += "' which is not a valid field.";
        throw std::out_of_range(e);
    }  
}
//----------------------------------------------------------------------------
template <>
std::string parser::get_value<std::string>(std::string key)
{
    try
    {
        return m_values[key][0];
    }
    catch(std::out_of_range &err)
    {
        std::string e("Tried to access value for field '");
        e += key;
        e += "' which is not a valid field.";
        throw std::out_of_range(e);
    }  
}
//----------------------------------------------------------------------------
template <>
double parser::get_value<double>(std::string key)
{
    try
    {
        return std::stod(m_values[key][0]);
    }
    catch(std::out_of_range &err)
    {
        std::string e("Tried to access value for field '");
        e += key;
        e += "' which is not a valid field.";
        throw std::out_of_range(e);
    }  
}
//----------------------------------------------------------------------------
template <>
float parser::get_value<float>(std::string key)
{
    try
    {
        return std::stof(m_values[key][0]);
    }
    catch(std::out_of_range &err)
    {
        std::string e("Tried to access value for field '");
        e += key;
        e += "' which is not a valid field.";
        throw std::out_of_range(e);
    }  
}
//----------------------------------------------------------------------------
template <>
int parser::get_value<int>(std::string key)
{
    try
    {
        return std::stoi(m_values[key][0]);
    }
    catch(std::out_of_range &err)
    {
        std::string e("Tried to access value for field '");
        e += key;
        e += "' which is not a valid field.";
        throw std::out_of_range(e);
    }  
}
//----------------------------------------------------------------------------
template <>
unsigned int parser::get_value<unsigned int>(std::string key)
{
    try
    {
        return std::stoul(m_values[key][0]);
    }
    catch(std::out_of_range &err)
    {
        std::string e("Tried to access value for field '");
        e += key;
        e += "' which is not a valid field.";
        throw std::out_of_range(e);
    }  
}
//----------------------------------------------------------------------------
template <>
std::vector<std::string> parser::get_value<std::vector<std::string>>(
    std::string key)
{
    try
    {
        return m_values[key];
    }
    catch(std::out_of_range &err)
    {
        std::string e("Tried to access value for field '");
        e += key;
        e += "' which is not a valid field.";
        throw std::out_of_range(e);
    }  
}
//----------------------------------------------------------------------------
template <>
std::vector<int> parser::get_value<std::vector<int>>(
    std::string key)
{
    try
    {
        std::vector<int> v;
        for (auto &entry : m_values[key])
        {
            v.push_back(std::stoi(entry));
        }
        return std::move(v);
    }
    catch(std::out_of_range &err)
    {
        std::string e("Tried to access value for field '");
        e += key;
        e += "' which is not a valid field.";
        throw std::out_of_range(e);
    }  
}
//----------------------------------------------------------------------------
inline std::string remove_character(std::string str, const char c)
{
    std::string::iterator end_pos = std::remove(str.begin(), str.end(), c);
    str.erase(end_pos, str.end());
    return str;
}

} // end namespace

#endif
