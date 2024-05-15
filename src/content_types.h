#ifndef CONTENT_TYPES_H
#define CONTENT_TYPES_H

#include <string_view>

template <typename T>
struct content_type;

template <>
struct content_type<std::string_view> {
    static constexpr std::string_view value = "text/plain";
};

template <>
struct content_type<struct json> {
    static constexpr std::string_view value = "application/json";
};

template <>
struct content_type<struct html> {
    static constexpr std::string_view value = "text/html";
};

template <>
struct content_type<struct css> {
    static constexpr std::string_view value = "text/css";
};

template <>
struct content_type<struct js> {
    static constexpr std::string_view value = "application/javascript";
};

template <>
struct content_type<struct jpg> {
    static constexpr std::string_view value = "image/jpeg";
};

// Add more specializations as needed

#endif // CONTENT_TYPES_H

