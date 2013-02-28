#!/usr/bin/python

###########################################################################
#
# Copyright 2013 BMW Car IT GmbH
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
###########################################################################

import sys, re, string
from common_modules.common import *
from common_modules.config import G_MAX_INDENTATION_THRESHOLD

def check_indentation(filename, file_contents, clean_file_contents, file_lines, clean_file_lines):
    #It is important to keep track of the changes made to the indent level because
    #curly braces of namespaces do not affect indentation, while curly braces of
    #other statements like if, for and while increase/decease indentation
    indent_levels_stack = [0]
    indent_level = 0
    parentheses_count = 0
    
    def is_incomplete_statement(line, former_line):
        incomplete_statement = False
        #include statements not preceded by a semicolon in previous line
        incomplete_statement |= former_line != None and re.search(re.compile(r";(\s*)$"), former_line) == None
        #exclude lines containing block beginnings and ends
        incomplete_statement &= re.search(re.compile(r"(\{|\})(\s*)$"), line) == None
        #exclude statements preceded by block beginnings and ends
        incomplete_statement &= former_line != None and re.search(re.compile(r"(\{|\})(\s*)$"), former_line) == None
        #exclude preprocessor directives (they dunt have to end with a ; for example)
        incomplete_statement &= re.match(re.compile(r"(\s*)#"), line) == None
        #exclude statements preceded by preprocessor directives that do not end with a slash \
        incomplete_statement &= former_line != None and re.match(re.compile(r"(\s*)#((?!\/).)*\/$"), former_line) == None


        #filter out the case where a line is preceeded by a label statement
        fl_label_statement = False
        if former_line != None:
            #case statements -> case id : expr that does not begin with a : (to avoid matching with :: operator)
            fl_label_statement = re.match(r'(\s*)case(\s*)((?!:).)*(\s*):(?!:)(\s*)$', former_line) != None
            #label statements -> label : expr that does not begin with a : (to avoid matching with :: operator)
            fl_label_statement |= re.match(r'(\s*)(\w+)(\s*):(?!:)(\s*)$', former_line) != None

        incomplete_statement &= not fl_label_statement

        #include statements that are contained inside parentheses
        incomplete_statement |= parentheses_count > 0
        
        return incomplete_statement
    
    def is_label_statement(line):
        label_statement = False
        if line != None:
            #case statements -> case id : expr that does not begin with a : (to avoid matching with :: operator)
            label_statement = re.match(r'(\s*)case(\s*)((?!:).)*(\s*):(?!:)', line) != None
            #label statements -> label : expr that does not begin with a : (to avoid matching with :: operator)
            label_statement |= re.match(r'(\s*)(\w+)(\s*):(?!:)', line) != None
        return label_statement
    
    def is_preprocessor_directive(line):
        return re.match(r"(\s*)#", line) != None

    def is_macro_body(former_line):
        return former_line != None and re.search(r"\\$", former_line) != None
    
    for i in range(len(clean_file_lines)):
        line = clean_file_lines[i]
        line_indent = 0
        
        #a loop is to account for cases where there are several braces on same line
        for _ in range(0, line.count("}") - line.count("{")):
            indent_level = indent_levels_stack.pop()
        
        #check correct line indentation
        found_match = re.search("(?! )", line)
        if found_match:
            line_indent = found_match.start() / 4.0
        
        def get_former_line():
            if i > 0:
                for j in range(i - 1, 0 , -1):
                    if len(clean_file_lines[j].strip(" \t\n\f\r")):
                        return clean_file_lines[j]
            return ""
        
        former_line = get_former_line()
        
        incomplete_statement = is_incomplete_statement(line, former_line)
        label_statement = is_label_statement(line)
        fl_label_statement = is_label_statement(former_line)
        preprocessor_directive = is_preprocessor_directive(line)
        macro_body = is_macro_body(former_line)
        
        if len(line.strip(" \t\n\r")) > 0:
            if macro_body:
                #do not check anything, just ignore line
                None
            elif preprocessor_directive:
                #warning if not at 0 level indentation
                if line_indent != 0:
                    log_warning(filename, i + 1, "incorrect indentation", file_lines[i].strip(" "))
            elif label_statement:
                #warning if label statement has one level less indentation than the normal indentation level
                if line_indent != indent_level - 1:
                    log_warning(filename, i + 1, "incorrect indentation", file_lines[i].strip(" "))
            elif (not incomplete_statement) and line_indent != indent_level:
                #warning if a complete statement (NON incomplete statement) is not at the normal level of indentation 
                log_warning(filename, i + 1, "incorrect indentation", file_lines[i].strip(" "))
            elif incomplete_statement:
                if fl_label_statement and line_indent != indent_level:
                    #warning if preceeded by a label statement but not exactly at normal indentation
                    log_warning(filename, i + 1, "incorrect indentation", file_lines[i].strip(" "))
                elif (not fl_label_statement ) and line_indent < indent_level:
                    #warning if incomplete statement and indentation is less than normal indentation
                    log_warning(filename, i + 1, "incorrect indentation", file_lines[i].strip(" "))
        
        #a loop is to account for cases where there are several braces on same line
        for _ in range(0, line.count("{") - line.count("}")):
            indent_levels_stack.append(indent_level)
            #if there is no namesapce or extern declaration before curly brace: increase indentation
            #if re.match(r"(\s*)((namespace(\s+\w+)?(\s*)\{)|(extern(((?!\{).)*)\{))", line) == None and re.match(r"(\s*)(namespace|extern)", former_line) == None:
            #if re.match(r"(\s*)(namespace)", line) == None and re.match(r"(\s*)namespace", former_line) == None:
            namespace_or_extern_re_text = r'\s*(namespace|(extern((\s*\S*\s*)?)(\s*)($|\{)))'
            namespace_or_extern_re = re.compile(namespace_or_extern_re_text)

            if re.match(namespace_or_extern_re, line) == None and re.match(namespace_or_extern_re, former_line) == None:
                indent_level += 1
                
                #warning if indentation level is too much
                if indent_level == G_MAX_INDENTATION_THRESHOLD + 1:
                    log_warning(filename, i + 1, "code block has too much indentation [maximum level of indentation is {0}]".format(G_MAX_INDENTATION_THRESHOLD),)
        
        parentheses_count += line.count("(") - line.count(")")


if __name__ == "__main__":
    targets = sys.argv[1:]
    targets = get_all_files(targets)
    
    if len(targets) == 0:
        print """
\t**** No input provided ***
\tTakes a list of files/directories as input and performs specific style checking on all files/directories

\tGives warnings if lines (except multi-line comments and macros) do not have correct indentation.
\tIt also checks if code blocks have too much indentation (too deep). Code should have a maximum of {0} indentation levels.
""".format(G_MAX_INDENTATION_THRESHOLD)
        exit(0)
    
    for t in targets:
        if t[-2:] == ".h" or t[-4:] == ".cpp" or t[-2] == ".c":
            file_contents, clean_file_contents, file_lines, clean_file_lines = read_file(t)
            check_indentation(t, file_contents, clean_file_contents, file_lines, clean_file_lines)
