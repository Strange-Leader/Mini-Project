; Data section
data input 72     ; ASCII value for 'H'
data shift 3      ; Shift value for cipher
data base 65      ; ASCII value for 'A'
data result 0     ; Store result here

; Main program
    ; Initialize stack pointer
    ldc 2000
    a2sp

    ; Load the character to encrypt
    ldl 0        ; Load input character ('H')
    stl 3        ; Store in result temporarily

    ; Convert to 0-25 range (subtract 'A')
    ldl 3        ; Load input character ('H')
    ldl 2        ; Load base value ('A')
    sub          ; Subtract base to get 0-25 range

    ; Apply shift
    ldl 1        ; Load shift value (3)
    add          ; Add shift to character value (3 + 7 = 10)

    ; Perform modulo 26 for wraparound
    ldc 26       ; Load 26
    sub          ; Subtract 26
    brlz wrap    ; If result < 0, go to wrap (adjust result)
    
wrap:
    add          ; Add 26 to get positive value if necessary

done:
    ldl 2        ; Load base value ('A')
    add          ; Convert back to ASCII ('A' + shifted value)
    stl 3        ; Store final result in result

    HALT         ; Stop the emulator