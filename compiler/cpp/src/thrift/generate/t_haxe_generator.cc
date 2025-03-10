/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <cctype>

#include <sys/stat.h>
#include <stdexcept>

#include "thrift/platform.h"
#include "thrift/generate/t_oop_generator.h"

using std::map;
using std::ostream;
using std::ostringstream;
using std::string;
using std::stringstream;
using std::vector;

/**
 * Haxe code generator.
 *
 */
class t_haxe_generator : public t_oop_generator {
public:
  t_haxe_generator(t_program* program,
                   const std::map<std::string, std::string>& parsed_options,
                   const std::string& option_string)
    : t_oop_generator(program) {
    (void)option_string;
    std::map<std::string, std::string>::const_iterator iter;

    rtti_ = false;
    buildmacro_ = "";
    for( iter = parsed_options.begin(); iter != parsed_options.end(); ++iter) {
      if( iter->first.compare("callbacks") == 0) {
        printf("Hint: The 'callbacks' option is no longer necessary.\n");
      } else if( iter->first.compare("rtti") == 0) {
        rtti_ = true;
      } else if( iter->first.compare("buildmacro") == 0) {
        buildmacro_ = (iter->second);
      } else {
        throw "unknown option haxe:" + iter->first;
      }
    }

    out_dir_base_ = "gen-haxe";
  }

  /**
   * Init and close methods
   */

  void init_generator() override;
  void close_generator() override;
  std::string display_name() const override;

  void generate_consts(std::vector<t_const*> consts) override;

  /**
   * Program-level generation functions
   */

  void generate_typedef(t_typedef* ttypedef) override;
  void generate_enum(t_enum* tenum) override;
  void generate_struct(t_struct* tstruct) override;
  void generate_xception(t_struct* txception) override;
  void generate_service(t_service* tservice) override;

  void print_const_value(std::ostream& out,
                         std::string name,
                         t_type* type,
                         t_const_value* value);

  void render_const_value(std::ostream& out,
                          t_type* type,
                          t_const_value* value);

  void render_struct_initializer(std::ostream& out,
                                 t_struct* type,
                                 t_const_value* value);
  void render_map_initializer(std::ostream& out,
                              t_map* type,
                              t_const_value* value);
  void render_list_initializer(std::ostream& out,
                               t_list* type,
                               t_const_value* value);
  void render_set_initializer(std::ostream& out,
                              t_set* type,
                              t_const_value* value);

  // helper
  std::string render_const_value_str( t_type* type, t_const_value* value);


  /**
   * Service-level generation functions
   */

  void generate_haxe_struct(t_struct* tstruct, bool is_exception, bool is_result = false);

  void generate_haxe_struct_definition(std::ostream& out,
                                       t_struct* tstruct,
                                       bool is_xception = false,
                                       bool is_result = false);
  // removed -- equality,compare_to
  void generate_haxe_struct_reader(std::ostream& out, t_struct* tstruct);
  void generate_haxe_validator(std::ostream& out, t_struct* tstruct);
  void generate_haxe_struct_result_writer(std::ostream& out, t_struct* tstruct);
  void generate_haxe_struct_writer(std::ostream& out, t_struct* tstruct);
  void generate_haxe_struct_tostring(std::ostream& out, t_struct* tstruct, bool is_override);
  void generate_haxe_meta_data_map(std::ostream& out, t_struct* tstruct);
  void generate_field_value_meta_data(std::ostream& out, t_type* type);
  std::string get_haxe_type_string(t_type* type);
  void generate_reflection_setters(std::ostringstream& out,
                                   t_type* type,
                                   std::string field_name,
                                   std::string cap_name);
  void generate_reflection_getters(std::ostringstream& out,
                                   t_type* type,
                                   std::string field_name,
                                   std::string cap_name);
  void generate_generic_field_getters_setters(std::ostream& out, t_struct* tstruct);
  void generate_generic_isset_method(std::ostream& out, t_struct* tstruct);
  void generate_property_getters_setters(std::ostream& out, t_struct* tstruct);

  void generate_function_helpers(t_function* tfunction);
  std::string get_cap_name(std::string name);
  std::string generate_isset_check(t_field* field);
  std::string generate_isset_check(std::string field);
  void generate_isset_set(ostream& out, t_field* field);
  // removed std::string isset_field_id(t_field* field);

  void generate_service_interface(t_service* tservice, bool combined);
  void generate_service_helpers(t_service* tservice);
  void generate_service_client(t_service* tservice);
  void generate_service_server(t_service* tservice);
  void generate_process_function(t_service* tservice, t_function* tfunction);
  void generate_service_method_signature(t_function* tfunction, bool is_interface, bool combined);

  /**
   * Serialization constructs
   */

  void generate_deserialize_field(std::ostream& out, t_field* tfield, std::string prefix = "");
  void generate_deserialize_struct(std::ostream& out, t_struct* tstruct, std::string prefix = "");
  void generate_deserialize_container(std::ostream& out, t_type* ttype, std::string prefix = "");
  void generate_deserialize_set_element(std::ostream& out, t_set* tset, std::string prefix = "");
  void generate_deserialize_map_element(std::ostream& out, t_map* tmap, std::string prefix = "");
  void generate_deserialize_list_element(std::ostream& out,
                                         t_list* tlist,
                                         std::string prefix = "");

  void generate_serialize_field(std::ostream& out, t_field* tfield, std::string prefix = "");
  void generate_serialize_struct(std::ostream& out, t_struct* tstruct, std::string prefix = "");
  void generate_serialize_container(std::ostream& out, t_type* ttype, std::string prefix = "");
  void generate_serialize_set_element(std::ostream& out, t_set* tmap, std::string iter);
  void generate_serialize_list_element(std::ostream& out, t_list* tlist, std::string iter);
  void generate_serialize_map_element(std::ostream& out,
                                      t_map* tmap,
                                      std::string iter,
                                      std::string map);

  void generate_haxe_doc(std::ostream& out, t_doc* tdoc);
  void generate_haxe_doc(std::ostream& out, t_function* tdoc);

  void generate_rtti_decoration(std::ostream& out);
  void generate_macro_decoration(std::ostream& out);

  /**
   * Helper rendering functions
   */

  std::string haxe_package();
  std::string haxe_type_imports();
  std::string haxe_thrift_imports();
  std::string haxe_thrift_gen_imports(t_struct* tstruct, string& imports);
  std::string haxe_thrift_gen_imports(t_service* tservice);
  std::string type_name(t_type* ttype, bool in_container = false, bool in_init = false);
  std::string base_type_name(t_base_type* tbase, bool in_container = false);
  std::string declare_field(t_field* tfield, bool init = false);
  std::string render_default_value_for_type(t_type* type, bool allow_null);
  std::string function_signature_combined(t_function* tfunction);
  std::string function_signature_normal(t_function* tfunction);
  std::string argument_list(t_struct* tstruct);
  std::string type_to_enum(t_type* ttype);
  std::string get_enum_class_name(t_type* type) override;
  string generate_service_method_onsuccess(t_function* tfunction, bool as_type, bool omit_name);
  void generate_service_method_signature_combined(t_function* tfunction, bool is_interface);
  void generate_service_method_signature_normal(t_function* tfunction, bool is_interface);
  void generate_deprecation_attribute(ostream& out, t_function* func, bool as_comment);
  string make_haxe_string_literal( string const& value);

  bool type_can_be_null(t_type* ttype) {
    ttype = get_true_type(ttype);

    if (ttype->is_container() || ttype->is_struct() || ttype->is_xception() || ttype->is_string()) {
      return true;
    }

    if (ttype->is_base_type()) {
      t_base_type::t_base tbase = ((t_base_type*)ttype)->get_base();
      switch (tbase) {
      case t_base_type::TYPE_STRING:
        // case t_base_type::TYPE_I64:  - Int64 is not really nullable, even though it behaved that
        // way before Haxe 3.2.0
        return true;
      default:
        return false;
      }
    }

    return false;
  }

  std::string constant_name(std::string name);
  std::string make_package_name(std::string value);

private:
  bool rtti_;
  string buildmacro_;

  /**
   * File streams
   */

  std::string package_name_;
  ofstream_with_content_based_conditional_update f_service_;
  std::string package_dir_;
};

/**
 * Prepares for file generation by opening up the necessary file output
 * streams.
 *
 * @param tprogram The program to generate
 */
void t_haxe_generator::init_generator() {
  // Make output directory
  MKDIR(get_out_dir().c_str());
  package_name_ = make_package_name( program_->get_namespace("haxe"));

  string dir = package_name_;
  string subdir = get_out_dir();
  string::size_type loc;
  while ((loc = dir.find(".")) != string::npos) {
    subdir = subdir + "/" + dir.substr(0, loc);
    MKDIR(subdir.c_str());
    dir = dir.substr(loc + 1);
  }
  if (dir.size() > 0) {
    subdir = subdir + "/" + dir;
    MKDIR(subdir.c_str());
  }

  package_dir_ = subdir;
}

// Haxe package names start with lowercase letters
std::string t_haxe_generator::make_package_name(std::string value) {
  std::string retval(value);

  if (retval.length() > 0) {
    retval[0] = tolower(retval[0]);
    size_t index = retval.find('.');
    while (index != std::string::npos) {
      if (++index < retval.length()) {
        retval[index] = tolower(retval[index]);
      }
      index = retval.find('.', index);
    }
  }

  return retval;
}

/**
 * Packages the generated file
 *
 * @return String of the package, i.e. "package org.apache.thriftdemo;"
 */
string t_haxe_generator::haxe_package() {
  if (!package_name_.empty()) {
    return string("package ") + package_name_;
  }
  return "package";
}

/**
 * Prints standard haxe imports
 *
 * @return List of imports for haxe types that are used in here
 */
string t_haxe_generator::haxe_type_imports() {
  return string()
       + "import org.apache.thrift.helper.*;\n"
       + "import haxe.io.Bytes;\n"
       + "import haxe.ds.IntMap;\n"
       + "import haxe.ds.StringMap;\n"
       + "import haxe.ds.ObjectMap;\n"
       + "import org.apache.thrift.helper.ObjectSet;\n"
       + "\n"
       + "#if flash\n"
       + "import flash.errors.ArgumentError;\n"
       + "#end\n"
       + "\n";
}

/**
 * Prints standard haxe imports
 *
 * @return List of imports necessary for thrift
 */
string t_haxe_generator::haxe_thrift_imports() {
  return string()
       + "import org.apache.thrift.*;\n"
       + "import org.apache.thrift.meta_data.*;\n"
       + "import org.apache.thrift.protocol.*;\n"
       + "\n";
}

/**
 * Prints imports needed for a given type
 *
 * @return List of imports necessary for a given t_struct
 */
string t_haxe_generator::haxe_thrift_gen_imports(t_struct* tstruct, string& imports) {

  const vector<t_field*>& members = tstruct->get_members();
  vector<t_field*>::const_iterator m_iter;

  // For each type check if it is from a different namespace
  for (m_iter = members.begin(); m_iter != members.end(); ++m_iter) {
    t_program* program = (*m_iter)->get_type()->get_program();
    if (program != nullptr && program != program_) {
      string package = make_package_name( program->get_namespace("haxe"));
      if (!package.empty()) {
        if (imports.find(package + "." + (*m_iter)->get_type()->get_name()) == string::npos) {
          imports.append("import " + package + "." + get_cap_name((*m_iter)->get_type()->get_name()) + ";\n");
        }
      }
    }
  }
  return imports;
}

/**
 * Prints imports needed for a given type
 *
 * @return List of imports necessary for a given t_service
 */
string t_haxe_generator::haxe_thrift_gen_imports(t_service* tservice) {
  string imports;
  const vector<t_function*>& functions = tservice->get_functions();
  vector<t_function*>::const_iterator f_iter;

  // For each type check if it is from a different namespace
  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
    t_program* program = (*f_iter)->get_returntype()->get_program();
    if (program != nullptr && program != program_) {
      string package = make_package_name( program->get_namespace("haxe"));
      if (!package.empty()) {
        if (imports.find(package + "." + (*f_iter)->get_returntype()->get_name()) == string::npos) {
          imports.append("import " + package + "." + get_cap_name((*f_iter)->get_returntype()->get_name())+ ";\n");
        }
      }
    }

    haxe_thrift_gen_imports((*f_iter)->get_arglist(), imports);
    haxe_thrift_gen_imports((*f_iter)->get_xceptions(), imports);
  }

  return imports;
}

/**
 * Nothing in haxe
 */
void t_haxe_generator::close_generator() {
}

/**
 * Generates a typedef. This is not done in haxe, since it does
 * not support arbitrary name replacements, and it'd be a wacky waste
 * of overhead to make wrapper classes.
 *
 * @param ttypedef The type definition
 */
void t_haxe_generator::generate_typedef(t_typedef* ttypedef) {
  (void)ttypedef;
}

/**
 * Enums are a class with a set of static constants.
 *
 * @param tenum The enumeration
 */
