/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <algorithm>
#include <ctime>
#include <string>
#include <cctype>

#include "util/StringUtil.h"

namespace huesdk_lib {

    std::string to_upper_case(std::string s) {
        std::transform(
                s.begin(), s.end(), s.begin(),
                [](unsigned char c) {return std::toupper(c); });
        return s;
    }
 
    std::string random_alphanumeric_string(size_t size) {
        std::string random_string = "";

        const std::string alphanumeric_chars {
                    "0123456789"
                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                    "abcdefghijklmnopqrstuvwxyz"};

        unsigned time_seed = static_cast<unsigned int>(time(0));
        srand(time_seed);

        for (unsigned i = 0; i < size; ++i) {
            random_string += alphanumeric_chars[rand() % (static_cast<int>(alphanumeric_chars.length()) - 1)];
        }

        return random_string;
    }
 
    std::vector<std::string> split(const std::string& s, char c) {
        std::vector<std::string> result;

        std::string::size_type j = s.find(c);

        if (j == std::string::npos) {
            result.push_back(s);
            return result;
        }

        std::string::size_type i = 0;
        while (j != std::string::npos) {
            result.push_back(s.substr(i, j - i));
            i = ++j;
            j = s.find(c, j);

            if (j == std::string::npos)
                result.push_back(s.substr(i, s.length()));
        }
        return result;
    }

    // find the most left separator from all separators into string from start position
    // return position the most left separator or npos and it's length
    std::string::size_type find_separator(const std::string& s, std::string::size_type start_position, const std::vector<std::string>& separators, std::string::size_type* separator_length) {
        std::string::size_type result = std::string::npos;
        *separator_length = 0;
        // go through separators
        for (size_t i = 0; i < separators.size(); i++) {
            // separator length
            std::string::size_type length = separators[i].length();
            if (length == 0)
                continue;
            
            // find
            std::string::size_type position = s.find(separators[i], start_position);
            
            // s has this separator ?
            if (position != std::string::npos) {
                // separator is lefter ?
                if ((result == std::string::npos) || (position <= result)) {
                    // separator has the same position but length is less ?
                    if ((result == position) && (*separator_length > length))
                        continue;
                    // bingo
                    result = position;
                    *separator_length = length;
                }
            }
        }
        return result;
    }

    std::vector<std::string> split(const std::string& s, const std::vector<std::string>& separators) {
        std::vector<std::string> result;
        
        // find the most left separator into s
        std::string::size_type separator_length = 0;
        std::string::size_type separator_position = find_separator(s, 0, separators, &separator_length);
        
        // if no separators into s
        if (separator_position == std::string::npos) {
            result.push_back(s);
            return result;
        }
        
        // go by s while there are separators in s
        std::string::size_type start_token_position = 0;
        while (separator_position != std::string::npos) {
            // extract token and push
            const std::string& token = s.substr(start_token_position, separator_position - start_token_position);
            if (token.length() > 0)
                result.push_back(token);
            
            // find separator from token beginning
            start_token_position = separator_position + separator_length;
            separator_position = find_separator(s, start_token_position, separators, &separator_length);
        }
        
        // extract the last token and push
        const std::string& token = s.substr(start_token_position, s.length());
        if (token.length() > 0)
           result.push_back(token);
        
        return result;
    }
    
    std::string replace_all(std::string subject, const std::string& search, const std::string& replace) {
        size_t pos = 0;
        
        while ((pos = subject.find(search, pos)) != std::string::npos) {
            subject.replace(pos, search.length(), replace);
            pos += replace.length();
        }
        
        return subject;
    }
    
    std::string trim(std::string s, const std::string whitespace) {
        size_t start_pos = s.find_first_not_of(whitespace);
        size_t end_pos = s.find_last_not_of(whitespace);

        if (start_pos == std::string::npos || end_pos == std::string::npos) {
            return std::string();
        }

        return s.substr(start_pos, end_pos - start_pos + 1);
    }

    std::string join(const std::vector<std::string>& elements, const std::string& delimiter) {
        std::stringstream result;

        for (size_t i = 0; i < elements.size(); i++) {
            if (i > 0) {
                result << delimiter;
            }

            result << elements[i];
        }

        return result.str();
    }

    std::vector<std::string> map_vector(
            const std::vector<std::string>& elements,
            std::function<std::string(const std::string&)> mapping) {
        std::vector<std::string> result(elements.size());
        std::transform(elements.begin(), elements.end(), result.begin(), mapping);
        return result;
    }

}  // namespace huesdk_lib
