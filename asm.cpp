/*DECLARATION OF AUTHORSHIP
I, Suman kumar , declare that this work is the result of my independent research and efforts.

Affiliated with Indian Institute of Technology Patna.

Roll: 2301CS57
*/
#include <bits/stdc++.h>
using namespace std;

// Data Structures
// Represents a assembly instruction with its components and metadata
struct Instruction {
    string label;
    string mnemonic;
    string operand;
    int operandType;
    bool labelPresent;
    string machineCode;
    int address;
};

// Tracks label information including definition and usage status
struct LabelInfo {
    int definition;
    bool isDefined;
    bool isUsed;
};

// Enum for different types of operands in assembly instructions
enum OperandType {
    NONE = 0,    // No operand
    VALUE = 1,   // Immediate value
    OFFSET = 2,  // Memory offset
    LABEL = 3    // Symbol reference
};

// Global data structures for tracking assembly process
map<string, pair<string, OperandType>> opcodeMap;    // Maps mnemonics to opcodes and operand types
map<string, LabelInfo> labelTracker;                 // Tracks label definitions and usage


// Logs error messages to a file for debugging and user feedback
void logError(const string& error) {
    ofstream logFile("logFile.log", ios::app);
    if (logFile) {
        logFile << error << endl;
        logFile.close();
    }
}

// Initializes the opcode map with supported assembly instructions
void initOpcodes() {
    opcodeMap["data"] = {"", VALUE};
    opcodeMap["ldc"] = {"00", VALUE};      // Load constant
    opcodeMap["adc"] = {"01", VALUE};      // Add constant
    opcodeMap["ldl"] = {"02", OFFSET};     // Load local
    opcodeMap["stl"] = {"03", OFFSET};     // Store local
    opcodeMap["ldnl"] = {"04", OFFSET};    // Load non-local
    opcodeMap["stnl"] = {"05", OFFSET};    // Store non-local
    opcodeMap["add"] = {"06", NONE};       // Add
    opcodeMap["sub"] = {"07", NONE};       // Subtract
    opcodeMap["shl"] = {"08", NONE};       // Shift left
    opcodeMap["shr"] = {"09", NONE};       // Shift right
    opcodeMap["adj"] = {"0A", VALUE};      // Adjust stack
    opcodeMap["a2sp"] = {"0B", NONE};      // A to stack pointer
    opcodeMap["sp2a"] = {"0C", NONE};      // Stack pointer to A
    opcodeMap["call"] = {"0D", OFFSET};    // Call subroutine
    opcodeMap["return"] = {"0E", NONE};    // Return from subroutine
    opcodeMap["brz"] = {"0F", OFFSET};     // Branch if zero
    opcodeMap["brlz"] = {"10", OFFSET};    // Branch if less than zero
    opcodeMap["br"] = {"11", OFFSET};      // Branch unconditional
    opcodeMap["HALT"] = {"12", NONE};      // Halt execution
}

// Removes comments and whitespace from a line of assembly code
string clean(const string& line) {
    size_t commentPos = line.find_first_of(";");
    string cleanedLine = (commentPos != string::npos) ? line.substr(0, commentPos) : line;
    cleanedLine.erase(0, cleanedLine.find_first_not_of(" \t"));
    cleanedLine.erase(cleanedLine.find_last_not_of(" \t") + 1);
    return cleanedLine;
}

// Reads assembly source file and returns cleaned lines
vector<string> readAndCleanInput(const string& filename) {
    vector<string> cleanedLines;
    ifstream infile(filename);
    
    if (!infile) {
        logError("Failed to open input file: " + filename);
        return cleanedLines;
    }

    string line;
    while (getline(infile, line)) {
        string cleanedLine = clean(line);
        if (!cleanedLine.empty()) {
            cleanedLines.push_back(cleanedLine);
        }
    }

    return cleanedLines;
}

