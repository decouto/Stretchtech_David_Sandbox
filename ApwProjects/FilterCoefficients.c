/***********************************************************************************
                     Sound Terminal coefficients array
                          Created with APWorkbench

No warranty:                                                                       
  THIS SOFTWARE IS PROVIDED BY ST "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY, OF         
  SATISFACTORY QUALITY, AND FITNESS FOR A PARTICULAR PURPOSE OR USE ARE            
  DISCLAIMED. ST MAKES NO REPRESENTATION THAT THE SOFTWARE WILL NOT INFRINGE ANY   
  PATENT, COPYRIGHT, TRADE SECRET OR OTHER PROPRIETARY RIGHT OF A THIRD PARTY.     
  ST does not warrant that use in whole or in part of the Software will be         
  uninterrupted or error free, will meet your requirements, or will operate with   
  the combination of hardware and software selected by you.                        
                                                                                   
  ST is under no obligation to provide bug fixes, patches, upgrades or other       
  enhancements or derivatives of the features, functionality or performance of     
  this Software.                                                                   
                                                                                   
  ALL WARRANTIES, CONDITIONS OR OTHER TERMS IMPLIED BY LAW ARE EXCLUDED TO THE     
  FULLEST EXTENT PERMITTED BY LAW.                                                 
                                                                                   
  You acknowledge that the Software, and processes and products related thereto    
  are not designed nor authorized for use in life supporting devices. You agree    
  to hold ST free and harmless from any liability in connection with the use of    
  the Software and products or processes related in any way thereto.               
                                                                                   
No liability:                                                                      
  ST SHALL HAVE NO LIABILITY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL,            
  CONSEQUENTIAL, EXEMPLARY, OR PUNITIVE DAMAGES OF ANY CHARACTER INCLUDING,        
  WITHOUT LIMITATION, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES, LOSS OF USE,    
  DATA OR PROFITS, OR BUSINESS INTERRUPTION, HOWEVER CAUSED AND ON ANY THEORY OF   
  CONTRACT, WARRANTY, TORT (INCLUDING NEGLIGENCE), PRODUCT LIABILITY OR            
  OTHERWISE, ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED   
  OF THE POSSIBILITY OF SUCH DAMAGES.                                              


Creation date:		Saturday, 01/August/'15
Tool revision:		APW v2.100
Selected product:	STA321MP DK
***********************************************************************************/


Speaker_EQ_Table_48000[] = {
	{0x00,0x000000},{0x01,0x000000},{0x02,0x000000},{0x03,0x000000},{0x04,0x400000},    // 48KFs, Biquad1, Address: 0x00~0x04 (stability: 0.0000)
	{0x05,0x8037D9},{0x06,0x7FC827},{0x07,0x7FC81B},{0x08,0x806F98},{0x09,0x3FE413},    // 48KFs, Biquad2, Address: 0x05~0x09 (stability: 0.9983)
	{0x0A,0x8037D9},{0x0B,0x7FC827},{0x0C,0x7FC81B},{0x0D,0x806F98},{0x0E,0x3FE413},    // 48KFs, Biquad3, Address: 0x0A~0x0E (stability: 0.9983)
	{0x0F,0x093EC0},{0x10,0x093EC0},{0x11,0x46FCC7},{0x12,0xCD0B6E},{0x13,0x049F60},    // 48KFs, Biquad4, Address: 0x0F~0x13 (stability: 0.6309)
	{0x14,0x093EC0},{0x15,0x093EC0},{0x16,0x46FCC7},{0x17,0xCD0B6E},{0x18,0x049F60},    // 48KFs, Biquad5, Address: 0x14~0x18 (stability: 0.6309)
	{0x19,0x093EC0},{0x1A,0x093EC0},{0x1B,0x46FCC7},{0x1C,0xCD0B6E},{0x1D,0x049F60},    // 48KFs, Biquad6, Address: 0x19~0x1D (stability: 0.6309)
	{0x1E,0xA418CA},{0x1F,0x5667B6},{0x20,0x5BE736},{0x21,0x985132},{0x22,0x48A38B},    // 48KFs, Biquad7, Address: 0x1E~0x22 (stability: 0.9000)
	{0x23,0x000000},{0x24,0x000000},{0x25,0x000000},{0x26,0x000000},{0x27,0x400000},    // 48KFs, Biquad8, Address: 0x23~0x27 (stability: 0.0000)
	{0x28,0x000000},{0x29,0x000000},{0x2A,0x000000},{0x2B,0x000000},{0x2C,0x400000},    // 48KFs, Biquad9, Address: 0x28~0x2C (stability: 0.0000)
	{0x2D,0x000000},{0x2E,0x000000},{0x2F,0x000000},{0x30,0x000000},{0x31,0x400000}     // 48KFs, Biquad10, Address: 0x2D~0x31 (stability: 0.0000)
};