void t_haxe_generator::generate_enum(t_enum* tenum) {
  // Make output file
  string f_enum_name = package_dir_ + "/" + get_cap_name(tenum->get_name()) + ".hx";
  ofstream_with_content_based_conditional_update f_enum;
  f_enum.open(f_enum_name.c_str());

  // Comment and package it
  f_enum << autogen_comment() << haxe_package() << ";" << '\n' << '\n';

  // Add haxe imports
  f_enum << string() + "import org.apache.thrift.helper.*;" << '\n' << '\n';

  generate_rtti_decoration(f_enum);
  generate_macro_decoration(f_enum);
  indent(f_enum) << "class " << get_cap_name(tenum->get_name()) << " ";
  scope_up(f_enum);

  vector<t_enum_value*> constants = tenum->get_constants();
  vector<t_enum_value*>::iterator c_iter;
  for (c_iter = constants.begin(); c_iter != constants.end(); ++c_iter) {
    int value = (*c_iter)->get_value();
    indent(f_enum) << "public static inline var " << (*c_iter)->get_name() << " : Int = " << value
                   << ";" << '\n';
  }

  // Create a static Set with all valid values for this enum
  f_enum << '\n';

  indent(f_enum) << "public static var VALID_VALUES = { new IntSet( [";
  indent_up();
  bool firstValue = true;
  for (c_iter = constants.begin(); c_iter != constants.end(); ++c_iter) {
    // populate set
    f_enum << (firstValue ? "" : ", ") << (*c_iter)->get_name();
    firstValue = false;
  }
  indent_down();
  f_enum << "]); };" << '\n';

  indent(f_enum) << "public static var VALUES_TO_NAMES = { [";
  indent_up();
  firstValue = true;
  for (c_iter = constants.begin(); c_iter != constants.end(); ++c_iter) {
    f_enum << (firstValue ? "" : ",") << '\n';
    indent(f_enum) << (*c_iter)->get_name() << " => \"" << (*c_iter)->get_name() << "\"";
    firstValue = false;
  }
  f_enum << '\n';
  indent_down();
  indent(f_enum) << "]; };" << '\n';

  scope_down(f_enum); // end class

  f_enum.close();
}

/**
 * Generates a class that holds all the constants.
 */
void t_haxe_generator::generate_consts(std::vector<t_const*> consts) {
  if (consts.empty()) {
    return;
  }

  string f_consts_name = package_dir_ + "/" + get_cap_name(program_name_) + "Constants.hx";
  ofstream_with_content_based_conditional_update f_consts;
  f_consts.open(f_consts_name.c_str());

  // Print header
  f_consts << autogen_comment() << haxe_package() << ";" << '\n' << '\n';

  f_consts << '\n';

  f_consts << haxe_type_imports();

  generate_rtti_decoration(f_consts);
  generate_macro_decoration(f_consts);
  indent(f_consts) << "class " << get_cap_name(program_name_) << "Constants {" << '\n' << '\n';
  indent_up();
  vector<t_const*>::iterator c_iter;
  for (c_iter = consts.begin(); c_iter != consts.end(); ++c_iter) {
    print_const_value(f_consts,
                      (*c_iter)->get_name(),
                      (*c_iter)->get_type(),
                      (*c_iter)->get_value());
  }
  indent_down();
  indent(f_consts) << "}" << '\n';
  f_consts.close();
}

void t_haxe_generator::print_const_value(std::ostream& out,
                                         string name,
                                         t_type* type,
                                         t_const_value* value) {
  type = get_true_type(type);
  bool complex = type->is_struct() || type->is_xception() || type->is_map() || type->is_list() || type->is_set();

  indent(out);

  out << "public static ";
  if (!complex) {
    out << "inline ";
  }
  out << "var " << name;
  if (complex) {
    out << " (default,null)";
  }
  out << " : " << get_cap_name(type_name(type)) << " = ";
  render_const_value(out, type, value);
  out << ";" << '\n' << '\n';
}

std::string t_haxe_generator::render_const_value_str( t_type* type, t_const_value* value) {
  std::ostringstream render;
  render_const_value(render, type, value);
  return render.str();
}


void t_haxe_generator::render_const_value(std::ostream& out,
                                          t_type* type,
                                          t_const_value* value) {
  type = get_true_type(type);

  if (type->is_base_type()) {
    t_base_type::t_base tbase = ((t_base_type*)type)->get_base();
    switch (tbase) {
    case t_base_type::TYPE_STRING:
      out << '"' << get_escaped_string(value) << '"';
      break;
    case t_base_type::TYPE_UUID:
      out << '"' << get_escaped_string(value) << '"';
      break;
    case t_base_type::TYPE_BOOL:
      out << ((value->get_integer() > 0) ? "true" : "false");
      break;
    case t_base_type::TYPE_I8:
      out << "(byte)" << value->get_integer();
      break;
    case t_base_type::TYPE_I16:
      out << "(short)" << value->get_integer();
      break;
    case t_base_type::TYPE_I32:
      out << value->get_integer();
      break;
    case t_base_type::TYPE_I64:
      out << value->get_integer() << "L";
      break;
    case t_base_type::TYPE_DOUBLE:
      if (value->get_type() == t_const_value::CV_INTEGER) {
        out << "(double)" << value->get_integer();
      } else {
        out << value->get_double();
      }
      break;
    default:
      throw "compiler error: no const of base type " + t_base_type::t_base_name(tbase);
    }
  } else if (type->is_enum()) {
    out << value->get_integer();
  } else if (type->is_struct() || type->is_xception()) {
    render_struct_initializer(out, (t_struct*)type, value);
  } else if (type->is_map()) {
    render_map_initializer(out, (t_map*)type, value);
  } else if (type->is_list()) {
    render_list_initializer(out, (t_list*)type, value);
  } else if (type->is_set()) {
    render_set_initializer(out, (t_set*)type, value);
  } else {
    throw "compiler error: no const of type " + type->get_name();
  }
}

void t_haxe_generator::render_struct_initializer(std::ostream& out,
                                                 t_struct* type,
                                                 t_const_value* value) {
  out << "(function() : " << get_cap_name(type_name(type)) << " {" << '\n';
  indent_up();
  indent(out) << "var tmp = new " << get_cap_name(type_name(type)) <<  "();" << '\n';

  const vector<t_field*>& fields = ((t_struct*)type)->get_members();
  vector<t_field*>::const_iterator f_iter;
  const map<t_const_value*, t_const_value*, t_const_value::value_compare>& values = value->get_map();
  map<t_const_value*, t_const_value*, t_const_value::value_compare>::const_iterator v_iter;
  for (v_iter = values.begin(); v_iter != values.end(); ++v_iter) {
    t_type* field_type = nullptr;
    for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
      if ((*f_iter)->get_name() == v_iter->first->get_string()) {
        field_type = (*f_iter)->get_type();
        break;
      }
    }
    if (field_type == nullptr) {
      throw "type error: " + type->get_name() + " has no field " + v_iter->first->get_string();
    }
    indent(out) << "tmp." << v_iter->first->get_string() << " = ";
    render_const_value(out, field_type, v_iter->second);
    out << ";" << '\n';
  }

  indent(out) << "return tmp;" << '\n';
  indent_down();
  indent(out) << "})()";  // no line break
}

void t_haxe_generator::render_map_initializer(std::ostream& out,
                                              t_map* type,
                                              t_const_value* value) {
  out << "(function() : " << get_cap_name(type_name(type)) << " {" << '\n';
  indent_up();
  indent(out) << "var tmp = new " << get_cap_name(type_name(type)) <<  "();" << '\n';

  t_type* key_type = ((t_map*)type)->get_key_type();
  t_type* val_type = ((t_map*)type)->get_val_type();

  const map<t_const_value*, t_const_value*, t_const_value::value_compare>& values = value->get_map();
  map<t_const_value*, t_const_value*, t_const_value::value_compare>::const_iterator v_iter;
  for (v_iter = values.begin(); v_iter != values.end(); ++v_iter) {
    indent(out) << "tmp.set(";
    render_const_value(out, key_type, v_iter->first);
    out << ", ";
    render_const_value(out, val_type, v_iter->second);
    out << ");" << '\n';
  }

  indent(out) << "return tmp;" << '\n';
  indent_down();
  indent(out) << "})()";  // no line break
}

void t_haxe_generator::render_list_initializer(std::ostream& out,
                                               t_list* type,
                                               t_const_value* value) {
  out << "(function() : " << get_cap_name(type_name(type)) << " {" << '\n';
  indent_up();
  indent(out) << "var tmp = new " << get_cap_name(type_name(type)) <<  "();" << '\n';

  t_type* elm_type = type->get_elem_type();

  const vector<t_const_value*>& values = value->get_list();
  vector<t_const_value*>::const_iterator v_iter;
  for (v_iter = values.begin(); v_iter != values.end(); ++v_iter) {
    indent(out) << "tmp.add(";
    render_const_value(out, elm_type, *v_iter);
    out << ");" << '\n';
  }

  indent(out) << "return tmp;" << '\n';
  indent_down();
  indent(out) << "})()";  // no line break
}

void t_haxe_generator::render_set_initializer(std::ostream& out,
                                              t_set* type,
                                              t_const_value* value) {
  out << "(function() : " << get_cap_name(type_name(type)) << " {" << '\n';
  indent_up();
  indent(out) << "var tmp = new " << get_cap_name(type_name(type)) <<  "();" << '\n';

  t_type* elm_type = type->get_elem_type();

  const vector<t_const_value*>& values = value->get_list();
  vector<t_const_value*>::const_iterator v_iter;
  for (v_iter = values.begin(); v_iter != values.end(); ++v_iter) {
    indent(out) << "tmp.add(";
    render_const_value(out, elm_type, *v_iter);
    out << ");" << '\n';
  }

  indent(out) << "return tmp;" << '\n';
  indent_down();
  indent(out) << "})()";  // no line break
}


/**
 * Generates a struct definition for a thrift data type. This is a class
 * with data members, read(), write(), and an inner Isset class.
 *
 * @param tstruct The struct definition
 */
void t_haxe_generator::generate_struct(t_struct* tstruct) {
  generate_haxe_struct(tstruct, false);
}

/**
 * Exceptions are structs, but they inherit from Exception
 *
 * @param tstruct The struct definition
 */
void t_haxe_generator::generate_xception(t_struct* txception) {
  generate_haxe_struct(txception, true);
}

/**
 * Haxe struct definition.
 *
 * @param tstruct The struct definition
 */
void t_haxe_generator::generate_haxe_struct(t_struct* tstruct, bool is_exception, bool is_result) {
  // Make output file
  string f_struct_name = package_dir_ + "/" + get_cap_name(tstruct->get_name()) + ".hx";
  ofstream_with_content_based_conditional_update f_struct;
  f_struct.open(f_struct_name.c_str());

  f_struct << autogen_comment() << haxe_package() << ";" << '\n';

  f_struct << '\n';

  string imports;

  f_struct << haxe_type_imports() << haxe_thrift_imports()
           << haxe_thrift_gen_imports(tstruct, imports) << '\n';

  generate_haxe_struct_definition(f_struct, tstruct, is_exception, is_result);

  f_struct.close();
}

/**
 * haxe struct definition. This has various parameters, as it could be
 * generated standalone or inside another class as a helper. If it
 * is a helper than it is a static class.
 *
 * @param tstruct      The struct definition
 * @param is_exception Is this an exception?
 * @param in_class     If inside a class, needs to be static class
 * @param is_result    If this is a result it needs a different writer
 */
