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
#  Note: Because that the PlatformIO hook is executed before building the binary
#        has finished, only the 2nd compile fill copy the binary version before!
#  ==============================================================================

import os
import sys
import subprocess
import shutil

def get_subdirectories(directory_path):
    try:
        contents = os.listdir(directory_path)
        subdirs = [item for item in contents if os.path.isdir(os.path.join(directory_path, item))]
        return subdirs
    except Exception as e:
        print(f"Error: {e}")
        return []
    
def convert_firmware(build_dir, out_dir, firmware_type):
    try:
        key = "rase"
        i = out_dir.lower().index(key);
        out_dir = os.path.join(out_dir[0:i+len(key)], "tool/debian/firmware_images");
        os.makedirs(out_dir, mode=0o777, exist_ok=False);
    except:
        pass
    
    try:
        firmware_bin = os.path.join(build_dir, "firmware.bin");
        firmware_img = f"image_{firmware_type}"
        firmware_out = os.path.join(out_dir, f"{firmware_img}.bin");

        print("---------------------------------------------------------")
        print(f"build_dir:     {build_dir}");
        print(f"out_dir:       {out_dir}");
        print(f"firmware_type: {firmware_type}");
        print(f"firmware_out:  {firmware_out}");

        if os.path.exists(firmware_bin):
            shutil.copy2(firmware_bin, firmware_out)
            print("Firmware binary copied successfully!")
        else:
            print("Error: Firmware image not found!")
        
        # if os.path.exists(firmware_bin):
        #     subprocess.run(['xxd', '-i', f'-n {firmware_img}', firmware_bin, firmware_out], check=True)
        #     print("Firmware converted successfully!")
        # else:
        #     print("Error: Firmware image not found!")

        print("---------------------------------------------------------")

    except subprocess.CalledProcessError as e:
        print(f"Error running xxd: {e}")
        sys.exit(1)
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

output_dir = os.path.join(sys.path[0], ".pio/build")
sub_dirs = get_subdirectories(output_dir);
for item in sub_dirs:
    convert_firmware(os.path.join(output_dir, item), sys.path[0], item);