//  Process and validate labels, building the symbol table
void processLabels(const vector<string>& cleanedLines, map<string, int>& labels) {
    int actualLineNumber = 0;
    
    for (size_t i = 0; i < cleanedLines.size(); ++i) {
        size_t colonPos = cleanedLines[i].find(':');
        
        if (colonPos != string::npos && colonPos > 0) {
            string label = cleanedLines[i].substr(0, colonPos);
            label.erase(0, label.find_first_not_of(" \t"));
            label.erase(label.find_last_not_of(" \t") + 1);

            if (!label.empty()) {
                // Check for duplicate labels
                if (!labels.insert({label, actualLineNumber}).second) {
                    logError("Duplicate label found: " + label + " at line " + to_string(i + 1));
                }
                labelTracker[label] = {static_cast<int>(i), true, false};
            }
        }
        
        // Only increment line number for actual instructions (not empty labels)
        if (colonPos == string::npos || 
            cleanedLines[i].substr(colonPos + 1).find_first_not_of(" \t") != string::npos) {
            actualLineNumber++;
        }
    }
}

// Parses numeric values in different formats (hex, octal, decimal)
int parseValue(const string& operand) {
    if (operand.substr(0, 2) == "0x") {
        return stoi(operand.substr(2), nullptr, 16);    // Hex format
    }
    if (operand == "0") {                               
            return 0;
        }
    if (operand[0] == '0') {
        return stoi(operand.substr(1), nullptr, 8);     // Octal format
    }
    return stoi(operand);                              // Decimal format
}

// First pass: Process instructions and build the instruction table
void firstPass(const vector<string>& cleanedLines, vector<Instruction>& instructions) {
    bool hasHalt = false;
    int currentAddress = 0;
    
    for (const string& line : cleanedLines) {
        istringstream iss(line);
        Instruction instruction = {};
        instruction.address = currentAddress;

        string token;
        if (!(iss >> token)) continue;

        // Handle labels at the start of lines
        if (token.back() == ':') {
            instruction.label = token.substr(0, token.size() - 1);
            instruction.labelPresent = true;
            if (!(iss >> instruction.mnemonic)) {
                instructions.push_back(instruction);
                continue;
            }
        } else {
            instruction.mnemonic = token;
        }

        // Validate mnemonic
        auto opcodeIt = opcodeMap.find(instruction.mnemonic);
        if (opcodeIt == opcodeMap.end()) {
            logError("Unknown mnemonic '" + instruction.mnemonic + "'");
            continue;
        }

        instruction.operandType = opcodeIt->second.second;
        getline(iss >> ws, instruction.operand);

         if (instruction.mnemonic == "data") {
            try {
                parseValue(instruction.operand);
                currentAddress++;
            } catch (const exception&) {
                logError("Invalid data value");
            }
        } else {
            if (instruction.mnemonic == "HALT") hasHalt = true;
            currentAddress++;
        }

        instructions.push_back(instruction);
    }

    if (!hasHalt) logError("Warning: No HALT instruction found.");
}

// Second pass: Generate machine code for all instructions
// Resolves labels and handles different instruction formats
void secondPass(vector<Instruction>& instructions, vector<string>& machineCode, const map<string, int>& labels) {
    for (auto& instruction : instructions) {
        if (instruction.mnemonic.empty()) continue;


        // Handle HALT instruction
        if (instruction.mnemonic == "HALT") {
            instruction.machineCode = "00000012";
            machineCode.push_back(instruction.machineCode);
            continue;
        }

        auto opcodeIt = opcodeMap.find(instruction.mnemonic);
        if (opcodeIt == opcodeMap.end()) continue;

        string opcode = opcodeIt->second.first;
        int operandValue = 0;
        bool isValidInstruction = true;

        // Handle data directive
        if (instruction.mnemonic == "data") {
            try {
                int value = parseValue(instruction.operand);
                stringstream ss;
                ss << setfill('0') << setw(8) << hex << (value & 0xFFFFFF);
                instruction.machineCode = ss.str();
                machineCode.push_back(instruction.machineCode);
            } catch (const exception&) {
                logError("Error processing data value: " + instruction.operand);
            }
            continue;
        }

        // Process operands (labels or values)
        if (!instruction.operand.empty()) {
            try {
                auto labelIt = labels.find(instruction.operand);
                if (labelIt != labels.end()) {
                    labelTracker[instruction.operand].isUsed = true;
                    operandValue = labelIt->second;
                    
                    // Calculate relative offsets for branch and call instructions
                    if (instruction.mnemonic == "br" || 
                        instruction.mnemonic == "brz" || 
                        instruction.mnemonic == "brlz" || 
                        instruction.mnemonic == "call") {
                        operandValue -= (instruction.address + 1);
                    }
                } else {
                    operandValue = parseValue(instruction.operand);
                }

                // Handle negative values and range checking
                if (operandValue < 0) {
                    if (operandValue < -(1 << 23)) {
                        logError("Negative value out of range: " + instruction.operand);
                        isValidInstruction = false;
                    } else {
                        operandValue = (1 << 24) + operandValue;  // Convert to 2's complement
                    }
                } else if (operandValue >= (1 << 24)) {
                    logError("Value out of range: " + instruction.operand);
                    isValidInstruction = false;
                }
            } catch (const exception&) {
                logError("Error processing operand: " + instruction.operand);
                isValidInstruction = false;
            }
        }

        // Generate machine code if instruction is valid
        if (isValidInstruction) {
            stringstream ss;
            ss << setfill('0') << setw(6) << hex << (operandValue & 0xFFFFFF)
               << setfill('0') << setw(2) << opcode;
            instruction.machineCode = ss.str();
            machineCode.push_back(instruction.machineCode);
        }
    }
}

