/**
 * @file simple_json.hpp
 * @author Devin-Can Firat (devinc.firat@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-06-14 05:04
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef SIMPLE_JSON_HPP
#define SIMPLE_JSON_HPP

#define DEBUG

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <sstream>

#define INDENT_AMT 4

#ifdef DEBUG
#include <iostream>
#endif

namespace simple_json
{
class simple_json_array;
class simple_json;

std::string to_str(const simple_json *, const size_t, const size_t);
std::string to_str(const simple_json_array *, const size_t, const size_t);

struct json_type
{
    enum
    {
        JSON_NULL,
        JSON_OBJECT,
        JSON_ARRAY,
        JSON_INT,
        JSON_BOOL,
        JSON_STRING
    } tag;
    union json_type_data {
        const simple_json *jsonObjectValue;
        const simple_json_array *jsonArrayValue;
        int intValue;
        bool booleanValue;
        std::string stringValue;
        json_type_data() {}
        ~json_type_data() {}
    } data;

    json_type &operator=(const json_type &rhs)
    {
        switch (rhs.tag)
        {
        case JSON_OBJECT:
        {
            tag = JSON_OBJECT;
            data.jsonObjectValue = rhs.data.jsonObjectValue;
            break;
        }
        case JSON_ARRAY:
        {
            tag = JSON_ARRAY;
            data.jsonArrayValue = rhs.data.jsonArrayValue;
            break;
        }
        case JSON_INT:
        {
            tag = JSON_INT;
            data.intValue = rhs.data.intValue;
            break;
        }
        case JSON_BOOL:
        {
            tag = JSON_BOOL;
            data.booleanValue = rhs.data.booleanValue;
            break;
        }
        case JSON_STRING:
        {
            tag = JSON_STRING;
            data.stringValue = rhs.data.stringValue;
            break;
        }
        case JSON_NULL:
        {
            tag = JSON_NULL;
            break;
        }
        default:
            throw std::runtime_error("Unknown JSON tag");
        }
        return *this;
    }

    std::string to_str(const size_t current_indentation, const size_t indent_amt = 0) const
    {
        switch (tag)
        {
        case JSON_NULL:
            return "null";
        case JSON_OBJECT:
            return ::simple_json::to_str(data.jsonObjectValue, current_indentation, indent_amt);
        case JSON_ARRAY:
            return ::simple_json::to_str(data.jsonArrayValue, current_indentation, indent_amt);
        case JSON_INT:
            return std::to_string(data.intValue);
        case JSON_BOOL:
            return data.booleanValue ? "true" : "false";
        }
        return "unknown";
    }

    json_type() : tag(JSON_NULL) {}

    json_type(const simple_json &val) : tag(JSON_OBJECT)
    {
        data.jsonObjectValue = &val;
    }
    json_type(const simple_json_array &val) : tag(JSON_ARRAY)
    {
        data.jsonArrayValue = &val;
    }
    json_type(int val) : tag(JSON_INT)
    {
        data.intValue = val;
    }
    json_type(bool val) : tag(JSON_BOOL)
    {
        data.booleanValue = val;
    }
    json_type(const std::string &val) : tag(JSON_STRING)
    {
        data.stringValue = val;
    }
};

class simple_json_array
{
public:
    std::string to_str(const size_t current_indentation, const size_t indent_amt = 0) const
    {
        return "array()";
    }

private:
    std::vector<json_type> members;
};

class simple_json
{
public:
    json_type &operator[](const std::string &objname)
    {
        auto ret = members.find(objname);
        if (ret == members.end())
            throw std::runtime_error("Key not found");
        return ret->second;
    }

    simple_json &operator=(const json_type &type)
    {
        return *this;
    }

    bool insert(const std::string &str, const simple_json &json)
    {
        members[str] = json_type(json);
        return true;
    }

    bool insert(const std::string &str, const json_type &json)
    {
        members[str] = json;
        return true;
    }

    std::string to_str(size_t current_indentation, const size_t indent_amt = 0) const
    {
        std::stringstream   ;
        for (auto i = 0; i < current_indentation; i++)
            ss << ' ';
        ss << "{\n";
        current_indentation += indent_amt;
        auto map_size = members.size();
        for (const auto &[key, json] : members)
        {
            if (--map_size == 0)
            {
                switch (json.tag)
                {
                case json_type::JSON_OBJECT:
                {
                    for (auto i = 0; i < current_indentation; i++)
                        ss << ' ';
                    ss << '"' << key << "\": \n"
                       << json.to_str(current_indentation, indent_amt) << '\n';
                    break;
                }
                case json_type::JSON_INT:
                case json_type::JSON_BOOL:
                case json_type::JSON_STRING:
                {
                    for (auto i = 0; i < current_indentation; i++)
                        ss << ' ';
                    ss << '"' << key << "\": " << json.to_str(current_indentation) << "\n";
                    break;
                }
                }
            }
            else
            {
                switch (json.tag)
                {
                case json_type::JSON_OBJECT:
                {
                    for (auto i = 0; i < current_indentation; i++)
                        ss << ' ';
                    ss << '"' << key << "\": \n"
                       << json.to_str(current_indentation, indent_amt) << '\n';
                    break;
                }
                case json_type::JSON_INT:
                case json_type::JSON_BOOL:
                case json_type::JSON_STRING:
                {
                    for (auto i = 0; i < current_indentation; i++)
                        ss << ' ';
                    ss << '"' << key << "\": " << json.to_str(current_indentation) << ",\n";
                    break;
                }
                }
            }
        }
        for (auto i = 0; i < current_indentation - indent_amt; i++)
            ss << ' ';
        if (current_indentation > indent_amt)
        {
            ss << "},";
        }
        else
        {
            ss << "}\n";
        }

        return ss.str();
    }

private:
    std::map<std::string, json_type> members;
};

std::string to_str(const simple_json *json, const size_t current_indentation, const size_t indent_amt = 0)
{
    return json->to_str(current_indentation, indent_amt);
}
std::string to_str(const simple_json_array *json, const size_t current_indentation, const size_t indent_amt = 0)
{
    return json->to_str(current_indentation, indent_amt);
}

simple_json parse(const std::string &jsonStr)
{
    simple_json json;
    json.insert("test", 13);
    json.insert("test2", "hello");
    simple_json json2;
    json2.insert("foo", 1337);
    simple_json json3;
    json3.insert("foo", 1337);
    json.insert("alpha", 10000000);
    json.insert("zzjson2", json2);
    json.insert("json3", json3);
    std::cout << json.to_str(0, 4);
    return json;
}

simple_json parse(std::string &&jsonStr)
{
    return parse(jsonStr);
}

} // namespace simple_json

#endif