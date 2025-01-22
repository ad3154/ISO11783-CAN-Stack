# copyright 2024 The Open-Agriculture Developers
import os, requests, re
from datetime import datetime
from pathlib import Path

ROOT_DIR = Path(__file__).parent.parent
ISOBUS_SRC_DIR = ROOT_DIR / "isobus/src"
ISOBUS_INCLUDE_DIR = ROOT_DIR / "isobus/include/isobus/isobus"

HEADER_FILE = ISOBUS_INCLUDE_DIR / "isobus_data_dictionary.hpp"
SOURCE_FILE = ISOBUS_SRC_DIR / "isobus_data_dictionary.cpp"

DDI_ARRAY_NAME = "DDI_ENTRIES"

# Regex to match the DDI array, it assumes the array consists of at most depth 2 of nested braces
DDI_ARRAY_REGEX = r"(?<=:" + DDI_ARRAY_NAME + r"\[\] = )(\{(?:[^{}]|\{(?:[^{}]|)*\})*\})"
# Regex to match the number of DDIs in the array, it assumes there are no lookups in the
# array where the index is a number, though lookups using variables as index is fine
DDI_ARRAY_LENGTH_REGEX = r"(?<=" + DDI_ARRAY_NAME + r"\[)(\d+)"
# Regex to match the file generation date
GENERATION_DATE_REGEX = r"(?<=This file was generated )(.*)(?=\.)"

if not HEADER_FILE.exists():
    exit("Could not find header file")

if not SOURCE_FILE.exists():
    exit("Could not find source file")

if os.path.exists("export.txt") and os.path.isfile("export.txt"):
    os.remove("export.txt")
    print("Removed stale export")

print("Downloading DDI database export")
exportURL = "https://www.isobus.net/isobus/exports/completeTXT"
r = requests.get(exportURL, allow_redirects=True)
print(r.headers.get('content-type'))
open("export.txt", 'wb').write(r.content)

numberOfDDIs = 0
with open("export.txt",'r', encoding="utf8") as f:
    processUnit = False
    for l_no, line in enumerate(f):
        if "DD Entity" in line and "Comment:" not in line:
            numberOfDDIs = numberOfDDIs + 1

with HEADER_FILE.open('r+', encoding="utf8") as f:
    contents = f.read()
    f.seek(0) # Move the cursor to the start of the file

    # Replace the generation date
    contents = re.sub(GENERATION_DATE_REGEX, datetime.today().strftime("%B %d, %Y"), contents)
    # Replace the number of DDIs
    contents = re.sub(DDI_ARRAY_LENGTH_REGEX, str(numberOfDDIs), contents)

    f.write(contents)
    f.truncate() # Remove the rest of the file (if any)


resultingArray = "{\n"
with open("export.txt",'r', encoding="utf8") as f:
    processUnit = False
    for l_no, line in enumerate(f):
        if "DD Entity" in line and "Comment:" not in line:
            processUnit = True
            entityLine = line.strip().split(' ', 3)
            strippedEntityLine = []

            for sub in entityLine:
                strippedEntityLine.append(sub.replace("\n", ""))
            
            print("Processing entity", line.replace("\n", ""))
            resultingArray += f"		{{ {strippedEntityLine[2]}, \"{strippedEntityLine[3]}\", "

        # Unit: mm³/m² - Capacity per area unit
        if "Unit: " in line and processUnit:
            entityLine = line.split(' ', 1)

            try:
                symbol, name = entityLine[1].split(' - ')
                if "n.a" in symbol or "not defined" in symbol:
                    symbol = ""
                    name = "n.a."
                    
                resultingArray += f"\"{symbol.strip()}\", \"{name.strip()}\", "
                processUnit = False
            except ValueError:
                print("Error parsing unit", line)
        
        if "Resolution: " in line:
            entityLine = line.split(' ', 1)
            entityLine[1] = entityLine[1].replace(",", ".")
            if '1' == entityLine[1].strip():
                entityLine[1] = '1.0'
            if '0' == entityLine[1].strip():
                entityLine[1] = '0.0'
            resultingArray += f"{entityLine[1].strip()}f, "
            
        # Display Range: 0,00 - 21474836,47
        if "Display Range: " in line:
            entityLine = line.split(' ', 2)
            rangeValues = entityLine[2].split(' - ')
            for i in range(2):
                rangeValues[i] = rangeValues[i].replace("\n", "").replace(",", ".")
                if '' == rangeValues[i].strip():
                    rangeValues[i] = '0.0'
                if rangeValues[i].count('0x') > 0:
                    rangeValues[i] = str(int(rangeValues[i], 16))
                if rangeValues[i].count('.') == 0:
                    rangeValues[i] += '.0'
            resultingArray += f"std::make_pair({rangeValues[0].strip()}f, {rangeValues[1].strip()}f) }},\n"

resultingArray += "	}"

with SOURCE_FILE.open('r+', encoding="utf8") as f:
    contents = f.read()
    f.seek(0) # Move the cursor to the start of the file

    # Replace the generation date
    contents = re.sub(GENERATION_DATE_REGEX, datetime.today().strftime("%B %d, %Y"), contents)
    # Replace the number of DDIs
    contents = re.sub(DDI_ARRAY_LENGTH_REGEX, str(numberOfDDIs), contents)
    # Replace the DDI array
    contents = re.sub(DDI_ARRAY_REGEX, resultingArray, contents)

    f.write(contents)
    f.truncate() # Remove the rest of the file (if any)