void t_haxe_generator::generate_haxe_struct_definition(ostream& out,
                                                       t_struct* tstruct,
                                                       bool is_exception,
                                                       bool is_result) {
  generate_haxe_doc(out, tstruct);

  string clsname = get_cap_name(tstruct->get_name());

  generate_rtti_decoration(out);
  generate_macro_decoration(out);
  indent(out) << "class " << clsname << " ";

  if (is_exception) {
    out << "extends TException ";
  }
  out << "implements TBase {" << '\n' << '\n';
  indent_up();

  indent(out) << "static var STRUCT_DESC = { new TStruct(\"" << tstruct->get_name() << "\"); };"
              << '\n';

  const vector<t_field*>& members = tstruct->get_members();
  vector<t_field*>::const_iterator m_iter;

  for (m_iter = members.begin(); m_iter != members.end(); ++m_iter) {
    indent(out) << "static var " << constant_name((*m_iter)->get_name())
                << "_FIELD_DESC = { new TField(\"" << (*m_iter)->get_name() << "\", "
                << type_to_enum((*m_iter)->get_type()) << ", " << (*m_iter)->get_key() << "); };"
                << '\n';
  }
  out << '\n';

  for (m_iter = members.begin(); m_iter != members.end(); ++m_iter) {
    generate_haxe_doc(out, *m_iter);
    // indent(out) << "private var _" << (*m_iter)->get_name() + " : " +
    // type_name((*m_iter)->get_type()) << ";" << '\n';
    indent(out) << "@:isVar" << '\n';
    indent(out) << "public var "
                << (*m_iter)->get_name() + "(get,set) : "
                   + get_cap_name(type_name((*m_iter)->get_type())) << ";" << '\n';
  }

  out << '\n';

  for (m_iter = members.begin(); m_iter != members.end(); ++m_iter) {
    indent(out) << "inline static var " << upcase_string((*m_iter)->get_name())
                << "_FIELD_ID : Int = " << (*m_iter)->get_key() << ";" << '\n';
  }

  out << '\n';

  // Inner Isset class
  if (members.size() > 0) {
    for (m_iter = members.begin(); m_iter != members.end(); ++m_iter) {
      if (!type_can_be_null((*m_iter)->get_type())) {
        indent(out) << "private var __isset_" << (*m_iter)->get_name() << " : Bool = false;"
                    << '\n';
      }
    }
  }

  out << '\n';

  // Static initializer to populate global class to struct metadata map
  if (false) {
    // TODO: reactivate when needed
    generate_haxe_meta_data_map(out, tstruct);
    indent(out) << "{" << '\n';
    indent_up();
    indent(out) << "FieldMetaData.addStructMetaDataMap(" << type_name(tstruct) << ", metaDataMap);"
                << '\n';
    indent_down();
    indent(out) << "}" << '\n';
    indent(out) << "}" << '\n';
  }

  // Default constructor
  indent(out) << "public function new() {" << '\n';
  indent_up();
  if (is_exception) {
    indent(out) << "super();" << '\n';
  }
  for (m_iter = members.begin(); m_iter != members.end(); ++m_iter) {
    if ((*m_iter)->get_value() != nullptr) {
      indent(out) << "this." << (*m_iter)->get_name() << " = ";
      render_const_value( out, (*m_iter)->get_type(), (*m_iter)->get_value());
      out << ";" << '\n';
    }
  }
  indent_down();
  indent(out) << "}" << '\n' << '\n';

  generate_property_getters_setters(out, tstruct);
  generate_generic_field_getters_setters(out, tstruct);
  generate_generic_isset_method(out, tstruct);

  generate_haxe_struct_reader(out, tstruct);
  if (is_result) {
    generate_haxe_struct_result_writer(out, tstruct);
  } else {
    generate_haxe_struct_writer(out, tstruct);
  }
  generate_haxe_struct_tostring(out, tstruct, is_exception);
  generate_haxe_validator(out, tstruct);
  scope_down(out);
  out << '\n';
}

/**
 * Generates a function to read all the fields of the struct.
 *
 * @param tstruct The struct definition
 */
void t_haxe_generator::generate_haxe_struct_reader(ostream& out, t_struct* tstruct) {
  out << indent() << "public function read( iprot : TProtocol) : Void {" << '\n';
  indent_up();

  const vector<t_field*>& fields = tstruct->get_members();
  vector<t_field*>::const_iterator f_iter;

  indent(out) << "iprot.IncrementRecursionDepth();" << '\n';
  indent(out) << "try" << '\n';
  scope_up(out);

  // Declare stack tmp variables and read struct header
  out << indent() << "var field : TField;" << '\n' << indent() << "iprot.readStructBegin();"
      << '\n';

  // Loop over reading in fields
  indent(out) << "while (true)" << '\n';
  scope_up(out);

  // Read beginning field marker
  indent(out) << "field = iprot.readFieldBegin();" << '\n';

  // Check for field STOP marker and break
  indent(out) << "if (field.type == TType.STOP) { " << '\n';
  indent_up();
  indent(out) << "break;" << '\n';
  indent_down();
  indent(out) << "}" << '\n';

  // Switch statement on the field we are reading
  indent(out) << "switch (field.id)" << '\n';

  scope_up(out);

  // Generate deserialization code for known cases
  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    indent(out) << "case " << upcase_string((*f_iter)->get_name()) << "_FIELD_ID:" << '\n';
    indent_up();
    indent(out) << "if (field.type == " << type_to_enum((*f_iter)->get_type()) << ") {" << '\n';
    indent_up();

    generate_deserialize_field(out, *f_iter, "this.");
    generate_isset_set(out, *f_iter);
    indent_down();
    out << indent() << "} else { " << '\n' << indent() << "  TProtocolUtil.skip(iprot, field.type);"
        << '\n' << indent() << "}" << '\n';
    indent_down();
  }

  // In the default case we skip the field
  out << indent() << "default:" << '\n' << indent() << "  TProtocolUtil.skip(iprot, field.type);"
      << '\n';

  scope_down(out);

  // Read field end marker
  indent(out) << "iprot.readFieldEnd();" << '\n';

  scope_down(out);

  out << indent() << "iprot.readStructEnd();" << '\n' << '\n';

  indent(out) << "iprot.DecrementRecursionDepth();" << '\n';
  scope_down(out);
  indent(out) << "catch(e:Dynamic)" << '\n';
  scope_up(out);
  indent(out) << "iprot.DecrementRecursionDepth();" << '\n';
  indent(out) << "throw e;" << '\n';
  scope_down(out);

  // check for required fields of primitive type
  // (which can be checked here but not in the general validate method)
  out << '\n' << indent() << "// check for required fields of primitive type, which can't be "
                             "checked in the validate method" << '\n';
  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    if ((*f_iter)->get_req() == t_field::T_REQUIRED && !type_can_be_null((*f_iter)->get_type())) {
      out << indent() << "if (!__isset_" << (*f_iter)->get_name() << ") {" << '\n' << indent()
          << "  throw new TProtocolException(TProtocolException.UNKNOWN, \"Required field '"
          << (*f_iter)->get_name()
          << "' was not found in serialized data! Struct: \" + toString());" << '\n' << indent()
          << "}" << '\n';
    }
  }

  // performs various checks (e.g. check that all required fields are set)
  indent(out) << "validate();" << '\n';

  indent_down();
  out << indent() << "}" << '\n' << '\n';
}

// generates haxe method to perform various checks
// (e.g. check that all required fields are set)
void t_haxe_generator::generate_haxe_validator(ostream& out, t_struct* tstruct) {
  indent(out) << "public function validate() : Void {" << '\n';
  indent_up();

  const vector<t_field*>& fields = tstruct->get_members();
  vector<t_field*>::const_iterator f_iter;

  out << indent() << "// check for required fields" << '\n';
  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    if ((*f_iter)->get_req() == t_field::T_REQUIRED) {
      if (type_can_be_null((*f_iter)->get_type())) {
        indent(out) << "if (" << (*f_iter)->get_name() << " == null) {" << '\n';
        indent(out)
            << "  throw new TProtocolException(TProtocolException.UNKNOWN, \"Required field '"
            << (*f_iter)->get_name() << "' was not present! Struct: \" + toString());" << '\n';
        indent(out) << "}" << '\n';
      } else {
        indent(out) << "// alas, we cannot check '" << (*f_iter)->get_name()
                    << "' because it's a primitive." << '\n';
      }
    }
  }

  // check that fields of type enum have valid values
  out << indent() << "// check that fields of type enum have valid values" << '\n';
  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    t_field* field = (*f_iter);
    t_type* type = field->get_type();
    // if field is an enum, check that its value is valid
    if (type->is_enum()) {
      indent(out) << "if (" << generate_isset_check(field) << " && !"
                  << get_cap_name(get_enum_class_name(type)) << ".VALID_VALUES.contains("
                  << field->get_name() << ")){" << '\n';
      indent_up();
      indent(out) << "throw new TProtocolException(TProtocolException.UNKNOWN, \"The field '"
                  << field->get_name() << "' has been assigned the invalid value \" + "
                  << field->get_name() << ");" << '\n';
      indent_down();
      indent(out) << "}" << '\n';
    }
  }

  indent_down();
  indent(out) << "}" << '\n' << '\n';
}

/**
 * Generates a function to write all the fields of the struct
 *
 * @param tstruct The struct definition
 */
void t_haxe_generator::generate_haxe_struct_writer(ostream& out, t_struct* tstruct) {
  out << indent() << "public function write(oprot:TProtocol) : Void {" << '\n';
  indent_up();

  string name = tstruct->get_name();
  const vector<t_field*>& fields = tstruct->get_sorted_members();
  vector<t_field*>::const_iterator f_iter;

  // performs various checks (e.g. check that all required fields are set)
  indent(out) << "validate();" << '\n';
  indent(out) << "oprot.IncrementRecursionDepth();" << '\n';
  indent(out) << "try" << '\n';
  scope_up(out);

  indent(out) << "oprot.writeStructBegin(STRUCT_DESC);" << '\n';

  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    bool could_be_unset = (*f_iter)->get_req() == t_field::T_OPTIONAL;
    if (could_be_unset) {
      indent(out) << "if (" << generate_isset_check(*f_iter) << ") {" << '\n';
      indent_up();
    }
    bool null_allowed = type_can_be_null((*f_iter)->get_type());
    if (null_allowed) {
      out << indent() << "if (this." << (*f_iter)->get_name() << " != null) {" << '\n';
      indent_up();
    }

    indent(out) << "oprot.writeFieldBegin(" << constant_name((*f_iter)->get_name())
                << "_FIELD_DESC);" << '\n';

    // Write field contents
    generate_serialize_field(out, *f_iter, "this.");

    // Write field closer
    indent(out) << "oprot.writeFieldEnd();" << '\n';

    if (null_allowed) {
      indent_down();
      indent(out) << "}" << '\n';
    }
    if (could_be_unset) {
      indent_down();
      indent(out) << "}" << '\n';
    }
  }

  indent(out) << "oprot.writeFieldStop();" << '\n';
  indent(out) << "oprot.writeStructEnd();" << '\n';

  indent(out) << "oprot.DecrementRecursionDepth();" << '\n';
  scope_down(out);
  indent(out) << "catch(e:Dynamic)" << '\n';
  scope_up(out);
  indent(out) << "oprot.DecrementRecursionDepth();" << '\n';
  indent(out) << "throw e;" << '\n';
  scope_down(out);

  indent_down();
  out << indent() << "}" << '\n' << '\n';
}

/**
 * Generates a function to write all the fields of the struct,
 * which is a function result. These fields are only written
 * if they are set in the Isset array, and only one of them
 * can be set at a time.
 *
 * @param tstruct The struct definition
 */
void t_haxe_generator::generate_haxe_struct_result_writer(ostream& out, t_struct* tstruct) {
  out << indent() << "public function write(oprot:TProtocol) : Void {" << '\n';
  indent_up();

  string name = tstruct->get_name();
  const vector<t_field*>& fields = tstruct->get_sorted_members();
  vector<t_field*>::const_iterator f_iter;

  indent(out) << "oprot.IncrementRecursionDepth();" << '\n';
  indent(out) << "try" << '\n';
  scope_up(out);

  indent(out) << "oprot.writeStructBegin(STRUCT_DESC);" << '\n';

  bool first = true;
  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    if (first) {
      first = false;
      out << '\n' << indent() << "if ";
    } else {
      out << " else if ";
    }

    out << "(this." << generate_isset_check(*f_iter) << ") {" << '\n';

    indent_up();

    indent(out) << "oprot.writeFieldBegin(" << constant_name((*f_iter)->get_name())
                << "_FIELD_DESC);" << '\n';

    // Write field contents
    generate_serialize_field(out, *f_iter, "this.");

    // Write field closer
    indent(out) << "oprot.writeFieldEnd();" << '\n';

    indent_down();
    indent(out) << "}";
  }

  indent(out) << '\n';
  indent(out) << "oprot.writeFieldStop();" << '\n';
  indent(out) << "oprot.writeStructEnd();" << '\n';

  indent(out) << "oprot.DecrementRecursionDepth();" << '\n';
  scope_down(out);
  indent(out) << "catch(e:Dynamic)" << '\n';
  scope_up(out);
  indent(out) << "oprot.DecrementRecursionDepth();" << '\n';
  indent(out) << "throw e;" << '\n';
  scope_down(out);

  indent_down();
  out << indent() << "}" << '\n' << '\n';
}

void t_haxe_generator::generate_reflection_getters(ostringstream& out,
                                                   t_type* type,
                                                   string field_name,
                                                   string cap_name) {
  (void)type;
  (void)cap_name;
  indent(out) << "case " << upcase_string(field_name) << "_FIELD_ID:" << '\n';
  indent_up();
  indent(out) << "return this." << field_name << ";" << '\n';
  indent_down();
}

void t_haxe_generator::generate_reflection_setters(ostringstream& out,
                                                   t_type* type,
                                                   string field_name,
                                                   string cap_name) {
  (void)type;
  (void)cap_name;
  indent(out) << "case " << upcase_string(field_name) << "_FIELD_ID:" << '\n';
  indent_up();
  indent(out) << "if (value == null) {" << '\n';
  indent(out) << "  unset" << get_cap_name(field_name) << "();" << '\n';
  indent(out) << "} else {" << '\n';
  indent(out) << "  this." << field_name << " = value;" << '\n';
  indent(out) << "}" << '\n' << '\n';

  indent_down();
}

