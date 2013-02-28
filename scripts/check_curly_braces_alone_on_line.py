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

def remove_tollerable_braces(clean_file_contents):
    #remove array assignments
    assignment_re = re.compile(r'=(\s*)(\{)((?!;).)*(\})(\s*);', re.DOTALL)
    
    no_assig_contents = clean_file_contents
    found_match = re.search(assignment_re, no_assig_contents)
    
    while found_match != None:
        matched_string = no_assig_contents[found_match.start(): found_match.end()]
        no_assig_contents = no_assig_contents[:found_match.start()] + "\n" * matched_string.count("\n") + no_assig_contents[found_match.end():]

        found_match = re.search(assignment_re, no_assig_contents)

    #remove braces that are used for implementing empty functions
    empty_func_re = re.compile(r'\{\s*\}')

    no_empty_func_contents = no_assig_contents
    found_match = re.search(empty_func_re, no_empty_func_contents)

    while found_match != None:
        matched_string = no_empty_func_contents[found_match.start(): found_match.end()]
        no_empty_func_contents = no_empty_func_contents[:found_match.start()] + "\n" * matched_string.count("\n") + no_empty_func_contents[found_match.end():]

        found_match = re.search(empty_func_re, no_empty_func_contents)

    return no_empty_func_contents

def check_curly_braces_alone_on_line(filename, file_contents, clean_file_contents, file_lines, clean_file_lines):
    clean_file_contents = remove_tollerable_braces(clean_file_contents)
    clean_file_lines = clean_file_contents.split('\n')
    
    typedecl_stack = []
    #a type declaration contains the words struct, class , union or enum
    typedecl_re = re.compile(r"(?!<\w)(typedef)?(\s*)(struct|class|enum|union)(?!\w)")
    #some statements are allowed to have curly brace on same
    permissible_re = re.compile(r'''(\s*)
                                (
                                    do| # do keyword
                                    (namespace((?!\{).)*)| # namespace keyword 
                                    (extern((?!\{).)*) # extern keyword
                                )?
                                (\s*)\{(\s*)
                                (\\?)(\s*)
                                $''', re.VERBOSE)

    
    for i in range(len(file_lines)):
        line = clean_file_lines[i]
        
        def get_former_line():
            if i > 0:
                for j in range(i - 1, 0 , -1):
                    if len(clean_file_lines[j].strip(" \t\n\f\r")):
                        return clean_file_lines[j]
            return None
        
        former_line = get_former_line()
        
        if line.count("{"):
            typedecl = re.search(typedecl_re, former_line)
            typedecl_stack.append(typedecl)
            
            if re.match(permissible_re, line) == None :
                log_warning(filename, i + 1, "other code on same line with curly brace", file_lines[i].strip(" "))
        
        if line.count("}"):
            typedecl = typedecl_stack.pop()
            if typedecl:
                if re.match(r"(\s*)\}", line) == None:
                    log_warning(filename, i + 1, "other code on same line with curly brace", file_lines[i].strip(" "))
            else:
                if re.match(r"(\s*)\}(\s*)((while((?!;).)*)?)(;?)(\s*)(\\?)(\s*)$", line) == None:
                    log_warning(filename, i + 1, "other code on same line with curly brace", file_lines[i].strip(" "))

if __name__ == "__main__":
    targets = sys.argv[1:]
    targets = get_all_files(targets)
    
    if len(targets) == 0:
        print """
\t**** No input provided ***
\tTakes a list of files/directories as input and performs specific style checking on all files/directories

\tGives warnings if unnecessary code exists on the same line with curly braces. 
"""
        exit(0)
    
    for t in targets:
        if t[-2:] == ".h" or t[-4:] == ".cpp" or t[-2] == ".c":
            file_contents, clean_file_contents, file_lines, clean_file_lines = read_file(t)
            check_curly_braces_alone_on_line(t, file_contents, clean_file_contents, file_lines, clean_file_lines)