// Writes binary machine code to output file
void writeOutput(const vector<string>& machineCode) {
    ofstream outFile("machineCode.o", ios::binary);
    if (!outFile) {
        logError("Failed to open machine code output file");
        return;
    }

    for (const auto& code : machineCode) {
        try {
            unsigned int value;
            stringstream ss;
            ss << hex << code;
            ss >> value;
            outFile.write(reinterpret_cast<const char*>(&value), sizeof(unsigned int));
        } catch (const exception& e) {
            logError("Error writing machine code: " + string(e.what()));
        }
    }
}

// Generates listing file with addresses and machine code
void writeListing(const vector<Instruction>& instructions) {
    ofstream listingFile("listing.txt");
    if (!listingFile) {
        logError("Failed to open listing output file");
        return;
    }

    for (const auto& instruction : instructions) {
        stringstream addrStr;
        addrStr << setfill('0') << setw(6) << dec << instruction.address;

        string line;
        if (!instruction.machineCode.empty()) {
            line = addrStr.str() + " " + instruction.machineCode + " ";
        } else if (instruction.labelPresent) {
            line = addrStr.str() + "          " + instruction.label + ": ";
        }

        if (!instruction.mnemonic.empty()) {
            line += instruction.mnemonic;
            if (!instruction.operand.empty()) {
                line += " " + instruction.operand;
            }
        }

        if (!line.empty()) {
            listingFile << line << endl;
        }
    }
}

// Main function: Coordinates the assembly process
int main() {
    ofstream("logFile.log", ios::trunc);
    initOpcodes();
    
    // Read and preprocess input file
    string filename ;
    cout << "Enter file name: Ex: input.asm" << endl;
    cin >> filename;
    
    vector<string> cleanedLines = readAndCleanInput(filename);
    if (cleanedLines.empty()) {
        cout << "No input lines found or error reading file." << endl;
        return 1;
    }

    // Data structures for assembly process
    map<string, int> labels;
    vector<Instruction> instructions;
    vector<string> machineCode;

    // Two-pass assembly process
    processLabels(cleanedLines, labels);    // First pass: Process labels
    firstPass(cleanedLines, instructions);  // First pass: Process instructions
    secondPass(instructions, machineCode, labels);  // Second pass: Generate machine code

    // Write output files
    writeOutput(machineCode);
    writeListing(instructions);

    // Output assembly statistics and warnings
    cout << "\nAssembly complete!" << endl;
    cout << "Generated " << machineCode.size() << " bytes of machine code" << endl;
    
    // Check for errors in log file
    ifstream logFileCheck("logFile.log");
    if (logFileCheck.peek() != ifstream::traits_type::eof()) {
        cout << "\nWarnings/Errors were generated. Please check logFile" << endl;
    }
    
    // Check for unused labels
    bool hasUnusedLabels = false;
    for (map<string, LabelInfo>::const_iterator it = labelTracker.begin(); it != labelTracker.end(); ++it) {
        if (it->second.isDefined && !it->second.isUsed) {
            logError("Warning: Label '" + it->first + "' defined but never used");
            hasUnusedLabels = true;
        }
    }
    

    return 0;
}