void t_haxe_generator::generate_generic_field_getters_setters(std::ostream& out,
                                                              t_struct* tstruct) {

  std::ostringstream getter_stream;
  std::ostringstream setter_stream;

  // build up the bodies of both the getter and setter at once
  const vector<t_field*>& fields = tstruct->get_members();
  vector<t_field*>::const_iterator f_iter;
  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    t_field* field = *f_iter;
    t_type* type = get_true_type(field->get_type());
    std::string field_name = field->get_name();
    std::string cap_name = get_cap_name(field_name);

    indent_up();
    generate_reflection_setters(setter_stream, type, field_name, cap_name);
    generate_reflection_getters(getter_stream, type, field_name, cap_name);
    indent_down();
  }

  // create the setter
  indent(out) << "public function setFieldValue(fieldID : Int, value : Dynamic) : Void {" << '\n';
  indent_up();

  if (fields.size() > 0) {
    indent(out) << "switch (fieldID) {" << '\n';
    out << setter_stream.str();
    indent(out) << "default:" << '\n';
    indent(out) << "  throw new ArgumentError(\"Field \" + fieldID + \" doesn't exist!\");" << '\n';
    indent(out) << "}" << '\n';
  } else {
    indent(out) << "throw new ArgumentError(\"Field \" + fieldID + \" doesn't exist!\");" << '\n';
  }

  indent_down();
  indent(out) << "}" << '\n' << '\n';

  // create the getter
  indent(out) << "public function getFieldValue(fieldID : Int) : Dynamic {" << '\n';
  indent_up();

  if (fields.size() > 0) {
    indent(out) << "switch (fieldID) {" << '\n';
    out << getter_stream.str();
    indent(out) << "default:" << '\n';
    indent(out) << "  throw new ArgumentError(\"Field \" + fieldID + \" doesn't exist!\");" << '\n';
    indent(out) << "}" << '\n';
  } else {
    indent(out) << "throw new ArgumentError(\"Field \" + fieldID + \" doesn't exist!\");" << '\n';
  }

  indent_down();

  indent(out) << "}" << '\n' << '\n';
}

// Creates a generic isSet method that takes the field number as argument
void t_haxe_generator::generate_generic_isset_method(std::ostream& out, t_struct* tstruct) {
  const vector<t_field*>& fields = tstruct->get_members();
  vector<t_field*>::const_iterator f_iter;

  // create the isSet method
  indent(out) << "// Returns true if field corresponding to fieldID is set (has been assigned a "
                 "value) and false otherwise" << '\n';
  indent(out) << "public function isSet(fieldID : Int) : Bool {" << '\n';
  indent_up();
  if (fields.size() > 0) {
    indent(out) << "switch (fieldID) {" << '\n';

    for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
      t_field* field = *f_iter;
      indent(out) << "case " << upcase_string(field->get_name()) << "_FIELD_ID:" << '\n';
      indent_up();
      indent(out) << "return " << generate_isset_check(field) << ";" << '\n';
      indent_down();
    }

    indent(out) << "default:" << '\n';
    indent(out) << "  throw new ArgumentError(\"Field \" + fieldID + \" doesn't exist!\");" << '\n';
    indent(out) << "}" << '\n';
  } else {
    indent(out) << "throw new ArgumentError(\"Field \" + fieldID + \" doesn't exist!\");" << '\n';
  }

  indent_down();
  indent(out) << "}" << '\n' << '\n';
}

/**
 * Generates a set of property setters/getters for the given struct.
 *
 * @param tstruct The struct definition
 */
void t_haxe_generator::generate_property_getters_setters(ostream& out, t_struct* tstruct) {
  const vector<t_field*>& fields = tstruct->get_members();
  vector<t_field*>::const_iterator f_iter;
  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    t_field* field = *f_iter;
    t_type* type = get_true_type(field->get_type());
    std::string field_name = field->get_name();
    std::string cap_name = get_cap_name(field_name);

    // Simple getter
    generate_haxe_doc(out, field);
    indent(out) << "public function get_" << field_name << "() : " << get_cap_name(type_name(type))
                << " {" << '\n';
    indent_up();
    indent(out) << "return this." << field_name << ";" << '\n';
    indent_down();
    indent(out) << "}" << '\n' << '\n';

    // Simple setter
    generate_haxe_doc(out, field);
    indent(out) << "public function set_" << field_name << "(" << field_name << ":"
                << get_cap_name(type_name(type)) << ") : " << get_cap_name(type_name(type)) << " {"
                << '\n';
    indent_up();
    indent(out) << "this." << field_name << " = " << field_name << ";" << '\n';
    generate_isset_set(out, field);
    indent(out) << "return this." << field_name << ";" << '\n';

    indent_down();
    indent(out) << "}" << '\n' << '\n';

    // Unsetter
    indent(out) << "public function unset" << cap_name << "() : Void {" << '\n';
    indent_up();
    if (type_can_be_null(type)) {
      indent(out) << "this." << field_name << " = null;" << '\n';
    } else {
      indent(out) << "this.__isset_" << field_name << " = false;" << '\n';
    }
    indent_down();
    indent(out) << "}" << '\n' << '\n';

    // isSet method
    indent(out) << "// Returns true if field " << field_name
                << " is set (has been assigned a value) and false otherwise" << '\n';
    indent(out) << "public function is" << get_cap_name("set") << cap_name << "() : Bool {" << '\n';
    indent_up();
    if (type_can_be_null(type)) {
      indent(out) << "return this." << field_name << " != null;" << '\n';
    } else {
      indent(out) << "return this.__isset_" << field_name << ";" << '\n';
    }
    indent_down();
    indent(out) << "}" << '\n' << '\n';
  }
}

/**
 * Generates a toString() method for the given struct
 *
 * @param tstruct The struct definition
 */
void t_haxe_generator::generate_haxe_struct_tostring(ostream& out, t_struct* tstruct, bool is_override) {
  out << indent() << "public ";
  if( is_override) {
    out << "override ";
  }
  out << "function toString() : String {" << '\n';
  indent_up();

  out << indent() << "var ret : String = \"" << tstruct->get_name() << "(\";" << '\n';
  out << indent() << "var first : Bool = true;" << '\n' << '\n';

  const vector<t_field*>& fields = tstruct->get_members();
  vector<t_field*>::const_iterator f_iter;
  bool first = true;
  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    bool could_be_unset = (*f_iter)->get_req() == t_field::T_OPTIONAL;
    if (could_be_unset) {
      indent(out) << "if (" << generate_isset_check(*f_iter) << ") {" << '\n';
      indent_up();
    }

    t_field* field = (*f_iter);

    if (!first) {
      indent(out) << "if (!first) ret +=  \", \";" << '\n';
    }
    indent(out) << "ret += \"" << (*f_iter)->get_name() << ":\";" << '\n';
    bool can_be_null = type_can_be_null(field->get_type());
    if (can_be_null) {
      indent(out) << "if (this." << (*f_iter)->get_name() << " == null) {" << '\n';
      indent(out) << "  ret += \"null\";" << '\n';
      indent(out) << "} else {" << '\n';
      indent_up();
    }

    if (field->get_type()->is_binary()) {
      indent(out) << "  ret += \"BINARY\";" << '\n';
    } else if (field->get_type()->is_enum()) {
      indent(out) << "var " << field->get_name()
                  << "_name : String = " << get_cap_name(get_enum_class_name(field->get_type()))
                  << ".VALUES_TO_NAMES[this." << (*f_iter)->get_name() << "];" << '\n';
      indent(out) << "if (" << field->get_name() << "_name != null) {" << '\n';
      indent(out) << "  ret += " << field->get_name() << "_name;" << '\n';
      indent(out) << "  ret += \" (\";" << '\n';
      indent(out) << "}" << '\n';
      indent(out) << "ret += this." << field->get_name() << ";" << '\n';
      indent(out) << "if (" << field->get_name() << "_name != null) {" << '\n';
      indent(out) << "  ret += \")\";" << '\n';
      indent(out) << "}" << '\n';
    } else {
      indent(out) << "ret += this." << (*f_iter)->get_name() << ";" << '\n';
    }

    if (can_be_null) {
      indent_down();
      indent(out) << "}" << '\n';
    }
    indent(out) << "first = false;" << '\n';

    if (could_be_unset) {
      indent_down();
      indent(out) << "}" << '\n';
    }
    first = false;
  }
  out << indent() << "ret += \")\";" << '\n' << indent() << "return ret;" << '\n';

  indent_down();
  indent(out) << "}" << '\n' << '\n';
}

/**
 * Generates a static map with meta data to store information such as fieldID to
 * fieldName mapping
 *
 * @param tstruct The struct definition
 */
void t_haxe_generator::generate_haxe_meta_data_map(ostream& out, t_struct* tstruct) {
  const vector<t_field*>& fields = tstruct->get_members();
  vector<t_field*>::const_iterator f_iter;

  // Static Map with fieldID -> FieldMetaData mappings
  indent(out) << "inline static var metaDataMap : IntMap = new IntMap();" << '\n';

  if (fields.size() > 0) {
    // Populate map
    scope_up(out);
    for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
      t_field* field = *f_iter;
      std::string field_name = field->get_name();
      indent(out) << "metaDataMap[" << upcase_string(field_name)
                  << "_FIELD_ID] = new FieldMetaData(\"" << field_name << "\", ";

      // Set field requirement type (required, optional, etc.)
      if (field->get_req() == t_field::T_REQUIRED) {
        out << "TFieldRequirementType.REQUIRED, ";
      } else if (field->get_req() == t_field::T_OPTIONAL) {
        out << "TFieldRequirementType.OPTIONAL, ";
      } else {
        out << "TFieldRequirementType.DEFAULT, ";
      }

      // Create value meta data
      generate_field_value_meta_data(out, field->get_type());
      out << ");" << '\n';
    }
    scope_down(out);
  }
}

/**
 * Returns a string with the haxe representation of the given thrift type
 * (e.g. for the type struct it returns "TType.STRUCT")
 */
std::string t_haxe_generator::get_haxe_type_string(t_type* type) {
  if (type->is_list()) {
    return "TType.LIST";
  } else if (type->is_map()) {
    return "TType.MAP";
  } else if (type->is_set()) {
    return "TType.SET";
  } else if (type->is_struct() || type->is_xception()) {
    return "TType.STRUCT";
  } else if (type->is_enum()) {
    return "TType.I32";
  } else if (type->is_typedef()) {
    return get_haxe_type_string(((t_typedef*)type)->get_type());
  } else if (type->is_base_type()) {
    switch (((t_base_type*)type)->get_base()) {
    case t_base_type::TYPE_VOID:
      return "TType.VOID_";
      break;
    case t_base_type::TYPE_STRING:
      return "TType.STRING";
      break;
    case t_base_type::TYPE_UUID:
      return "TType.UUID";
      break;
    case t_base_type::TYPE_BOOL:
      return "TType.BOOL";
      break;
    case t_base_type::TYPE_I8:
      return "TType.BYTE";
      break;
    case t_base_type::TYPE_I16:
      return "TType.I16";
      break;
    case t_base_type::TYPE_I32:
      return "TType.I32";
      break;
    case t_base_type::TYPE_I64:
      return "TType.I64";
      break;
    case t_base_type::TYPE_DOUBLE:
      return "TType.DOUBLE";
      break;
    default:
      throw std::runtime_error("Unknown thrift type \"" + type->get_name()
                               + "\" passed to t_haxe_generator::get_haxe_type_string!");
      break; // This should never happen!
    }
  } else {
    throw std::runtime_error(
        "Unknown thrift type \"" + type->get_name()
        + "\" passed to t_haxe_generator::get_haxe_type_string!"); // This should never happen!
  }
}

void t_haxe_generator::generate_field_value_meta_data(std::ostream& out, t_type* type) {
  out << '\n';
  indent_up();
  indent_up();
  if (type->is_struct()) {
    indent(out) << "new StructMetaData(TType.STRUCT, " << type_name(type);
  } else if (type->is_container()) {
    if (type->is_list()) {
      indent(out) << "new ListMetaData(TType.LIST, ";
      t_type* elem_type = ((t_list*)type)->get_elem_type();
      generate_field_value_meta_data(out, elem_type);
    } else if (type->is_set()) {
      indent(out) << "new SetMetaData(TType.SET, ";
      t_type* elem_type = ((t_list*)type)->get_elem_type();
      generate_field_value_meta_data(out, elem_type);
    } else { // map
      indent(out) << "new MapMetaData(TType.MAP, ";
      t_type* key_type = ((t_map*)type)->get_key_type();
      t_type* val_type = ((t_map*)type)->get_val_type();
      generate_field_value_meta_data(out, key_type);
      out << ", ";
      generate_field_value_meta_data(out, val_type);
    }
  } else {
    indent(out) << "new FieldValueMetaData(" << get_haxe_type_string(type);
  }
  out << ")";
  indent_down();
  indent_down();
}

/**
 * Generates a thrift service. In C++, this comprises an entirely separate
 * header and source file. The header file defines the methods and includes
 * the data types defined in the main header file, and the implementation
 * file contains implementations of the basic printer and default interfaces.
 *
 * @param tservice The service definition
 */
