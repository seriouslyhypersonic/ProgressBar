//
// Created by nuno on 29-09-2018.
//

#ifndef STREAMSTYLE_H
#define STREAMSTYLE_H

#include <iostream>
#include <type_traits> // For std::enable_if std::is_same

namespace streamstyle
{
    /// Attribute "font": a display effect or font style
    enum class font
    {
        regular       = 0,
        bold          = 1,
        bold_off      = 21,
        underline     = 4,
        underline_off = 24,
        blink         = 5,
        blink_off     = 25,
        inverse       = 7,
        inverse_off   = 27
    };

    /// Attribute "color": foreground (font) color
    enum class fg
    {
        black    = 30,
        red      = 31,
        green    = 32,
        yellow   = 33,
        blue     = 34,
        magenta  = 35,
        cyan     = 36,
        white    = 37,
        default_ = 39
    };

    /// Attribute "bg": background color
    enum class bg
    {
        black    = 40,
        red      = 41,
        green    = 42,
        yellow   = 43,
        blue     = 44,
        magenta  = 45,
        cyan     = 46,
        white    = 47,
        default_ = 49
    };

    /// StyleRule is a collection of attributes that defines a streamstyle
    struct StyleRule
    {
        font font_style_ = font::regular;
        fg fg_color_ = fg::default_;
        bg bg_color_ = bg::default_;
    };

    namespace style
    {
        constexpr StyleRule none {};
        constexpr StyleRule warning {font::bold, fg::yellow, bg::default_};
        constexpr StyleRule error {font::bold, fg::red, bg::default_};
        constexpr StyleRule success {font::bold, fg::green, bg::default_};
    }

    template <typename T>
    using ostreamRef_ifIsAttribute = std::enable_if_t<std::is_same<T, fg>::value ||
                                                      std::is_same<T, bg>::value ||
                                                      std::is_same<T, font>::value, std::ostream&>;

    // Insert stream using an attribute is called when T is an Attribute ('fg', 'bg', 'font')
    template <typename T>
    ostreamRef_ifIsAttribute<T> operator<<(std::ostream& out, const T& attribute)
    {
        return out << "\033[" << static_cast<int>(attribute) << 'm';
    }

    // Insert stream using a StyleRule
    std::ostream& operator<<(std::ostream& out, const StyleRule& styleRule)
    {
        out << "\033[" << static_cast<int>(styleRule.font_style_) << ';' <<
            static_cast<int>(styleRule.fg_color_) << ';' <<
            static_cast<int>(styleRule.bg_color_) << 'm';
        return out;
    }
}

#endif //STREAMSTYLE_H
