/*
 * RISC-V translation routines for the RV64A Standard Extension.
 *
 * Copyright (c) 2016-2017 Sagar Karandikar, sagark@eecs.berkeley.edu
 * Copyright (c) 2018 Peer Adelt, peer.adelt@hni.uni-paderborn.de
 *                    Bastian Koppelmann, kbastian@mail.uni-paderborn.de
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2 or later, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

static void gen_clz(TCGContext *tcg_ctx, TCGv ret, TCGv arg1)
{
    tcg_gen_clzi_tl(tcg_ctx, ret, arg1, TARGET_LONG_BITS);
}

static void gen_clzw(TCGContext *tcg_ctx, TCGv ret, TCGv arg1)
{
    TCGv t = tcg_temp_new(tcg_ctx);
    tcg_gen_shli_tl(tcg_ctx, t, arg1, 32);
    tcg_gen_clzi_tl(tcg_ctx, ret, t, 32);
}

static bool trans_clz(DisasContext *ctx, arg_clz *a)
{
    return gen_unary_per_ol(ctx, a, gen_clz, gen_clzw);
}

static void gen_ctz(TCGContext *tcg_ctx, TCGv ret, TCGv arg1)
{
    tcg_gen_ctzi_tl(tcg_ctx, ret, arg1, TARGET_LONG_BITS);
}

static void gen_ctzw(TCGContext *tcg_ctx, TCGv ret, TCGv arg1)
{
    tcg_gen_ctzi_tl(tcg_ctx, ret, arg1, 32);
}

static bool trans_ctz(DisasContext *ctx, arg_ctz *a)
{
    return gen_unary_per_ol(ctx, a, gen_ctz, gen_ctzw);
}

static bool trans_minu(DisasContext *ctx, arg_minu *a)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    return gen_arith(tcg_ctx, a, tcg_gen_umin_tl);
}