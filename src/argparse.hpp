#pragma once

#include <cctype>
#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "argparse_except.hpp"

template <typename T>
using string_map = std::unordered_map<std::string, T>;
template <typename T>
using string_pair = std::pair<std::string, T>;

using string_set = std::unordered_set<std::string>;
using string_mapset = string_map<string_set>;
using string_vector = std::vector<std::string>;

class Args {

public:
  template <typename T>
  using conversor = std::function<T(const std::string&)>;

private:
  static const string_vector empty_vector;

  string_map<string_vector> _data;
  string_map<bool> _set;
  using map_iterator = string_vector::iterator;

  template <typename T>
  class iterator
  : public std::iterator<std::bidirectional_iterator_tag, const T> {

    map_iterator _real;
    conversor<T> _convert;

  public:
    iterator (const map_iterator real, const conversor<T>& convert)
    : _real(real), _convert(convert) {};

    inline iterator& operator ++ (void) { ++this->_real; return *this; }
    inline iterator& operator -- (void) { --this->_real; return *this; }

    inline iterator operator ++ (int) { return iterator(this->_real++); }
    inline iterator operator -- (int) { return iterator(this->_real--); }

    inline bool operator == (const iterator& ot) { return this->_real == ot._real; }
    inline bool operator != (const iterator& ot) { return this->_real != ot._real; }

    inline const T operator * (void) { return this->_convert(*this->_real); }
    inline const T operator -> (void) { return **this; }

  };

public:

  template <typename T> static typename std::enable_if<
    std::is_same<T, bool>::value, T
  >::type default_conversor (const std::string& value) {
    return !(value.empty() || value == "0");
  }

  template <typename T> static typename std::enable_if<
    std::is_integral<T>::value && std::is_unsigned<T>::value &&
    !std::is_same<T, bool>::value, T
  >::type default_conversor (const std::string& value) {
    return std::stoull(value);
  }

  template <typename T> static typename std::enable_if<
    std::is_integral<T>::value && std::is_signed<T>::value &&
    !std::is_same<T, bool>::value, T
  >::type default_conversor (const std::string& value) {
    return std::stoll(value);
  }

  template <typename T> static typename std::enable_if<
    std::is_floating_point<T>::value, T
  >::type default_conversor (const std::string& value) {
    return std::stold(value);
  }

  template <typename T> static typename std::enable_if<
    std::is_constructible<T, std::string>::value, T
  >::type default_conversor (const std::string& value) {
    return T(value);
  }

  template <typename T>
  static T default_conversor (const std::string& value) {
    // if constexpr (std::is_same<T, std::string>::value) {
    //   return value;
    // } else if constexpr (std::is_same<T, bool>::value) {
    //   return !(value.empty() || value == "0");
    // } else if constexpr (std::is_unsigned<T>::value) {
    //   return std::stoull(value);
    // } else if constexpr (std::is_signed<T>::value) {
    //   return std::stoll(value);
    // } else if constexpr (std::is_floating_point<T>::value) {
    //   return std::stold(value);
    // } else if constexpr (std::is_constructible<T, std::string>::value) {
    //   return T(value);
    // }

    std::cout << typeid(T).name() << std::endl;

    throw std::domain_error("Unsopported type.");
  }

  Args (
    const string_map<std::string>& def = {},
    const string_set& non_default = {}
  );

  bool is_set (const std::string& name) const;

  void set (const std::string& name, const std::string& val, bool replace = false);

  std::string& get_ref (const std::string& name);
  const std::string& get_ref (const std::string& name) const;

  template <typename T>
  T get (const std::string& name, const conversor<T>& convert = Args::default_conversor<T>) const;

  template <typename T>
  const T& get (const std::string& name, const string_map<T>& convert, const T& def) const;

  template <typename T>
  bool get (const std::string& name, iterator<T>& beg, iterator<T>& end, const conversor<T>& convert = Args::default_conversor<T>) const;

  friend std::ostream& operator << (std::ostream& out, const Args& args);

};

template <typename T>
T Args::get (const std::string& name, const conversor<T>& convert) const {
  auto it = this->_data.find(name);

  if (it != this->_data.end()) {
    return convert(it->second.front());
  }

  throw std::out_of_range("Parameter '" + name + "' not found.");
}

template <typename T>
const T& Args::get (const std::string& name, const string_map<T>& convert, const T& def) const {
  const std::string& value = this->get_ref(name);
  auto it = convert.find(value);

  if (it != convert.end()) {
    return it->second;
  }

  return def;
}

template <typename T>
bool Args::get (const std::string& name, iterator<T>& beg, iterator<T>& end, const conversor<T>& convert) const {
  auto it = this->_data.find(name);

  if (it != this->_data.end()) {
    beg = iterator<T>(it->second.begin(), convert);
    end = iterator<T>(it->second.end(), convert);
    return true;
  }

  beg = iterator<T>(empty_vector.begin(), convert);
  beg = iterator<T>(empty_vector.end(), convert);

  return false;
}

class ArgParse {

public:

  class Opt {
    uint _value;

  public:
    static constexpr uint
      nop = 0b00000U, req = 0b00001U, mul = 0b00010U,
      ena = 0b00100U, dis = 0b01000U, cnt = 0b10000U;

    constexpr Opt (uint value = 0) : _value(value) {};

    constexpr bool required (void) const { return this->_value & req; }
    constexpr bool multiple (void) const { return this->_value & mul; }
    constexpr bool enable (void) const { return this->_value & ena; }
    constexpr bool disable (void) const { return this->_value & dis; }
    constexpr bool count (void) const { return this->_value & cnt; }

    constexpr Opt operator | (const Opt& ot) const { return this->_value | ot._value; }
    constexpr Opt operator | (const uint& ot) const { return this->_value | ot; }

    constexpr Opt& operator |= (const Opt& ot) { this->_value |= ot._value; return *this; }
    constexpr Opt& operator |= (const uint& ot) { this->_value |= ot; return *this; }
  };

  static bool valid_name (const std::string& name);

private:
  string_map<Opt> _options;
  string_mapset _choices;
  string_map<std::string> _default;
  string_set _non_default;
  string_vector _positional;

  void assert_param_exists (const std::string& name) const;
  void assert_param_does_not_exists (const std::string& name) const;
  void assert_param_name_valid (const std::string& name) const;
  void assert_param_positional (const std::string&name, const Opt& options) const;
  void assert_options (const std::string&name, const Opt& options) const;
  void assert_non_default (const std::string& name, const Opt& options) const;
  void assert_choices (const std::string& name, const Opt& options, const string_set& choices) const;
  void assert_choices_arg (const std::string& name, const std::string& param) const;
  void assert_param_required (const std::string& name, const Opt& options, const Args& args) const;

  void fix_assert_new_param (const std::string& name, Opt& options, const string_set& choices);

public:
  ArgParse (void) {}

  Args parse (const uintmax_t& argc, const char* const* argv);

  void add (const std::string& name, std::string def, Opt options = Opt::nop, const string_set& choices = {});
  void add (const std::string& name, Opt options = Opt::nop, const string_set& choices = {});

  void add (const std::string& name, std::string def, const string_set& choices) {
    this->add(name, def, Opt::nop, choices);
  }

  void add (const std::string& name, const string_set& choices) {
    this->add(name, Opt::nop, choices);
  }

  inline bool exists (const std::string& name) const {
    return this->_options.count(name);
  }

  inline bool maybe_arg (const std::string& name) const {
    return name[0] == '-' && this->exists(name);
  }

};
