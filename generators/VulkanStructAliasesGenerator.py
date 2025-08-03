import sys
import re

# Parse file

IGNORE_ENUMS = {
    "VK_STRUCTURE_TYPE_LOADER_INSTANCE_CREATE_INFO",
    "VK_STRUCTURE_TYPE_LOADER_DEVICE_CREATE_INFO",
}

PLATFORMS = {
    "XLIB", "XCB", "WAYLAND", "ANDROID", "WIN32"
}

EXT_SUFFIXES = {
    "KHR", "EXT", "NV", "NVX", "AMD", "QCOM", "GOOGLE", "INTEL",
    "HUAWEI", "SAMSUNG", "VALVE", "FUCHSIA", "APPLE", "ARM", "IMG",
    "MESA", "MESAX",
    "ID", "LOD", "ASTC", "HDR", "GGP" # not suffixes but stays capitalized
}

with open(sys.argv[1], "r", encoding="utf-8") as file:
    text = file.read()

matches = re.findall(r'\bVK_STRUCTURE_TYPE_[A-Z0-9_]+', text)

parsed_matches: list[tuple[str, str, str]] = []

for match in matches:
    if not match in IGNORE_ENUMS:
        tail = match.split("VK_STRUCTURE_TYPE_", 1)[1]
        parts = [p for p in tail.split("_") if p]
        
        required_define = ""
        camel_parts = []
        for p in parts:
            if p in EXT_SUFFIXES or (len(p) == 2 and p[0].isdigit() and p[1] == 'D'):
                camel_parts.append(p)
            else:
                for i, ch in enumerate(p):
                    if ch.isalpha():
                        first_letter = i + 1
                        break
                camel_parts.append(p[:first_letter].upper() + p[first_letter:].lower())

            if p in PLATFORMS:
                required_define = "VK_USE_PLATFORM_" + p + "_KHR"

        type_name = "".join(camel_parts)
        parsed_matches.append((match, type_name, required_define))

# Create header file

prefix = """#pragma once
#include "VulkanStructWrapper.h"

namespace vkw
{

"""

suffix = """
}
"""

aliases = ""

for parsed_match in parsed_matches:
    if parsed_match[2] != "":
        aliases += "#ifdef " + parsed_match[2] + "\n"

    aliases += "using " + parsed_match[1] + " = VulkanStructWrapper<Vk" + parsed_match[1] + ", " + parsed_match[0] + ">;\n"
    
    if parsed_match[2] != "":
        aliases += "#endif\n"

header_file = prefix + aliases + suffix;
with open("VulkanStructAliases.h", "w", encoding="utf-8") as file:
    file.write(header_file)

print("Results outputted to VulkanStructAliases.h")
