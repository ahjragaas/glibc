/* Coefficients and table entries for vector log2

   Copyright (C) 2023-2025 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#include "vecmath_config.h"

#define N (1 << V_LOG2_TABLE_BITS)

const struct v_log2_data __v_log2_data = {

  /* Each coefficient was generated to approximate log(r) for |r| < 0x1.fp-9
     and N = 128, then scaled by log2(e) in extended precision and rounded back
     to double precision.  */
  .poly = { -0x1.71547652b83p-1, 0x1.ec709dc340953p-2, -0x1.71547651c8f35p-2,
	    0x1.2777ebe12dda5p-2, -0x1.ec738d616fe26p-3 },

  .invln2 = 0x1.71547652b82fep0,

  /* Derived from tables in v_log_data.c in a similar way as v_log10_data.c.
     This means invc is unchanged and log2c was calculated by scaling log(c) by
     log2(e) in extended precision and rounding back to double precision.  */
  .table = { { 0x1.6a133d0dec120p+0, -0x1.00130d57f5fadp-1 },
	     { 0x1.6815f2f3e42edp+0, -0x1.f802661bd725ep-2 },
	     { 0x1.661e39be1ac9ep+0, -0x1.efea1c6f73a5bp-2 },
	     { 0x1.642bfa30ac371p+0, -0x1.e7dd1dcd06f05p-2 },
	     { 0x1.623f1d916f323p+0, -0x1.dfdb4ae024809p-2 },
	     { 0x1.60578da220f65p+0, -0x1.d7e484d101958p-2 },
	     { 0x1.5e75349dea571p+0, -0x1.cff8ad452f6ep-2 },
	     { 0x1.5c97fd387a75ap+0, -0x1.c817a666c997fp-2 },
	     { 0x1.5abfd2981f200p+0, -0x1.c04152d640419p-2 },
	     { 0x1.58eca051dc99cp+0, -0x1.b87595a3f64b2p-2 },
	     { 0x1.571e526d9df12p+0, -0x1.b0b4526c44d07p-2 },
	     { 0x1.5554d555b3fcbp+0, -0x1.a8fd6d1a90f5ep-2 },
	     { 0x1.539015e2a20cdp+0, -0x1.a150ca2559fc6p-2 },
	     { 0x1.51d0014ee0164p+0, -0x1.99ae4e62cca29p-2 },
	     { 0x1.50148538cd9eep+0, -0x1.9215df1a1e842p-2 },
	     { 0x1.4e5d8f9f698a1p+0, -0x1.8a8761fe1f0d9p-2 },
	     { 0x1.4cab0edca66bep+0, -0x1.8302bd1cc9a54p-2 },
	     { 0x1.4afcf1a9db874p+0, -0x1.7b87d6fb437f6p-2 },
	     { 0x1.495327136e16fp+0, -0x1.741696673a86dp-2 },
	     { 0x1.47ad9e84af28fp+0, -0x1.6caee2b3c6fe4p-2 },
	     { 0x1.460c47b39ae15p+0, -0x1.6550a3666c27ap-2 },
	     { 0x1.446f12b278001p+0, -0x1.5dfbc08de02a4p-2 },
	     { 0x1.42d5efdd720ecp+0, -0x1.56b022766c84ap-2 },
	     { 0x1.4140cfe001a0fp+0, -0x1.4f6db1c955536p-2 },
	     { 0x1.3fafa3b421f69p+0, -0x1.4834579063054p-2 },
	     { 0x1.3e225c9c8ece5p+0, -0x1.4103fd2249a76p-2 },
	     { 0x1.3c98ec29a211ap+0, -0x1.39dc8c3fe6dabp-2 },
	     { 0x1.3b13442a413fep+0, -0x1.32bdeed4b5c8fp-2 },
	     { 0x1.399156baa3c54p+0, -0x1.2ba80f41e20ddp-2 },
	     { 0x1.38131639b4cdbp+0, -0x1.249ad8332f4a7p-2 },
	     { 0x1.36987540fbf53p+0, -0x1.1d96347e7f3ebp-2 },
	     { 0x1.352166b648f61p+0, -0x1.169a0f7d6604ap-2 },
	     { 0x1.33adddb3eb575p+0, -0x1.0fa654a221909p-2 },
	     { 0x1.323dcd99fc1d3p+0, -0x1.08baefcf8251ap-2 },
	     { 0x1.30d129fefc7d2p+0, -0x1.01d7cd14deecdp-2 },
	     { 0x1.2f67e6b72fe7dp+0, -0x1.f5f9b1ad55495p-3 },
	     { 0x1.2e01f7cf8b187p+0, -0x1.e853ff76a77afp-3 },
	     { 0x1.2c9f518ddc86ep+0, -0x1.dabe5d624cba1p-3 },
	     { 0x1.2b3fe86e5f413p+0, -0x1.cd38a5cef4822p-3 },
	     { 0x1.29e3b1211b25cp+0, -0x1.bfc2b38d315f9p-3 },
	     { 0x1.288aa08b373cfp+0, -0x1.b25c61f5edd0fp-3 },
	     { 0x1.2734abcaa8467p+0, -0x1.a5058d18e9cacp-3 },
	     { 0x1.25e1c82459b81p+0, -0x1.97be1113e47a3p-3 },
	     { 0x1.2491eb1ad59c5p+0, -0x1.8a85cafdf5e27p-3 },
	     { 0x1.23450a54048b5p+0, -0x1.7d5c97e8fc45bp-3 },
	     { 0x1.21fb1bb09e578p+0, -0x1.704255d6486e4p-3 },
	     { 0x1.20b415346d8f7p+0, -0x1.6336e2cedd7bfp-3 },
	     { 0x1.1f6fed179a1acp+0, -0x1.563a1d9b0cc6ap-3 },
	     { 0x1.1e2e99b93c7b3p+0, -0x1.494be541aaa6fp-3 },
	     { 0x1.1cf011a7a882ap+0, -0x1.3c6c1964dd0f2p-3 },
	     { 0x1.1bb44b97dba5ap+0, -0x1.2f9a99f19a243p-3 },
	     { 0x1.1a7b3e66cdd4fp+0, -0x1.22d747344446p-3 },
	     { 0x1.1944e11dc56cdp+0, -0x1.1622020d4f7f5p-3 },
	     { 0x1.18112aebb1a6ep+0, -0x1.097aabb3553f3p-3 },
	     { 0x1.16e013231b7e9p+0, -0x1.f9c24b48014c5p-4 },
	     { 0x1.15b1913f156cfp+0, -0x1.e0aaa3bdc858ap-4 },
	     { 0x1.14859cdedde13p+0, -0x1.c7ae257c952d6p-4 },
	     { 0x1.135c2dc68cfa4p+0, -0x1.aecc960a03e58p-4 },
	     { 0x1.12353bdb01684p+0, -0x1.9605bb724d541p-4 },
	     { 0x1.1110bf25b85b4p+0, -0x1.7d595ca7147cep-4 },
	     { 0x1.0feeafd2f8577p+0, -0x1.64c74165002d9p-4 },
	     { 0x1.0ecf062c51c3bp+0, -0x1.4c4f31c86d344p-4 },
	     { 0x1.0db1baa076c8bp+0, -0x1.33f0f70388258p-4 },
	     { 0x1.0c96c5bb3048ep+0, -0x1.1bac5abb3037dp-4 },
	     { 0x1.0b7e20263e070p+0, -0x1.0381272495f21p-4 },
	     { 0x1.0a67c2acd0ce3p+0, -0x1.d6de4eba2de2ap-5 },
	     { 0x1.0953a6391e982p+0, -0x1.a6ec4e8156898p-5 },
	     { 0x1.0841c3caea380p+0, -0x1.772be542e3e1bp-5 },
	     { 0x1.07321489b13eap+0, -0x1.479cadcde852dp-5 },
	     { 0x1.062491aee9904p+0, -0x1.183e4265faa5p-5 },
	     { 0x1.05193497a7cc5p+0, -0x1.d2207fdaa1b85p-6 },
	     { 0x1.040ff6b5f5e9fp+0, -0x1.742486cb4a6a2p-6 },
	     { 0x1.0308d19aa6127p+0, -0x1.1687d77cfc299p-6 },
	     { 0x1.0203beedb0c67p+0, -0x1.7293623a6b5dep-7 },
	     { 0x1.010037d38bcc2p+0, -0x1.70ec80ec8f25dp-8 },
	     { 1.0, 0.0 },
	     { 0x1.fc06d493cca10p-1, 0x1.704c1ca6b6bc9p-7 },
	     { 0x1.f81e6ac3b918fp-1, 0x1.6eac8ba664beap-6 },
	     { 0x1.f44546ef18996p-1, 0x1.11e67d040772dp-5 },
	     { 0x1.f07b10382c84bp-1, 0x1.6bc665e2105dep-5 },
	     { 0x1.ecbf7070e59d4p-1, 0x1.c4f8a9772bf1dp-5 },
	     { 0x1.e91213f715939p-1, 0x1.0ebff10fbb951p-4 },
	     { 0x1.e572a9a75f7b7p-1, 0x1.3aaf4d7805d11p-4 },
	     { 0x1.e1e0e2c530207p-1, 0x1.664ba81a4d717p-4 },
	     { 0x1.de5c72d8a8be3p-1, 0x1.9196387da6de4p-4 },
	     { 0x1.dae50fa5658ccp-1, 0x1.bc902f2b7796p-4 },
	     { 0x1.d77a71145a2dap-1, 0x1.e73ab5f584f28p-4 },
	     { 0x1.d41c51166623ep-1, 0x1.08cb78510d232p-3 },
	     { 0x1.d0ca6ba0bb29fp-1, 0x1.1dd2fe2f0dcb5p-3 },
	     { 0x1.cd847e8e59681p-1, 0x1.32b4784400df4p-3 },
	     { 0x1.ca4a499693e00p-1, 0x1.47706f3d49942p-3 },
	     { 0x1.c71b8e399e821p-1, 0x1.5c0768ee4a4dcp-3 },
	     { 0x1.c3f80faf19077p-1, 0x1.7079e86fc7c6dp-3 },
	     { 0x1.c0df92dc2b0ecp-1, 0x1.84c86e1183467p-3 },
	     { 0x1.bdd1de3cbb542p-1, 0x1.98f377a34b499p-3 },
	     { 0x1.baceb9e1007a3p-1, 0x1.acfb803bc924bp-3 },
	     { 0x1.b7d5ef543e55ep-1, 0x1.c0e10098b025fp-3 },
	     { 0x1.b4e749977d953p-1, 0x1.d4a46efe103efp-3 },
	     { 0x1.b20295155478ep-1, 0x1.e8463f45b8d0bp-3 },
	     { 0x1.af279f8e82be2p-1, 0x1.fbc6e3228997fp-3 },
	     { 0x1.ac5638197fdf3p-1, 0x1.079364f2e5aa8p-2 },
	     { 0x1.a98e2f102e087p-1, 0x1.1133306010a63p-2 },
	     { 0x1.a6cf5606d05c1p-1, 0x1.1ac309631bd17p-2 },
	     { 0x1.a4197fc04d746p-1, 0x1.24432485370c1p-2 },
	     { 0x1.a16c80293dc01p-1, 0x1.2db3b5449132fp-2 },
	     { 0x1.9ec82c4dc5bc9p-1, 0x1.3714ee1d7a32p-2 },
	     { 0x1.9c2c5a491f534p-1, 0x1.406700ab52c94p-2 },
	     { 0x1.9998e1480b618p-1, 0x1.49aa1d87522b2p-2 },
	     { 0x1.970d9977c6c2dp-1, 0x1.52de746d7ecb2p-2 },
	     { 0x1.948a5c023d212p-1, 0x1.5c0434336b343p-2 },
	     { 0x1.920f0303d6809p-1, 0x1.651b8ad6c90d1p-2 },
	     { 0x1.8f9b698a98b45p-1, 0x1.6e24a56ab5831p-2 },
	     { 0x1.8d2f6b81726f6p-1, 0x1.771fb04ec29b1p-2 },
	     { 0x1.8acae5bb55badp-1, 0x1.800cd6f19c25ep-2 },
	     { 0x1.886db5d9275b8p-1, 0x1.88ec441df11dfp-2 },
	     { 0x1.8617ba567c13cp-1, 0x1.91be21b7c93f5p-2 },
	     { 0x1.83c8d27487800p-1, 0x1.9a8298f8c7454p-2 },
	     { 0x1.8180de3c5dbe7p-1, 0x1.a339d255c04ddp-2 },
	     { 0x1.7f3fbe71cdb71p-1, 0x1.abe3f59f43db7p-2 },
	     { 0x1.7d055498071c1p-1, 0x1.b48129deca9efp-2 },
	     { 0x1.7ad182e54f65ap-1, 0x1.bd119575364c1p-2 },
	     { 0x1.78a42c3c90125p-1, 0x1.c5955e23ebcbcp-2 },
	     { 0x1.767d342f76944p-1, 0x1.ce0ca8f4e1557p-2 },
	     { 0x1.745c7ef26b00ap-1, 0x1.d6779a5a75774p-2 },
	     { 0x1.7241f15769d0fp-1, 0x1.ded6563550d27p-2 },
	     { 0x1.702d70d396e41p-1, 0x1.e728ffafd840ep-2 },
	     { 0x1.6e1ee3700cd11p-1, 0x1.ef6fb96c8d739p-2 },
	     { 0x1.6c162fc9cbe02p-1, 0x1.f7aaa57907219p-2 } }
};
