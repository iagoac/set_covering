#include "argparse.hpp"

const string_vector Args::empty_vector;

Args::Args (const string_map<std::string>& def, const string_set& non_default) {
  for (const std::string &v : non_default) {
    this->_data.emplace(v, empty_vector);
    this->_set[v] = false;
  }

  for (const string_pair<std::string>& kv : def) {
    this->_data.emplace(kv.first, empty_vector).first->second.emplace_back(kv.second);
    this->_set[kv.first] = false;
  }
}

bool Args::is_set (const std::string& name) const {
  auto it = this->_set.find(name);
  return it != this->_set.end() && it->second;
}

void Args::set (const std::string& name, const std::string& val, bool replace) {
  auto it = this->_data.find(name);
  bool is_set = this->is_set(name);

  replace |= !is_set;

  if (it == this->_data.end()) {
    it = this->_data.emplace(name, empty_vector).first;
  } else if (replace) {
    it->second.clear();
  }

  it->second.emplace_back(val);

  if (!is_set) {
    this->_set[name] = true;
  }
}

std::string& Args::get_ref (const std::string& name) {
  auto it = this->_data.find(name);

  if (it != this->_data.end()) {
    return it->second.front();
  }

  throw std::out_of_range("Parameter '" + name + "' not found.");
}

const std::string& Args::get_ref (const std::string& name) const {
  auto it = this->_data.find(name);

  if (it != this->_data.end()) {
    return it->second.front();
  }

  throw std::out_of_range("Parameter '" + name + "' not found.");
}

std::ostream& operator << (std::ostream& out, const Args& args) {
  for (const string_pair<string_vector>& kv : args._data) {
    out << kv.first << ": ";
    bool first = true;

    for (const std::string& v : kv.second) {
      if (!first) {
        out << ", ";
      } else {
        first = false;
      }

      out << v;
    }

    out << std::endl;
  }

  return out;
}

bool ArgParse::valid_name (const std::string& name) {
  for (const char c : name) {
    if (!isalnum(c) && c != '-' && c != '_') {
      return false;
    }
  }

  return !name.empty();
}

void ArgParse::assert_param_exists (const std::string& name) const {
  if (!this->exists(name)) {
    throw ParamNameDoesNotExists(name);
  }
}

void ArgParse::assert_param_does_not_exists (const std::string& name) const {
  if (this->exists(name)) {
    throw ParamNameExists(name);
  }
}

void ArgParse::assert_param_name_valid (const std::string& name) const {
  if (!ArgParse::valid_name(name)) {
    throw ParamNameInvalid(name);
  }
}

void ArgParse::assert_param_positional (const std::string&name, const Opt& options) const {
  if (options.enable() || options.disable() || options.count()) {
    throw ParamPositionalInvalidOptions(name);
  }

  if (!this->_positional.empty()) {
    if (this->_options.at(this->_positional.back()).multiple()) {
      throw ParamPositionalMultipleExists(name, this->_positional.back());
    }
  }
}

void ArgParse::assert_options (const std::string&name, const Opt& options) const {
  if (options.enable() || options.disable() || options.count()) {
    if (options.multiple()) {
      throw OptionsInvalidEDCMultiple(name);
    } else if (options.required()) {
      throw OptionsInvalidEDCRequired(name);
    } else if (options.enable() && options.count()) {
      throw OptionsInvalidEnableCount(name);
    } else if (options.enable() && options.disable()) {
      throw OptionsInvalidEnableDisable(name);
    } else if (options.disable() && options.count()) {
      throw OptionsInvalidDisableCount(name);
    }
  }
}

void ArgParse::assert_non_default (const std::string& name, const Opt& options) const {
  if (!(options.required() || options.multiple() || options.enable() || options.disable() || options.count())) {
    throw ParamNonDefaultInvalid(name);
  }
}

void ArgParse::assert_choices (const std::string& name, const Opt& options, const string_set& choices) const {
  if (!choices.empty() && (options.enable() || options.disable() || options.count())) {
    throw ParamChoicesEDC(name);
  }
}

void ArgParse::assert_choices_arg (const std::string& name, const std::string& param) const {
  auto it = this->_choices.find(name);

  if (it != this->_choices.end() && !it->second.count(param)) {
    throw ParamChoicesNotExists(name, param);
  }
}

void ArgParse::assert_param_required (const std::string& name, const Opt& options, const Args& args) const {
  if (options.required() && !args.is_set(name)) {
    throw ParamIsRequired(name);
  }
}

void ArgParse::fix_assert_new_param (const std::string& name, Opt& options, const string_set& choices) {

  this->assert_param_does_not_exists(name);
  this->assert_param_name_valid(name);
  this->assert_options(name, options);
  this->assert_choices(name, options, choices);

  if (name[0] != '-') {
    options |= Opt::req;
    this->assert_param_positional(name, options);
    this->_positional.emplace_back(name);
  }

}

Args ArgParse::parse (const uintmax_t& argc, const char* const* argv) {
  Args result(this->_default, this->_non_default);
  uintmax_t i = 1;

  while (i < argc) {
    std::string name;

    if (i <= this->_positional.size()) {
      name = this->_positional[i - 1];
    } else {
      name = argv[i++];
    }

    this->assert_param_exists(name);

    const Opt& options = this->_options[name];

    if (options.enable()) {
      result.set(name, "1", true);
      continue;
    }

    if (options.disable()) {
      result.set(name, "0", true);
      continue;
    }

    if (options.count()) {
      std::string& value = result.get_ref(name);
      value = std::to_string(std::stoull(value) + 1);
      continue;
    }

    if (options.multiple()) {
      while (i < argc) {
        const std::string arg(argv[i]);

        if (this->maybe_arg(arg)) {
          break;
        }

        this->assert_choices_arg(name, arg);
        result.set(name, arg);
        ++i;
      }
      continue;
    }

    if (i < argc) {
      const std::string arg(argv[i]);

      if (!this->maybe_arg(arg)) {
        this->assert_choices_arg(name, arg);
        result.set(name, arg, true);
        ++i;
      }
    }
  }

  for (const string_pair<Opt>& name_opt : this->_options) {
    this->assert_param_required(name_opt.first, name_opt.second, result);
  }

  return result;
}

void ArgParse::add (const std::string& name, std::string def, Opt options, const string_set& choices) {
  this->fix_assert_new_param(name, options, choices);

  if (options.disable()) {
    def = "1";
  } else if (options.enable() || (def.empty() && options.count())) {
    def = "0";
  }

  this->_options.emplace(name, std::move(options));
  this->_default.emplace(name, def);

  if (!choices.empty()) {
    this->_choices.emplace(name, choices);
  }
}

void ArgParse::add (const std::string& name, Opt options, const string_set& choices) {

  this->fix_assert_new_param(name, options, choices);
  this->assert_non_default(name, options);

  this->_options.emplace(name, std::move(options));

  if (options.enable() || options.count()) {
    this->_default.emplace(name, "0");
  } else if (options.disable()) {
    this->_default.emplace(name, "1");
  } else {
    this->_non_default.emplace(name);
  }

  if (!choices.empty()) {
    this->_choices.emplace(name, choices);
  }
}