void t_haxe_generator::generate_service(t_service* tservice) {
  // Make service interface file with only "normal" calls
  string f_service_name = package_dir_ + "/" + get_cap_name(service_name_) + "_service.hx";
  f_service_.open(f_service_name.c_str());

  f_service_ << autogen_comment() << haxe_package() << ";" << '\n';

  f_service_ << '\n' << haxe_type_imports() << haxe_thrift_imports()
             << haxe_thrift_gen_imports(tservice);

  if (tservice->get_extends() != nullptr) {
    t_type* parent = tservice->get_extends();
    string parent_namespace = make_package_name( parent->get_program()->get_namespace("haxe"));
    if (!parent_namespace.empty() && parent_namespace != package_name_) {
      f_service_ << "import " << get_cap_name(type_name(parent)) << "_service;" << '\n';
    }
  }

  f_service_ << '\n';

  generate_service_interface(tservice,false);
  f_service_.close();

  // Client interface file with dual suppport ("normal" and "callback" style)
  f_service_name = package_dir_ + "/" + get_cap_name(service_name_) + ".hx";
  f_service_.open(f_service_name.c_str());

  f_service_ << autogen_comment() << haxe_package() << ";" << '\n';

  f_service_ << '\n' << haxe_type_imports() << haxe_thrift_imports()
             << haxe_thrift_gen_imports(tservice);

  if (tservice->get_extends() != nullptr) {
    t_type* parent = tservice->get_extends();
    string parent_namespace = make_package_name( parent->get_program()->get_namespace("haxe"));
    if (!parent_namespace.empty() && parent_namespace != package_name_) {
      f_service_ << "import " << get_cap_name(type_name(parent)) << ";" << '\n';
    }
  }

  f_service_ << '\n';

  generate_service_interface(tservice,true);
  f_service_.close();

  // Now make the implementation/client file
  f_service_name = package_dir_ + "/" + get_cap_name(service_name_) + "Impl.hx";
  f_service_.open(f_service_name.c_str());

  f_service_ << autogen_comment() << haxe_package() << ";" << '\n' << '\n' << haxe_type_imports()
             << haxe_thrift_imports() << haxe_thrift_gen_imports(tservice) << '\n';

  if (tservice->get_extends() != nullptr) {
    t_type* parent = tservice->get_extends();
    string parent_namespace = make_package_name( parent->get_program()->get_namespace("haxe"));
    if (!parent_namespace.empty() && parent_namespace != package_name_) {
      f_service_ << "import " << get_cap_name(type_name(parent)) << "Impl;" << '\n';
    }
  }

  f_service_ << '\n';

  generate_service_client(tservice);
  f_service_.close();

  // Now make the helper class files
  generate_service_helpers(tservice);

  // Now make the processor/server file
  f_service_name = package_dir_ + "/" + get_cap_name(service_name_) + "Processor.hx";
  f_service_.open(f_service_name.c_str());

  f_service_ << autogen_comment() << haxe_package() << ";" << '\n'
             << '\n'
             << haxe_type_imports()
             << haxe_thrift_imports()
             << haxe_thrift_gen_imports(tservice)
             << '\n';

  if (!package_name_.empty()) {
    f_service_ << "import " << package_name_ << ".*;" << '\n';
    f_service_ << "import " << package_name_ << "." << get_cap_name(service_name_).c_str() << "Impl;" << '\n';
    f_service_ << '\n';
  }

  generate_service_server(tservice);
  f_service_.close();
}

/**
 * Generates the code snippet for the onSuccess callbacks
 *
 * @param tfunction The service function to generate code for.
 */
string t_haxe_generator::generate_service_method_onsuccess(t_function* tfunction,
                                                           bool as_type,
                                                           bool omit_name) {
  if (tfunction->is_oneway()) {
    return "";
  }

  string name = "";
  if (!omit_name) {
    name = "onSuccess";
    if (as_type) {
      name += " : ";
    }
  }

  if (tfunction->get_returntype()->is_void()) {
    if (as_type) {
      return name + "Void->Void = null";
    } else {
      return name + "() : Void";
    }
  }

  if (as_type) {
    return name + type_name(tfunction->get_returntype()) + "->Void = null";
  } else {
    return name + "( retval : " + type_name(tfunction->get_returntype()) + ")";
  }
}

/**
 * Generates a service method header
 *
 * @param tfunction The service function to generate code for.
 */
void t_haxe_generator::generate_service_method_signature(t_function* tfunction, bool is_interface, bool combined) {
  if( combined) {
    generate_service_method_signature_combined(tfunction, is_interface);
  } else {
    generate_service_method_signature_normal(tfunction, is_interface);
  }
}

/**
 * Generates a service method header in "normal" style
 *
 * @param tfunction The service function to generate code for.
 */
void t_haxe_generator::generate_service_method_signature_normal(t_function* tfunction,
                                                                bool is_interface) {
  if (is_interface) {
    generate_deprecation_attribute(f_service_, tfunction, true);
    indent(f_service_) << function_signature_normal(tfunction) << ";" << '\n' << '\n';
  } else {
    indent(f_service_) << "public " << function_signature_normal(tfunction) << " {" << '\n';
  }
}

/**
 * Generates a service method header in "callback" style
 *
 * @param tfunction The service function to generate code for.
 */
void t_haxe_generator::generate_service_method_signature_combined(t_function* tfunction,
                                                                  bool is_interface) {
  if (!tfunction->is_oneway()) {
    std::string on_success_impl = generate_service_method_onsuccess(tfunction, false, false);
    indent(f_service_) << "// function onError(Dynamic) : Void;" << '\n';
    indent(f_service_) << "// function " << on_success_impl.c_str() << ";" << '\n';
  }

  if (is_interface) {
    generate_deprecation_attribute(f_service_, tfunction, false);
    indent(f_service_) << function_signature_combined(tfunction) << ";" << '\n' << '\n';
  } else {
    indent(f_service_) << "public " << function_signature_combined(tfunction) << " {" << '\n';
  }
}

string t_haxe_generator::make_haxe_string_literal( string const& value)
{
  if (value.length() == 0) {
    return "";
  }

  std::stringstream result;
  result << "\"";
  for (signed char const c: value) {
    if( (c >= 0) && (c < 32)) {  // convert ctrl chars, but leave UTF-8 alone
      int width = std::min( (int)sizeof(c), 6);
      result << "\\u{" << std::hex << std::setw(width) << std::setfill('0') << (int)c << '}';
    } else if ((c == '\\') || (c == '"')) {
      result << "\\" << c;
    } else {
      result << c;   // anything else "as is"
    }
  }
  result << "\"";

  return result.str();
}

void t_haxe_generator::generate_deprecation_attribute(ostream& out, t_function* func, bool as_comment)
{
  auto iter = func->annotations_.find("deprecated");
  if( func->annotations_.end() != iter) {
    if( as_comment) {
      out << indent() << "// DEPRECATED";
    } else {
      out << indent() << "@:deprecated";
    }

    // empty annotation values end up with "1" somewhere, ignore these as well
    if ((iter->second.back().length() > 0) && (iter->second.back() != "1")) {
      string text = make_haxe_string_literal(iter->second.back());
      if( as_comment) {
        out << ": " << text;
      } else {
        out << "(" << text << ")";
      }
    }

    out << '\n';
  }
}

/**
 * Generates a service interface definition.
 *
 * @param tservice The service to generate a header definition for
 */
void t_haxe_generator::generate_service_interface(t_service* tservice, bool combined) {
  string cbk_postfix = combined ? "" : "_service";

  string extends_iface = "";
  if (tservice->get_extends() != nullptr) {
    extends_iface = " extends " + get_cap_name(type_name(tservice->get_extends())) + cbk_postfix;
  }

  vector<t_function*> functions = tservice->get_functions();
  vector<t_function*>::iterator f_iter;

  generate_haxe_doc(f_service_, tservice);
  generate_rtti_decoration(f_service_);
  generate_macro_decoration(f_service_);
  f_service_ << indent() << "interface " << get_cap_name(service_name_) << cbk_postfix << extends_iface << " {"
             << '\n' << '\n';
  indent_up();
  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
    generate_haxe_doc(f_service_, *f_iter);
    generate_service_method_signature(*f_iter, true, combined);
  }
  indent_down();
  f_service_ << indent() << "}" << '\n' << '\n';
}

/**
 * Generates structs for all the service args and return types
 *
 * @param tservice The service
 */
void t_haxe_generator::generate_service_helpers(t_service* tservice) {
  f_service_ << '\n' << '\n';
  vector<t_function*> functions = tservice->get_functions();
  vector<t_function*>::iterator f_iter;
  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
    t_struct* ts = (*f_iter)->get_arglist();
    generate_haxe_struct(ts, false);
    generate_function_helpers(*f_iter);
  }
}

/**
 * Generates a service client definition.
 *
 * @param tservice The service to generate a server for.
 */
void t_haxe_generator::generate_service_client(t_service* tservice) {
  string extends = "";
  string extends_client = "";
  if (tservice->get_extends() != nullptr) {
    extends = get_cap_name(type_name(tservice->get_extends()));
    extends_client = " extends " + extends + "Impl";
  }

  generate_rtti_decoration(f_service_);
  // build macro is inherited from interface
  indent(f_service_) << "class " << get_cap_name(service_name_) << "Impl" << extends_client
                     << " implements " << get_cap_name(service_name_) << " {" << '\n' << '\n';
  indent_up();

  indent(f_service_) << "public function new( iprot : TProtocol, oprot : TProtocol = null)" << '\n';
  scope_up(f_service_);
  if (extends.empty()) {
    f_service_ << indent() << "iprot_ = iprot;" << '\n';
    f_service_ << indent() << "if (oprot == null) {" << '\n';
    indent_up();
    f_service_ << indent() << "oprot_ = iprot;" << '\n';
    indent_down();
    f_service_ << indent() << "} else {" << '\n';
    indent_up();
    f_service_ << indent() << "oprot_ = oprot;" << '\n';
    indent_down();
    f_service_ << indent() << "}" << '\n';
  } else {
    f_service_ << indent() << "super(iprot, oprot);" << '\n';
  }
  scope_down(f_service_);
  f_service_ << '\n';

  if (extends.empty()) {
    f_service_ << indent() << "private var iprot_ : TProtocol;" << '\n' << indent()
               << "private var oprot_ : TProtocol;" << '\n' << indent()
               << "private var seqid_ : Int;" << '\n' << '\n';

    indent(f_service_) << "public function getInputProtocol() : TProtocol" << '\n';
    scope_up(f_service_);
    indent(f_service_) << "return this.iprot_;" << '\n';
    scope_down(f_service_);
    f_service_ << '\n';

    indent(f_service_) << "public function getOutputProtocol() : TProtocol" << '\n';
    scope_up(f_service_);
    indent(f_service_) << "return this.oprot_;" << '\n';
    scope_down(f_service_);
    f_service_ << '\n';
  }

  // Generate client method implementations
  vector<t_function*> functions = tservice->get_functions();
  vector<t_function*>::const_iterator f_iter;
  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
    string funname = (*f_iter)->get_name();

    // Open function
    generate_service_method_signature(*f_iter, false, true);

    indent_up();

    // Get the struct of function call params
    t_struct* arg_struct = (*f_iter)->get_arglist();

    string argsname = get_cap_name((*f_iter)->get_name() + "_args");
    vector<t_field*>::const_iterator fld_iter;
    const vector<t_field*>& fields = arg_struct->get_members();

    // Serialize the request
    string args = tmp("args");
    string calltype = (*f_iter)->is_oneway() ? "ONEWAY" : "CALL";
    f_service_ << indent() << "oprot_.writeMessageBegin(new TMessage(\"" << funname
               << "\", TMessageType." << calltype << ", seqid_));" << '\n' << indent()
               << "var " << args << " : " << argsname << " = new " << argsname << "();" << '\n';

    for (fld_iter = fields.begin(); fld_iter != fields.end(); ++fld_iter) {
      f_service_ << indent() << args << "." << (*fld_iter)->get_name() << " = "
                 << (*fld_iter)->get_name() << ";" << '\n';
    }

    f_service_ << indent() << args << ".write(oprot_);" << '\n' << indent()
               << "oprot_.writeMessageEnd();" << '\n';

    string retval = tmp("retval");
    if (!((*f_iter)->is_oneway() || (*f_iter)->get_returntype()->is_void())) {
      f_service_ << indent() << "var " << retval << " : " << type_name((*f_iter)->get_returntype())
                 << " = " << render_default_value_for_type((*f_iter)->get_returntype(),true)
                 << ";" << '\n';
    }

    if ((*f_iter)->is_oneway()) {
      f_service_ << indent() << "oprot_.getTransport().flush();" << '\n';
    } else {
      indent(f_service_) << "oprot_.getTransport().flush(function(error:Dynamic) : Void {" << '\n';
      indent_up();
      indent(f_service_) << "try {" << '\n';
      indent_up();
      string appex = tmp("appex");
      indent(f_service_) << "var " << appex << " : TApplicationException;" << '\n';
      string resultname = get_cap_name((*f_iter)->get_name() + "_result");
      indent(f_service_) << "if (error != null) {" << '\n';
      indent_up();
      indent(f_service_) << "if (onError == null)" << '\n';
      indent_up();
      indent(f_service_) << "throw error;" << '\n';
      indent_down();
      indent(f_service_) << "onError(error);" << '\n';
      indent(f_service_) << "return;" << '\n';
      indent_down();
      indent(f_service_) << "}" << '\n' << '\n';
      string msg = tmp("msg");
      indent(f_service_) << "var " << msg << " : TMessage = iprot_.readMessageBegin();" << '\n';
      indent(f_service_) << "if (" << msg << ".type == TMessageType.EXCEPTION) {" << '\n';
      indent_up();
      indent(f_service_) << appex << " = TApplicationException.read(iprot_);" << '\n';
      indent(f_service_) << "iprot_.readMessageEnd();" << '\n';
      indent(f_service_) << "if (onError == null)" << '\n';
      indent_up();
      indent(f_service_) << "throw " << appex << ";" << '\n';
      indent_down();
      indent(f_service_) << "onError(" << appex << ");" << '\n';
      indent(f_service_) << "return;" << '\n';
      indent_down();
      indent(f_service_) << "}" << '\n' << '\n';
      string result = tmp("result");
      indent(f_service_) << "var " << result << " : " << resultname << " = new " << resultname << "();" << '\n';
      indent(f_service_) << "" << result << ".read(iprot_);" << '\n';
      indent(f_service_) << "iprot_.readMessageEnd();" << '\n';

      // Careful, only return _result if not a void function
      if (!(*f_iter)->get_returntype()->is_void()) {
        indent(f_service_) << "if (" << result << "." << generate_isset_check("success") << ") {" << '\n';
        indent_up();
        indent(f_service_) << "if (onSuccess != null)" << '\n';
        indent_up();
        indent(f_service_) << "onSuccess(" << result << ".success);" << '\n';
        indent_down();
        indent(f_service_) << retval << " = " << result << ".success;" << '\n';
        indent(f_service_) << "return;" << '\n';
        indent_down();
        indent(f_service_) << "}" << '\n' << '\n';
      }

      t_struct* xs = (*f_iter)->get_xceptions();
      const std::vector<t_field*>& xceptions = xs->get_members();
      vector<t_field*>::const_iterator x_iter;
      for (x_iter = xceptions.begin(); x_iter != xceptions.end(); ++x_iter) {
        indent(f_service_) << "if (" << result << "." << (*x_iter)->get_name() << " != null) {" << '\n';
        indent_up();
        indent(f_service_) << "if (onError == null)" << '\n';
        indent_up();
        indent(f_service_) << "throw " << result << "." << (*x_iter)->get_name() << ";" << '\n';
        indent_down();
        indent(f_service_) << "onError(" << result << "." << (*x_iter)->get_name() << ");" << '\n';
        indent(f_service_) << "return;" << '\n';
        indent_down();
        indent(f_service_) << "}" << '\n' << '\n';
      }

      // If you get here it's an exception, unless a void function
      if ((*f_iter)->get_returntype()->is_void()) {
        indent(f_service_) << "if (onSuccess != null)" << '\n';
        indent_up();
        indent(f_service_) << "onSuccess();" << '\n';
        indent_down();
        indent(f_service_) << "return;" << '\n';
      } else {
        indent(f_service_) << appex << " = new TApplicationException("
                           << "TApplicationException.MISSING_RESULT,"
                           << "\"" << (*f_iter)->get_name() << " failed: unknown result\");" << '\n';
        indent(f_service_) << "if (onError == null)" << '\n';
        indent_up();
        indent(f_service_) << "throw " << appex << ";" << '\n';
        indent_down();
        indent(f_service_) << "onError(" << appex << ");" << '\n';
        indent(f_service_) << "return;" << '\n';
      }

      indent_down();
      indent(f_service_) << '\n';
      indent(f_service_) << "} catch( e : TException) {" << '\n';
      indent_up();
      indent(f_service_) << "if (onError == null)" << '\n';
      indent_up();
      indent(f_service_) << "throw e;" << '\n';
      indent_down();
      indent(f_service_) << "onError(e);" << '\n';
      indent(f_service_) << "return;" << '\n';
      indent_down();
      indent(f_service_) << "}" << '\n';

      indent_down();
      indent(f_service_) << "});" << '\n' << '\n';
    }

    if (!((*f_iter)->is_oneway() || (*f_iter)->get_returntype()->is_void())) {
      f_service_ << indent() << "return " << retval << ";" << '\n';
    }

    // Close function
    scope_down(f_service_);
    f_service_ << '\n';
  }

  indent_down();
  indent(f_service_) << "}" << '\n';
}

