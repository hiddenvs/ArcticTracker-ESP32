idf_component_register(
    SRCS "adc.c" "clock.c" "cmd_system.c" "config.c" "fbuf.c" 
         "gps.c" "main.c" "sr_frs.c" "system.c" "tone.c" "radio.c"
    
    INCLUDE_DIRS "." "../components/ui" "../components/aprs"
)
