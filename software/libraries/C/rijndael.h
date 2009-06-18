/*
Optimised ANSI C code for the Rijndael cipher (now AES)

Authors:
    Vincent Rijmen <vincent.rijmen@esat.kuleuven.ac.be>
    Antoon Bosselaers <antoon.bosselaers@esat.kuleuven.ac.be>
    Paulo Barreto <paulo.barreto@terra.com.br>

All code contained in this distributed is placed in the public domain.
========================================================================

Disclaimer:

THIS SOFTWARE IS PROVIDED BY THE AUTHORS ''AS IS'' AND ANY EXPRESS 
OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE 
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 

========================================================================

Acknowledgements:

We are deeply indebted to the following people for their bug reports,
fixes, and improvement suggestions to the API implementation. Though we
tried to list all contributions, we apologise in advance for any
missing reference:

Andrew Bales <Andrew.Bales@Honeywell.com>
Markus Friedl <markus.friedl@informatik.uni-erlangen.de>
John Skodon <skodonj@webquill.com>

========================================================================

Description:

This optimised implementation of Rijndael is noticeably faster than the
previous versions on Intel processors under Win32 w/ MSVC 6.0.  On the
same processor under Linux w/ gcc-2.95.2, the key setup is also
considerably faster, but normal encryption/decryption is only marginally
faster.

To enable full loop unrolling for encryption/decryption, define the
conditional compilation directive FULL_UNROLL.  This may help increase
performance or not, depending on the platform.

To compute the intermediate value tests, define the conditional
compilation directive INTERMEDIATE_VALUE_KAT.  It may be worthwhile to
define the TRACE_KAT_MCT directive too, which provides useful progress
information during the generation of the KAT and MCT sets.

taken from: http://www.efgh.com/software/rijndael.htm
*/

#ifndef H__RIJNDAEL
#define H__RIJNDAEL

int rijndaelSetupEncrypt(unsigned long *rk, const unsigned char *key,
  int keybits);
int rijndaelSetupDecrypt(unsigned long *rk, const unsigned char *key,
  int keybits);
void rijndaelEncrypt(const unsigned long *rk, int nrounds,
  const unsigned char plaintext[16], unsigned char ciphertext[16]);
void rijndaelDecrypt(const unsigned long *rk, int nrounds,
  const unsigned char ciphertext[16], unsigned char plaintext[16]);

#define KEYLENGTH(keybits) ((keybits)/8)
#define RKLENGTH(keybits)  ((keybits)/8+28)
#define NROUNDS(keybits)   ((keybits)/32+6)

#endif