/**
 * Generates a service server definition.
 *
 * @param tservice The service to generate a server for.
 */
void t_haxe_generator::generate_service_server(t_service* tservice) {
  // Generate the dispatch methods
  vector<t_function*> functions = tservice->get_functions();
  vector<t_function*>::iterator f_iter;

  // Extends stuff
  string extends = "";
  string extends_processor = "";
  if (tservice->get_extends() != nullptr) {
    extends = get_cap_name(type_name(tservice->get_extends()));
    extends_processor = " extends " + extends + "Processor";
  }

  // Generate the header portion
  generate_rtti_decoration(f_service_);
  generate_macro_decoration(f_service_);
  indent(f_service_) << "class " << get_cap_name(service_name_) << "Processor" << extends_processor
                     << " implements TProcessor {" << '\n' << '\n';
  indent_up();

  f_service_ << indent() << "private var " << get_cap_name(service_name_)
             << "_iface_ : " << get_cap_name(service_name_) << "_service;" << '\n';

  if (extends.empty()) {
    f_service_ << indent()
               << "private var PROCESS_MAP = new StringMap< Int->TProtocol->TProtocol->Void >();"
               << '\n';
  }

  f_service_ << '\n';

  indent(f_service_) << "public function new( iface : " << get_cap_name(service_name_) << "_service)"
                     << '\n';
  scope_up(f_service_);
  if (!extends.empty()) {
    f_service_ << indent() << "super(iface);" << '\n';
  }
  f_service_ << indent() << get_cap_name(service_name_) << "_iface_ = iface;" << '\n';

  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
    f_service_ << indent() << "PROCESS_MAP.set(\"" << (*f_iter)->get_name() << "\", "
               << (*f_iter)->get_name() << "());" << '\n';
  }

  scope_down(f_service_);
  f_service_ << '\n';

  // Generate the server implementation
  string override = "";
  if (tservice->get_extends() != nullptr) {
    override = "override ";
  }
  indent(f_service_) << override
                     << "public function process( iprot : TProtocol, oprot : TProtocol) : Bool"
                     << '\n';
  scope_up(f_service_);

  f_service_ << indent() << "var msg : TMessage = iprot.readMessageBegin();" << '\n';

  // TODO(mcslee): validate message, was the seqid etc. legit?

  f_service_
      << indent() << "var fn  = PROCESS_MAP.get(msg.name);" << '\n'
      << indent() << "if (fn == null) {" << '\n'
      << indent() << "  TProtocolUtil.skip(iprot, TType.STRUCT);" << '\n'
      << indent() << "  iprot.readMessageEnd();" << '\n'
      << indent() << "  var appex = new TApplicationException(TApplicationException.UNKNOWN_METHOD, "
                  << "\"Invalid method name: '\"+msg.name+\"'\");" << '\n'
      << indent() << "  oprot.writeMessageBegin(new TMessage(msg.name, TMessageType.EXCEPTION, msg.seqid));" << '\n'
      << indent() << "  appex.write(oprot);" << '\n' << indent() << "  oprot.writeMessageEnd();" << '\n'
      << indent() << "  oprot.getTransport().flush();" << '\n'
      << indent() << "  return true;" << '\n' << indent() << "}" << '\n'
      << indent() << "fn( msg.seqid, iprot, oprot);" << '\n'
      ;

  f_service_ << indent() << "return true;" << '\n';

  scope_down(f_service_);
  f_service_ << '\n';

  // Generate the process subfunctions
  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
    generate_process_function(tservice, *f_iter);
  }

  indent_down();
  indent(f_service_) << "}" << '\n' << '\n';
}

/**
 * Generates a struct and helpers for a function.
 *
 * @param tfunction The function
 */
void t_haxe_generator::generate_function_helpers(t_function* tfunction) {
  if (tfunction->is_oneway()) {
    return;
  }

  string resultname = get_cap_name(tfunction->get_name() + "_result");
  t_struct result(program_, resultname);
  t_field success(tfunction->get_returntype(), "success", 0);
  if (!tfunction->get_returntype()->is_void()) {
    result.append(&success);
  }

  t_struct* xs = tfunction->get_xceptions();
  const vector<t_field*>& fields = xs->get_members();
  vector<t_field*>::const_iterator f_iter;
  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    result.append(*f_iter);
  }

  generate_haxe_struct(&result, false, true);
}

/**
 * Generates a process function definition.
 *
 * @param tfunction The function to write a dispatcher for
 */
void t_haxe_generator::generate_process_function(t_service* tservice, t_function* tfunction) {
  (void)tservice;
  // Open class
  indent(f_service_) << "private function " << tfunction->get_name()
                     << "() : Int->TProtocol->TProtocol->Void {" << '\n';
  indent_up();

  // Open function
  indent(f_service_) << "return function( seqid : Int, iprot : TProtocol, oprot : TProtocol) : Void"
                     << '\n';
  scope_up(f_service_);

  string argsname = get_cap_name(tfunction->get_name() + "_args");
  string resultname = get_cap_name(tfunction->get_name() + "_result");

  f_service_ << indent() << "var args : " << argsname << " = new " << argsname << "();" << '\n'
             << indent() << "args.read(iprot);" << '\n' << indent() << "iprot.readMessageEnd();"
             << '\n';

  t_struct* xs = tfunction->get_xceptions();
  const std::vector<t_field*>& xceptions = xs->get_members();
  vector<t_field*>::const_iterator x_iter;

  // Declare result for non oneway function
  if (!tfunction->is_oneway()) {
    f_service_ << indent() << "var result : " << resultname << " = new " << resultname << "();" << '\n';
  }

  // Try block for any  function to catch (defined or undefined) exceptions
  f_service_ << indent() << "try {" << '\n';
  indent_up();


  // normal function():result style

  // Generate the function call
  t_struct* arg_struct = tfunction->get_arglist();
  const std::vector<t_field*>& fields = arg_struct->get_members();
  vector<t_field*>::const_iterator f_iter;

  f_service_ << indent();
  if (!(tfunction->is_oneway() || tfunction->get_returntype()->is_void())) {
    f_service_ << "result.success = ";
  }
  f_service_ << get_cap_name(service_name_) << "_iface_." << tfunction->get_name() << "(";
  bool first = true;
  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    if (first) {
      first = false;
    } else {
      f_service_ << ", ";
    }
    f_service_ << "args." << (*f_iter)->get_name();
  }
  f_service_ << ");" << '\n';

  indent_down();
  f_service_ << indent() << "}";
  if (!tfunction->is_oneway()) {
    // catch exceptions defined in the IDL
    for (x_iter = xceptions.begin(); x_iter != xceptions.end(); ++x_iter) {
      f_service_ << " catch (" << (*x_iter)->get_name() << ":"
                 << get_cap_name(type_name((*x_iter)->get_type(), false, false)) << ") {" << '\n';
      if (!tfunction->is_oneway()) {
        indent_up();
        f_service_ << indent() << "result." << (*x_iter)->get_name() << " = "
                   << (*x_iter)->get_name() << ";" << '\n';
        indent_down();
        f_service_ << indent() << "}";
      } else {
        f_service_ << "}";
      }
    }
  }

  // always catch all exceptions to prevent from service denial
  string appex = tmp("appex");
  f_service_ << " catch (th : Dynamic) {" << '\n';
  indent_up();
  indent(f_service_) << "trace(\"Internal error processing " << tfunction->get_name() << "\", th);" << '\n';
  if (!tfunction->is_oneway()) {
    indent(f_service_) << "var appex = new TApplicationException(TApplicationException.INTERNAL_ERROR, "
                          "\"Internal error processing " << tfunction->get_name() << "\");" << '\n';
    indent(f_service_) << "oprot.writeMessageBegin(new TMessage(\"" << tfunction->get_name()
                       << "\", TMessageType.EXCEPTION, seqid));" << '\n';
    indent(f_service_) << "appex.write(oprot);" << '\n';
    indent(f_service_) << "oprot.writeMessageEnd();" << '\n';
    indent(f_service_) << "oprot.getTransport().flush();" << '\n';
  }
  indent(f_service_) << "return;" << '\n';
  indent_down();
  f_service_ << indent() << "}" << '\n';

  // Shortcut out here for oneway functions
  if (tfunction->is_oneway()) {
    f_service_ << indent() << "return;" << '\n';
    scope_down(f_service_);

    // Close class
    indent_down();
    f_service_ << indent() << "}" << '\n' << '\n';
    return;
  }

  f_service_ << indent() << "oprot.writeMessageBegin(new TMessage(\"" << tfunction->get_name()
             << "\", TMessageType.REPLY, seqid));" << '\n' << indent() << "result.write(oprot);"
             << '\n' << indent() << "oprot.writeMessageEnd();" << '\n' << indent()
             << "oprot.getTransport().flush();" << '\n';

  // Close function
  scope_down(f_service_);
  f_service_ << '\n';

  // Close class
  indent_down();
  f_service_ << indent() << "}" << '\n' << '\n';
}

/**
 * Deserializes a field of any type.
 *
 * @param tfield The field
 * @param prefix The variable name or container for this field
 */
