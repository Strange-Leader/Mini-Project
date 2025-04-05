    ; Get arr[j] into temp
    ldc array_address     
    ldc j_address        
    ldnl 00             ; Get j value
    add                ; array_address + j
    ldnl 00             ; Get arr[j] value
    ldc temp_address    
    stnl 00             ; Store arr[j] in temp

    ; Calculate arr[j] address and store in temp2
    ldc array_address
    ldc j_address       
    ldnl 00             ; Get j
    add                ; array_address + j
    ldc temp2_address   ; Store this address in temp2
    stnl 00  

    ; Calculate arr[j+1] address and store in temp3
    ldc array_address
    ldc j_address       
    ldnl 00             
    adc 1              ; j + 1
    add                ; array_address + (j+1)
    ldc temp3_address   ; Store this address in temp3
    stnl 00

    ; Get arr[j+1] and put into arr[j]
    ldc temp3_address   ; Get arr[j+1] address
    ldnl 00
    ldnl 00             ; Get arr[j+1] value
    ldc temp2_address   ; Get arr[j] address
    ldnl 00
    stnl 00             ; Store arr[j+1] into arr[j]

    ; Put temp into arr[j+1]
    ldc temp_address   ; Get arr[j+1] address
    ldnl 00
    ldc temp3_address
    ldnl 00             ; Get original arr[j] from temp
    stnl 00             ; Store in arr[j+1]

end_sort:
    HALT

; Data section
array_address:        ; Array of 5 elements
    data 50
    data 40
    data 30
    data 20
    data 10
j_address:
    data 00
temp_address:
    data 00
temp2_address:
    data 00  
temp3_address:
    data 00
