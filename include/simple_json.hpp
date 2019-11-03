/**
 * @file simple_json.hpp
 * @author Devin Can Firat (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-10-25
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef SIMPLE_JSON_HPPm_
#define SIMPLE_JSON_HPPm_

#define JSON_INDENT_AMOUNT 4

#define DEBUG

#ifdef DEBUG
#include <iostream>
#define deb(A) std::cout << A << '\n';
#else
#define deb(A)
#endif

#include <map>
#include <string>
#include <variant>
#include <vector>
#include <sstream>
#include <any>

#include <cstdlib>
#include <cxxabi.h>
template <typename T>
std::string type_name()
{
    int status;
    std::string tname = typeid(T).name();
    char *demangled_name = abi::__cxa_demangle(tname.c_str(), NULL, NULL, &status);
    if (status == 0)
    {
        tname = demangled_name;
        std::free(demangled_name);
    }
    return tname;
}

namespace simple_json
{

class json;

std::string indentation_string(const int indent)
{
    return std::string(indent, ' ');
}

template <typename T>
constexpr int8_t type_to_int() { return -1; }

template <>
constexpr int8_t type_to_int<int>() { return 0; }
template <>
constexpr int8_t type_to_int<double>() { return 1; }
template <>
constexpr int8_t type_to_int<bool>() { return 2; }
template <>
constexpr int8_t type_to_int<std::string>() { return 4; }
template <>
constexpr int8_t type_to_int<json>() { return 8; }

enum json_type : int8_t
{
    JSON_TYPE_INT = 0,
    JSON_TYPE_DOUBLE = 1,
    JSON_TYPE_BOOL = 2,
    JSON_TYPE_STRING = 4,
    JSON_TYPE_OBJ = 8,
    JSON_TYPE_ARR = 16
};

class wrong_type_exception : public std::exception
{
private:
    std::string m_msg;

public:
    wrong_type_exception(json_type type) : m_msg("conversion to " + std::to_string(type) + " failed") {}
    wrong_type_exception(const int8_t type) : m_msg("conversion to " + std::to_string(static_cast<json_type>(type)) + " failed") {}
    const char *what() const throw()
    {
        return m_msg.c_str();
    }
};

class primitive_type_access_exception : public std::exception
{
private:

public:
    primitive_type_access_exception() {}
    const char *what() const throw()
    {
        return "Tried accessing the index of a primitive type";
    }
};

class key_not_exist_exception : public std::exception
{
private:
    std::string m_msg;

public:
    key_not_exist_exception(std::string key) : m_msg("key " + key+ " doesnt exist") {}
    const char *what() const throw()
    {
        return m_msg.c_str();
    }
};

class json_value;

std::string json_to_string(const json json, const int indent_amount,const int current_indent);
std::string any_to_string(std::any any, const int indent_amount, const int current_indent);
json_value& get_json_value_from_any(std::any any, const std::string& key);

class json_value
{

private:
    json_type m_type;
    std::any m_value;

public:
    json_value(json_type type, std::any val) : m_type(type), m_value(val) {}

    json_value() = default;
    json_value(const json_value &rhs)
    {
        *this = rhs;
    }

    inline json_value &operator[](const std::string &key)
    {
        if(m_type != JSON_TYPE_OBJ){
            throw primitive_type_access_exception();
        }
        return get_json_value_from_any(m_value, key);
    }

    template <typename T>
    explicit operator T()
    {
        constexpr int8_t t = type_to_int<T>();
        if constexpr (t != -1)
        {
            if (m_type == t)
            {
                return std::any_cast<T>(m_value);
            }
            throw wrong_type_exception(t);
        }
        deb("type == -1")
            deb(type_name<T>())
    }

    template <typename T>
    json_value &operator=(const T &rhs)
    {
        constexpr int8_t t = type_to_int<T>();
        if constexpr (t != -1)
        {
            if (t != m_type)
                    m_type = static_cast<json_type>(t);
                m_value = rhs;
                return *this;
        }
        return *this;
    }

    template <std::size_t N>
    json_value &operator=(const char (&rhs)[N])
    {
        if (m_type != 4)
            m_type = JSON_TYPE_STRING;
        m_value = std::string(rhs);
        return *this;
    }

    template <typename T>
    json_value &operator=(const T &&rhs)
    {
        constexpr int8_t t = type_to_int<T>();
        if constexpr (t != -1)
        {
            if (t != m_type)
                m_type = static_cast<json_type>(t);
            m_value = rhs;
            return *this;
        }
        deb("type == -1")
            deb(type_name<T>()) return *this;
    }

    std::string to_str(const int indent_amount, const int current_indent)
    {
        switch (m_type)
        {
        case JSON_TYPE_INT:
            return std::to_string(std::any_cast<int>(m_value));
        case JSON_TYPE_DOUBLE:
            return std::to_string(std::any_cast<double>(m_value));
        case JSON_TYPE_BOOL:
            return std::any_cast<bool>(m_value) ? "true" : "false";
        case JSON_TYPE_STRING:
            return '"' + std::any_cast<std::string>(m_value) + '"';
        case JSON_TYPE_OBJ:
        {
            return any_to_string(m_value, indent_amount, current_indent + indent_amount);
        }
        default:
            return "\"\"";
        }
    }
};

class json_object
{
private:
    std::map<std::string, json_value> m_data;

public:
    inline json_value &operator[](const std::string &key)
    {
        auto it = m_data.find(key);
        if (it == m_data.end())
            throw key_not_exist_exception(key);

        return m_data[key];
    }

    template <typename T>
    void insert(const std::string &key, const T &val)
    {

        m_data.insert({key, json_value(static_cast<json_type>(type_to_int<T>()), val)});
    }

    std::map<std::string, json_value> get_data() const 
    {
        return m_data;
    }
};

class json
{
private:
    std::map<std::string, json_value> m_data;

public:
    inline json_value &operator[](const std::string &key)
    {
        auto it = m_data.find(key);
        if (it == m_data.end())
            throw key_not_exist_exception(key);

        return m_data[key];
    }

    template <typename T>
    void insert(const std::string &key, const T &val)
    {
        m_data.insert({key, json_value(static_cast<json_type>(type_to_int<T>()), val)});
    }

    std::map<std::string, json_value> get_data() const 
    {
        return m_data;
    }

    inline std::string to_str()
    {
        return to_str(JSON_INDENT_AMOUNT);
    }

    std::string to_str(const int indent_amount)
    {
        return json_to_string(*this, indent_amount, 0);
    }

};


json_value& get_json_value_from_any(std::any any, const std::string& key)
{
    return std::any_cast<json>(any)[key];
}

std::string json_to_string(const json json, const int indent_amount, const int current_indent)
{
    std::stringstream ss;
    ss << "{\n";
    int loop_counter = 0;
    auto data = json.get_data();
    for (auto &[key, val] : data)
    {
        ss << indentation_string(current_indent + indent_amount) << '"' << key << "\": " << val.to_str(indent_amount, current_indent);
        if (++loop_counter < data.size())
        {
            ss << ',';
        }
        ss << '\n';
    }
    ss << indentation_string(current_indent) << '}';
    return ss.str();
}

std::string any_to_string(std::any any, const int indent_amount, const int current_indent)
{
    return json_to_string(std::any_cast<json>(any),indent_amount, current_indent );
}

} // namespace simple_json

#endif

