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

lines = text.splitlines()

matches = re.findall(r'\bVK_STRUCTURE_TYPE_[A-Z0-9_]+ =', text)

parsed_matches: list[tuple[str, str, list[str]]] = []

for match in matches:
    match = match.removesuffix(" =")

    if not match in IGNORE_ENUMS:
        tail = match.split("VK_STRUCTURE_TYPE_", 1)[1]
        parts = [p for p in tail.split("_") if p]
        
        required_defines: list[str] = []
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
                required_defines.append("VK_USE_PLATFORM_" + p + "_KHR")

        # get previous line to check which version is required
        for i in range(len(lines)):
            if match + " =" in lines[i] and i > 0 and "VK_VERSION_1_" in lines[i - 1]:
                required_defines.append("VK_API_" + re.findall("VERSION_1_[0-9]", lines[i - 1])[0])
                

        type_name = "".join(camel_parts)
        parsed_matches.append((match, type_name, required_defines))

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

def has_same_elements(list1, list2):
    for x in list1:
        if x not in list2:
            return False
    return len(list1) == len(list2)

for i in range(len(parsed_matches)):
    if len(parsed_matches[i][2]) > 0 and not (i > 0 and has_same_elements(parsed_matches[i][2], parsed_matches[i - 1][2])):
        aliases += "#if "

        for j in range(len(parsed_matches[i][2])):
            if j != 0:
                aliases += " && "
            aliases += "defined(" + parsed_matches[i][2][j] + ")"

        aliases += "\n"

    aliases += "using " + parsed_matches[i][1] + " = VulkanStructWrapper<Vk" + parsed_matches[i][1] + ", " + parsed_matches[i][0] + ">;\n"
    
    if len(parsed_matches[i][2]) > 0 and not (i + 1 < len(parsed_matches) and has_same_elements(parsed_matches[i][2], parsed_matches[i + 1][2])):
        aliases += "#endif\n"

header_file = prefix + aliases + suffix;
with open("VulkanStructAliases.h", "w", encoding="utf-8") as file:
    file.write(header_file)

print("Results outputted to VulkanStructAliases.h")