void t_haxe_generator::generate_deserialize_field(ostream& out, t_field* tfield, string prefix) {
  t_type* type = get_true_type(tfield->get_type());

  if (type->is_void()) {
    throw "CANNOT GENERATE DESERIALIZE CODE FOR void TYPE: " + prefix + tfield->get_name();
  }

  string name = prefix + tfield->get_name();

  if (type->is_struct() || type->is_xception()) {
    generate_deserialize_struct(out, (t_struct*)type, name);
  } else if (type->is_container()) {
    generate_deserialize_container(out, type, name);
  } else if (type->is_base_type() || type->is_enum()) {

    indent(out) << name << " = iprot.";

    if (type->is_base_type()) {
      t_base_type::t_base tbase = ((t_base_type*)type)->get_base();
      switch (tbase) {
      case t_base_type::TYPE_VOID:
        throw "compiler error: cannot serialize void field in a struct: " + name;
        break;
      case t_base_type::TYPE_STRING:
        if (type->is_binary()) {
          out << "readBinary();";
        } else {
          out << "readString();";
        }
        break;
      case t_base_type::TYPE_UUID:
        out << "readUuid();";
        break;
      case t_base_type::TYPE_BOOL:
        out << "readBool();";
        break;
      case t_base_type::TYPE_I8:
        out << "readByte();";
        break;
      case t_base_type::TYPE_I16:
        out << "readI16();";
        break;
      case t_base_type::TYPE_I32:
        out << "readI32();";
        break;
      case t_base_type::TYPE_I64:
        out << "readI64();";
        break;
      case t_base_type::TYPE_DOUBLE:
        out << "readDouble();";
        break;
      default:
        throw "compiler error: no Haxe name for base type " + t_base_type::t_base_name(tbase);
      }
    } else if (type->is_enum()) {
      out << "readI32();";
    }
    out << '\n';
  } else {
    printf("DO NOT KNOW HOW TO DESERIALIZE FIELD '%s' TYPE '%s'\n",
           tfield->get_name().c_str(),
           type_name(type).c_str());
  }
}

/**
 * Generates an unserializer for a struct, invokes read()
 */
void t_haxe_generator::generate_deserialize_struct(ostream& out,
                                                   t_struct* tstruct,
                                                   string prefix) {
  out << indent() << prefix << " = new " << get_cap_name(type_name(tstruct)) << "();" << '\n'
      << indent() << prefix << ".read(iprot);" << '\n';
}

/**
 * Deserializes a container by reading its size and then iterating
 */
void t_haxe_generator::generate_deserialize_container(ostream& out, t_type* ttype, string prefix) {
  scope_up(out);

  string obj;

  if (ttype->is_map()) {
    obj = tmp("_map");
  } else if (ttype->is_set()) {
    obj = tmp("_set");
  } else if (ttype->is_list()) {
    obj = tmp("_list");
  }

  // Declare variables, read header
  if (ttype->is_map()) {
    indent(out) << "var " << obj << " = iprot.readMapBegin();" << '\n';
  } else if (ttype->is_set()) {
    indent(out) << "var " << obj << " = iprot.readSetBegin();" << '\n';
  } else if (ttype->is_list()) {
    indent(out) << "var " << obj << " = iprot.readListBegin();" << '\n';
  }

  indent(out) << prefix << " = new " << type_name(ttype, false, true)
              // size the collection correctly
              << "("
              << ");" << '\n';

  // For loop iterates over elements
  string i = tmp("_i");
  indent(out) << "for( " << i << " in 0 ... " << obj << ".size)" << '\n';

  scope_up(out);

  if (ttype->is_map()) {
    generate_deserialize_map_element(out, (t_map*)ttype, prefix);
  } else if (ttype->is_set()) {
    generate_deserialize_set_element(out, (t_set*)ttype, prefix);
  } else if (ttype->is_list()) {
    generate_deserialize_list_element(out, (t_list*)ttype, prefix);
  }

  scope_down(out);

  // Read container end
  if (ttype->is_map()) {
    indent(out) << "iprot.readMapEnd();" << '\n';
  } else if (ttype->is_set()) {
    indent(out) << "iprot.readSetEnd();" << '\n';
  } else if (ttype->is_list()) {
    indent(out) << "iprot.readListEnd();" << '\n';
  }

  scope_down(out);
}

/**
 * Generates code to deserialize a map
 */
void t_haxe_generator::generate_deserialize_map_element(ostream& out, t_map* tmap, string prefix) {
  string key = tmp("_key");
  string val = tmp("_val");
  t_field fkey(tmap->get_key_type(), key);
  t_field fval(tmap->get_val_type(), val);

  indent(out) << declare_field(&fkey) << '\n';
  indent(out) << declare_field(&fval) << '\n';

  generate_deserialize_field(out, &fkey);
  generate_deserialize_field(out, &fval);

  indent(out) << prefix << ".set( " << key << ", " << val << ");" << '\n';
}

/**
 * Deserializes a set element
 */
void t_haxe_generator::generate_deserialize_set_element(ostream& out, t_set* tset, string prefix) {
  string elem = tmp("_elem");
  t_field felem(tset->get_elem_type(), elem);

  indent(out) << declare_field(&felem) << '\n';

  generate_deserialize_field(out, &felem);

  indent(out) << prefix << ".add(" << elem << ");" << '\n';
}

/**
 * Deserializes a list element
 */
void t_haxe_generator::generate_deserialize_list_element(ostream& out,
                                                         t_list* tlist,
                                                         string prefix) {
  string elem = tmp("_elem");
  t_field felem(tlist->get_elem_type(), elem);

  indent(out) << declare_field(&felem) << '\n';

  generate_deserialize_field(out, &felem);

  indent(out) << prefix << ".add(" << elem << ");" << '\n';
}

/**
 * Serializes a field of any type.
 *
 * @param tfield The field to serialize
 * @param prefix Name to prepend to field name
 */
void t_haxe_generator::generate_serialize_field(ostream& out, t_field* tfield, string prefix) {
  t_type* type = get_true_type(tfield->get_type());

  // Do nothing for void types
  if (type->is_void()) {
    throw "CANNOT GENERATE SERIALIZE CODE FOR void TYPE: " + prefix + tfield->get_name();
  }

  if (type->is_struct() || type->is_xception()) {
    generate_serialize_struct(out, (t_struct*)type, prefix + tfield->get_name());
  } else if (type->is_container()) {
    generate_serialize_container(out, type, prefix + tfield->get_name());
  } else if (type->is_base_type() || type->is_enum()) {

    string name = prefix + tfield->get_name();
    indent(out) << "oprot.";

    if (type->is_base_type()) {
      t_base_type::t_base tbase = ((t_base_type*)type)->get_base();
      switch (tbase) {
      case t_base_type::TYPE_VOID:
        throw "compiler error: cannot serialize void field in a struct: " + name;
        break;
      case t_base_type::TYPE_STRING:
        if (type->is_binary()) {
          out << "writeBinary(" << name << ");";
        } else {
          out << "writeString(" << name << ");";
        }
        break;
      case t_base_type::TYPE_UUID:
        out << "writeUuid(" << name << ");";
        break;
      case t_base_type::TYPE_BOOL:
        out << "writeBool(" << name << ");";
        break;
      case t_base_type::TYPE_I8:
        out << "writeByte(" << name << ");";
        break;
      case t_base_type::TYPE_I16:
        out << "writeI16(" << name << ");";
        break;
      case t_base_type::TYPE_I32:
        out << "writeI32(" << name << ");";
        break;
      case t_base_type::TYPE_I64:
        out << "writeI64(" << name << ");";
        break;
      case t_base_type::TYPE_DOUBLE:
        out << "writeDouble(" << name << ");";
        break;
      default:
        throw "compiler error: no Haxe name for base type " + t_base_type::t_base_name(tbase);
      }
    } else if (type->is_enum()) {
      out << "writeI32(" << name << ");";
    }
    out << '\n';
  } else {
    printf("DO NOT KNOW HOW TO SERIALIZE FIELD '%s%s' TYPE '%s'\n",
           prefix.c_str(),
           tfield->get_name().c_str(),
           type_name(type).c_str());
  }
}

/**
 * Serializes all the members of a struct.
 *
 * @param tstruct The struct to serialize
 * @param prefix  String prefix to attach to all fields
 */
void t_haxe_generator::generate_serialize_struct(ostream& out, t_struct* tstruct, string prefix) {
  (void)tstruct;
  out << indent() << prefix << ".write(oprot);" << '\n';
}

/**
 * Serializes a container by writing its size then the elements.
 *
 * @param ttype  The type of container
 * @param prefix String prefix for fields
 */
void t_haxe_generator::generate_serialize_container(ostream& out, t_type* ttype, string prefix) {

  if (ttype->is_map()) {
    string iter = tmp("_key");
    string counter = tmp("_sizeCounter");
    indent(out) << "var " << counter << " : Int = 0;" << '\n';
    indent(out) << "for( " << iter << " in " << prefix << ") {" << '\n';
    indent(out) << "  " << counter << +"++;" << '\n';
    indent(out) << "}" << '\n';

    indent(out) << "oprot.writeMapBegin(new TMap(" << type_to_enum(((t_map*)ttype)->get_key_type())
                << ", " << type_to_enum(((t_map*)ttype)->get_val_type()) << ", " << counter << "));"
                << '\n';
  } else if (ttype->is_set()) {
    indent(out) << "oprot.writeSetBegin(new TSet(" << type_to_enum(((t_set*)ttype)->get_elem_type())
                << ", " << prefix << ".size));" << '\n';
  } else if (ttype->is_list()) {
    indent(out) << "oprot.writeListBegin(new TList("
                << type_to_enum(((t_list*)ttype)->get_elem_type()) << ", " << prefix << ".length));"
                << '\n';
  }

  string iter = tmp("elem");
  if (ttype->is_map()) {
    indent(out) << "for( " << iter << " in " << prefix << ".keys())" << '\n';
  } else if (ttype->is_set()) {
    indent(out) << "for( " << iter << " in " << prefix << ".toArray())" << '\n';
  } else if (ttype->is_list()) {
    indent(out) << "for( " << iter << " in " << prefix << ")" << '\n';
  }

  scope_up(out);

  if (ttype->is_map()) {
    generate_serialize_map_element(out, (t_map*)ttype, iter, prefix);
  } else if (ttype->is_set()) {
    generate_serialize_set_element(out, (t_set*)ttype, iter);
  } else if (ttype->is_list()) {
    generate_serialize_list_element(out, (t_list*)ttype, iter);
  }

  scope_down(out);

  if (ttype->is_map()) {
    indent(out) << "oprot.writeMapEnd();" << '\n';
  } else if (ttype->is_set()) {
    indent(out) << "oprot.writeSetEnd();" << '\n';
  } else if (ttype->is_list()) {
    indent(out) << "oprot.writeListEnd();" << '\n';
  }

}

/**
 * Serializes the members of a map.
 */
void t_haxe_generator::generate_serialize_map_element(ostream& out,
                                                      t_map* tmap,
                                                      string iter,
                                                      string map) {
  t_field kfield(tmap->get_key_type(), iter);
  generate_serialize_field(out, &kfield, "");
  t_field vfield(tmap->get_val_type(), map + ".get(" + iter + ")");
  generate_serialize_field(out, &vfield, "");
}

/**
 * Serializes the members of a set.
 */
void t_haxe_generator::generate_serialize_set_element(ostream& out, t_set* tset, string iter) {
  t_field efield(tset->get_elem_type(), iter);
  generate_serialize_field(out, &efield, "");
}

/**
 * Serializes the members of a list.
 */
void t_haxe_generator::generate_serialize_list_element(ostream& out, t_list* tlist, string iter) {
  t_field efield(tlist->get_elem_type(), iter);
  generate_serialize_field(out, &efield, "");
}

/**
 * Returns a haxe type name
 *
 * @param ttype The type
 * @param container Is the type going inside a container?
 * @return haxe type name, i.e. HashMap<Key,Value>
 */
