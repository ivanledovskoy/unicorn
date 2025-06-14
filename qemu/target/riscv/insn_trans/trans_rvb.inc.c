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

static bool trans_cpop(DisasContext *ctx, arg_cpop *a)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    return gen_unary(tcg_ctx, a, tcg_gen_ctpop_tl);
}

static bool trans_andn(DisasContext *ctx, arg_andn *a)
{
    return gen_logic(ctx, a, tcg_gen_andc_tl);
}

static bool trans_orn(DisasContext *ctx, arg_orn *a)
{
    return gen_logic(ctx, a, tcg_gen_orc_tl);
}

static bool trans_xnor(DisasContext *ctx, arg_xnor *a)
{
    return gen_logic(ctx, a, tcg_gen_eqv_tl);
}

static bool trans_min(DisasContext *ctx, arg_min *a)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    return gen_arith(tcg_ctx, a, tcg_gen_smin_tl);
}

static bool trans_max(DisasContext *ctx, arg_max *a)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    return gen_arith(tcg_ctx, a, tcg_gen_smax_tl);
}

static bool trans_minu(DisasContext *ctx, arg_minu *a)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    return gen_arith(tcg_ctx, a, tcg_gen_umin_tl);
}

static bool trans_maxu(DisasContext *ctx, arg_maxu *a)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    return gen_arith(tcg_ctx, a, tcg_gen_umax_tl);
}

static bool trans_sext_b(DisasContext *ctx, arg_sext_b *a)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    return gen_unary(tcg_ctx, a, tcg_gen_ext8s_tl);
}

static bool trans_sext_h(DisasContext *ctx, arg_sext_h *a)
{
    TCGContext *tcg_ctx = ctx->uc->tcg_ctx;
    return gen_unary(tcg_ctx, a, tcg_gen_ext16s_tl);
}

static void gen_sbop_mask(TCGContext *tcg_ctx, TCGv ret, TCGv shamt)
{
    tcg_gen_movi_tl(tcg_ctx, ret, 1);
    tcg_gen_shl_tl(tcg_ctx, ret, ret, shamt);
}

static void gen_bset(TCGContext *tcg_ctx, TCGv ret, TCGv arg1, TCGv shamt)
{
    TCGv t = tcg_temp_new(tcg_ctx);

    gen_sbop_mask(tcg_ctx, t, shamt);
    tcg_gen_or_tl(tcg_ctx, ret, arg1, t);
}

static bool trans_bset(DisasContext *ctx, arg_bset *a)
{
    return gen_shift(ctx, a, gen_bset);
}

static bool trans_bseti(DisasContext *ctx, arg_bseti *a)
{
    return gen_shift_imm_tl(ctx, a, gen_bset);
}

static void gen_bclr(TCGContext *tcg_ctx, TCGv ret, TCGv arg1, TCGv shamt)
{
    TCGv t = tcg_temp_new(tcg_ctx);

    gen_sbop_mask(tcg_ctx, t, shamt);
    tcg_gen_andc_tl(tcg_ctx, ret, arg1, t);
}

static bool trans_bclr(DisasContext *ctx, arg_bclr *a)
{
    return gen_shift(ctx, a, gen_bclr);
}

static bool trans_bclri(DisasContext *ctx, arg_bclri *a)
{
    return gen_shift_imm_tl(ctx, a, gen_bclr);
}

static void gen_binv(TCGContext *tcg_ctx, TCGv ret, TCGv arg1, TCGv shamt)
{
    TCGv t = tcg_temp_new(tcg_ctx);

    gen_sbop_mask(tcg_ctx, t, shamt);
    tcg_gen_xor_tl(tcg_ctx, ret, arg1, t);
}

static bool trans_binv(DisasContext *ctx, arg_binv *a)
{
    return gen_shift(ctx, a, gen_binv);
}

static bool trans_binvi(DisasContext *ctx, arg_binvi *a)
{
    return gen_shift_imm_tl(ctx, a, gen_binv);
}

static void gen_bext(TCGContext *tcg_ctx, TCGv ret, TCGv arg1, TCGv shamt)
{
    tcg_gen_shr_tl(tcg_ctx, ret, arg1, shamt);
    tcg_gen_andi_tl(tcg_ctx, ret, ret, 1);
}

static bool trans_bext(DisasContext *ctx, arg_bext *a)
{
    return gen_shift(ctx, a, gen_bext);
}

static bool trans_bexti(DisasContext *ctx, arg_bexti *a)
{
    return gen_shift_imm_tl(ctx, a, gen_bext);
}

static void gen_rorw(TCGContext *tcg_ctx, TCGv ret, TCGv arg1, TCGv arg2)
{
    TCGv_i32 t1 = tcg_temp_new_i32(tcg_ctx);
    TCGv_i32 t2 = tcg_temp_new_i32(tcg_ctx);

    /* truncate to 32-bits */
    tcg_gen_trunc_tl_i32(tcg_ctx, t1, arg1);
    tcg_gen_trunc_tl_i32(tcg_ctx, t2, arg2);

    tcg_gen_rotr_i32(tcg_ctx, t1, t1, t2);

    /* sign-extend 64-bits */
    tcg_gen_ext_i32_tl(tcg_ctx, ret, t1);
}

static bool trans_ror(DisasContext *ctx, arg_ror *a)
{
    return gen_shift_per_ol(ctx, a, tcg_gen_rotr_tl, gen_rorw);
}