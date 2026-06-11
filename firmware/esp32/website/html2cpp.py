# ==============================================================================
#
#  PROJECT:     "Rase" Radio Sensor Project, HTML into C-strings conversion tool
#  COPYRIGHT:   (C)2025-2026 KKS-Elektronik,  M. Kreck, <makreck@googlemail.com>
#
#  This program is free software: you can redistribute it and/or modify it under
#  the terms of the GNU General Public License as published by the Free Software
#  Foundation, either version 3 of the License, or (at your option) any later
#  version.
#
#  This program is distributed in the hope that it will be useful,   but WITHOUT
#  ANY WARRANTY, without even the implied warranty of MERCHANTABILITY or FITNESS
#  FOR A PARTICULAR PURPOSE, see the GNU General Public License for details.
#
#  You should have received a copy of the  GNU General Public License along with
#  this program. If not, see <https://www.gnu.org/licenses/>.
#  
#  ==============================================================================

import os, sys
from pathlib import Path

class Html2Cpp:
    def __init__(self):
        pass
    
    def html_files_in_current_directory(self):
        # current_dir = Path('.')
        current_dir = Path(sys.path[0])
        print(f"Current directory is: \"{current_dir}\"")
        html_files = [f for f in current_dir.iterdir() if f.is_file() and f.suffix.lower() in ['.html', '.htm']]
        for index, file_path in enumerate(html_files, 1):
            yield (index, file_path)

    def run(self):
        print("HTML files with enumeration:")
        for index, path in self.html_files_in_current_directory():
            in_path = path.resolve()
            out_path = str(in_path).replace(".html", ".cpp")
            print(f"{index}: from \"{in_path}\" to \"{out_path}\"")
            with open(out_path, "w", encoding='utf-8') as out_file:
                out_file.write(
"""
#include "includes.hpp"
#include "app.hpp"

const char* WebServer::webserver_resp_str = 
""")
                with open(in_path, 'r', encoding='utf-8') as in_file:
                    for line in in_file:
                        if not line.strip().startswith("//"):
                            output = "\t\"" + line.rstrip().replace('"', '\\"') + "\\n\"" + "\n"
                            out_file.write(output)
                out_file.write("\t\"\";\n")
        return self

if __name__ == "__main__":
    Html2Cpp().run()
