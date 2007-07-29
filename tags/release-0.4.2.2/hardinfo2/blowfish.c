/*
blowfish.c:  C implementation of the Blowfish algorithm.

Copyright (C) 1997 by Paul Kocher

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.
This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  
	

COMMENTS ON USING THIS CODE:

Normal usage is as follows:
   [1] Allocate a BLOWFISH_CTX.  (It may be too big for the stack.)
   [2] Call Blowfish_Init with a pointer to your BLOWFISH_CTX, a pointer to
       the key, and the number of bytes in the key.
   [3] To encrypt a 64-bit block, call Blowfish_Encrypt with a pointer to
       BLOWFISH_CTX, a pointer to the 32-bit left half of the plaintext
	   and a pointer to the 32-bit right half.  The plaintext will be
	   overwritten with the ciphertext.
   [4] Decryption is the same as encryption except that the plaintext and
       ciphertext are reversed.

Warning #1:  The code does not check key lengths. (Caveat encryptor.) 
Warning #2:  Beware that Blowfish keys repeat such that "ab" = "abab".
Warning #3:  It is normally a good idea to zeroize the BLOWFISH_CTX before
  freeing it.
Warning #4:  Endianness conversions are the responsibility of the caller.
  (To encrypt bytes on a little-endian platforms, you'll probably want
  to swap bytes around instead of just casting.)
Warning #5:  Make sure to use a reasonable mode of operation for your
  application.  (If you don't know what CBC mode is, see Warning #7.)
Warning #6:  This code is susceptible to timing attacks.
Warning #7:  Security engineering is risky and non-intuitive.  Have someone 
  check your work.  If you don't know what you are doing, get help.


This is code is fast enough for most applications, but is not optimized for
speed.

If you require this code under a license other than LGPL, please ask.  (I 
can be located using your favorite search engine.)  Unfortunately, I do not 
have time to provide unpaid support for everyone who uses this code.  

                                             -- Paul Kocher
*/  
    
#include "blowfish.h"
    
