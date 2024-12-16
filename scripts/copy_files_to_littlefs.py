###########################################################################################################
# This script will automatically be called by PlatformIO during the build process (as pre-action script)
# It is responsible for copying the correct images/files to the LittleFS directory based
# on the buildflags and config.h defines.
#
# You do NOT need to run it manually
###########################################################################################################

import re, shutil, os, os.path
from SCons.Script import Builder, Import

# Extract macros from the config header file
config_header_path = "firmware/config/config.h"
buildDir = "build"
outDir = os.path.join(buildDir, "littlefs")

# Map macros to directories and their respective files
# The final filename is constructed as "dir + file" so the dir should end with a "/" unless you know what you are doing
embed_map = {
    "USE_CLOCK_CUSTOM > 0": [ # Condition
        ["images/clock/custom0/", # Source directory
            ["0.jpg", "1.jpg", "2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg", "7.jpg", "8.jpg", "9.jpg", "10.jpg", "11.jpg"], # Files
            1], # Skip first level while copying ("images/")
    ],
    "USE_CLOCK_CUSTOM > 1": [
        ["images/clock/custom1/", 
            ["0.jpg", "1.jpg", "2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg", "7.jpg", "8.jpg", "9.jpg", "10.jpg", "11.jpg"],
            1],
    ],
    "USE_CLOCK_CUSTOM > 2": [
        ["images/clock/custom2/", 
            ["0.jpg", "1.jpg", "2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg", "7.jpg", "8.jpg", "9.jpg", "10.jpg", "11.jpg"],
            1],
    ],
    "USE_CLOCK_CUSTOM > 3": [
        ["images/clock/custom3/", 
            ["0.jpg", "1.jpg", "2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg", "7.jpg", "8.jpg", "9.jpg", "10.jpg", "11.jpg"],
            1],
    ],
    "USE_CLOCK_CUSTOM > 4": [
        ["images/clock/custom4/", 
            ["0.jpg", "1.jpg", "2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg", "7.jpg", "8.jpg", "9.jpg", "10.jpg", "11.jpg"],
            1],
    ],
    "USE_CLOCK_CUSTOM > 5": [
        ["images/clock/custom5/", 
            ["0.jpg", "1.jpg", "2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg", "7.jpg", "8.jpg", "9.jpg", "10.jpg", "11.jpg"],
            1],
    ],
    "USE_CLOCK_CUSTOM > 6": [
        ["images/clock/custom6/", 
            ["0.jpg", "1.jpg", "2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg", "7.jpg", "8.jpg", "9.jpg", "10.jpg", "11.jpg"],
            1],
    ],
    "USE_CLOCK_CUSTOM > 7": [
        ["images/clock/custom7/", 
            ["0.jpg", "1.jpg", "2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg", "7.jpg", "8.jpg", "9.jpg", "10.jpg", "11.jpg"],
            1],
    ],
    "USE_CLOCK_CUSTOM > 8": [
        ["images/clock/custom8/", 
            ["0.jpg", "1.jpg", "2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg", "7.jpg", "8.jpg", "9.jpg", "10.jpg", "11.jpg"],
            1],
    ],
    "USE_CLOCK_CUSTOM > 9": [
        ["images/clock/custom9/", 
            ["0.jpg", "1.jpg", "2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg", "7.jpg", "8.jpg", "9.jpg", "10.jpg", "11.jpg"],
            1],
    ],
}

Import("env")

board = env.BoardConfig()

# Function to extract macros and their values from a header file
def extract_macros_with_values(file_path):
    macros = {}
    # Regex to match #define statements
    macro_pattern = re.compile(r"^\s*#define\s+(\w+)(?:\s+(.+))?")
    try:
        with open(file_path, "r") as file:
            for line in file:
                # Remove inline comments
                line = line.split("//", 1)[0].strip()
                if not line:  # Skip empty lines after stripping
                    continue
                match = macro_pattern.match(line)
                if match:
                    macro_name = match.group(1)
                    macro_value = match.group(2)
                    if macro_value is not None:
                        # Attempt to parse numeric values
                        try:
                            macro_value = int(macro_value)
                        except ValueError:
                            try:
                                macro_value = float(macro_value)
                            except ValueError:
                                macro_value = macro_value.strip()
                    macros[macro_name] = macro_value
    except FileNotFoundError:
        print(f"Warning: Header file '{file_path}' not found.")
    return macros

def copy_files_to_builddir(files):
    for srcFile, dstFile in files:
        base = os.path.dirname(dstFile)
        finalPath = os.path.join(outDir, base)
        os.makedirs(finalPath, exist_ok=True)
        shutil.copy(srcFile, finalPath)


# Extract -D flags from BUILD_FLAGS
build_flags = env.get("BUILD_FLAGS", [])
cpp_defines = {flag[2:].split("=")[0].strip(): flag[2:].split("=")[1].strip() if "=" in flag else None for flag in build_flags if flag.startswith("-D")}

# Extract macros from the config header file
header_macros = extract_macros_with_values(config_header_path)

# Combine all macros
all_macros = {**header_macros, **cpp_defines}
print("Extracted macros with values:", all_macros)

# Function to evaluate conditions in embed_map
def evaluate_condition(condition, macros):
    try:
        # Replace undefined identifiers with quoted strings
        condition = re.sub(
            r"\b([A-Za-z_][A-Za-z0-9_]*)\b",
            lambda match: f"'{match.group(1)}'" if match.group(1) not in macros else match.group(1),
            condition,
        )
        return eval(condition, {}, macros)
    except Exception as e:
        print(f"Error evaluating condition '{condition}': {e}")
        return False

# Determine which files to embed based on conditions in embed_map
to_copy = []
for condition, directories in embed_map.items():
    if evaluate_condition(condition, all_macros):
        for directory, files, skip_level in directories:
            # Skip some levels if necessary
            parts = directory.split("/")
            final_directory = "/".join(parts[skip_level:])
            # Add [src, dst] to out list
            to_copy.extend([[directory + file, final_directory + file] for file in files])

# Update the build environment with embedded files
if len(to_copy) > 0:
    print(f"Copying files to LittleFS: {to_copy}")
    copy_files_to_builddir(to_copy)
else:
    print("No files selected for embedding.")