string t_haxe_generator::type_name(t_type* ttype, bool in_container, bool in_init) {
  (void)in_init;

  // typedefs are just resolved to their real type
  ttype = get_true_type(ttype);
  string prefix;

  if (ttype->is_base_type()) {
    return base_type_name((t_base_type*)ttype, in_container);
  }

  if (ttype->is_enum()) {
    return "Int";
  }

  if (ttype->is_map()) {
    t_type* tkey = get_true_type(((t_map*)ttype)->get_key_type());
    t_type* tval = get_true_type(((t_map*)ttype)->get_val_type());
    if (tkey->is_base_type()) {
      t_base_type::t_base tbase = ((t_base_type*)tkey)->get_base();
      switch (tbase) {
      case t_base_type::TYPE_STRING:
        if (!(tkey->is_binary())) {
          return "StringMap< " + type_name(tval) + ">";
        }
        break; // default to ObjectMap<>
      case t_base_type::TYPE_UUID:
        return "StringMap< " + type_name(tval) + ">";  // uuids are stored as strings
      case t_base_type::TYPE_I8:
      case t_base_type::TYPE_I16:
      case t_base_type::TYPE_I32:
        return "IntMap< " + type_name(tval) + ">";
      case t_base_type::TYPE_I64:
        return "Int64Map< " + type_name(tval) + ">";
      default:
        break; // default to ObjectMap<>
      }
    }
    if (tkey->is_enum()) {
      return "IntMap< " + type_name(tval) + ">";
    }
    return "ObjectMap< " + type_name(tkey) + ", " + type_name(tval) + ">";
  }

  if (ttype->is_set()) {
    t_type* tkey = get_true_type(((t_set*)ttype)->get_elem_type());
    if (tkey->is_base_type()) {
      t_base_type::t_base tbase = ((t_base_type*)tkey)->get_base();
      switch (tbase) {
      case t_base_type::TYPE_STRING:
        if (!(tkey->is_binary())) {
          return "StringSet";
        }
        break; // default to ObjectSet
      case t_base_type::TYPE_UUID:
        return "StringSet";  // uuids are stored as strings
      case t_base_type::TYPE_I8:
      case t_base_type::TYPE_I16:
      case t_base_type::TYPE_I32:
        return "IntSet";
      case t_base_type::TYPE_I64:
        return "Int64Set";
      default:
        break; // default to ObjectSet
      }
    }
    if (tkey->is_enum()) {
      return "IntSet";
    }
    return "ObjectSet< " + type_name(tkey) + ">";
  }

  if (ttype->is_list()) {
    t_type* telm = ((t_list*)ttype)->get_elem_type();
    return "List< " + type_name(telm) + ">";
  }

  // Check for namespacing
  t_program* program = ttype->get_program();
  if (program != nullptr && program != program_) {
    string package = make_package_name( program->get_namespace("haxe"));
    if (!package.empty()) {
      return package + "." + ttype->get_name();
    }
  }

  return ttype->get_name();
}

/**
 * Returns the haxe type that corresponds to the thrift type.
 *
 * @param tbase The base type
 * @param container Is it going in a haxe container?
 */
string t_haxe_generator::base_type_name(t_base_type* type, bool in_container) {
  (void)in_container;
  t_base_type::t_base tbase = type->get_base();

  switch (tbase) {
  case t_base_type::TYPE_VOID:
    return "Void";
  case t_base_type::TYPE_STRING:
    if (type->is_binary()) {
      return "haxe.io.Bytes";
    } else {
      return "String";
    }
  case t_base_type::TYPE_UUID:
    return "String";
  case t_base_type::TYPE_BOOL:
    return "Bool";
  case t_base_type::TYPE_I8:
  case t_base_type::TYPE_I16:
  case t_base_type::TYPE_I32:
    return "haxe.Int32";
  case t_base_type::TYPE_I64:
    return "haxe.Int64";
  case t_base_type::TYPE_DOUBLE:
    return "Float";
  default:
    throw "compiler error: no Haxe name for base type " + t_base_type::t_base_name(tbase);
  }
}

/**
 * Declares a field, which may include initialization as necessary.
 *
 * @param ttype The type
 */
string t_haxe_generator::declare_field(t_field* tfield, bool init) {
  string result = "var " + tfield->get_name() + " : " + type_name(tfield->get_type());
  if (init) {
    t_type* ttype = get_true_type(tfield->get_type());
    if (ttype->is_base_type() && tfield->get_value() != nullptr) {
      result += " = " + render_const_value_str( ttype, tfield->get_value());
    } else {
      result += " = " + render_default_value_for_type( ttype, false);
    }
  }
  return result + ";";
}

string t_haxe_generator::render_default_value_for_type( t_type* type, bool allow_null) {
  t_type* ttype = get_true_type(type);

  if (ttype->is_base_type()) {
    t_base_type::t_base tbase = ((t_base_type*)ttype)->get_base();
    switch (tbase) {
    case t_base_type::TYPE_VOID:
      throw "NO T_VOID CONSTRUCT";
    case t_base_type::TYPE_STRING:
      return "null";
    case t_base_type::TYPE_UUID:
      return "uuid.Uuid.NIL";
    case t_base_type::TYPE_BOOL:
      return "false";
    case t_base_type::TYPE_I8:
    case t_base_type::TYPE_I16:
    case t_base_type::TYPE_I32:
    case t_base_type::TYPE_I64:
      return "0";
    case t_base_type::TYPE_DOUBLE:
      return "0.0";
    default:
      throw "unhandled type";
    }
  } else if (ttype->is_enum()) {
    return "0";
  } else if (ttype->is_container()) {
    return allow_null ? "null" : "new " + type_name(ttype, false, true) + "()";
  } else {
    return allow_null ? "null" : "new " + type_name(ttype, false, true) + "()";
  }
}

/**
 * Renders a function signature of the form 'type name(args)'
 *
 * @param tfunction Function definition
 * @return String of rendered function definition
 */
string t_haxe_generator::function_signature_combined(t_function* tfunction) {
  std::string on_error_success = "onError : Dynamic->Void = null, "
                                 + generate_service_method_onsuccess(tfunction, true, false);

  std::string arguments = argument_list(tfunction->get_arglist());
  if (!tfunction->is_oneway()) {
    if (arguments != "") {
      arguments += ", ";
    }
    arguments += on_error_success; //"onError : Function, onSuccess : Function";
  }

  std::string resulttype;
  if (tfunction->is_oneway() || tfunction->get_returntype()->is_void()) {
    resulttype = "Void";
  } else {
    resulttype = type_name(tfunction->get_returntype());
  }

  std::string result = "function " + tfunction->get_name() + "(" + arguments + ") : "+resulttype;
  return result;
}

/**
 * Renders a function signature of the form 'type name(args)'
 *
 * @param tfunction Function definition
 * @return String of rendered function definition
 */
string t_haxe_generator::function_signature_normal(t_function* tfunction) {
  std::string arguments = argument_list(tfunction->get_arglist());

  std::string resulttype;
  if (tfunction->is_oneway() || tfunction->get_returntype()->is_void()) {
    resulttype = "Void";
  } else {
    resulttype = type_name(tfunction->get_returntype());
  }

  std::string result = "function " + tfunction->get_name() + "(" + arguments + ") : " + resulttype;
  return result;
}

/**
 * Renders a comma separated field list, with type names
 */
string t_haxe_generator::argument_list(t_struct* tstruct) {
  string result = "";

  const vector<t_field*>& fields = tstruct->get_members();
  vector<t_field*>::const_iterator f_iter;
  bool first = true;
  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    if (first) {
      first = false;
    } else {
      result += ", ";
    }
    result += (*f_iter)->get_name() + " : " + type_name((*f_iter)->get_type());
  }
  return result;
}

/**
 * Converts the parse type to a C++ enum string for the given type.
 */
string t_haxe_generator::type_to_enum(t_type* type) {
  type = get_true_type(type);

  if (type->is_base_type()) {
    t_base_type::t_base tbase = ((t_base_type*)type)->get_base();
    switch (tbase) {
    case t_base_type::TYPE_VOID:
      throw "NO T_VOID CONSTRUCT";
    case t_base_type::TYPE_STRING:
      return "TType.STRING";
    case t_base_type::TYPE_UUID:
      return "TType.UUID";
    case t_base_type::TYPE_BOOL:
      return "TType.BOOL";
    case t_base_type::TYPE_I8:
      return "TType.BYTE";
    case t_base_type::TYPE_I16:
      return "TType.I16";
    case t_base_type::TYPE_I32:
      return "TType.I32";
    case t_base_type::TYPE_I64:
      return "TType.I64";
    case t_base_type::TYPE_DOUBLE:
      return "TType.DOUBLE";
    default:
      break;
    }
  } else if (type->is_enum()) {
    return "TType.I32";
  } else if (type->is_struct() || type->is_xception()) {
    return "TType.STRUCT";
  } else if (type->is_map()) {
    return "TType.MAP";
  } else if (type->is_set()) {
    return "TType.SET";
  } else if (type->is_list()) {
    return "TType.LIST";
  }

  throw "INVALID TYPE IN type_to_enum: " + type->get_name();
}

/**
 * Haxe class names must start with uppercase letter, but Haxe namespaces must not.
 */
std::string t_haxe_generator::get_cap_name(std::string name) {
  if (name.length() == 0) {
    return name;
  }

  // test.for.Generic< data.Type, or.the.Like> and handle it recursively
  size_t generic_first = name.find('<');
  size_t generic_last = name.rfind('>');
  if ((generic_first != std::string::npos) && (generic_last != std::string::npos)) {
    string outer_type = name.substr(0, generic_first);
    string inner_types = name.substr(generic_first + 1, generic_last - generic_first - 1);

    string new_inner = "";
    size_t comma_start = 0;
    while (comma_start < inner_types.length()) {
      size_t comma_pos = comma_start;
      int nested = 0;

      while (comma_pos < inner_types.length()) {
        bool found = false;
        switch (inner_types[comma_pos]) {
        case '<':
          ++nested;
          break;
        case '>':
          --nested;
          break;
        case ',':
          found = (nested == 0);
          break;
        }
        if (found) {
          break;
        }
        ++comma_pos;
      }

      if (new_inner.length() > 0) {
        new_inner += ",";
      }

      string inner = inner_types.substr(comma_start, comma_pos - comma_start);
      new_inner += get_cap_name(inner);
      comma_start = ++comma_pos;
    }

    return get_cap_name(outer_type) + "<" + new_inner + ">";
  }

  // package name
  size_t index = name.find_first_not_of(" \n\r\t");
  if (index < name.length()) {
    name[index] = tolower(name[index]);
    index = name.find('.');
    while (index != std::string::npos) {
      if (++index < name.length()) {
        name[index] = tolower(name[index]);
      }
      index = name.find('.', index);
    }
  }

  // class name
  index = name.rfind('.');
  if (index != std::string::npos) {
    ++index;
  } else {
    index = name.find_first_not_of(" \n\r\t");
  }

  if (index < name.length()) {
    name[index] = toupper(name[index]);
  }

  return name;
}

string t_haxe_generator::constant_name(string name) {
  string constant_name;

  bool is_first = true;
  bool was_previous_char_upper = false;
  for (char character : name) {
    bool is_upper = isupper(character);

    if (is_upper && !is_first && !was_previous_char_upper) {
      constant_name += '_';
    }
    constant_name += toupper(character);

    is_first = false;
    was_previous_char_upper = is_upper;
  }

  return constant_name;
}

/**
 * Enables RTTI for a class or interface
 */
void t_haxe_generator::generate_rtti_decoration(ostream& out) {
  if (rtti_) {
    out << "@:rtti" << '\n';
  }
}

/**
 * Adds build macros to a class or interface
 */
void t_haxe_generator::generate_macro_decoration(ostream& out) {
  if (!buildmacro_.empty()) {
    out << "#if ! macro" << '\n';
    out << "@:build( " << buildmacro_ << ")" << '\n';     // current class/interface
    out << "@:autoBuild( " << buildmacro_ << ")" << '\n'; // inherited classes/interfaces
    out << "#end" << '\n';
  }
}

/**
 * Emits a haxeDoc comment if the provided object has a doc in Thrift
 */
void t_haxe_generator::generate_haxe_doc(ostream& out, t_doc* tdoc) {
  if (tdoc->has_doc()) {
    generate_docstring_comment(out, "/**\n", " * ", tdoc->get_doc(), " */\n");
  }
}

/**
 * Emits a haxeDoc comment if the provided function object has a doc in Thrift
 */
void t_haxe_generator::generate_haxe_doc(ostream& out, t_function* tfunction) {
  if (tfunction->has_doc()) {
    stringstream ss;
    ss << tfunction->get_doc();
    const vector<t_field*>& fields = tfunction->get_arglist()->get_members();
    vector<t_field*>::const_iterator p_iter;
    for (p_iter = fields.begin(); p_iter != fields.end(); ++p_iter) {
      t_field* p = *p_iter;
      ss << "\n@param " << p->get_name();
      if (p->has_doc()) {
        ss << " " << p->get_doc();
      }
    }
    generate_docstring_comment(out, "/**\n", " * ", ss.str(), " */\n");
  }
}

std::string t_haxe_generator::generate_isset_check(t_field* field) {
  return generate_isset_check(field->get_name());
}

std::string t_haxe_generator::generate_isset_check(std::string field_name) {
  return "is" + get_cap_name("set") + get_cap_name(field_name) + "()";
}

void t_haxe_generator::generate_isset_set(ostream& out, t_field* field) {
  if (!type_can_be_null(field->get_type())) {
    indent(out) << "this.__isset_" << field->get_name() << " = true;" << '\n';
  }
}

std::string t_haxe_generator::get_enum_class_name(t_type* type) {
  string package = "";
  t_program* program = type->get_program();
  if (program != nullptr /*&& program != program_*/) {
    package = make_package_name( program->get_namespace("haxe")) + ".";
  }
  return package + type->get_name();
}

std::string t_haxe_generator::display_name() const {
  return "Haxe";
}


THRIFT_REGISTER_GENERATOR(
    haxe,
    "Haxe",
    "    rtti             Enable @:rtti for generated classes and interfaces\n"
    "    buildmacro=my.macros.Class.method(args)\n"
    "                     Add @:build macro calls to generated classes and interfaces\n")