#define N               16

    { 
0x299F31D0L, 0x082EFA98L, 0xEC4E6C89L, 
0xC97C50DDL, 0x3F84D5B5L, 0xB5470917L, 
};

	{0xD1310BA6L, 0x98DFB5ACL, 0x2FFD72DBL, 0xD01ADFB7L, 
	 0x6A267E96L, 0xBA7C9045L, 0xF12C7F99L, 
	 0x0801F2E2L, 0x858EFC16L, 
	 0xF4933D7EL, 
	 
	 0x286085F0L, 0xCA417918L, 0xB8DB38EFL, 
	 0x6C9E0E8BL, 0xB01E8A3EL, 
	 0x55605C60L, 
	 
	 0x7C72E993L, 0xB3EE1411L, 0x636FBC2AL, 
	 0xCE5C3E16L, 0x9B87931EL, 
	 0x28958677L, 
	 
	 0xE98575B1L, 0xDC262302L, 0xEB651B88L, 
	 0x0F6D6FF3L, 0x83F44239L, 
	 0x9E1F9B5EL, 
	 
	 0x137A3BE4L, 0xBA3BF050L, 0x7EFB2A98L, 
	 0x66CA593EL, 0x82430E88L, 
	 0x3B8B5EBEL, 
	 
	 0xD00A1248L, 0xDB0FEAD3L, 0x49F1C09BL, 
	 0x25D479D8L, 0xF6E8DEF7L, 
	 0x04C006BAL, 
	 
	 0x9B30952CL, 0xCC814544L, 0xAF5EBD09L, 
	 0x660F2807L, 0x192E4BB3L, 
	 0xB9D3FBDBL, 
	 
	 0xFD616B15L, 0x2F501EC8L, 0xAD0552ABL, 
	 0x53317B48L, 0x3E00DF82L, 
	 0xDF1769DBL, 
	 
	 0xFD2183B8L, 0x4AFCB56CL, 0x2DD1D35BL, 
	 0xD28E49BCL, 0x4BFB9790L, 
	 0xCEE4C6E8L, 
	 
	 0xAFC725E0L, 0x8E3C5B2FL, 0x8E7594B7L, 
	 0x8888B812L, 0x900DF01CL, 
	 0xB3A8C1ADL, 
	 
	 0xFD13E0B7L, 0x7CC43B81L, 0xD2ADA8D9L, 
	 0x93CC7314L, 0x211A1477L, 
	 0xFB9D35CFL, 
	 
	 0xF009B91EL, 0x5563911DL, 0x59DFA6AAL, 
	 0x207D5BA2L, 0x02E5B9C5L, 
	 0x4E734A41L, 
	 
	 0x571BE91FL, 0xF296EC6BL, 0x2A0DD915L, 
	 0xFF34052EL, 0xC5855664L, 
	 0x6E85076AL}, 
			 0xC4192623L, 
			 0x9CEE60B8L, 0x8FEDB266L, 
			 0x699A17FFL, 0x5664526CL, 0xC2B19EE1L,
			 
			 0xE4183A3EL, 
			 0x6B8FE4D6L, 0x99F73FD6L, 
			 0xEFE830F5L, 0x4D2D38E6L, 0xF0255DC1L,
			 
			 0x021ECC5EL, 
			 0x3C971814L, 0x6B6A70A1L, 
			 0x52A0E286L, 0xB79C5305L, 0xAA500737L,
			 
			 0x5716F2B8L, 
			 0xF01C1F04L, 0x0200B3FFL, 
			 0x3CB574B2L, 0x25837A58L, 0xDC0921BDL,
			 
			 0x22F54701L, 
			 0xC8B57634L, 0x9AF3DDA7L, 
			 0x0FD0030EL, 0xECC8C73EL, 0xA4751E41L,
			 
			 0x183EB331L, 
			 0x6F420D03L, 0xF60A04BFL, 
			 0x24977C79L, 0x5679B072L, 0xBCAF89AFL,
			 
			 0xDCCF3F2EL, 
			 0x501ADDE6L, 0x9F84CD87L, 
			 0x7408DA17L, 0xBC9F9ABCL, 0xE94B7D8CL,
			 
			 0xC464C3D2L, 
			 0xDD433B37L, 0x24C2BA16L, 
			 0x2A65C451L, 0x50940002L, 0x133AE4DDL,
			 
			 0x5F11199BL, 
			 0x3C11183BL, 0x5924A509L, 
			 0x97F1FBFAL, 0x9EBABF2CL, 0x1E153C6EL,
			 
			 0x5A3E2AB3L, 
			 0x2965DCB9L, 0x99E71D0FL, 
			 0x5266C825L, 0x2E4CC978L, 0x9C10B36AL,
			 
			 0x1E0A2DF4L, 
			 0x1939260FL, 0x19C27960L, 
			 0xF71312B6L, 0xEBADFE6EL, 0xEAC31F66L,
			 
			 0x018CFF28L, 
			 0x65582185L, 0x68AB9802L, 
			 0xDB2F953BL, 0x2AEF7DADL, 0x5B6E2F84L,
			 
			 0x619F1510L, 
			 0x0334FE1EL, 0xAA0363CFL, 
			 0x4C70A239L, 0xD59E9E0BL, 0xCBAADE14L,
			 
			 0xB2F3846EL, 
			 0xA02369B9L, 0x655ABB50L, 
			 0x3C2AB4B3L, 0x319EE9D5L, 0xC021B8F7L,
			 
			 0x623D7DA8L, 
			 0x11ED935FL, 0x16681281L, 
			 0xC7E61FD6L, 0x96DEDFA1L, 0x7858BA99L,
			 
			 0x1AC24696L, 
			 0x8FD948E4L, 0x6DBC3128L, 
			 0x34C6FFEAL, 0xFE28ED61L, 0xEE7C3C73L,
			 
			 0x203E13E0L, 
			 0xDB6C4F15L, 0xFACB4FD0L, 
			 0xEF6ABBB5L, 0x654F3B1DL, 0x41CD2105L,
			 
			 0x3D816250L, 
			 0xFC8883A0L, 0xC1C7B6A3L, 
			 0x69CB7492L, 0x47848A0BL, 0x5692B285L,
			 
			 0x23820E00L, 
			 0x1DADF43EL, 0x233F7061L, 
			 0x8D937E41L, 0xD65FECF1L, 0x6C223BDBL,
			 
			 0xCE77326EL, 
			 0xE8EFD855L, 0x61D99735L, 
			 0xC50C06C2L, 0x5A04ABFCL, 0x800BCADCL,
			 
			 0x0E1E9EC9L, 
			 0x675FDA79L, 0xE3674340L, 
			 0x713E38D8L, 0x3D28F89EL, 0xF16DFF20L,
			 
			 0xDB83ADF7L}, 
					 0xF64C261CL, 0x94692934L,
					 
					 0xBCF46B2EL, 0xD4A20068L,
					 
					 0x43B7D4B7L, 0x500061AFL,
					 
					 0x14214F74L, 0xBF8B8840L,
					 
					 0x70F4DDD3L, 0x66A02F45L,
					 
					 0x7FAC6DD0L, 0x31CB8504L,
					 
					 0xDA2547E6L, 0xABCA0A9AL,
					 
					 0x0A2C86DAL, 0xE9B66DFBL,
					 
					 0x680EC0A4L, 0x27A18DEEL,
					 
					 0xB58CE006L, 0x7AF4D6B6L,
					 
					 0xCE78A399L, 0x406B2A42L,
					 
					 0xEE39D7ABL, 0x3B124E8BL,
					 
					 0x26A36631L, 0xEAE397B2L,
					 
					 0x6841E7F7L, 0xCA7820FBL,
					 
					 0x454056ACL, 0xBA489527L,
					 
					 0xFE6BA9B7L, 0xD096954BL,
					 
					 0xCCA92963L, 0x99E1DB33L,
					 
					 0x5EF47E1CL, 0x9029317CL,
					 
					 0x80BB155CL, 0x05282CE3L,
					 
					 0x48C1133FL, 0xC70F86DCL,
					 
					 0x404779A4L, 0x5D886E17L,
					 
					 0xF2BCC18FL, 0x41113564L,
					 
					 0xDFF8E8A3L, 0x1F636C1BL,
					 
					 0xAF664FD1L, 0xCAD18115L,
					 
					 0x3B240B62L, 0xEEBEB922L,
					 
					 0xDE720C8CL, 0x2DA2F728L,
					 
					 0x647D0862L, 0xE7CCF5F0L,
					 
					 0xC39DFD27L, 0xF33E8D1EL,
					 
					 0x3A6F6EABL, 0xF4F8FD37L,
					 
					 0x991BE14CL, 0xDB6E6B0DL,
					 
					 0x2765D43BL, 0xDCD0E804L,
					 
					 0xB5390F92L, 0x690FED0BL,
					 
					 0xA091CF0BL, 0xD9155EA3L,
					 
					 0x7B9479BFL, 0x763BD6EBL,
					 
					 0x8026E297L, 0xF42E312DL,
					 
					 0x12754CCCL, 0x782EF11CL,
					 
					 0x06A1BBE6L, 0x4BFB6350L,
					 
					 0x3D25BDD8L, 0xE2E1C3C9L,
					 
					 0xD90CEC6EL, 0xD5ABEA2AL,
					 
					 0xBEBFE988L, 0x64E4C3FEL,
					 
					 0x60787BF8L, 0x6003604DL,
					 
					 0x7745AE04L, 0xD736FCCCL,
					 
					 0xB0804187L, 0x3C005E5FL,
					 
					 0x55464299L, 0xBF582E61L,
					 
					 0xF474EF38L, 0x8789BDC2L,
					 
					 0xB475F255L, 0x46FCD9B9L,
					 
					 0x846A0E79L, 0x915F95E2L,
					 
					 0x8CD55591L, 0xC902DE4CL,
					 
					 0x11A86248L, 0x7574A99EL,
					 
					 0x662D09A1L, 0xC4324633L,
					 
					 0x4A99A025L, 0x1D6EFE10L,
					 
					 0xA186F20FL, 0x2868F169L,
					 
					 0xA1E2CE9BL, 0x4FCD7F52L,
					 
					 0xA002B5C4L, 0x0DE6D027L,
					 
					 0xC3604C06L, 0x61A806B5L,
					 
					 0x006058AAL, 0x30DC7D62L,
					 
					 0x53C2DD94L, 0xC2C21634L,
					 
					 0xEBFC7DA1L, 0xCE591D76L,
					 
					 0x39720A3DL, 0x7C927C24L,
					 
					 0x1AC15BB4L, 0xD39EB8FCL,
					 
					 0xD83D7CD3L, 0x4DAD0FC4L,
					 
					 0xA28514D9L, 0x6C51133CL,
					 
					 0x362ABFCEL, 0xDDC6C837L,
					 
					 0x670EFA8EL, 0x406000E0L},
    
      0x4FA33742L, 0xD3822740L, 0x99BC9BBEL, 
      0xD62D1C7EL, 0xC700C47BL, 
      0x6A366EB4L, 
      
      0xA9BA4650L, 0xAC9526E8L, 0xBE5EE304L, 
      0x63EF8CE2L, 0x9A86EE22L, 
      0x9CF2D0A4L, 
      
      0xF752F7DAL, 0x3F046F69L, 0x77FA0A59L, 
      0x9B09E6ADL, 0x3B3EE593L, 
      0x022B8B51L, 
      
      0xE019A5E6L, 0x47B0ACFDL, 0xED93FA9BL, 
      0xF8D56629L, 0x79132E28L, 
      0xE3D35E8CL, 
      
      0x1E6321F5L, 0xF59C66FBL, 0x26DCF319L, 
      0x03563482L, 0x8ABA3CBBL, 
      0xCCAD925FL, 
      
      0xC3293D46L, 0x48DE5369L, 0x6413E680L, 
      0x69852DFDL, 0x09072166L, 
      0x1C20C8AEL, 
      
      0xFA6484BBL, 0x8D6612AEL, 0xBF3C6F47L, 
      0xAEC2771BL, 0xF64E6370L, 
      0xAF537D5DL, 
      
      0x3520AB82L, 0x011A1D4BL, 0x277227F8L, 
      0xBB3A792BL, 0x344525BDL, 
      0xA01FBAC9L, 
      
      0xC6913667L, 0x8DF9317CL, 0xE0B12B4FL, 
      0xF2D519FFL, 0x27D9459CL, 
      0x9B941525L, 
      
      0x1698DB3BL, 0x4C98A0BEL, 0x3278E964L, 
      0xD3A0342BL, 0x8971F21EL, 
      0xC37632D8L, 
      
      0xFD2C1D05L, 0x848FD2C5L, 0xF6FB2299L, 
      0x93A83531L, 0x56CCCD02L, 
      0x88D273CCL, 
      
      0x62A80F00L, 0xBB25BFE2L, 0x35BDD2F6L, 
      0xB6CBCF7CL, 0xCD769C2BL, 
      0x2547ADF0L, 
      
      0x02FB8A8CL, 0x01C36AE4L, 0xD6EBE1F9L, 
      0x3F09252DL, 0xC208E69FL, 
      0x3AC372E6L} 
};

{
    
    
    
    
    
    
    
    
    
    
    
    
    


			 unsigned long *xr)
{
    
    
    
    
    
    
    
	
	
	
	
	
    
    
    
    
    
    
    
    


			 unsigned long *xr)
{
    
    
    
    
    
    
    
	
	
	
	    /* Exchange Xl and Xr */ 
	    temp = Xl;
	
	
    
    
	/* Exchange Xl and Xr */ 
	temp = Xl;
    
    
    
    
    
    


{
    
    
    
	
	    
    
    
    
	
	
	    
	    
	    
		
	
	
    
    
    
    
	
	
	
    
    
	
	    
	    
	    
	
    


