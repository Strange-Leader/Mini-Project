/*DECLARATION OF AUTHORSHIP
I, Suman kumar , declare that this work is the result of my independent research and efforts.

Affiliated with Indian Institute of Technology Patna.

Roll: 2301CS57
*/
#include <bits/stdc++.h>
using namespace std;

// CPU registers and memory
int32_t A = 0;        // Accumulator
int32_t B = 0;        // Secondary register
int32_t PC = 0;       // Program counter
int32_t SP = 0;       // Stack pointer
vector<uint32_t> memory;
bool halted = false;
int instructionCount = 0;

// Helper function to sign-extend 24-bit values
int32_t signExtend24(uint32_t value) {
    if (value & 0x800000) {  
        return value | 0xFF000000;  // Sign extend
    }
    return value & 0x00FFFFFF;  // Zero extend
}

void reset() {
    A = 0;
    B = 0;
    PC = 0;
    SP = 0;
    halted = false;
    instructionCount = 0;
    fill(memory.begin(), memory.end(), 0);
}

bool isHalted() { 
    return halted; 
}

int getInstructionCount() { 
    return instructionCount; 
}

// Load program into memory
bool loadProgram(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cout << "Error: Could not open file " << filename << endl;
        return false;
    }

    uint32_t word;
    size_t addr = 0;
    while (file.read(reinterpret_cast<char*>(&word), sizeof(word))) {
        if (addr >= memory.size()) {
            cout << "Error: Program too large for memory" << endl;
            return false;
        }
        memory[addr++] = word;
    }
    return true;
}

// Execute a single instruction
void step() {
    if (halted) return;
    
    // Fetch
    uint32_t instruction = memory[PC];
    uint32_t opcode = instruction & 0xFF;         // Last 8 bits
    uint32_t operand = (instruction >> 8) & 0xFFFFFF;  // First 24 bits
    int32_t signedOperand = signExtend24(operand);
    
    // Increment PC before execution
    PC++;
    instructionCount++;

    // Decode and Execute
    switch (opcode) {
        case 0x00:  // ldc
            B = A;
            A = signedOperand;
            break; 

        case 0x01:  // adc
            A += signedOperand;
            break;

        case 0x02:  // ldl
            B = A;
            A = memory[SP + signedOperand];
            break;

        case 0x03:  // stl
            memory[SP + signedOperand] = A;
            A = B;
            break;

        case 0x04:  // ldnl
            A = memory[A + signedOperand];
            break;

        case 0x05:  // stnl
            memory[A + signedOperand] = B;
            break;

        case 0x06:  // add
            A = B + A;
            break;

        case 0x07:  // sub
            A = B - A;
            break;

        case 0x08:  // shl
            A = B << A;
            break;

        case 0x09:  // shr
            A = B >> A;
            break;

        case 0x0A:  // adj
            SP = SP + signedOperand;
            break;

        case 0x0B:  // a2sp
            SP = A;
            A = B;
            break;

        case 0x0C:  // sp2a
            B = A;
            A = SP;
            break;

        case 0x0D:  // call
            B = A;
            A = PC;
            PC += signedOperand;
            break;

        case 0x0E:  // return
            PC = A;
            A = B;
            break;

        case 0x0F:  // brz
            if (A == 0) PC += signedOperand;
            break;

        case 0x10:  // brlz
            if (A < 0) PC += signedOperand;
            break;

        case 0x11:  // br
            PC += signedOperand;
            break;

        case 0x12:  // HALT
            halted = true;
            break;


        default:
            cout << "Error: Unknown opcode 0x" << hex << opcode << " at PC=" << PC-1 << dec << endl;
            halted = true;
            break;
    }
}

// Debug functions
void dumpRegisters() {
    cout << "Registers:" << endl;
    cout << "A:  0x" << hex << setw(8) << setfill('0') << A << " (" << dec << A << ")" << endl;
    cout << "B:  0x" << hex << setw(8) << setfill('0') << B << " (" << dec << B << ")" << endl;
    cout << "PC: 0x" << hex << setw(8) << setfill('0') << PC << endl;
    cout << "SP: 0x" << hex << setw(8) << setfill('0') << SP << endl;
    cout << dec;
}

void dumpMemory(uint32_t start, uint32_t count) {
    cout << "Memory dump from 0x" << hex << start << ":" << endl;
    for (uint32_t i = 0; i < count; i += 4) {
        cout << hex << setw(6) << setfill('0') << (start + i) << ":  ";
        for (uint32_t j = 0; j < 4 && (i + j) < count; j++) {
            cout << setw(8) << setfill('0') << memory[start + i + j] << " ";
        }
        cout << endl;
    }
    cout << dec;
}

void showHelp() {
    cout << "Available commands:" << endl;
    cout << "  load <filename>  - Load a binary file into memory" << endl;
    cout << "  run             - Run the program until HALT" << endl;
    cout << "  step            - Execute one instruction" << endl;
    cout << "  reset           - Reset the CPU" << endl;
    cout << "  registers       - Display register contents" << endl;
    cout << "  memory <addr> <count> - Display memory contents" << endl;
    cout << "  help            - Show this help message" << endl;
    cout << "  quit            - Exit the emulator" << endl;
}

int main() {
    // Initialize memory with 16MB (24-bit addressable space)
    memory.resize(1 << 24, 0);
    
    string command;
    
    cout << "Simple CPU Emulator" << endl;
    showHelp();

    while (true) {
        cout << "\n> ";
        cin >> command;

        if (command == "quit") {
            break;
        }
        else if (command == "help") {
            showHelp();
        }
        else if (command == "load") {
            string filename;
            cin >> filename;
            if (loadProgram(filename)) {
                cout << "Program loaded successfully" << endl;
            }
        }
        else if (command == "run") {
            while (!isHalted()) {
                step();
            }
            cout << "Program halted after " << getInstructionCount() << " instructions" << endl;
        }
        else if (command == "step") {
            if (!isHalted()) {
                step();
                dumpRegisters();
            } else {
                cout << "CPU is halted" << endl;
            }
        }
        else if (command == "reset") {
            reset();
            cout << "CPU reset" << endl;
        }
        else if (command == "registers") {
            dumpRegisters();
        }
        else if (command == "memory") {
            uint32_t addr, count;
            cin >> hex >> addr >> count;
            dumpMemory(addr, count);
        }
        else {
            cout << "Unknown command. Type 'help' for available commands." << endl;
        }
    }

    return 0;
}