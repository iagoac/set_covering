#pragma once

#include <exception>
#include <stdexcept>

class ArgParseException : public std::runtime_error {
public:
  ArgParseException (const std::string& what)
  : std::runtime_error(what) {}
};

class ArgParseParamException : public ArgParseException {
public:
  ArgParseParamException (const std::string& what)
  : ArgParseException(what) {};
};

class ParamNameExists : public ArgParseParamException {
public:
  ParamNameExists (const std::string& name)
  : ArgParseParamException("Param '" + name + "' exists.") {};
};

class ParamNameDoesNotExists : public ArgParseParamException {
public:
  ParamNameDoesNotExists (const std::string& name)
  : ArgParseParamException("Param '" + name + "' does not exists.") {};
};

class ParamNameInvalid : public ArgParseParamException {
public:
  ParamNameInvalid (const std::string& name)
  : ArgParseParamException("Invalid param '" + name + "'.") {};
};

class ParamPositionalInvalid : public ArgParseParamException {
public:
  ParamPositionalInvalid (const std::string& what)
  : ArgParseParamException(what) {};
};

class ParamPositionalInvalidOptions : public ParamPositionalInvalid {
public:
  ParamPositionalInvalidOptions (const std::string& name)
  : ParamPositionalInvalid(
    "Positional arg '" + name + "' should not be enable, disable or count."
  ) {};
};

class ParamPositionalMultipleExists : public ParamPositionalInvalid {
public:
  ParamPositionalMultipleExists (const std::string& name, const std::string& posit)
  : ParamPositionalInvalid(
    "Can not add positional arg '" + name +
    "' after positional multiple arg '" + posit + "'."
  ) {};
};

class ParamNonDefaultInvalid : public ArgParseParamException {
public:
  ParamNonDefaultInvalid (const std::string& name)
  : ArgParseParamException(name +
    " should be required, multiple, enable, disable or count"
    " to not require a default."
  ) {};
};

class ParamChoicesInvalid : public ArgParseParamException {
public:
  ParamChoicesInvalid (const std::string& what)
  : ArgParseParamException(what) {};
};

class ParamIsRequired : public ArgParseParamException {
public:
  ParamIsRequired (const std::string& name)
  : ArgParseParamException("Param '" + name + "' is required.") {};
};


class ParamChoicesEDC : public ParamChoicesInvalid {
public:
  ParamChoicesEDC (const std::string& name)
  : ParamChoicesInvalid(
    "Enable, disable or count param '" + name + "' can not have choices."
  ) {};
};

class ParamChoicesNotExists : public ParamChoicesInvalid {
public:
  ParamChoicesNotExists (const std::string& name, const std::string& value)
  : ParamChoicesInvalid(
    "Choice '" + value + "' not enabled for param '" + name + "'."
  ) {};
};

class ArgParseOptionException : public ArgParseException {
public:
  ArgParseOptionException (const std::string& what)
  : ArgParseException(what) {};
};

class OptionsInvalidEDCMultiple : public ArgParseOptionException {
public:
  OptionsInvalidEDCMultiple (const std::string& name)
  : ArgParseOptionException(
    "Enable, disable or count param '" + name +
    "' can not have multiple arguments."
  ) {};
};

class OptionsInvalidEDCRequired : public ArgParseOptionException {
public:
  OptionsInvalidEDCRequired (const std::string& name)
  : ArgParseOptionException(
    "Enable, disable or count param '" + name + "' can not be required."
  ) {};
};

class OptionsInvalidEnableCount : public ArgParseOptionException {
public:
  OptionsInvalidEnableCount (const std::string& name)
  : ArgParseOptionException(
    "Enable param '" + name + "' can not be count."
  ) {};
};

class OptionsInvalidEnableDisable : public ArgParseOptionException {
public:
  OptionsInvalidEnableDisable (const std::string& name)
  : ArgParseOptionException(
    "Enable param '" + name + "' can not be disable."
  ) {};
};

class OptionsInvalidDisableCount : public ArgParseOptionException {
public:
  OptionsInvalidDisableCount (const std::string& name)
  : ArgParseOptionException(
    "Disable param '" + name + "' can not be count."
  ) {};
};
