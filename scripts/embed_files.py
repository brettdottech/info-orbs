###########################################################################################################
# This script will automatically be called by PlatformIO during the build process (as pre-action script)
# It is responsible for embedding the correct images/files into the firmware depending
# on the buildflags and config.h defines.
#
# You do NOT need to run it manually
###########################################################################################################

import re
from os.path import basename, join
from SCons.Script import Builder, Import

# Extract macros from the config header file
config_header_path = "firmware/config/config.h"

# Map macros to directories and their respective files
# The final filename is constructed as "dir + file" so the dir should end with a "/" unless you know what you are doing
embed_map = {
    "USE_CLOCK_NIXIE == NIXIE_NOHOLES": [
        ["images/ClockWidget/nixie.no-holes/", 
            ["0.jpg", "1.jpg", "2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg", "7.jpg", "8.jpg", "9.jpg", "colon_on.jpg", "colon_off.jpg"]],
    ],
    "USE_CLOCK_NIXIE == NIXIE_HOLES": [
        ["images/ClockWidget/nixie.holes/", 
            ["0.jpg", "1.jpg", "2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg", "7.jpg", "8.jpg", "9.jpg", "colon_on.jpg", "colon_off.jpg"]],
    ],
    "USE_CLOCK_CUSTOM > 0": [
        ["images/ClockWidget/custom/0_", 
            ["0.jpg", "1.jpg", "2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg", "7.jpg", "8.jpg", "9.jpg", "colon_on.jpg", "colon_off.jpg"]],
    ],
    "USE_CLOCK_CUSTOM > 1": [
        ["images/ClockWidget/custom/1_", 
            ["0.jpg", "1.jpg", "2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg", "7.jpg", "8.jpg", "9.jpg", "colon_on.jpg", "colon_off.jpg"]],
    ],
    "USE_CLOCK_CUSTOM > 2": [
        ["images/ClockWidget/custom/2_", 
            ["0.jpg", "1.jpg", "2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg", "7.jpg", "8.jpg", "9.jpg", "colon_on.jpg", "colon_off.jpg"]],
    ],
    "USE_CLOCK_CUSTOM > 3": [
        ["images/ClockWidget/custom/3_", 
            ["0.jpg", "1.jpg", "2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg", "7.jpg", "8.jpg", "9.jpg", "colon_on.jpg", "colon_off.jpg"]],
    ],
    "USE_CLOCK_CUSTOM > 4": [
        ["images/ClockWidget/custom/4_", 
            ["0.jpg", "1.jpg", "2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg", "7.jpg", "8.jpg", "9.jpg", "colon_on.jpg", "colon_off.jpg"]],
    ],
    "USE_CLOCK_CUSTOM > 5": [
        ["images/ClockWidget/custom/5_", 
            ["0.jpg", "1.jpg", "2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg", "7.jpg", "8.jpg", "9.jpg", "colon_on.jpg", "colon_off.jpg"]],
    ],
    "USE_CLOCK_CUSTOM > 6": [
        ["images/ClockWidget/custom/6_", 
            ["0.jpg", "1.jpg", "2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg", "7.jpg", "8.jpg", "9.jpg", "colon_on.jpg", "colon_off.jpg"]],
    ],
    "USE_CLOCK_CUSTOM > 7": [
        ["images/ClockWidget/custom/7_", 
            ["0.jpg", "1.jpg", "2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg", "7.jpg", "8.jpg", "9.jpg", "colon_on.jpg", "colon_off.jpg"]],
    ],
    "USE_CLOCK_CUSTOM > 8": [
        ["images/ClockWidget/custom/8_", 
            ["0.jpg", "1.jpg", "2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg", "7.jpg", "8.jpg", "9.jpg", "colon_on.jpg", "colon_off.jpg"]],
    ],
    "USE_CLOCK_CUSTOM > 9": [
        ["images/ClockWidget/custom/9_", 
            ["0.jpg", "1.jpg", "2.jpg", "3.jpg", "4.jpg", "5.jpg", "6.jpg", "7.jpg", "8.jpg", "9.jpg", "colon_on.jpg", "colon_off.jpg"]],
    ],
}

Import("env")

board = env.BoardConfig()

#################################################### begin copy ####################################################
# Copied from ~/.platformio/platforms/espressif32/builder/frameworks/_embed_files.py

def embed_files(files, files_type):
    for f in files:
        filename = basename(f) + ".txt.o"
        file_target = env.TxtToBin(join("$BUILD_DIR", filename), f)
        env.Depends("$PIOMAINPROG", file_target)
        if files_type == "embed_txtfiles":
            env.AddPreAction(file_target, prepare_file)
            env.AddPostAction(file_target, revert_original_file)
        env.AppendUnique(PIOBUILDFILES=[env.File(join("$BUILD_DIR", filename))])

mcu = board.get("build.mcu", "esp32")
env.Append(
    BUILDERS=dict(
        TxtToBin=Builder(
            action=env.VerboseAction(
                " ".join(
                    [
                        "riscv32-esp-elf-objcopy"
                        if mcu in ("esp32c3", "esp32c6")
                        else "xtensa-%s-elf-objcopy" % mcu,
                        "--input-target",
                        "binary",
                        "--output-target",
                        "elf32-littleriscv" if mcu in ("esp32c3","esp32c6") else "elf32-xtensa-le",
                        "--binary-architecture",
                        "riscv" if mcu in ("esp32c3","esp32c6") else "xtensa",
                        "--rename-section",
                        ".data=.rodata.embedded",
                        "$SOURCE",
                        "$TARGET",
                    ]
                ),
                "Converting $TARGET",
            ),
            suffix=".txt.o",
        )
    )
)
##################################################### end copy #####################################################

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
to_embed = []
for condition, directories in embed_map.items():
    if evaluate_condition(condition, all_macros):
        for directory, files in directories:
            to_embed.extend(["../" + directory + file for file in files])

# Update the build environment with embedded files
if embed_files:
    print(f"Embedding files: {to_embed}")
    embed_files(to_embed, "embed_files")
else:
    print("No files selected for embedding.")

