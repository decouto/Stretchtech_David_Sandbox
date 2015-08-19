/* ApWbHelpTxt.h -- Ap Workbench Command Scripting Command Help 
======================================================================================
                              Help information                                      
======================================================================================
RW <regAdd> <regValue>                        Register write                                      
RWM <regAdd> <regValue> <regValue> ...        Register write multiple-bytes                       
RR <regAdd>                                   Register read                                       
RCMP <regAdd> <regValue>                      Register compare                                    
RBS <regAdd> <nBitId> <0/1>                   Register set bit                                    
CW1 <coefAdd> <coefValue>                     Coefficient write single (hex value)                
CW1F <coefAdd> <coefValue> <leftBits>         Coefficient write single (float value)              
CWA <coefAdd> <5 coefValues>                  Coefficient write all (5 coeffs. - hex values)      
CWAF <coefAdd> <5 coefValues> <leftBits>      Coefficient write all (5 coeffs. - float values)    
CWAP <coefAdd> <T> <F> <G> <Q> <S> <leftBits> Coefficient write all (5 coeffs. - parametric)      
CR1 <coefAdd>                                 Coefficient read single                             
CRA <coefAdd>                                 Coefficient read all (5)                            
CCMP1 <coefAdd> <coefExpValue>                Coefficient compare single (vs. expected hex value) 
CCMPA <coefAdd> <5 coefExpValues>             Coefficient compare all (vs. 5 expected hex values) 
P <N>                                         Pause N milliseconds                                
SECTION_BEGIN <N>                             Section begin marker (then repeat N times)          
SECTION_END                                   Section end marker                                  
RESET                                         Device reset (high->low->high cycle)                
SET_RST <0/1>                                 Set RESET line to a specific logic level            
SET_SEL <0/1>                                 Set SELECT line to a specific logic level           
SET_PWDN <0/1>                                Set PWDN line to a specific logic level             
APWLINK_SEL_DUT                               Send all subsequent I2C commands to the DUT         
APWLINK_SEL_ADC                               Send all subsequent I2C commands to the onboard ADC 
APWLINK_INIT_ADC                              Initialize APWLink onboard ADC (analog->I2S master) 
APWLINK_ADC_RW <regAdd> <regValue>            Write APWLink onboard ADC register                  
APWLINK_ADC_RR <regAdd>                       Read APWLink onboard ADC register                   
IIC_GET_DEV_ADDRESS                           Show I2C device address currently in use            
IIC_SET_DEV_ADDRESS <devAdd>                  Set I2C device address to be used henceforth        
IIC_SET_SPEED <0...3>                         Set I2C speed (0=50, 1=100, 2=200, 3=400, in KHz)   
CLEAR                                         Clear the log viewer contents                       
LOG_SAVE <filename>                           Save the log viewer contents to the specified file  
DUMP_REGISTERS                                Dump device registers                               
DUMP_COEFFICIENTS                             Dump device coefficients                            
MONITOR_CLEAR_ALL                             Clear all event monitors                            
MONITOR_LIST                                  List all event monitors                             
MONITOR_REG_BIT <regAdd> <regBit> <0/1>       Monitor register bit toggling to either 0/1         
MONITOR_SIGNAL <sigName> <0/1>                Monitor signal toggling to either 0/1			   
MONITOR_SHOW_OSD <eventId> <0/1>              Enable(1) or disable(0) OSD upon event triggering   
MONITOR_SET_MSG <eventId> <Msg>               Set message to be logged upon event triggering      
MONITOR_SET_ACTION <eventId> <CmdList>        Set commands to be performed upon event triggering  
MONITOR_ENABLE <eventId> <0/1>                Enable(1) or disable(0) event monitoring            
MONITOR_SET_TIMER <pollingInterval>           Set global monitoring polling interval (ms)         
REFRESH_UI                                    Refresh UI controls (no retrieve from DUT)          
REFRESH_UI_AUTO <0/1>                         Enable(1) or disable(0) UI auto-refresh on every cmd
TERMINATE                                     Force early script termination                      
HELP                                          Show this help message                              
======================================================================================

*/
