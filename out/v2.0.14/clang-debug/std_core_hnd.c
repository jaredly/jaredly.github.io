// Koka generated module: "std/core/hnd", koka version: 2.0.14
#include "std_core_hnd.h"
/*---------------------------------------------------------------------------
  Copyright 2020 Microsoft Corporation.

  This is free software; you can redistribute it and/or modify it under the
  terms of the Apache License, Version 2.0. A copy of the License can be
  found in the file "license.txt" at the root of this distribution.
---------------------------------------------------------------------------*/
/*
typedef datatype_t kk_std_core_hnd__ev;
struct kk_std_core_hnd_Ev {
  kk_std_core_hnd__htag htag;
  kk_box_t hnd;
  int32_t cfc;  // control flow context
  kk_std_core_hnd__evv hevv;
  kk_std_core_hnd__marker marker;
};
*/

static kk_evv_vector_t kk_evv_vector_alloc(size_t length, int32_t cfc, kk_context_t* ctx) {
  kk_assert_internal(length>=0);
  kk_evv_vector_t v = (kk_evv_vector_t)kk_block_alloc(sizeof(struct kk_evv_vector_s) + (length-1)*sizeof(void*), length + 1 /* cfc */, KK_TAG_EVV_VECTOR, ctx);
  v->cfc = kk_integer_from_int32(cfc,ctx);
  return v;
}

static kk_std_core_hnd__ev* kk_evv_vector_buf(kk_evv_vector_t vec, size_t* len) {
  if (len != NULL) { *len = kk_block_scan_fsize(&vec->_block) - 1; }
  return &vec->vec[0];
}

static kk_std_core_hnd__ev* kk_evv_as_vec(kk_evv_t evv, size_t* len, kk_std_core_hnd__ev* single) {
  if (kk_evv_is_vector(evv)) {
    kk_evv_vector_t vec = kk_evv_as_vector(evv);
    *len = kk_block_scan_fsize(&vec->_block) - 1;
    return &vec->vec[0];
  }
  else {
    // single evidence
    *single = kk_evv_as_ev(evv);
    *len = 1;
    return single;
  }
}

kk_std_core_hnd__ev kk_ev_none(kk_context_t* ctx) {
  static kk_std_core_hnd__ev ev_none_singleton;
  if (ev_none_singleton==NULL) {
    ev_none_singleton = kk_std_core_hnd__new_Ev(
      kk_reuse_null,
      kk_std_core_hnd__new_Htag(kk_string_empty(),ctx), // tag ""
      kk_std_core_hnd__new_Marker(0,ctx),               // marker 0
      kk_box_null,                                      // no handler
      -1,                                               // bot
      kk_evv_empty(ctx),
      ctx
    );
  }
  return kk_std_core_hnd__ev_dup(ev_none_singleton);
}


size_t kk_evv_index( struct kk_std_core_hnd_Htag htag, kk_context_t* ctx ) {
  // todo: drop htag?
  size_t len;
  kk_std_core_hnd__ev single;
  kk_std_core_hnd__ev* vec = kk_evv_as_vec(ctx->evv,&len,&single);
  for(size_t i = 0; i < len; i++) {
    struct kk_std_core_hnd_Ev* ev = kk_std_core_hnd__as_Ev(vec[i]);
    if (kk_string_cmp_borrow(htag.tagname,ev->htag.tagname) <= 0) return i; // break on insertion point
  }
  //string_t evvs = kk_evv_show(dup_datatype_as(kk_evv_t,ctx->evv),ctx);
  //fatal_error(EFAULT,"cannot find tag '%s' in: %s", string_cbuf_borrow(htag.htag), string_cbuf_borrow(evvs));
  //drop_string_t(evvs,ctx);
  return len;
}

kk_std_core_hnd__ev kk_evv_lookup( struct kk_std_core_hnd_Htag htag, kk_context_t* ctx ) {
  // todo: drop htag
  size_t idx = kk_evv_index(htag,ctx);
  return kk_evv_at(idx,ctx);
}

static inline int32_t kk_cfc_lub(int32_t cfc1, int32_t cfc2) {
  if (cfc1 < 0) return cfc2;
  else if (cfc1+cfc2 == 1) return 2;
  else if (cfc1>cfc2) return cfc1;
  else return cfc2;
}

static inline struct kk_std_core_hnd_Ev* kk_evv_as_Ev( kk_evv_t evv ) {
  return kk_std_core_hnd__as_Ev(kk_evv_as_ev(evv));
}


static int32_t kk_evv_cfc_of_borrow(kk_evv_t evv, kk_context_t* ctx) {
  if (kk_evv_is_vector(evv)) {
    kk_evv_vector_t vec = kk_evv_as_vector(evv);
    return kk_integer_clamp32(vec->cfc,ctx);
  }
  else {
    struct kk_std_core_hnd_Ev* ev = kk_evv_as_Ev(evv);
    return ev->cfc;
  }
}

int32_t kk_evv_cfc(kk_context_t* ctx) {
  return kk_evv_cfc_of_borrow(ctx->evv,ctx);
}

static void kk_evv_update_cfc_borrow(kk_evv_t evv, int32_t cfc, kk_context_t* ctx) {
  kk_assert_internal(!kk_evv_is_empty(evv)); // should never happen (as named handlers are always in some context)
  if (kk_evv_is_vector(evv)) {
    kk_evv_vector_t vec = kk_evv_as_vector(evv);
    vec->cfc = kk_integer_from_int32(kk_cfc_lub(kk_integer_clamp32(vec->cfc,ctx),cfc), ctx);
  }
  else {
    struct kk_std_core_hnd_Ev* ev = kk_evv_as_Ev(evv);
    ev->cfc = kk_cfc_lub(ev->cfc,cfc);
  }
}

kk_evv_t kk_evv_insert(kk_evv_t evvd, kk_std_core_hnd__ev evd, kk_context_t* ctx) {
  struct kk_std_core_hnd_Ev* ev = kk_std_core_hnd__as_Ev(evd);
  // update ev with parent evidence vector (either at init, or due to non-scoped resumptions)
  int32_t marker = ev->marker.m;
  if (marker==0) { kk_std_core_hnd__ev_drop(evd,ctx); return evvd; } // ev-none
  kk_evv_drop(ev->hevv,ctx);
  ev->hevv = kk_evv_dup(evvd);
  if (marker<0) { // negative marker is used for named evidence; this means this evidence should not be inserted into the evidence vector
    kk_evv_update_cfc_borrow(evvd,ev->cfc,ctx); // update cfc in-place for named evidence
    kk_std_core_hnd__ev_drop(evd,ctx);
    return evvd;
  }
  // for regular handler evidence, insert ev
  size_t n;
  kk_std_core_hnd__ev single;
  kk_std_core_hnd__ev* const evv1 = kk_evv_as_vec(evvd, &n, &single);
  if (n == 0) {
    // use ev directly as the evidence vector
    kk_evv_drop(evvd, ctx);
    return &evd->_block;
  }
  else {
    // create evidence vector
    const int32_t cfc = kk_cfc_lub(kk_evv_cfc_of_borrow(evvd, ctx), ev->cfc);
    ev->cfc = cfc; // update in place
    kk_evv_vector_t vec2 = kk_evv_vector_alloc(n+1, cfc, ctx);
    kk_std_core_hnd__ev* const evv2 = kk_evv_vector_buf(vec2, NULL);
    size_t i;
    for (i = 0; i < n; i++) {
      struct kk_std_core_hnd_Ev* ev1 = kk_std_core_hnd__as_Ev(evv1[i]);
      if (kk_string_cmp_borrow(ev->htag.tagname, ev1->htag.tagname) <= 0) break;
      evv2[i] = kk_std_core_hnd__ev_dup(&ev1->_base);
    }
    evv2[i] = evd;
    for (; i < n; i++) {
      evv2[i+1] = kk_std_core_hnd__ev_dup(evv1[i]);
    }
    kk_evv_drop(evvd, ctx);  // assigned to evidence already
    return &vec2->_block;
  }
}

kk_evv_t kk_evv_delete(kk_evv_t evvd, size_t index, bool behind, kk_context_t* ctx) {
  size_t n;
  kk_std_core_hnd__ev single;
  const kk_std_core_hnd__ev* evv1 = kk_evv_as_vec(evvd, &n, &single);
  if (n <= 1) {
    kk_evv_drop(evvd,ctx);
    return kk_evv_total(ctx);
  }
  if (behind) index++;
  kk_assert_internal(index < n);
  // todo: copy without dupping (and later dropping) when possible
  const int32_t cfc1 = kk_evv_cfc_of_borrow(evvd,ctx);
  kk_evv_vector_t const vec2 = kk_evv_vector_alloc(n-1,cfc1,ctx);
  kk_std_core_hnd__ev* const evv2 = kk_evv_vector_buf(vec2,NULL);
  size_t i;
  for(i = 0; i < index; i++) {
    evv2[i] = kk_std_core_hnd__ev_dup(evv1[i]);
  }
  for(; i < n-1; i++) {
    evv2[i] = kk_std_core_hnd__ev_dup(evv1[i+1]);
  }
  struct kk_std_core_hnd_Ev* ev = kk_std_core_hnd__as_Ev(evv1[index]);
  if (ev->cfc >= cfc1) {
    int32_t cfc = kk_std_core_hnd__as_Ev(evv2[0])->cfc;
    for(i = 1; i < n-1; i++) {
      cfc = kk_cfc_lub(cfc,kk_std_core_hnd__as_Ev(evv2[i])->cfc);
    }
    vec2->cfc = kk_integer_from_int32(cfc,ctx);
  }
  kk_evv_drop(evvd,ctx);
  return &vec2->_block;
}

kk_evv_t kk_evv_create(kk_evv_t evv1, kk_vector_t indices, kk_context_t* ctx) {
  size_t len;
  kk_box_t* elems = kk_vector_buf(indices,&len); // borrows
  kk_evv_vector_t evv2 = kk_evv_vector_alloc(len,kk_evv_cfc_of_borrow(evv1,ctx),ctx);
  kk_std_core_hnd__ev* buf2 = kk_evv_vector_buf(evv2,NULL);
  kk_assert_internal(kk_evv_is_vector(evv1));
  size_t len1;
  kk_std_core_hnd__ev single;
  kk_std_core_hnd__ev* buf1 = kk_evv_as_vec(evv1,&len1,&single);
  for(size_t i = 0; i < len; i++) {
    size_t idx = kk_size_unbox(elems[i],ctx);
    kk_assert_internal(idx < len1);
    buf2[i] = kk_std_core_hnd__ev_dup( buf1[idx] );
  }
  kk_vector_drop(indices,ctx);
  kk_evv_drop(evv1,ctx);
  return &evv2->_block;
}

kk_evv_t kk_evv_swap_create( kk_vector_t indices, kk_context_t* ctx ) {
  size_t len;
  kk_box_t* vec = kk_vector_buf(indices,&len);
  if (len==0) {
    kk_vector_drop(indices,ctx);
    return kk_evv_swap_create0(ctx);
  }
  if (len==1) {
    size_t i = kk_size_unbox(vec[0],ctx);
    kk_vector_drop(indices,ctx);
    return kk_evv_swap_create1(i,ctx);
  }
  return kk_evv_swap( kk_evv_create(kk_evv_dup(ctx->evv),indices,ctx), ctx );
}


kk_string_t kk_evv_show(kk_evv_t evv, kk_context_t* ctx) {
  return kk_string_alloc_dup("(not yet implemented: kk_evv_show)",ctx);
}


/*-----------------------------------------------------------------------
  Compose continuations
-----------------------------------------------------------------------*/

struct kcompose_fun_s {
  struct kk_function_s _base;
  kk_box_t      count;
  kk_function_t conts[1];
};

// kleisli composition of continuations
static kk_box_t kcompose( kk_function_t fself, kk_box_t x, kk_context_t* ctx) {
  struct kcompose_fun_s* self = kk_function_as(struct kcompose_fun_s*,fself);
  kk_intx_t count = kk_intx_unbox(self->count);
  kk_function_t* conts = &self->conts[0];
  // call each continuation in order
  for(kk_intx_t i = 0; i < count; i++) {
    // todo: take uniqueness of fself into account to avoid dup_function
    kk_function_t f = kk_function_dup(conts[i]);
    x = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, x, ctx));
    if (kk_yielding(ctx)) {
      // if yielding, `yield_next` all continuations that still need to be done
      while(++i < count) {
        // todo: if fself is unique, we could copy without dup?
        kk_yield_extend(kk_function_dup(conts[i]),ctx);
      }
      kk_function_drop(fself,ctx);
      // kk_box_drop(x,ctx);  // don't drop as we were yielding
      return kk_box_any(ctx); // return yielding
    }
  }
  kk_function_drop(fself,ctx);
  return x;
}

static kk_function_t new_kcompose( kk_function_t* conts, size_t count, kk_context_t* ctx ) {
  if (count==0) return kk_function_id(ctx);
  if (count==1) return conts[0];
  struct kcompose_fun_s* f = kk_block_as(struct kcompose_fun_s*,
                               kk_block_alloc(sizeof(struct kcompose_fun_s) - sizeof(kk_function_t) + (count*sizeof(kk_function_t)),
                                 2 + count /* scan size */, KK_TAG_FUNCTION, ctx));
  f->_base.fun = kk_cfun_ptr_box(&kcompose,ctx);
  f->count = kk_enum_box(count);
  memcpy(f->conts, conts, count * sizeof(kk_function_t));
  return (&f->_base);
}

/*-----------------------------------------------------------------------
  Yield extension
-----------------------------------------------------------------------*/

kk_box_t kk_yield_extend( kk_function_t next, kk_context_t* ctx ) {
  kk_yield_t* yield = &ctx->yield;
  kk_assert_internal(kk_yielding(ctx));  // cannot extend if not yielding
  if (kk_unlikely(kk_yielding_final(ctx))) {
    // todo: can we optimize this so `next` is never allocated in the first place?
    kk_function_drop(next,ctx); // ignore extension if never resuming
  }
  else {
    if (kk_unlikely(yield->conts_count >= KK_YIELD_CONT_MAX)) {
      // alloc a function to compose all continuations in the array
      kk_function_t comp = new_kcompose( yield->conts, yield->conts_count, ctx );
      yield->conts[0] = comp;
      yield->conts_count = 1;
    }
    yield->conts[yield->conts_count++] = next;
  }
  return kk_box_any(ctx);
}

// cont_apply: \x -> f(cont,x)
struct cont_apply_fun_s {
  struct kk_function_s _base;
  kk_function_t f;
  kk_function_t cont;
};

static kk_box_t cont_apply( kk_function_t fself, kk_box_t x, kk_context_t* ctx ) {
  struct cont_apply_fun_s* self = kk_function_as(struct cont_apply_fun_s*, fself);
  kk_function_t f = self->f;
  kk_function_t cont = self->cont;
  kk_drop_match(self,{kk_function_dup(f);kk_function_dup(cont);},{},ctx);
  return kk_function_call( kk_box_t, (kk_function_t, kk_function_t, kk_box_t, kk_context_t* ctx), f, (f, cont, x, ctx));
}

static kk_function_t kk_new_cont_apply( kk_function_t f, kk_function_t cont, kk_context_t* ctx ) {
  struct cont_apply_fun_s* self = kk_function_alloc_as(struct cont_apply_fun_s, 3, ctx);
  self->_base.fun = kk_cfun_ptr_box(&cont_apply,ctx);
  self->f = f;
  self->cont = cont;
  return (&self->_base);
}

// Unlike `yield_extend`, `yield_cont` gets access to the current continuation. This is used in `yield_prompt`.
kk_box_t kk_yield_cont( kk_function_t f, kk_context_t* ctx ) {
  kk_yield_t* yield = &ctx->yield;
  kk_assert_internal(kk_yielding(ctx)); // cannot extend if not yielding
  if (kk_unlikely(kk_yielding_final(ctx))) {
    kk_function_drop(f,ctx); // ignore extension if never resuming
  }
  else {
    kk_function_t cont = new_kcompose(yield->conts, yield->conts_count, ctx);
    yield->conts_count = 1;
    yield->conts[0] = kk_new_cont_apply(f, cont, ctx);
  }
  return kk_box_any(ctx);
}

kk_function_t kk_yield_to( struct kk_std_core_hnd_Marker m, kk_function_t clause, kk_context_t* ctx ) {
  kk_yield_t* yield = &ctx->yield;
  kk_assert_internal(!kk_yielding(ctx)); // already yielding
  ctx->yielding = KK_YIELD_NORMAL;
  yield->marker = m.m;
  yield->clause = clause;
  yield->conts_count = 0;
  return kk_basetype_unbox_as(kk_function_t,kk_box_any(ctx));
}

kk_box_t kk_yield_final( struct kk_std_core_hnd_Marker m, kk_function_t clause, kk_context_t* ctx ) {
  kk_yield_to(m,clause,ctx);
  ctx->yielding = KK_YIELD_FINAL;
  return kk_box_any(ctx);
}

kk_box_t kk_fatal_resume_final(kk_context_t* ctx) {
  kk_fatal_error(EFAULT,"trying to resume a finalized resumption");
  return kk_box_any(ctx);
}

static kk_box_t _fatal_resume_final(kk_function_t self, kk_context_t* ctx) {
  kk_function_drop(self,ctx);
  return kk_fatal_resume_final(ctx);
}
static kk_function_t fun_fatal_resume_final(kk_context_t* ctx) {
  kk_define_static_function(f,_fatal_resume_final,ctx);
  return kk_function_dup(f);
}


struct kk_std_core_hnd_yld_s kk_yield_prompt( struct kk_std_core_hnd_Marker m, kk_context_t* ctx ) {
  kk_yield_t* yield = &ctx->yield;
  if (ctx->yielding == KK_YIELD_NONE) {
    return kk_std_core_hnd__new_Pure(ctx);
  }
  else if (yield->marker != m.m) {
    return (ctx->yielding == KK_YIELD_FINAL ? kk_std_core_hnd__new_YieldingFinal(ctx) : kk_std_core_hnd__new_Yielding(ctx));
  }
  else {
    kk_function_t cont = (ctx->yielding == KK_YIELD_FINAL ? fun_fatal_resume_final(ctx) : new_kcompose(yield->conts, yield->conts_count, ctx));
    kk_function_t clause = yield->clause;
    ctx->yielding = KK_YIELD_NONE;
    #ifndef NDEBUG
    memset(yield,0,sizeof(kk_yield_t));
    #endif
    return kk_std_core_hnd__new_Yield(clause, cont, ctx);
  }
}

kk_unit_t  kk_evv_guard(kk_evv_t evv, kk_context_t* ctx) {
  bool eq = (ctx->evv == evv);
  kk_evv_drop(evv,ctx);
  if (!eq) {
    // todo: improve error message with diagnostics
    kk_fatal_error(EFAULT,"trying to resume outside the (handler) scope of the original handler");
  }
  return kk_Unit;
}

typedef struct yield_info_s {
  struct kk_std_core_hnd__yield_info_s _base;
  kk_function_t clause;
  kk_function_t conts[KK_YIELD_CONT_MAX];
  size_t     conts_count;
  int32_t    marker;
  uint8_t    yielding;
}* yield_info_t;

kk_std_core_hnd__yield_info kk_yield_capture(kk_context_t* ctx) {
  kk_assert_internal(kk_yielding(ctx));
  yield_info_t yld = kk_block_alloc_as(struct yield_info_s, 1 + KK_YIELD_CONT_MAX, (kk_tag_t)1, ctx);
  yld->clause = ctx->yield.clause;
  size_t i = 0;
  for( ; i < ctx->yield.conts_count; i++) {
    yld->conts[i] = ctx->yield.conts[i];
  }
  for( ; i < KK_YIELD_CONT_MAX; i++) {
    yld->conts[i] = kk_function_null(ctx);
  }
  yld->conts_count = ctx->yield.conts_count;
  yld->marker = ctx->yield.marker;
  yld->yielding = ctx->yielding;
  ctx->yielding = 0;
  ctx->yield.conts_count = 0;
  return kk_datatype_from_base(&yld->_base);
}

kk_box_t kk_yield_reyield( kk_std_core_hnd__yield_info yldinfo, kk_context_t* ctx) {
  kk_assert_internal(!kk_yielding(ctx));
  yield_info_t yld = kk_datatype_as_assert(yield_info_t, yldinfo, (kk_tag_t)1);
  ctx->yield.clause = kk_function_dup(yld->clause);
  ctx->yield.marker = yld->marker;
  ctx->yield.conts_count = yld->conts_count;
  ctx->yielding = yld->yielding;
  for(size_t i = 0; i < yld->conts_count; i++) {
    ctx->yield.conts[i] = kk_function_dup(yld->conts[i]);
  }
  kk_constructor_drop(yld,ctx);
  return kk_box_any(ctx);
}


kk_std_core_hnd__ev kk_std_core_hnd__copy_2(kk_std_core_hnd__ev _this, kk_std_core_types__optional htag0, kk_std_core_hnd__marker marker, kk_box_t hnd, kk_std_core_types__optional cfc0, kk_evv_t hevv, kk_context_t* _ctx) { /* forall<a,e,b> (ev<a>, htag : optional<htag<a>>, marker : marker<e,b>, hnd : a<e,b>, cfc : optional<cfc>, hevv : evv<e>) -> ev<a> */ 
  kk_std_core_hnd__htag _x10506;
  if (kk_std_core_types__is_Optional(htag0)) {
    kk_box_t _box_x10223 = htag0._cons.Optional.value;
    kk_std_core_hnd__htag _htag_2001 = kk_std_core_hnd__htag_unbox(_box_x10223, NULL);
    _x10506 = _htag_2001; /*std/core/hnd/htag<2027>*/
    goto _match10507;
  }
  struct kk_std_core_hnd_Ev* _con10509 = kk_std_core_hnd__as_Ev(_this);
  kk_std_core_hnd__htag _x = _con10509->htag;
  kk_std_core_hnd__htag_dup(_x);
  _x10506 = _x; /*std/core/hnd/htag<2027>*/
  _match10507: ;
  int32_t _x10510;
  if (kk_std_core_types__is_Optional(cfc0)) {
    kk_box_t _box_x10224 = cfc0._cons.Optional.value;
    int32_t _cfc_2008 = kk_int32_unbox(_box_x10224, NULL);
    kk_std_core_hnd__ev_dropn(_this, ((int32_t)3), _ctx);
    _x10510 = _cfc_2008; /*std/core/hnd/cfc*/
    goto _match10511;
  }
  struct kk_std_core_hnd_Ev* _con10513 = kk_std_core_hnd__as_Ev(_this);
  kk_std_core_hnd__htag _pat01 = _con10513->htag;
  kk_box_t _pat21 = _con10513->hnd;
  int32_t _x0 = _con10513->cfc;
  kk_evv_t _pat30 = _con10513->hevv;
  if (kk_std_core_hnd__ev_is_unique(_this)) {
    kk_evv_drop(_pat30, _ctx);
    kk_box_drop(_pat21, _ctx);
    kk_std_core_hnd__htag_drop(_pat01, _ctx);
    kk_std_core_hnd__ev_free(_this);
  }
  else {
    kk_std_core_hnd__ev_decref(_this, _ctx);
  }
  _x10510 = _x0; /*std/core/hnd/cfc*/
  _match10511: ;
  return kk_std_core_hnd__new_Ev(kk_reuse_null, _x10506, marker, hnd, _x10510, hevv, _ctx);
}


// lift anonymous function
struct kk_std_core_hnd__copy_fun10515__t_3 {
  struct kk_function_s _base;
  kk_box_t _fun_unbox_x10230;
};
static kk_box_t kk_std_core_hnd__copy_fun10515_3(kk_function_t _fself, kk_std_core_hnd__marker _b_10234, kk_std_core_hnd__ev _b_10235, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd__new_copy_fun10515_3(kk_box_t _fun_unbox_x10230, kk_context_t* _ctx) {
  struct kk_std_core_hnd__copy_fun10515__t_3* _self = kk_function_alloc_as(struct kk_std_core_hnd__copy_fun10515__t_3, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd__copy_fun10515_3, kk_context());
  _self->_fun_unbox_x10230 = _fun_unbox_x10230;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd__copy_fun10515_3(kk_function_t _fself, kk_std_core_hnd__marker _b_10234, kk_std_core_hnd__ev _b_10235, kk_context_t* _ctx) {
  struct kk_std_core_hnd__copy_fun10515__t_3* _self = kk_function_as(struct kk_std_core_hnd__copy_fun10515__t_3*, _fself);
  kk_box_t _fun_unbox_x10230 = _self->_fun_unbox_x10230; /* 108 */
  kk_drop_match(_self, {kk_box_dup(_fun_unbox_x10230);}, {}, _ctx)
  kk_function_t _x10516 = kk_function_unbox(_fun_unbox_x10230); /*(10231, 10232) -> 2124 10233*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_context_t*), _x10516, (_x10516, kk_std_core_hnd__marker_box(_b_10234, _ctx), kk_std_core_hnd__ev_box(_b_10235, _ctx), _ctx));
}

kk_std_core_hnd__clause0 kk_std_core_hnd__copy_3(kk_std_core_hnd__clause0 _this, kk_std_core_types__optional clause0, kk_context_t* _ctx) { /* forall<a,b,e,c> (clause0<a,b,e,c>, clause : optional<(marker<e,c>, ev<b>) -> e a>) -> clause0<a,b,e,c> */ 
  kk_function_t _x10514;
  if (kk_std_core_types__is_Optional(clause0)) {
    kk_box_t _fun_unbox_x10230 = clause0._cons.Optional.value;
    kk_std_core_hnd__clause0_drop(_this, _ctx);
    _x10514 = kk_std_core_hnd__new_copy_fun10515_3(_fun_unbox_x10230, _ctx); /*(std/core/hnd/marker<2124,2125>, std/core/hnd/ev<2123>) -> 2124 10233*/
  }
  else {
    kk_function_t _x = _this.clause;
    _x10514 = _x; /*(std/core/hnd/marker<2124,2125>, std/core/hnd/ev<2123>) -> 2124 10233*/
  }
  return kk_std_core_hnd__new_Clause0(_x10514, _ctx);
}


// lift anonymous function
struct kk_std_core_hnd__copy_fun10518__t_4 {
  struct kk_function_s _base;
  kk_box_t _fun_unbox_x10243;
};
static kk_box_t kk_std_core_hnd__copy_fun10518_4(kk_function_t _fself, kk_std_core_hnd__marker _b_10248, kk_std_core_hnd__ev _b_10249, kk_box_t _b_10250, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd__new_copy_fun10518_4(kk_box_t _fun_unbox_x10243, kk_context_t* _ctx) {
  struct kk_std_core_hnd__copy_fun10518__t_4* _self = kk_function_alloc_as(struct kk_std_core_hnd__copy_fun10518__t_4, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd__copy_fun10518_4, kk_context());
  _self->_fun_unbox_x10243 = _fun_unbox_x10243;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd__copy_fun10518_4(kk_function_t _fself, kk_std_core_hnd__marker _b_10248, kk_std_core_hnd__ev _b_10249, kk_box_t _b_10250, kk_context_t* _ctx) {
  struct kk_std_core_hnd__copy_fun10518__t_4* _self = kk_function_as(struct kk_std_core_hnd__copy_fun10518__t_4*, _fself);
  kk_box_t _fun_unbox_x10243 = _self->_fun_unbox_x10243; /* 108 */
  kk_drop_match(_self, {kk_box_dup(_fun_unbox_x10243);}, {}, _ctx)
  kk_function_t _x10519 = kk_function_unbox(_fun_unbox_x10243); /*(10244, 10245, 10246) -> 2342 10247*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_box_t, kk_context_t*), _x10519, (_x10519, kk_std_core_hnd__marker_box(_b_10248, _ctx), kk_std_core_hnd__ev_box(_b_10249, _ctx), _b_10250, _ctx));
}

kk_std_core_hnd__clause1 kk_std_core_hnd__copy_4(kk_std_core_hnd__clause1 _this, kk_std_core_types__optional clause0, kk_context_t* _ctx) { /* forall<a,b,c,e,d> (clause1<a,b,c,e,d>, clause : optional<(marker<e,d>, ev<c>, a) -> e b>) -> clause1<a,b,c,e,d> */ 
  kk_function_t _x10517;
  if (kk_std_core_types__is_Optional(clause0)) {
    kk_box_t _fun_unbox_x10243 = clause0._cons.Optional.value;
    kk_std_core_hnd__clause1_drop(_this, _ctx);
    _x10517 = kk_std_core_hnd__new_copy_fun10518_4(_fun_unbox_x10243, _ctx); /*(std/core/hnd/marker<2342,2343>, std/core/hnd/ev<2341>, 2339) -> 2342 10247*/
  }
  else {
    kk_function_t _x = _this.clause;
    _x10517 = _x; /*(std/core/hnd/marker<2342,2343>, std/core/hnd/ev<2341>, 2339) -> 2342 10247*/
  }
  return kk_std_core_hnd__new_Clause1(_x10517, _ctx);
}


// lift anonymous function
struct kk_std_core_hnd__copy_fun10521__t_5 {
  struct kk_function_s _base;
  kk_box_t _fun_unbox_x10260;
};
static kk_box_t kk_std_core_hnd__copy_fun10521_5(kk_function_t _fself, kk_std_core_hnd__marker _b_10266, kk_std_core_hnd__ev _b_10267, kk_box_t _b_10268, kk_box_t _b_10269, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd__new_copy_fun10521_5(kk_box_t _fun_unbox_x10260, kk_context_t* _ctx) {
  struct kk_std_core_hnd__copy_fun10521__t_5* _self = kk_function_alloc_as(struct kk_std_core_hnd__copy_fun10521__t_5, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd__copy_fun10521_5, kk_context());
  _self->_fun_unbox_x10260 = _fun_unbox_x10260;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd__copy_fun10521_5(kk_function_t _fself, kk_std_core_hnd__marker _b_10266, kk_std_core_hnd__ev _b_10267, kk_box_t _b_10268, kk_box_t _b_10269, kk_context_t* _ctx) {
  struct kk_std_core_hnd__copy_fun10521__t_5* _self = kk_function_as(struct kk_std_core_hnd__copy_fun10521__t_5*, _fself);
  kk_box_t _fun_unbox_x10260 = _self->_fun_unbox_x10260; /* 108 */
  kk_drop_match(_self, {kk_box_dup(_fun_unbox_x10260);}, {}, _ctx)
  kk_function_t _x10522 = kk_function_unbox(_fun_unbox_x10260); /*(10261, 10262, 10263, 10264) -> 2661 10265*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_box_t, kk_box_t, kk_context_t*), _x10522, (_x10522, kk_std_core_hnd__marker_box(_b_10266, _ctx), kk_std_core_hnd__ev_box(_b_10267, _ctx), _b_10268, _b_10269, _ctx));
}

kk_std_core_hnd__clause2 kk_std_core_hnd__copy_5(kk_std_core_hnd__clause2 _this, kk_std_core_types__optional clause0, kk_context_t* _ctx) { /* forall<a,b,c,d,e,a1> (clause2<a,b,c,d,e,a1>, clause : optional<(marker<e,a1>, ev<d>, a, b) -> e c>) -> clause2<a,b,c,d,e,a1> */ 
  kk_function_t _x10520;
  if (kk_std_core_types__is_Optional(clause0)) {
    kk_box_t _fun_unbox_x10260 = clause0._cons.Optional.value;
    kk_std_core_hnd__clause2_drop(_this, _ctx);
    _x10520 = kk_std_core_hnd__new_copy_fun10521_5(_fun_unbox_x10260, _ctx); /*(std/core/hnd/marker<2661,2662>, std/core/hnd/ev<2660>, 2657, 2658) -> 2661 10265*/
  }
  else {
    kk_function_t _x = _this.clause;
    _x10520 = _x; /*(std/core/hnd/marker<2661,2662>, std/core/hnd/ev<2660>, 2657, 2658) -> 2661 10265*/
  }
  return kk_std_core_hnd__new_Clause2(_x10520, _ctx);
}


// lift anonymous function
struct kk_std_core_hnd__copy_fun10524__t_6 {
  struct kk_function_s _base;
  kk_box_t _fun_unbox_x10274;
};
static kk_box_t kk_std_core_hnd__copy_fun10524_6(kk_function_t _fself, kk_function_t _b_10277, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd__new_copy_fun10524_6(kk_box_t _fun_unbox_x10274, kk_context_t* _ctx) {
  struct kk_std_core_hnd__copy_fun10524__t_6* _self = kk_function_alloc_as(struct kk_std_core_hnd__copy_fun10524__t_6, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd__copy_fun10524_6, kk_context());
  _self->_fun_unbox_x10274 = _fun_unbox_x10274;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd__copy_fun10524_6(kk_function_t _fself, kk_function_t _b_10277, kk_context_t* _ctx) {
  struct kk_std_core_hnd__copy_fun10524__t_6* _self = kk_function_as(struct kk_std_core_hnd__copy_fun10524__t_6*, _fself);
  kk_box_t _fun_unbox_x10274 = _self->_fun_unbox_x10274; /* 108 */
  kk_drop_match(_self, {kk_box_dup(_fun_unbox_x10274);}, {}, _ctx)
  kk_function_t _x10525 = kk_function_unbox(_fun_unbox_x10274); /*(10275) -> 2836 10276*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x10525, (_x10525, kk_function_box(_b_10277), _ctx));
}

kk_std_core_hnd__resume_context kk_std_core_hnd__copy_6(kk_std_core_hnd__resume_context _this, kk_std_core_types__optional k0, kk_context_t* _ctx) { /* forall<a,e,b> (resume-context<a,e,b>, k : optional<(() -> a) -> e b>) -> resume-context<a,e,b> */ 
  kk_function_t _x10523;
  if (kk_std_core_types__is_Optional(k0)) {
    kk_box_t _fun_unbox_x10274 = k0._cons.Optional.value;
    kk_std_core_hnd__resume_context_drop(_this, _ctx);
    _x10523 = kk_std_core_hnd__new_copy_fun10524_6(_fun_unbox_x10274, _ctx); /*(() -> 2835) -> 2836 10276*/
  }
  else {
    kk_function_t _x = _this.k;
    _x10523 = _x; /*(() -> 2835) -> 2836 10276*/
  }
  return kk_std_core_hnd__new_Resume_context(_x10523, _ctx);
}
 
// (dynamically) find evidence insertion/deletion index in the evidence vector

size_t kk_std_core_hnd__evv_index(kk_std_core_hnd__htag htag0, kk_context_t* _ctx) { /* forall<e,a> (htag : htag<a>) -> e ev-index */ 
  return kk_evv_index(htag0,kk_context());
}

bool kk_std_core_hnd__evv_is_affine(kk_context_t* _ctx) { /* () -> bool */ 
  return kk_evv_cfc(kk_context())<=2;
}

kk_std_core_hnd__ev kk_std_core_hnd__evv_lookup(kk_std_core_hnd__htag htag0, kk_context_t* _ctx) { /* forall<a> (htag : htag<a>) -> ev<a> */ 
  return kk_evv_lookup(htag0,kk_context());
}

kk_evv_t kk_std_core_hnd_evv_get(kk_context_t* _ctx) { /* forall<e> () -> e evv<e> */ 
  return kk_evv_get(kk_context());
}

kk_evv_t kk_std_core_hnd_evv_insert(kk_evv_t evv, kk_std_core_hnd__ev ev, kk_context_t* _ctx) { /* forall<e,e1,a> (evv : evv<e>, ev : ev<a>) -> e evv<e1> */ 
  return kk_evv_insert(evv,ev,kk_context());
}

int32_t kk_std_core_hnd_fresh_marker_int(kk_context_t* _ctx) { /* () -> int32 */ 
  return kk_marker_unique(kk_context());
}

bool kk_std_core_hnd_evv_eq(kk_evv_t evv0, kk_evv_t evv1, kk_context_t* _ctx) { /* forall<e> (evv0 : evv<e>, evv1 : evv<e>) -> bool */ 
  return kk_evv_eq(evv0,evv1,kk_context());
}

kk_unit_t kk_std_core_hnd_guard(kk_evv_t w, kk_context_t* _ctx) { /* forall<e> (w : evv<e>) -> e () */ 
  kk_evv_guard(w,kk_context()); return kk_Unit;
}

kk_box_t kk_std_core_hnd_yield_cont(kk_function_t f, kk_context_t* _ctx) { /* forall<a,e,b> (f : ((a) -> e a, a) -> e b) -> e b */ 
  return kk_yield_cont(f,kk_context());
}

kk_std_core_hnd__yld kk_std_core_hnd_yield_prompt(kk_std_core_hnd__marker m0, kk_context_t* _ctx) { /* forall<a,e,b> (m : marker<e,b>) -> yld<e,a,b> */ 
  return kk_yield_prompt(m0,kk_context());
}

kk_evv_t kk_std_core_hnd_evv_swap_delete(size_t i, bool behind, kk_context_t* _ctx) { /* forall<e,e1> (i : ev-index, behind : bool) -> e1 evv<e> */ 
  return kk_evv_swap_delete(i,behind,kk_context());
}

int32_t kk_std_core_hnd_fresh_marker_named_int(kk_context_t* _ctx) { /* () -> int32 */ 
  return -kk_marker_unique(kk_context());
}

kk_evv_t kk_std_core_hnd_evv_swap_create(kk_vector_t indices, kk_context_t* _ctx) { /* forall<e> (indices : vector<ev-index>) -> e evv<e> */ 
  return kk_evv_swap_create(indices,kk_context());
}

kk_box_t kk_std_core_hnd_yield_extend(kk_function_t next, kk_context_t* _ctx) { /* forall<a,b,e> (next : (a) -> e b) -> e b */ 
  return kk_yield_extend(next,kk_context());
}

kk_function_t kk_std_core_hnd_yield_to_prim(kk_std_core_hnd__marker m0, kk_function_t clause0, kk_context_t* _ctx) { /* forall<a,e,e1,b> (m : marker<e1,b>, clause : ((() -> a) -> e1 b) -> e1 b) -> e (() -> a) */ 
  return kk_yield_to(m0,clause0,kk_context());
}

kk_box_t kk_std_core_hnd_yield_to_final(kk_std_core_hnd__marker m0, kk_function_t clause0, kk_context_t* _ctx) { /* forall<a,e,e1,b> (m : marker<e1,b>, clause : ((() -> a) -> e1 b) -> e1 b) -> e a */ 
  return kk_yield_final(m0,clause0,kk_context());
}
extern kk_box_t kk_std_core_hnd_clause_tail_noyield0_fun10530(kk_function_t _fself, kk_std_core_hnd__marker __m, kk_std_core_hnd__ev __ev, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail_noyield0_fun10530__t* _self = kk_function_as(struct kk_std_core_hnd_clause_tail_noyield0_fun10530__t*, _fself);
  kk_function_t op = _self->op; /* () -> 4064 4066 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(__ev, ((int32_t)3), _ctx);
  return kk_function_call(kk_box_t, (kk_function_t, kk_context_t*), op, (op, _ctx));
}
extern kk_box_t kk_std_core_hnd_clause_tail_noyield1_fun10531(kk_function_t _fself, kk_std_core_hnd__marker __m, kk_std_core_hnd__ev __ev, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail_noyield1_fun10531__t* _self = kk_function_as(struct kk_std_core_hnd_clause_tail_noyield1_fun10531__t*, _fself);
  kk_function_t op = _self->op; /* (4097) -> 4095 4098 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(__ev, ((int32_t)3), _ctx);
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), op, (op, x, _ctx));
}
extern kk_box_t kk_std_core_hnd_clause_tail_noyield2_fun10532(kk_function_t _fself, kk_std_core_hnd__marker __m, kk_std_core_hnd__ev __ev, kk_box_t x1, kk_box_t x2, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail_noyield2_fun10532__t* _self = kk_function_as(struct kk_std_core_hnd_clause_tail_noyield2_fun10532__t*, _fself);
  kk_function_t op = _self->op; /* (4134, 4135) -> 4132 4136 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(__ev, ((int32_t)3), _ctx);
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_context_t*), op, (op, x1, x2, _ctx));
}

kk_evv_t kk_std_core_hnd_evv_swap_with(kk_std_core_hnd__ev ev, kk_context_t* _ctx) { /* forall<a,e> (ev : ev<a>) -> evv<e> */ 
  kk_evv_t _x10533;
  struct kk_std_core_hnd_Ev* _con10534 = kk_std_core_hnd__as_Ev(ev);
  kk_std_core_hnd__htag _pat0 = _con10534->htag;
  kk_box_t _pat2 = _con10534->hnd;
  kk_evv_t w = _con10534->hevv;
  if (kk_std_core_hnd__ev_is_unique(ev)) {
    kk_box_drop(_pat2, _ctx);
    kk_std_core_hnd__htag_drop(_pat0, _ctx);
    kk_std_core_hnd__ev_free(ev);
  }
  else {
    kk_evv_dup(w);
    kk_std_core_hnd__ev_decref(ev, _ctx);
  }
  _x10533 = w; /*std/core/hnd/evv<4141>*/
  return kk_evv_swap(_x10533,kk_context());
}
extern kk_box_t kk_std_core_hnd_clause_value_fun10535(kk_function_t _fself, kk_std_core_hnd__marker __m, kk_std_core_hnd__ev __ev, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_value_fun10535__t* _self = kk_function_as(struct kk_std_core_hnd_clause_value_fun10535__t*, _fself);
  kk_box_t v = _self->v; /* 4185 */
  kk_drop_match(_self, {kk_box_dup(v);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(__ev, ((int32_t)3), _ctx);
  return v;
}

kk_string_t kk_std_core_hnd_evv_show(kk_evv_t evv, kk_context_t* _ctx) { /* forall<e> (evv : evv<e>) -> string */ 
  return kk_evv_show(evv,kk_context());
}

kk_box_t kk_std_core_hnd_unsafe_reyield(kk_std_core_hnd__yield_info yld, kk_context_t* _ctx) { /* forall<a,e> (yld : yield-info) -> e a */ 
  return kk_yield_reyield(yld,kk_context());
}

kk_std_core_hnd__yield_info kk_std_core_hnd_yield_capture(kk_context_t* _ctx) { /* forall<e> () -> e yield-info */ 
  return kk_yield_capture(kk_context());
}
extern kk_box_t kk_std_core_hnd_resume_fun10536(kk_function_t _fself, kk_context_t* _ctx) {
  struct kk_std_core_hnd_resume_fun10536__t* _self = kk_function_as(struct kk_std_core_hnd_resume_fun10536__t*, _fself);
  kk_box_t x = _self->x; /* 4296 */
  kk_drop_match(_self, {kk_box_dup(x);}, {}, _ctx)
  return x;
}

kk_box_t kk_std_core_hnd_resume_final(kk_context_t* _ctx) { /* forall<a> () -> a */ 
  return kk_fatal_resume_final(kk_context());
}


// lift anonymous function
struct kk_std_core_hnd_prompt_fun10541__t {
  struct kk_function_s _base;
  kk_std_core_hnd__ev ev;
  kk_function_t ret;
  kk_evv_t w0;
  kk_evv_t w1;
  kk_std_core_hnd__marker m0;
};
static kk_box_t kk_std_core_hnd_prompt_fun10541(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_prompt_fun10541(kk_std_core_hnd__ev ev, kk_function_t ret, kk_evv_t w0, kk_evv_t w1, kk_std_core_hnd__marker m0, kk_context_t* _ctx) {
  struct kk_std_core_hnd_prompt_fun10541__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_prompt_fun10541__t, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_prompt_fun10541, kk_context());
  _self->ev = ev;
  _self->ret = ret;
  _self->w0 = w0;
  _self->w1 = w1;
  _self->m0 = m0;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_prompt_fun10541(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd_prompt_fun10541__t* _self = kk_function_as(struct kk_std_core_hnd_prompt_fun10541__t*, _fself);
  kk_std_core_hnd__ev ev = _self->ev; /* std/core/hnd/ev<4465> */
  kk_function_t ret = _self->ret; /* (4463) -> 4464 4466 */
  kk_evv_t w0 = _self->w0; /* std/core/hnd/evv<4464> */
  kk_evv_t w1 = _self->w1; /* std/core/hnd/evv<4464> */
  kk_std_core_hnd__marker m0 = _self->m0; /* std/core/hnd/marker<4464,4466> */
  kk_drop_match(_self, {kk_std_core_hnd__ev_dup(ev);kk_function_dup(ret);kk_evv_dup(w0);kk_evv_dup(w1);kk_std_core_hnd__marker_dup(m0);}, {}, _ctx)
  kk_evv_t w0_sq_ = kk_std_core_hnd_evv_get(_ctx); /*std/core/hnd/evv<4464>*/;
  kk_evv_t w1_sq_;
  bool _match_10493;
  kk_evv_t _x10542 = kk_evv_dup(w0_sq_); /*std/core/hnd/evv<4464>*/
  _match_10493 = kk_std_core_hnd_evv_eq(w0, _x10542, _ctx); /*bool*/
  if (_match_10493) {
    w1_sq_ = w1; /*std/core/hnd/evv<4464>*/
  }
  else {
    kk_evv_drop(w1, _ctx);
    kk_evv_t _x10543 = kk_evv_dup(w0_sq_); /*std/core/hnd/evv<4464>*/
    kk_std_core_hnd__ev _x10544 = kk_std_core_hnd__ev_dup(ev); /*std/core/hnd/ev<4465>*/
    w1_sq_ = kk_std_core_hnd_evv_insert(_x10543, _x10544, _ctx); /*std/core/hnd/evv<4464>*/
  }
  kk_unit_t __1 = kk_Unit;
  kk_evv_t _x10545 = kk_evv_dup(w1_sq_); /*std/core/hnd/evv<4464>*/
  kk_evv_set(_x10545,kk_context());
  kk_box_t _x10546 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), cont, (cont, res, _ctx)); /*4463*/
  return kk_std_core_hnd_prompt(w0_sq_, w1_sq_, ev, m0, ret, _x10546, _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_prompt_fun10547__t {
  struct kk_function_s _base;
  kk_function_t cont0;
  kk_std_core_hnd__ev ev;
  kk_function_t ret;
  kk_evv_t w0;
  kk_evv_t w1;
  kk_std_core_hnd__marker m0;
};
static kk_box_t kk_std_core_hnd_prompt_fun10547(kk_function_t _fself, kk_function_t f, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_prompt_fun10547(kk_function_t cont0, kk_std_core_hnd__ev ev, kk_function_t ret, kk_evv_t w0, kk_evv_t w1, kk_std_core_hnd__marker m0, kk_context_t* _ctx) {
  struct kk_std_core_hnd_prompt_fun10547__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_prompt_fun10547__t, 6, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_prompt_fun10547, kk_context());
  _self->cont0 = cont0;
  _self->ev = ev;
  _self->ret = ret;
  _self->w0 = w0;
  _self->w1 = w1;
  _self->m0 = m0;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_prompt_fun10547(kk_function_t _fself, kk_function_t f, kk_context_t* _ctx) {
  struct kk_std_core_hnd_prompt_fun10547__t* _self = kk_function_as(struct kk_std_core_hnd_prompt_fun10547__t*, _fself);
  kk_function_t cont0 = _self->cont0; /* (() -> 4400) -> 4464 4463 */
  kk_std_core_hnd__ev ev = _self->ev; /* std/core/hnd/ev<4465> */
  kk_function_t ret = _self->ret; /* (4463) -> 4464 4466 */
  kk_evv_t w0 = _self->w0; /* std/core/hnd/evv<4464> */
  kk_evv_t w1 = _self->w1; /* std/core/hnd/evv<4464> */
  kk_std_core_hnd__marker m0 = _self->m0; /* std/core/hnd/marker<4464,4466> */
  kk_drop_match(_self, {kk_function_dup(cont0);kk_std_core_hnd__ev_dup(ev);kk_function_dup(ret);kk_evv_dup(w0);kk_evv_dup(w1);kk_std_core_hnd__marker_dup(m0);}, {}, _ctx)
  kk_evv_t w00_sq_ = kk_std_core_hnd_evv_get(_ctx); /*std/core/hnd/evv<4464>*/;
  kk_evv_t w10_sq_;
  bool _match_10492;
  kk_evv_t _x10548 = kk_evv_dup(w00_sq_); /*std/core/hnd/evv<4464>*/
  _match_10492 = kk_std_core_hnd_evv_eq(w0, _x10548, _ctx); /*bool*/
  if (_match_10492) {
    w10_sq_ = w1; /*std/core/hnd/evv<4464>*/
  }
  else {
    kk_evv_drop(w1, _ctx);
    kk_evv_t _x10549 = kk_evv_dup(w00_sq_); /*std/core/hnd/evv<4464>*/
    kk_std_core_hnd__ev _x10550 = kk_std_core_hnd__ev_dup(ev); /*std/core/hnd/ev<4465>*/
    w10_sq_ = kk_std_core_hnd_evv_insert(_x10549, _x10550, _ctx); /*std/core/hnd/evv<4464>*/
  }
  kk_unit_t __2 = kk_Unit;
  kk_evv_t _x10551 = kk_evv_dup(w10_sq_); /*std/core/hnd/evv<4464>*/
  kk_evv_set(_x10551,kk_context());
  kk_box_t _x10552 = kk_function_call(kk_box_t, (kk_function_t, kk_function_t, kk_context_t*), cont0, (cont0, f, _ctx)); /*4463*/
  return kk_std_core_hnd_prompt(w00_sq_, w10_sq_, ev, m0, ret, _x10552, _ctx);
}

kk_box_t kk_std_core_hnd_prompt(kk_evv_t w0, kk_evv_t w1, kk_std_core_hnd__ev ev, kk_std_core_hnd__marker m0, kk_function_t ret, kk_box_t result, kk_context_t* _ctx) { /* forall<a,e,b,c> (w0 : evv<e>, w1 : evv<e>, ev : ev<b>, m : marker<e,c>, ret : (a) -> e c, result : a) -> e c */ 
  kk_unit_t __ = kk_Unit;
  kk_evv_t _x10539 = kk_evv_dup(w1); /*std/core/hnd/evv<4464>*/
  kk_std_core_hnd_guard(_x10539, _ctx);
  kk_unit_t __0 = kk_Unit;
  kk_evv_t _x10540 = kk_evv_dup(w0); /*std/core/hnd/evv<4464>*/
  kk_evv_set(_x10540,kk_context());
  kk_std_core_hnd__yld _match_10491 = kk_std_core_hnd_yield_prompt(m0, _ctx); /*std/core/hnd/yld<3839,3838,3840>*/;
  if (kk_std_core_hnd__is_Pure(_match_10491)) {
    kk_std_core_hnd__ev_dropn(ev, ((int32_t)3), _ctx);
    kk_evv_drop(w0, _ctx);
    kk_evv_drop(w1, _ctx);
    return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), ret, (ret, result, _ctx));
  }
  if (kk_std_core_hnd__is_YieldingFinal(_match_10491)) {
    kk_std_core_hnd__ev_dropn(ev, ((int32_t)3), _ctx);
    kk_box_drop(result, _ctx);
    kk_function_drop(ret, _ctx);
    kk_evv_drop(w0, _ctx);
    kk_evv_drop(w1, _ctx);
    return kk_box_any(kk_context());
  }
  if (kk_std_core_hnd__is_Yielding(_match_10491)) {
    kk_box_drop(result, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd_new_prompt_fun10541(ev, ret, w0, w1, m0, _ctx), _ctx);
  }
  kk_function_t clause0 = _match_10491._cons.Yield.clause;
  kk_function_t cont0 = _match_10491._cons.Yield.cont;
  kk_box_drop(result, _ctx);
  return kk_function_call(kk_box_t, (kk_function_t, kk_function_t, kk_context_t*), clause0, (clause0, kk_std_core_hnd_new_prompt_fun10547(cont0, ev, ret, w0, w1, m0, _ctx), _ctx));
}

kk_box_t kk_std_core_hnd__hhandle(kk_std_core_hnd__htag tag, int32_t cfc0, kk_box_t h, kk_function_t ret, kk_function_t action, kk_context_t* _ctx) { /* forall<a,e,e1,b,c> (tag : htag<b>, cfc : cfc, h : b<e,c>, ret : (a) -> e c, action : () -> e1 a) -> e c */ 
  kk_evv_t w0 = kk_std_core_hnd_evv_get(_ctx); /*std/core/hnd/evv<4564>*/;
  kk_std_core_hnd__marker m0 = kk_std_core_hnd_fresh_marker(_ctx); /*std/core/hnd/marker<4564,4567>*/;
  kk_std_core_hnd__ev ev;
  kk_evv_t _x10553 = kk_evv_dup(w0); /*std/core/hnd/evv<4564>*/
  ev = kk_std_core_hnd__new_Ev(kk_reuse_null, tag, m0, h, cfc0, _x10553, _ctx); /*std/core/hnd/ev<4566>*/
  kk_evv_t w1;
  kk_evv_t _x10554 = kk_evv_dup(w0); /*std/core/hnd/evv<4564>*/
  kk_std_core_hnd__ev _x10555 = kk_std_core_hnd__ev_dup(ev); /*std/core/hnd/ev<4566>*/
  w1 = kk_std_core_hnd_evv_insert(_x10554, _x10555, _ctx); /*std/core/hnd/evv<4564>*/
  kk_unit_t __ = kk_Unit;
  kk_evv_t _x10556 = kk_evv_dup(w1); /*std/core/hnd/evv<4564>*/
  kk_evv_set(_x10556,kk_context());
  kk_box_t _x10557 = kk_function_call(kk_box_t, (kk_function_t, kk_context_t*), action, (action, _ctx)); /*4563*/
  return kk_std_core_hnd_prompt(w0, w1, ev, m0, ret, _x10557, _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_mask_at1_fun10558__t {
  struct kk_function_s _base;
  bool behind;
  size_t i;
};
static kk_box_t kk_std_core_hnd_mask_at1_fun10558(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_mask_at1_fun10558(bool behind, size_t i, kk_context_t* _ctx) {
  struct kk_std_core_hnd_mask_at1_fun10558__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_mask_at1_fun10558__t, 1, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_mask_at1_fun10558, kk_context());
  _self->behind = behind;
  _self->i = i;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_mask_at1_fun10558(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd_mask_at1_fun10558__t* _self = kk_function_as(struct kk_std_core_hnd_mask_at1_fun10558__t*, _fself);
  bool behind = _self->behind; /* bool */
  size_t i = _self->i; /* std/core/hnd/ev-index */
  kk_drop_match(_self, {;;}, {}, _ctx)
  return kk_std_core_hnd_mask_at1(i, behind, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd_mask_at1(size_t i, bool behind, kk_function_t action, kk_box_t x, kk_context_t* _ctx) { /* forall<a,b,e,e1> (i : ev-index, behind : bool, action : (a) -> e b, x : a) -> e1 b */ 
  kk_evv_t w0 = kk_std_core_hnd_evv_swap_delete(i, behind, _ctx); /*std/core/hnd/evv<_4572>*/;
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), action, (action, x, _ctx)); /*4675*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w0,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd_new_mask_at1_fun10558(behind, i, _ctx), _ctx);
  }
  return y;
}


// lift anonymous function
struct kk_std_core_hnd__mask_at_fun10559__t {
  struct kk_function_s _base;
  bool behind;
  size_t i;
};
static kk_box_t kk_std_core_hnd__mask_at_fun10559(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd__new_mask_at_fun10559(bool behind, size_t i, kk_context_t* _ctx) {
  struct kk_std_core_hnd__mask_at_fun10559__t* _self = kk_function_alloc_as(struct kk_std_core_hnd__mask_at_fun10559__t, 1, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd__mask_at_fun10559, kk_context());
  _self->behind = behind;
  _self->i = i;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd__mask_at_fun10559(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd__mask_at_fun10559__t* _self = kk_function_as(struct kk_std_core_hnd__mask_at_fun10559__t*, _fself);
  bool behind = _self->behind; /* bool */
  size_t i = _self->i; /* std/core/hnd/ev-index */
  kk_drop_match(_self, {;;}, {}, _ctx)
  return kk_std_core_hnd_mask_at1(i, behind, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd__mask_at(size_t i, bool behind, kk_function_t action, kk_context_t* _ctx) { /* forall<a,e,e1> (i : ev-index, behind : bool, action : () -> e a) -> e1 a */ 
  kk_evv_t w0 = kk_std_core_hnd_evv_swap_delete(i, behind, _ctx); /*std/core/hnd/evv<_4681>*/;
  kk_box_t x = kk_function_call(kk_box_t, (kk_function_t, kk_context_t*), action, (action, _ctx)); /*4767*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w0,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(x, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd__new_mask_at_fun10559(behind, i, _ctx), _ctx);
  }
  return x;
}

kk_box_t kk_std_core_hnd__named_handle(kk_std_core_hnd__htag tag, int32_t cfc0, kk_box_t h, kk_function_t ret, kk_function_t action, kk_context_t* _ctx) { /* forall<a,e,e1,b,c> (tag : htag<b>, cfc : cfc, h : b<e,c>, ret : (a) -> e c, action : (ev<b>) -> e1 a) -> e c */ 
  kk_std_core_hnd__marker m0 = kk_std_core_hnd_fresh_marker_named(_ctx); /*std/core/hnd/marker<4884,4887>*/;
  kk_evv_t w0 = kk_std_core_hnd_evv_get(_ctx); /*std/core/hnd/evv<4884>*/;
  kk_std_core_hnd__ev ev;
  kk_evv_t _x10561 = kk_evv_dup(w0); /*std/core/hnd/evv<4884>*/
  ev = kk_std_core_hnd__new_Ev(kk_reuse_null, tag, m0, h, cfc0, _x10561, _ctx); /*std/core/hnd/ev<4886>*/
  kk_evv_t _x10562 = kk_evv_dup(w0); /*std/core/hnd/evv<4884>*/
  kk_std_core_hnd__ev _x10563 = kk_std_core_hnd__ev_dup(ev); /*std/core/hnd/ev<4886>*/
  kk_box_t _x10564 = kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__ev, kk_context_t*), action, (action, ev, _ctx)); /*4883*/
  return kk_std_core_hnd_prompt(_x10562, w0, _x10563, m0, ret, _x10564, _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_open_at1_fun10565__t {
  struct kk_function_s _base;
  size_t i;
};
static kk_box_t kk_std_core_hnd_open_at1_fun10565(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_open_at1_fun10565(size_t i, kk_context_t* _ctx) {
  struct kk_std_core_hnd_open_at1_fun10565__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_open_at1_fun10565__t, 1, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_open_at1_fun10565, kk_context());
  _self->i = i;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_open_at1_fun10565(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd_open_at1_fun10565__t* _self = kk_function_as(struct kk_std_core_hnd_open_at1_fun10565__t*, _fself);
  size_t i = _self->i; /* std/core/hnd/ev-index */
  kk_drop_match(_self, {;}, {}, _ctx)
  return kk_std_core_hnd_open_at1(i, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd_open_at1(size_t i, kk_function_t f, kk_box_t x, kk_context_t* _ctx) { /* forall<a,b,e,e1> (i : ev-index, f : (a) -> e b, x : a) -> e1 b */ 
  kk_evv_t w = kk_evv_swap_create1(i,kk_context()); /*std/core/hnd/evv<4994>*/;
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, x, _ctx)); /*4992*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd_new_open_at1_fun10565(i, _ctx), _ctx);
  }
  return y;
}


// lift anonymous function
struct kk_std_core_hnd__open_at0_fun10566__t {
  struct kk_function_s _base;
  size_t i;
};
static kk_box_t kk_std_core_hnd__open_at0_fun10566(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd__new_open_at0_fun10566(size_t i, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open_at0_fun10566__t* _self = kk_function_alloc_as(struct kk_std_core_hnd__open_at0_fun10566__t, 1, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd__open_at0_fun10566, kk_context());
  _self->i = i;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd__open_at0_fun10566(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open_at0_fun10566__t* _self = kk_function_as(struct kk_std_core_hnd__open_at0_fun10566__t*, _fself);
  size_t i = _self->i; /* std/core/hnd/ev-index */
  kk_drop_match(_self, {;}, {}, _ctx)
  return kk_std_core_hnd_open_at1(i, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd__open_at0(size_t i, kk_function_t f, kk_context_t* _ctx) { /* forall<a,e,e1> (i : ev-index, f : () -> e a) -> e1 a */ 
  kk_evv_t w = kk_evv_swap_create1(i,kk_context()); /*std/core/hnd/evv<5083>*/;
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_context_t*), f, (f, _ctx)); /*5081*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd__new_open_at0_fun10566(i, _ctx), _ctx);
  }
  return y;
}


// lift anonymous function
struct kk_std_core_hnd__open_at1_fun10567__t {
  struct kk_function_s _base;
  size_t i;
};
static kk_box_t kk_std_core_hnd__open_at1_fun10567(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd__new_open_at1_fun10567(size_t i, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open_at1_fun10567__t* _self = kk_function_alloc_as(struct kk_std_core_hnd__open_at1_fun10567__t, 1, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd__open_at1_fun10567, kk_context());
  _self->i = i;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd__open_at1_fun10567(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open_at1_fun10567__t* _self = kk_function_as(struct kk_std_core_hnd__open_at1_fun10567__t*, _fself);
  size_t i = _self->i; /* std/core/hnd/ev-index */
  kk_drop_match(_self, {;}, {}, _ctx)
  return kk_std_core_hnd_open_at1(i, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd__open_at1(size_t i, kk_function_t f, kk_box_t x, kk_context_t* _ctx) { /* forall<a,b,e,e1> (i : ev-index, f : (a) -> e b, x : a) -> e1 b */ 
  kk_evv_t w = kk_evv_swap_create1(i,kk_context()); /*std/core/hnd/evv<5187>*/;
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, x, _ctx)); /*5185*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd__new_open_at1_fun10567(i, _ctx), _ctx);
  }
  return y;
}


// lift anonymous function
struct kk_std_core_hnd__open_at2_fun10568__t {
  struct kk_function_s _base;
  size_t i;
};
static kk_box_t kk_std_core_hnd__open_at2_fun10568(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd__new_open_at2_fun10568(size_t i, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open_at2_fun10568__t* _self = kk_function_alloc_as(struct kk_std_core_hnd__open_at2_fun10568__t, 1, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd__open_at2_fun10568, kk_context());
  _self->i = i;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd__open_at2_fun10568(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open_at2_fun10568__t* _self = kk_function_as(struct kk_std_core_hnd__open_at2_fun10568__t*, _fself);
  size_t i = _self->i; /* std/core/hnd/ev-index */
  kk_drop_match(_self, {;}, {}, _ctx)
  return kk_std_core_hnd_open_at1(i, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd__open_at2(size_t i, kk_function_t f, kk_box_t x1, kk_box_t x2, kk_context_t* _ctx) { /* forall<a,b,c,e,e1> (i : ev-index, f : (a, b) -> e c, x1 : a, x2 : b) -> e1 c */ 
  kk_evv_t w = kk_evv_swap_create1(i,kk_context()); /*std/core/hnd/evv<5306>*/;
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_context_t*), f, (f, x1, x2, _ctx)); /*5304*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd__new_open_at2_fun10568(i, _ctx), _ctx);
  }
  return y;
}


// lift anonymous function
struct kk_std_core_hnd__open_at3_fun10569__t {
  struct kk_function_s _base;
  size_t i;
};
static kk_box_t kk_std_core_hnd__open_at3_fun10569(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd__new_open_at3_fun10569(size_t i, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open_at3_fun10569__t* _self = kk_function_alloc_as(struct kk_std_core_hnd__open_at3_fun10569__t, 1, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd__open_at3_fun10569, kk_context());
  _self->i = i;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd__open_at3_fun10569(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open_at3_fun10569__t* _self = kk_function_as(struct kk_std_core_hnd__open_at3_fun10569__t*, _fself);
  size_t i = _self->i; /* std/core/hnd/ev-index */
  kk_drop_match(_self, {;}, {}, _ctx)
  return kk_std_core_hnd_open_at1(i, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd__open_at3(size_t i, kk_function_t f, kk_box_t x1, kk_box_t x2, kk_box_t x3, kk_context_t* _ctx) { /* forall<a,b,c,d,e,e1> (i : ev-index, f : (a, b, c) -> e d, x1 : a, x2 : b, x3 : c) -> e1 d */ 
  kk_evv_t w = kk_evv_swap_create1(i,kk_context()); /*std/core/hnd/evv<5375>*/;
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_box_t, kk_context_t*), f, (f, x1, x2, x3, _ctx)); /*5373*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd__new_open_at3_fun10569(i, _ctx), _ctx);
  }
  return y;
}


// lift anonymous function
struct kk_std_core_hnd__open_at4_fun10570__t {
  struct kk_function_s _base;
  size_t i;
};
static kk_box_t kk_std_core_hnd__open_at4_fun10570(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd__new_open_at4_fun10570(size_t i, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open_at4_fun10570__t* _self = kk_function_alloc_as(struct kk_std_core_hnd__open_at4_fun10570__t, 1, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd__open_at4_fun10570, kk_context());
  _self->i = i;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd__open_at4_fun10570(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open_at4_fun10570__t* _self = kk_function_as(struct kk_std_core_hnd__open_at4_fun10570__t*, _fself);
  size_t i = _self->i; /* std/core/hnd/ev-index */
  kk_drop_match(_self, {;}, {}, _ctx)
  return kk_std_core_hnd_open_at1(i, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd__open_at4(size_t i, kk_function_t f, kk_box_t x1, kk_box_t x2, kk_box_t x3, kk_box_t x4, kk_context_t* _ctx) { /* forall<a,b,c,d,a1,e,e1> (i : ev-index, f : (a, b, c, d) -> e a1, x1 : a, x2 : b, x3 : c, x4 : d) -> e1 a1 */ 
  kk_evv_t w = kk_evv_swap_create1(i,kk_context()); /*std/core/hnd/evv<5450>*/;
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_box_t, kk_box_t, kk_context_t*), f, (f, x1, x2, x3, x4, _ctx)); /*5448*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd__new_open_at4_fun10570(i, _ctx), _ctx);
  }
  return y;
}


// lift anonymous function
struct kk_std_core_hnd_open1_fun10572__t {
  struct kk_function_s _base;
  kk_vector_t indices;
};
static kk_box_t kk_std_core_hnd_open1_fun10572(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_open1_fun10572(kk_vector_t indices, kk_context_t* _ctx) {
  struct kk_std_core_hnd_open1_fun10572__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_open1_fun10572__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_open1_fun10572, kk_context());
  _self->indices = indices;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_open1_fun10572(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd_open1_fun10572__t* _self = kk_function_as(struct kk_std_core_hnd_open1_fun10572__t*, _fself);
  kk_vector_t indices = _self->indices; /* vector<std/core/hnd/ev-index> */
  kk_drop_match(_self, {kk_vector_dup(indices);}, {}, _ctx)
  return kk_std_core_hnd_open1(indices, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd_open1(kk_vector_t indices, kk_function_t f, kk_box_t x, kk_context_t* _ctx) { /* forall<a,b,e,e1> (indices : vector<ev-index>, f : (a) -> e b, x : a) -> e1 b */ 
  kk_evv_t w;
  kk_vector_t _x10571 = kk_vector_dup(indices); /*vector<std/core/hnd/ev-index>*/
  w = kk_std_core_hnd_evv_swap_create(_x10571, _ctx); /*std/core/hnd/evv<5557>*/
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, x, _ctx)); /*5555*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd_new_open1_fun10572(indices, _ctx), _ctx);
  }
  kk_vector_drop(indices, _ctx);
  return y;
}


// lift anonymous function
struct kk_std_core_hnd__open0_fun10574__t {
  struct kk_function_s _base;
  kk_vector_t indices;
};
static kk_box_t kk_std_core_hnd__open0_fun10574(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd__new_open0_fun10574(kk_vector_t indices, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open0_fun10574__t* _self = kk_function_alloc_as(struct kk_std_core_hnd__open0_fun10574__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd__open0_fun10574, kk_context());
  _self->indices = indices;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd__open0_fun10574(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open0_fun10574__t* _self = kk_function_as(struct kk_std_core_hnd__open0_fun10574__t*, _fself);
  kk_vector_t indices = _self->indices; /* vector<std/core/hnd/ev-index> */
  kk_drop_match(_self, {kk_vector_dup(indices);}, {}, _ctx)
  return kk_std_core_hnd_open1(indices, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd__open0(kk_vector_t indices, kk_function_t f, kk_context_t* _ctx) { /* forall<a,e,e1> (indices : vector<ev-index>, f : () -> e a) -> e1 a */ 
  kk_evv_t w;
  kk_vector_t _x10573 = kk_vector_dup(indices); /*vector<std/core/hnd/ev-index>*/
  w = kk_std_core_hnd_evv_swap_create(_x10573, _ctx); /*std/core/hnd/evv<5646>*/
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_context_t*), f, (f, _ctx)); /*5644*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd__new_open0_fun10574(indices, _ctx), _ctx);
  }
  kk_vector_drop(indices, _ctx);
  return y;
}


// lift anonymous function
struct kk_std_core_hnd__open1_fun10576__t {
  struct kk_function_s _base;
  kk_vector_t indices;
};
static kk_box_t kk_std_core_hnd__open1_fun10576(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd__new_open1_fun10576(kk_vector_t indices, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open1_fun10576__t* _self = kk_function_alloc_as(struct kk_std_core_hnd__open1_fun10576__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd__open1_fun10576, kk_context());
  _self->indices = indices;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd__open1_fun10576(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open1_fun10576__t* _self = kk_function_as(struct kk_std_core_hnd__open1_fun10576__t*, _fself);
  kk_vector_t indices = _self->indices; /* vector<std/core/hnd/ev-index> */
  kk_drop_match(_self, {kk_vector_dup(indices);}, {}, _ctx)
  return kk_std_core_hnd_open1(indices, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd__open1(kk_vector_t indices, kk_function_t f, kk_box_t x, kk_context_t* _ctx) { /* forall<a,b,e,e1> (indices : vector<ev-index>, f : (a) -> e b, x : a) -> e1 b */ 
  kk_evv_t w;
  kk_vector_t _x10575 = kk_vector_dup(indices); /*vector<std/core/hnd/ev-index>*/
  w = kk_std_core_hnd_evv_swap_create(_x10575, _ctx); /*std/core/hnd/evv<5750>*/
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, x, _ctx)); /*5748*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd__new_open1_fun10576(indices, _ctx), _ctx);
  }
  kk_vector_drop(indices, _ctx);
  return y;
}


// lift anonymous function
struct kk_std_core_hnd__open2_fun10578__t {
  struct kk_function_s _base;
  kk_vector_t indices;
};
static kk_box_t kk_std_core_hnd__open2_fun10578(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd__new_open2_fun10578(kk_vector_t indices, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open2_fun10578__t* _self = kk_function_alloc_as(struct kk_std_core_hnd__open2_fun10578__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd__open2_fun10578, kk_context());
  _self->indices = indices;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd__open2_fun10578(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open2_fun10578__t* _self = kk_function_as(struct kk_std_core_hnd__open2_fun10578__t*, _fself);
  kk_vector_t indices = _self->indices; /* vector<std/core/hnd/ev-index> */
  kk_drop_match(_self, {kk_vector_dup(indices);}, {}, _ctx)
  return kk_std_core_hnd_open1(indices, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd__open2(kk_vector_t indices, kk_function_t f, kk_box_t x1, kk_box_t x2, kk_context_t* _ctx) { /* forall<a,b,c,e,e1> (indices : vector<ev-index>, f : (a, b) -> e c, x1 : a, x2 : b) -> e1 c */ 
  kk_evv_t w;
  kk_vector_t _x10577 = kk_vector_dup(indices); /*vector<std/core/hnd/ev-index>*/
  w = kk_std_core_hnd_evv_swap_create(_x10577, _ctx); /*std/core/hnd/evv<5869>*/
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_context_t*), f, (f, x1, x2, _ctx)); /*5867*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd__new_open2_fun10578(indices, _ctx), _ctx);
  }
  kk_vector_drop(indices, _ctx);
  return y;
}


// lift anonymous function
struct kk_std_core_hnd__open3_fun10580__t {
  struct kk_function_s _base;
  kk_vector_t indices;
};
static kk_box_t kk_std_core_hnd__open3_fun10580(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd__new_open3_fun10580(kk_vector_t indices, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open3_fun10580__t* _self = kk_function_alloc_as(struct kk_std_core_hnd__open3_fun10580__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd__open3_fun10580, kk_context());
  _self->indices = indices;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd__open3_fun10580(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open3_fun10580__t* _self = kk_function_as(struct kk_std_core_hnd__open3_fun10580__t*, _fself);
  kk_vector_t indices = _self->indices; /* vector<std/core/hnd/ev-index> */
  kk_drop_match(_self, {kk_vector_dup(indices);}, {}, _ctx)
  return kk_std_core_hnd_open1(indices, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd__open3(kk_vector_t indices, kk_function_t f, kk_box_t x1, kk_box_t x2, kk_box_t x3, kk_context_t* _ctx) { /* forall<a,b,c,d,e,e1> (indices : vector<ev-index>, f : (a, b, c) -> e d, x1 : a, x2 : b, x3 : c) -> e1 d */ 
  kk_evv_t w;
  kk_vector_t _x10579 = kk_vector_dup(indices); /*vector<std/core/hnd/ev-index>*/
  w = kk_std_core_hnd_evv_swap_create(_x10579, _ctx); /*std/core/hnd/evv<5938>*/
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_box_t, kk_context_t*), f, (f, x1, x2, x3, _ctx)); /*5936*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd__new_open3_fun10580(indices, _ctx), _ctx);
  }
  kk_vector_drop(indices, _ctx);
  return y;
}


// lift anonymous function
struct kk_std_core_hnd__open4_fun10582__t {
  struct kk_function_s _base;
  kk_vector_t indices;
};
static kk_box_t kk_std_core_hnd__open4_fun10582(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd__new_open4_fun10582(kk_vector_t indices, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open4_fun10582__t* _self = kk_function_alloc_as(struct kk_std_core_hnd__open4_fun10582__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd__open4_fun10582, kk_context());
  _self->indices = indices;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd__open4_fun10582(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd__open4_fun10582__t* _self = kk_function_as(struct kk_std_core_hnd__open4_fun10582__t*, _fself);
  kk_vector_t indices = _self->indices; /* vector<std/core/hnd/ev-index> */
  kk_drop_match(_self, {kk_vector_dup(indices);}, {}, _ctx)
  return kk_std_core_hnd_open1(indices, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd__open4(kk_vector_t indices, kk_function_t f, kk_box_t x1, kk_box_t x2, kk_box_t x3, kk_box_t x4, kk_context_t* _ctx) { /* forall<a,b,c,d,a1,e,e1> (indices : vector<ev-index>, f : (a, b, c, d) -> e a1, x1 : a, x2 : b, x3 : c, x4 : d) -> e1 a1 */ 
  kk_evv_t w;
  kk_vector_t _x10581 = kk_vector_dup(indices); /*vector<std/core/hnd/ev-index>*/
  w = kk_std_core_hnd_evv_swap_create(_x10581, _ctx); /*std/core/hnd/evv<6013>*/
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_box_t, kk_box_t, kk_context_t*), f, (f, x1, x2, x3, x4, _ctx)); /*6011*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd__new_open4_fun10582(indices, _ctx), _ctx);
  }
  kk_vector_drop(indices, _ctx);
  return y;
}


// lift anonymous function
struct kk_std_core_hnd_yield_to_fun10585__t {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_hnd_yield_to_fun10585(kk_function_t _fself, kk_box_t _b_10300, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_yield_to_fun10585(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_hnd_yield_to_fun10585, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_hnd_yield_to_fun10585(kk_function_t _fself, kk_box_t _b_10300, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  kk_function_t _x10586 = kk_function_unbox(_b_10300); /*() -> 6147 10301*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_context_t*), _x10586, (_x10586, _ctx));
}

kk_box_t kk_std_core_hnd_yield_to(kk_std_core_hnd__marker m0, kk_function_t clause0, kk_context_t* _ctx) { /* forall<a,e,b> (m : marker<e,b>, clause : ((() -> a) -> e b) -> e b) -> e a */ 
  kk_evv_t w0 = kk_std_core_hnd_evv_get(_ctx); /*std/core/hnd/evv<6147>*/;
  kk_evv_drop(w0, _ctx);
  kk_function_t g = kk_std_core_hnd_yield_to_prim(m0, clause0, _ctx); /*() -> 6146*/;
  kk_function_drop(g, _ctx);
  return kk_std_core_hnd_yield_extend(kk_std_core_hnd_new_yield_to_fun10585(_ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_clause_control_raw0_fun10587__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_control_raw0_fun10587(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev __ev, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control_raw0_fun10587(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control_raw0_fun10587__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control_raw0_fun10587__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control_raw0_fun10587, kk_context());
  _self->op = op;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_control_raw0_fun10588__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_control_raw0_fun10588(kk_function_t _fself, kk_function_t k0, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control_raw0_fun10588(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control_raw0_fun10588__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control_raw0_fun10588__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control_raw0_fun10588, kk_context());
  _self->op = op;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_control_raw0_fun10588(kk_function_t _fself, kk_function_t k0, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control_raw0_fun10588__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control_raw0_fun10588__t*, _fself);
  kk_function_t op = _self->op; /* (std/core/hnd/resume-context<6183,6184,6186>) -> 6184 6186 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__resume_context _x10589 = kk_std_core_hnd__new_Resume_context(k0, _ctx); /*std/core/hnd/resume-context<83,84,85>*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__resume_context, kk_context_t*), op, (op, _x10589, _ctx));
}
static kk_box_t kk_std_core_hnd_clause_control_raw0_fun10587(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev __ev, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control_raw0_fun10587__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control_raw0_fun10587__t*, _fself);
  kk_function_t op = _self->op; /* (std/core/hnd/resume-context<6183,6184,6186>) -> 6184 6186 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(__ev, ((int32_t)3), _ctx);
  return kk_std_core_hnd_yield_to(m0, kk_std_core_hnd_new_clause_control_raw0_fun10588(op, _ctx), _ctx);
}

kk_std_core_hnd__clause0 kk_std_core_hnd_clause_control_raw0(kk_function_t op, kk_context_t* _ctx) { /* forall<a,e,b,c> (op : (resume-context<a,e,c>) -> e c) -> clause0<a,b,e,c> */ 
  return kk_std_core_hnd__new_Clause0(kk_std_core_hnd_new_clause_control_raw0_fun10587(op, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_clause_control_raw1_fun10590__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_control_raw1_fun10590(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev __ev, kk_box_t x, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control_raw1_fun10590(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control_raw1_fun10590__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control_raw1_fun10590__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control_raw1_fun10590, kk_context());
  _self->op = op;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_control_raw1_fun10591__t {
  struct kk_function_s _base;
  kk_function_t op;
  kk_box_t x;
};
static kk_box_t kk_std_core_hnd_clause_control_raw1_fun10591(kk_function_t _fself, kk_function_t k0, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control_raw1_fun10591(kk_function_t op, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control_raw1_fun10591__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control_raw1_fun10591__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control_raw1_fun10591, kk_context());
  _self->op = op;
  _self->x = x;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_control_raw1_fun10591(kk_function_t _fself, kk_function_t k0, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control_raw1_fun10591__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control_raw1_fun10591__t*, _fself);
  kk_function_t op = _self->op; /* (x : 6226, r : std/core/hnd/resume-context<6227,6228,6230>) -> 6228 6230 */
  kk_box_t x = _self->x; /* 6226 */
  kk_drop_match(_self, {kk_function_dup(op);kk_box_dup(x);}, {}, _ctx)
  kk_std_core_hnd__resume_context _x10592 = kk_std_core_hnd__new_Resume_context(k0, _ctx); /*std/core/hnd/resume-context<83,84,85>*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_std_core_hnd__resume_context, kk_context_t*), op, (op, x, _x10592, _ctx));
}
static kk_box_t kk_std_core_hnd_clause_control_raw1_fun10590(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev __ev, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control_raw1_fun10590__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control_raw1_fun10590__t*, _fself);
  kk_function_t op = _self->op; /* (x : 6226, r : std/core/hnd/resume-context<6227,6228,6230>) -> 6228 6230 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(__ev, ((int32_t)3), _ctx);
  return kk_std_core_hnd_yield_to(m0, kk_std_core_hnd_new_clause_control_raw1_fun10591(op, x, _ctx), _ctx);
}

kk_std_core_hnd__clause1 kk_std_core_hnd_clause_control_raw1(kk_function_t op, kk_context_t* _ctx) { /* forall<a,b,e,c,d> (op : (x : a, r : resume-context<b,e,d>) -> e d) -> clause1<a,b,c,e,d> */ 
  return kk_std_core_hnd__new_Clause1(kk_std_core_hnd_new_clause_control_raw1_fun10590(op, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_clause_control_raw2_fun10593__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_control_raw2_fun10593(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev __ev, kk_box_t x1, kk_box_t x2, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control_raw2_fun10593(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control_raw2_fun10593__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control_raw2_fun10593__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control_raw2_fun10593, kk_context());
  _self->op = op;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_control_raw2_fun10594__t {
  struct kk_function_s _base;
  kk_function_t op;
  kk_box_t x1;
  kk_box_t x2;
};
static kk_box_t kk_std_core_hnd_clause_control_raw2_fun10594(kk_function_t _fself, kk_function_t k0, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control_raw2_fun10594(kk_function_t op, kk_box_t x1, kk_box_t x2, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control_raw2_fun10594__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control_raw2_fun10594__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control_raw2_fun10594, kk_context());
  _self->op = op;
  _self->x1 = x1;
  _self->x2 = x2;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_control_raw2_fun10594(kk_function_t _fself, kk_function_t k0, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control_raw2_fun10594__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control_raw2_fun10594__t*, _fself);
  kk_function_t op = _self->op; /* (x1 : 6275, x2 : 6276, r : std/core/hnd/resume-context<6277,6278,6280>) -> 6278 6280 */
  kk_box_t x1 = _self->x1; /* 6275 */
  kk_box_t x2 = _self->x2; /* 6276 */
  kk_drop_match(_self, {kk_function_dup(op);kk_box_dup(x1);kk_box_dup(x2);}, {}, _ctx)
  kk_std_core_hnd__resume_context _x10595 = kk_std_core_hnd__new_Resume_context(k0, _ctx); /*std/core/hnd/resume-context<83,84,85>*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_std_core_hnd__resume_context, kk_context_t*), op, (op, x1, x2, _x10595, _ctx));
}
static kk_box_t kk_std_core_hnd_clause_control_raw2_fun10593(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev __ev, kk_box_t x1, kk_box_t x2, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control_raw2_fun10593__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control_raw2_fun10593__t*, _fself);
  kk_function_t op = _self->op; /* (x1 : 6275, x2 : 6276, r : std/core/hnd/resume-context<6277,6278,6280>) -> 6278 6280 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(__ev, ((int32_t)3), _ctx);
  return kk_std_core_hnd_yield_to(m0, kk_std_core_hnd_new_clause_control_raw2_fun10594(op, x1, x2, _ctx), _ctx);
}

kk_std_core_hnd__clause2 kk_std_core_hnd_clause_control_raw2(kk_function_t op, kk_context_t* _ctx) { /* forall<a,b,c,e,d,a1> (op : (x1 : a, x2 : b, r : resume-context<c,e,a1>) -> e a1) -> clause2<a,b,c,d,e,a1> */ 
  return kk_std_core_hnd__new_Clause2(kk_std_core_hnd_new_clause_control_raw2_fun10593(op, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_clause_control_raw3_fun10596__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_control_raw3_fun10596(kk_function_t _fself, kk_std_core_hnd__marker _b_10305, kk_std_core_hnd__ev _b_10306, kk_box_t _b_10307, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control_raw3_fun10596(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control_raw3_fun10596__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control_raw3_fun10596__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control_raw3_fun10596, kk_context());
  _self->op = op;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_control_raw3_fun10597__t {
  struct kk_function_s _base;
  kk_box_t _b_10307;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_control_raw3_fun10597(kk_function_t _fself, kk_function_t k0, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control_raw3_fun10597(kk_box_t _b_10307, kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control_raw3_fun10597__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control_raw3_fun10597__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control_raw3_fun10597, kk_context());
  _self->_b_10307 = _b_10307;
  _self->op = op;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_control_raw3_fun10597(kk_function_t _fself, kk_function_t k0, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control_raw3_fun10597__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control_raw3_fun10597__t*, _fself);
  kk_box_t _b_10307 = _self->_b_10307; /* 52 */
  kk_function_t op = _self->op; /* (x1 : 6533, x2 : 6534, x3 : 6535, r : std/core/hnd/resume-context<6536,6537,6539>) -> 6537 6539 */
  kk_drop_match(_self, {kk_box_dup(_b_10307);kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__resume_context r_10160 = kk_std_core_hnd__new_Resume_context(k0, _ctx); /*std/core/hnd/resume-context<6536,6537,6539>*/;
  kk_box_t _x10598;
  kk_std_core_types__tuple3_ _match_10474;
  kk_box_t _x10599 = kk_box_dup(_b_10307); /*52*/
  _match_10474 = kk_std_core_types__tuple3__unbox(_x10599, _ctx); /*(6533, 6534, 6535)*/
  kk_box_t _x = _match_10474.fst;
  kk_box_dup(_x);
  kk_std_core_types__tuple3__drop(_match_10474, _ctx);
  _x10598 = _x; /*6533*/
  kk_box_t _x10600;
  kk_std_core_types__tuple3_ _match_10473;
  kk_box_t _x10601 = kk_box_dup(_b_10307); /*52*/
  _match_10473 = kk_std_core_types__tuple3__unbox(_x10601, _ctx); /*(6533, 6534, 6535)*/
  kk_box_t _x0 = _match_10473.snd;
  kk_box_dup(_x0);
  kk_std_core_types__tuple3__drop(_match_10473, _ctx);
  _x10600 = _x0; /*6534*/
  kk_box_t _x10602;
  kk_std_core_types__tuple3_ _match_10472 = kk_std_core_types__tuple3__unbox(_b_10307, _ctx); /*(6533, 6534, 6535)*/;
  kk_box_t _x1 = _match_10472.thd;
  kk_box_dup(_x1);
  kk_std_core_types__tuple3__drop(_match_10472, _ctx);
  _x10602 = _x1; /*6535*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_box_t, kk_std_core_hnd__resume_context, kk_context_t*), op, (op, _x10598, _x10600, _x10602, r_10160, _ctx));
}
static kk_box_t kk_std_core_hnd_clause_control_raw3_fun10596(kk_function_t _fself, kk_std_core_hnd__marker _b_10305, kk_std_core_hnd__ev _b_10306, kk_box_t _b_10307, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control_raw3_fun10596__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control_raw3_fun10596__t*, _fself);
  kk_function_t op = _self->op; /* (x1 : 6533, x2 : 6534, x3 : 6535, r : std/core/hnd/resume-context<6536,6537,6539>) -> 6537 6539 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(_b_10306, ((int32_t)3), _ctx);
  return kk_std_core_hnd_yield_to(_b_10305, kk_std_core_hnd_new_clause_control_raw3_fun10597(_b_10307, op, _ctx), _ctx);
}

kk_std_core_hnd__clause1 kk_std_core_hnd_clause_control_raw3(kk_function_t op, kk_context_t* _ctx) { /* forall<a,b,c,d,e,a1,b1> (op : (x1 : a, x2 : b, x3 : c, r : resume-context<d,e,b1>) -> e b1) -> clause1<(a, b, c),d,a1,e,b1> */ 
  return kk_std_core_hnd__new_Clause1(kk_std_core_hnd_new_clause_control_raw3_fun10596(op, _ctx), _ctx);
}
 
// monadic lift

kk_box_t kk_std_core_hnd__mlift10143_finalize(kk_box_t res, kk_box_t wild__x, kk_context_t* _ctx) { /* forall<a,e,b> (res : a, wild_x : b) -> e a */ 
  kk_box_drop(wild__x, _ctx);
  return res;
}


// lift anonymous function
struct kk_std_core_hnd_finalize_fun10606__t {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_hnd_finalize_fun10606(kk_function_t _fself, kk_box_t _x110605, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_finalize_fun10606(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_hnd_finalize_fun10606, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_hnd_finalize_fun10606(kk_function_t _fself, kk_box_t _x110605, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  return kk_std_core_types_id(_x110605, _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_finalize_fun10609__t {
  struct kk_function_s _base;
  kk_box_t res;
  kk_std_core_hnd__marker m0;
};
static kk_box_t kk_std_core_hnd_finalize_fun10609(kk_function_t _fself, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_finalize_fun10609(kk_box_t res, kk_std_core_hnd__marker m0, kk_context_t* _ctx) {
  struct kk_std_core_hnd_finalize_fun10609__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_finalize_fun10609__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_finalize_fun10609, kk_context());
  _self->res = res;
  _self->m0 = m0;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_finalize_fun10610__t {
  struct kk_function_s _base;
  kk_box_t res;
};
static kk_box_t kk_std_core_hnd_finalize_fun10610(kk_function_t _fself, kk_function_t __k, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_finalize_fun10610(kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd_finalize_fun10610__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_finalize_fun10610__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_finalize_fun10610, kk_context());
  _self->res = res;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_finalize_fun10610(kk_function_t _fself, kk_function_t __k, kk_context_t* _ctx) {
  struct kk_std_core_hnd_finalize_fun10610__t* _self = kk_function_as(struct kk_std_core_hnd_finalize_fun10610__t*, _fself);
  kk_box_t res = _self->res; /* 6627 */
  kk_drop_match(_self, {kk_box_dup(res);}, {}, _ctx)
  kk_function_drop(__k, _ctx);
  return res;
}
static kk_box_t kk_std_core_hnd_finalize_fun10609(kk_function_t _fself, kk_context_t* _ctx) {
  struct kk_std_core_hnd_finalize_fun10609__t* _self = kk_function_as(struct kk_std_core_hnd_finalize_fun10609__t*, _fself);
  kk_box_t res = _self->res; /* 6627 */
  kk_std_core_hnd__marker m0 = _self->m0; /* std/core/hnd/marker<6629,6627> */
  kk_drop_match(_self, {kk_box_dup(res);kk_std_core_hnd__marker_dup(m0);}, {}, _ctx)
  return kk_std_core_hnd_yield_to_final(m0, kk_std_core_hnd_new_finalize_fun10610(res, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_finalize_fun10611__t {
  struct kk_function_s _base;
  kk_box_t res;
};
static kk_box_t kk_std_core_hnd_finalize_fun10611(kk_function_t _fself, kk_box_t wild__x, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_finalize_fun10611(kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd_finalize_fun10611__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_finalize_fun10611__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_finalize_fun10611, kk_context());
  _self->res = res;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_finalize_fun10611(kk_function_t _fself, kk_box_t wild__x, kk_context_t* _ctx) {
  struct kk_std_core_hnd_finalize_fun10611__t* _self = kk_function_as(struct kk_std_core_hnd_finalize_fun10611__t*, _fself);
  kk_box_t res = _self->res; /* 6627 */
  kk_drop_match(_self, {kk_box_dup(res);}, {}, _ctx)
  return kk_std_core_hnd__mlift10143_finalize(res, wild__x, _ctx);
}

kk_box_t kk_std_core_hnd_finalize(kk_function_t cont, kk_box_t res, kk_context_t* _ctx) { /* forall<a,b,e,c> (cont : (() -> b) -> e c, res : a) -> e a */ 
  kk_std_core_hnd__marker m0 = kk_std_core_hnd_fresh_marker(_ctx); /*std/core/hnd/marker<6629,6627>*/;
  kk_evv_t w = kk_std_core_hnd_evv_get(_ctx); /*std/core/hnd/evv<6629>*/;
  kk_evv_t _x10603 = kk_evv_dup(w); /*std/core/hnd/evv<6629>*/
  kk_std_core_hnd__ev _x10604 = kk_ev_none(kk_context()); /*std/core/hnd/ev<187>*/
  kk_box_t _x10607;
  kk_box_t x_10161;
  kk_function_t _x10608;
  kk_box_dup(res);
  _x10608 = kk_std_core_hnd_new_finalize_fun10609(res, m0, _ctx); /*() -> 4038*/
  x_10161 = kk_function_call(kk_box_t, (kk_function_t, kk_function_t, kk_context_t*), cont, (cont, _x10608, _ctx)); /*6630*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x_10161, _ctx);
    _x10607 = kk_std_core_hnd_yield_extend(kk_std_core_hnd_new_finalize_fun10611(res, _ctx), _ctx); /*3991*/
  }
  else {
    _x10607 = kk_std_core_hnd__mlift10143_finalize(res, x_10161, _ctx); /*3991*/
  }
  return kk_std_core_hnd_prompt(_x10603, w, _x10604, m0, kk_std_core_hnd_new_finalize_fun10606(_ctx), _x10607, _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_finalize_fun10615__t_1 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_hnd_finalize_fun10615_1(kk_function_t _fself, kk_box_t _x110614, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_finalize_fun10615_1(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_hnd_finalize_fun10615_1, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_hnd_finalize_fun10615_1(kk_function_t _fself, kk_box_t _x110614, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  return kk_std_core_types_id(_x110614, _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_finalize_fun10618__t_1 {
  struct kk_function_s _base;
  kk_box_t x;
  kk_std_core_hnd__marker m0;
};
static kk_box_t kk_std_core_hnd_finalize_fun10618_1(kk_function_t _fself, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_finalize_fun10618_1(kk_box_t x, kk_std_core_hnd__marker m0, kk_context_t* _ctx) {
  struct kk_std_core_hnd_finalize_fun10618__t_1* _self = kk_function_alloc_as(struct kk_std_core_hnd_finalize_fun10618__t_1, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_finalize_fun10618_1, kk_context());
  _self->x = x;
  _self->m0 = m0;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_finalize_fun10619__t_1 {
  struct kk_function_s _base;
  kk_box_t x;
};
static kk_box_t kk_std_core_hnd_finalize_fun10619_1(kk_function_t _fself, kk_function_t __k, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_finalize_fun10619_1(kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_hnd_finalize_fun10619__t_1* _self = kk_function_alloc_as(struct kk_std_core_hnd_finalize_fun10619__t_1, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_finalize_fun10619_1, kk_context());
  _self->x = x;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_finalize_fun10619_1(kk_function_t _fself, kk_function_t __k, kk_context_t* _ctx) {
  struct kk_std_core_hnd_finalize_fun10619__t_1* _self = kk_function_as(struct kk_std_core_hnd_finalize_fun10619__t_1*, _fself);
  kk_box_t x = _self->x; /* 6700 */
  kk_drop_match(_self, {kk_box_dup(x);}, {}, _ctx)
  kk_function_drop(__k, _ctx);
  return x;
}
static kk_box_t kk_std_core_hnd_finalize_fun10618_1(kk_function_t _fself, kk_context_t* _ctx) {
  struct kk_std_core_hnd_finalize_fun10618__t_1* _self = kk_function_as(struct kk_std_core_hnd_finalize_fun10618__t_1*, _fself);
  kk_box_t x = _self->x; /* 6700 */
  kk_std_core_hnd__marker m0 = _self->m0; /* std/core/hnd/marker<6702,6700> */
  kk_drop_match(_self, {kk_box_dup(x);kk_std_core_hnd__marker_dup(m0);}, {}, _ctx)
  return kk_std_core_hnd_yield_to_final(m0, kk_std_core_hnd_new_finalize_fun10619_1(x, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_finalize_fun10621__t_1 {
  struct kk_function_s _base;
  kk_box_t x;
};
static kk_box_t kk_std_core_hnd_finalize_fun10621_1(kk_function_t _fself, kk_box_t wild__x, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_finalize_fun10621_1(kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_hnd_finalize_fun10621__t_1* _self = kk_function_alloc_as(struct kk_std_core_hnd_finalize_fun10621__t_1, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_finalize_fun10621_1, kk_context());
  _self->x = x;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_finalize_fun10621_1(kk_function_t _fself, kk_box_t wild__x, kk_context_t* _ctx) {
  struct kk_std_core_hnd_finalize_fun10621__t_1* _self = kk_function_as(struct kk_std_core_hnd_finalize_fun10621__t_1*, _fself);
  kk_box_t x = _self->x; /* 6700 */
  kk_drop_match(_self, {kk_box_dup(x);}, {}, _ctx)
  return kk_std_core_hnd__mlift10143_finalize(x, wild__x, _ctx);
}

kk_box_t kk_std_core_hnd_finalize_1(kk_std_core_hnd__resume_context r, kk_box_t x, kk_context_t* _ctx) { /* forall<a,b,e,c> (r : resume-context<b,e,c>, x : a) -> e a */ 
  kk_std_core_hnd__marker m0 = kk_std_core_hnd_fresh_marker(_ctx); /*std/core/hnd/marker<6702,6700>*/;
  kk_evv_t w = kk_std_core_hnd_evv_get(_ctx); /*std/core/hnd/evv<6702>*/;
  kk_evv_t _x10612 = kk_evv_dup(w); /*std/core/hnd/evv<6702>*/
  kk_std_core_hnd__ev _x10613 = kk_ev_none(kk_context()); /*std/core/hnd/ev<187>*/
  kk_box_t _x10616;
  kk_box_t x0_10165;
  kk_function_t _x10620;
  kk_function_t _x = r.k;
  _x10620 = _x; /*(() -> 6701) -> 6702 6703*/
  kk_function_t _x10617;
  kk_box_dup(x);
  _x10617 = kk_std_core_hnd_new_finalize_fun10618_1(x, m0, _ctx); /*() -> 4038*/
  x0_10165 = kk_function_call(kk_box_t, (kk_function_t, kk_function_t, kk_context_t*), _x10620, (_x10620, _x10617, _ctx)); /*6703*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x0_10165, _ctx);
    _x10616 = kk_std_core_hnd_yield_extend(kk_std_core_hnd_new_finalize_fun10621_1(x, _ctx), _ctx); /*3991*/
  }
  else {
    _x10616 = kk_std_core_hnd__mlift10143_finalize(x, x0_10165, _ctx); /*3991*/
  }
  return kk_std_core_hnd_prompt(_x10612, w, _x10613, m0, kk_std_core_hnd_new_finalize_fun10615_1(_ctx), _x10616, _ctx);
}

kk_box_t kk_std_core_hnd_protect_check(kk_ref_t resumed, kk_function_t k0, kk_box_t res, kk_context_t* _ctx) { /* forall<a,b,e,c> (resumed : ref<global,bool>, k : (() -> b) -> e c, res : a) -> e a */ 
  bool did_resume;
  kk_box_t _x10622 = kk_ref_get(resumed,kk_context()); /*179*/
  did_resume = kk_bool_unbox(_x10622); /*bool*/
  bool _match_10468 = !(did_resume); /*bool*/;
  if (_match_10468) {
    return kk_std_core_hnd_finalize(k0, res, _ctx);
  }
  kk_function_drop(k0, _ctx);
  return res;
}


// lift anonymous function
struct kk_std_core_hnd_protect_fun10624__t {
  struct kk_function_s _base;
  kk_function_t k0;
  kk_ref_t resumed;
};
static kk_box_t kk_std_core_hnd_protect_fun10624(kk_function_t _fself, kk_box_t ret, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_protect_fun10624(kk_function_t k0, kk_ref_t resumed, kk_context_t* _ctx) {
  struct kk_std_core_hnd_protect_fun10624__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_protect_fun10624__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_protect_fun10624, kk_context());
  _self->k0 = k0;
  _self->resumed = resumed;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_protect_fun10625__t {
  struct kk_function_s _base;
  kk_box_t ret;
};
static kk_box_t kk_std_core_hnd_protect_fun10625(kk_function_t _fself, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_protect_fun10625(kk_box_t ret, kk_context_t* _ctx) {
  struct kk_std_core_hnd_protect_fun10625__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_protect_fun10625__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_protect_fun10625, kk_context());
  _self->ret = ret;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_protect_fun10625(kk_function_t _fself, kk_context_t* _ctx) {
  struct kk_std_core_hnd_protect_fun10625__t* _self = kk_function_as(struct kk_std_core_hnd_protect_fun10625__t*, _fself);
  kk_box_t ret = _self->ret; /* 6967 */
  kk_drop_match(_self, {kk_box_dup(ret);}, {}, _ctx)
  return ret;
}
static kk_box_t kk_std_core_hnd_protect_fun10624(kk_function_t _fself, kk_box_t ret, kk_context_t* _ctx) {
  struct kk_std_core_hnd_protect_fun10624__t* _self = kk_function_as(struct kk_std_core_hnd_protect_fun10624__t*, _fself);
  kk_function_t k0 = _self->k0; /* (() -> 6967) -> 6968 6969 */
  kk_ref_t resumed = _self->resumed; /* ref<global,bool> */
  kk_drop_match(_self, {kk_function_dup(k0);kk_ref_dup(resumed);}, {}, _ctx)
  kk_unit_t __ = kk_Unit;
  kk_ref_set(resumed,(kk_bool_box(true)),kk_context());
  return kk_function_call(kk_box_t, (kk_function_t, kk_function_t, kk_context_t*), k0, (k0, kk_std_core_hnd_new_protect_fun10625(ret, _ctx), _ctx));
}


// lift anonymous function
struct kk_std_core_hnd_protect_fun10626__t {
  struct kk_function_s _base;
  kk_function_t k0;
  kk_ref_t resumed;
};
static kk_box_t kk_std_core_hnd_protect_fun10626(kk_function_t _fself, kk_box_t xres, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_protect_fun10626(kk_function_t k0, kk_ref_t resumed, kk_context_t* _ctx) {
  struct kk_std_core_hnd_protect_fun10626__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_protect_fun10626__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_protect_fun10626, kk_context());
  _self->k0 = k0;
  _self->resumed = resumed;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_protect_fun10626(kk_function_t _fself, kk_box_t xres, kk_context_t* _ctx) {
  struct kk_std_core_hnd_protect_fun10626__t* _self = kk_function_as(struct kk_std_core_hnd_protect_fun10626__t*, _fself);
  kk_function_t k0 = _self->k0; /* (() -> 6967) -> 6968 6969 */
  kk_ref_t resumed = _self->resumed; /* ref<global,bool> */
  kk_drop_match(_self, {kk_function_dup(k0);kk_ref_dup(resumed);}, {}, _ctx)
  return kk_std_core_hnd_protect_check(resumed, k0, xres, _ctx);
}

kk_box_t kk_std_core_hnd_protect(kk_box_t x, kk_function_t clause0, kk_function_t k0, kk_context_t* _ctx) { /* forall<a,b,e,c> (x : a, clause : (x : a, k : (b) -> e c) -> e c, k : (() -> b) -> e c) -> e c */ 
  kk_ref_t resumed = kk_ref_alloc((kk_bool_box(false)),kk_context()); /*ref<global,bool>*/;
  kk_box_t res;
  kk_function_t _x10623;
  kk_function_dup(k0);
  kk_ref_dup(resumed);
  _x10623 = kk_std_core_hnd_new_protect_fun10624(k0, resumed, _ctx); /*(ret : 6967) -> 6968 6969*/
  res = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_function_t, kk_context_t*), clause0, (clause0, x, _x10623, _ctx)); /*6969*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(res, _ctx);
    return kk_std_core_hnd_yield_extend(kk_std_core_hnd_new_protect_fun10626(k0, resumed, _ctx), _ctx);
  }
  return kk_std_core_hnd_protect_check(resumed, k0, res, _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_protect_fun10628__t_1 {
  struct kk_function_s _base;
  kk_function_t k0;
  kk_ref_t resumed;
};
static kk_box_t kk_std_core_hnd_protect_fun10628_1(kk_function_t _fself, kk_box_t ret, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_protect_fun10628_1(kk_function_t k0, kk_ref_t resumed, kk_context_t* _ctx) {
  struct kk_std_core_hnd_protect_fun10628__t_1* _self = kk_function_alloc_as(struct kk_std_core_hnd_protect_fun10628__t_1, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_protect_fun10628_1, kk_context());
  _self->k0 = k0;
  _self->resumed = resumed;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_protect_fun10629__t_1 {
  struct kk_function_s _base;
  kk_box_t ret;
};
static kk_box_t kk_std_core_hnd_protect_fun10629_1(kk_function_t _fself, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_protect_fun10629_1(kk_box_t ret, kk_context_t* _ctx) {
  struct kk_std_core_hnd_protect_fun10629__t_1* _self = kk_function_alloc_as(struct kk_std_core_hnd_protect_fun10629__t_1, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_protect_fun10629_1, kk_context());
  _self->ret = ret;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_protect_fun10629_1(kk_function_t _fself, kk_context_t* _ctx) {
  struct kk_std_core_hnd_protect_fun10629__t_1* _self = kk_function_as(struct kk_std_core_hnd_protect_fun10629__t_1*, _fself);
  kk_box_t ret = _self->ret; /* 7061 */
  kk_drop_match(_self, {kk_box_dup(ret);}, {}, _ctx)
  return ret;
}
static kk_box_t kk_std_core_hnd_protect_fun10628_1(kk_function_t _fself, kk_box_t ret, kk_context_t* _ctx) {
  struct kk_std_core_hnd_protect_fun10628__t_1* _self = kk_function_as(struct kk_std_core_hnd_protect_fun10628__t_1*, _fself);
  kk_function_t k0 = _self->k0; /* (() -> 7061) -> 7062 7063 */
  kk_ref_t resumed = _self->resumed; /* ref<global,bool> */
  kk_drop_match(_self, {kk_function_dup(k0);kk_ref_dup(resumed);}, {}, _ctx)
  kk_unit_t __ = kk_Unit;
  kk_ref_set(resumed,(kk_bool_box(true)),kk_context());
  return kk_function_call(kk_box_t, (kk_function_t, kk_function_t, kk_context_t*), k0, (k0, kk_std_core_hnd_new_protect_fun10629_1(ret, _ctx), _ctx));
}


// lift anonymous function
struct kk_std_core_hnd_protect_fun10630__t_1 {
  struct kk_function_s _base;
  kk_function_t k0;
  kk_ref_t resumed;
};
static kk_box_t kk_std_core_hnd_protect_fun10630_1(kk_function_t _fself, kk_box_t xres, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_protect_fun10630_1(kk_function_t k0, kk_ref_t resumed, kk_context_t* _ctx) {
  struct kk_std_core_hnd_protect_fun10630__t_1* _self = kk_function_alloc_as(struct kk_std_core_hnd_protect_fun10630__t_1, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_protect_fun10630_1, kk_context());
  _self->k0 = k0;
  _self->resumed = resumed;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_protect_fun10630_1(kk_function_t _fself, kk_box_t xres, kk_context_t* _ctx) {
  struct kk_std_core_hnd_protect_fun10630__t_1* _self = kk_function_as(struct kk_std_core_hnd_protect_fun10630__t_1*, _fself);
  kk_function_t k0 = _self->k0; /* (() -> 7061) -> 7062 7063 */
  kk_ref_t resumed = _self->resumed; /* ref<global,bool> */
  kk_drop_match(_self, {kk_function_dup(k0);kk_ref_dup(resumed);}, {}, _ctx)
  return kk_std_core_hnd_protect_check(resumed, k0, xres, _ctx);
}

kk_box_t kk_std_core_hnd_protect_1(kk_box_t x1, kk_box_t x2, kk_function_t clause0, kk_function_t k0, kk_context_t* _ctx) { /* forall<a,b,c,e,d> (x1 : a, x2 : b, clause : (x : a, x : b, k : (c) -> e d) -> e d, k : (() -> c) -> e d) -> e d */ 
  kk_ref_t resumed = kk_ref_alloc((kk_bool_box(false)),kk_context()); /*ref<global,bool>*/;
  kk_box_t res;
  kk_function_t _x10627;
  kk_function_dup(k0);
  kk_ref_dup(resumed);
  _x10627 = kk_std_core_hnd_new_protect_fun10628_1(k0, resumed, _ctx); /*(ret : 7061) -> 7062 7063*/
  res = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_function_t, kk_context_t*), clause0, (clause0, x1, x2, _x10627, _ctx)); /*7063*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(res, _ctx);
    return kk_std_core_hnd_yield_extend(kk_std_core_hnd_new_protect_fun10630_1(k0, resumed, _ctx), _ctx);
  }
  return kk_std_core_hnd_protect_check(resumed, k0, res, _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_clause_control0_fun10631__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_control0_fun10631(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev __ev, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control0_fun10631(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control0_fun10631__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control0_fun10631__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control0_fun10631, kk_context());
  _self->op = op;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_control0_fun10632__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_control0_fun10632(kk_function_t _fself, kk_function_t k0, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control0_fun10632(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control0_fun10632__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control0_fun10632__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control0_fun10632, kk_context());
  _self->op = op;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_control0_fun10634__t {
  struct kk_function_s _base;
  kk_function_t k0;
  kk_ref_t resumed;
};
static kk_box_t kk_std_core_hnd_clause_control0_fun10634(kk_function_t _fself, kk_box_t ret, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control0_fun10634(kk_function_t k0, kk_ref_t resumed, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control0_fun10634__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control0_fun10634__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control0_fun10634, kk_context());
  _self->k0 = k0;
  _self->resumed = resumed;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_control0_fun10635__t {
  struct kk_function_s _base;
  kk_box_t ret;
};
static kk_box_t kk_std_core_hnd_clause_control0_fun10635(kk_function_t _fself, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control0_fun10635(kk_box_t ret, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control0_fun10635__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control0_fun10635__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control0_fun10635, kk_context());
  _self->ret = ret;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_control0_fun10635(kk_function_t _fself, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control0_fun10635__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control0_fun10635__t*, _fself);
  kk_box_t ret = _self->ret; /* 7164 */
  kk_drop_match(_self, {kk_box_dup(ret);}, {}, _ctx)
  return ret;
}
static kk_box_t kk_std_core_hnd_clause_control0_fun10634(kk_function_t _fself, kk_box_t ret, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control0_fun10634__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control0_fun10634__t*, _fself);
  kk_function_t k0 = _self->k0; /* (() -> 7164) -> 7165 7167 */
  kk_ref_t resumed = _self->resumed; /* ref<global,bool> */
  kk_drop_match(_self, {kk_function_dup(k0);kk_ref_dup(resumed);}, {}, _ctx)
  kk_unit_t __ = kk_Unit;
  kk_ref_set(resumed,(kk_bool_box(true)),kk_context());
  return kk_function_call(kk_box_t, (kk_function_t, kk_function_t, kk_context_t*), k0, (k0, kk_std_core_hnd_new_clause_control0_fun10635(ret, _ctx), _ctx));
}


// lift anonymous function
struct kk_std_core_hnd_clause_control0_fun10636__t {
  struct kk_function_s _base;
  kk_function_t k0;
  kk_ref_t resumed;
};
static kk_box_t kk_std_core_hnd_clause_control0_fun10636(kk_function_t _fself, kk_box_t xres, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control0_fun10636(kk_function_t k0, kk_ref_t resumed, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control0_fun10636__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control0_fun10636__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control0_fun10636, kk_context());
  _self->k0 = k0;
  _self->resumed = resumed;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_control0_fun10636(kk_function_t _fself, kk_box_t xres, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control0_fun10636__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control0_fun10636__t*, _fself);
  kk_function_t k0 = _self->k0; /* (() -> 7164) -> 7165 7167 */
  kk_ref_t resumed = _self->resumed; /* ref<global,bool> */
  kk_drop_match(_self, {kk_function_dup(k0);kk_ref_dup(resumed);}, {}, _ctx)
  return kk_std_core_hnd_protect_check(resumed, k0, xres, _ctx);
}
static kk_box_t kk_std_core_hnd_clause_control0_fun10632(kk_function_t _fself, kk_function_t k0, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control0_fun10632__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control0_fun10632__t*, _fself);
  kk_function_t op = _self->op; /* ((7164) -> 7165 7167) -> 7165 7167 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_ref_t resumed = kk_ref_alloc((kk_bool_box(false)),kk_context()); /*ref<global,bool>*/;
  kk_box_t res;
  kk_function_t _x10633;
  kk_function_dup(k0);
  kk_ref_dup(resumed);
  _x10633 = kk_std_core_hnd_new_clause_control0_fun10634(k0, resumed, _ctx); /*(ret : 7164) -> 7165 7167*/
  res = kk_function_call(kk_box_t, (kk_function_t, kk_function_t, kk_context_t*), op, (op, _x10633, _ctx)); /*7167*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(res, _ctx);
    return kk_std_core_hnd_yield_extend(kk_std_core_hnd_new_clause_control0_fun10636(k0, resumed, _ctx), _ctx);
  }
  return kk_std_core_hnd_protect_check(resumed, k0, res, _ctx);
}
static kk_box_t kk_std_core_hnd_clause_control0_fun10631(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev __ev, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control0_fun10631__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control0_fun10631__t*, _fself);
  kk_function_t op = _self->op; /* ((7164) -> 7165 7167) -> 7165 7167 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(__ev, ((int32_t)3), _ctx);
  return kk_std_core_hnd_yield_to(m0, kk_std_core_hnd_new_clause_control0_fun10632(op, _ctx), _ctx);
}

kk_std_core_hnd__clause0 kk_std_core_hnd_clause_control0(kk_function_t op, kk_context_t* _ctx) { /* forall<a,e,b,c> (op : ((a) -> e c) -> e c) -> clause0<a,b,e,c> */ 
  return kk_std_core_hnd__new_Clause0(kk_std_core_hnd_new_clause_control0_fun10631(op, _ctx), _ctx);
}
extern kk_box_t kk_std_core_hnd_clause_control1_fun10638(kk_function_t _fself, kk_function_t k0, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control1_fun10638__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control1_fun10638__t*, _fself);
  kk_function_t clause0 = _self->clause0; /* (x : 7237, k : (7238) -> 7239 7241) -> 7239 7241 */
  kk_box_t x = _self->x; /* 7237 */
  kk_drop_match(_self, {kk_function_dup(clause0);kk_box_dup(x);}, {}, _ctx)
  return kk_std_core_hnd_protect(x, clause0, k0, _ctx);
}
extern kk_box_t kk_std_core_hnd_clause_control1_fun10637(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev __ev, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control1_fun10637__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control1_fun10637__t*, _fself);
  kk_function_t clause0 = _self->clause0; /* (x : 7237, k : (7238) -> 7239 7241) -> 7239 7241 */
  kk_drop_match(_self, {kk_function_dup(clause0);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(__ev, ((int32_t)3), _ctx);
  return kk_std_core_hnd_yield_to(m0, kk_std_core_hnd_new_clause_control1_fun10638(clause0, x, _ctx), _ctx);
}
extern kk_box_t kk_std_core_hnd_clause_control2_fun10640(kk_function_t _fself, kk_function_t k0, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control2_fun10640__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control2_fun10640__t*, _fself);
  kk_function_t clause0 = _self->clause0; /* (x1 : 7320, x2 : 7321, k : (7322) -> 7323 7325) -> 7323 7325 */
  kk_box_t x1 = _self->x1; /* 7320 */
  kk_box_t x2 = _self->x2; /* 7321 */
  kk_drop_match(_self, {kk_function_dup(clause0);kk_box_dup(x1);kk_box_dup(x2);}, {}, _ctx)
  return kk_std_core_hnd_protect_1(x1, x2, clause0, k0, _ctx);
}
extern kk_box_t kk_std_core_hnd_clause_control2_fun10639(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev __ev, kk_box_t x1, kk_box_t x2, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control2_fun10639__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control2_fun10639__t*, _fself);
  kk_function_t clause0 = _self->clause0; /* (x1 : 7320, x2 : 7321, k : (7322) -> 7323 7325) -> 7323 7325 */
  kk_drop_match(_self, {kk_function_dup(clause0);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(__ev, ((int32_t)3), _ctx);
  return kk_std_core_hnd_yield_to(m0, kk_std_core_hnd_new_clause_control2_fun10640(clause0, x1, x2, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_clause_control3_fun10641__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_control3_fun10641(kk_function_t _fself, kk_std_core_hnd__marker _b_10339, kk_std_core_hnd__ev _b_10340, kk_box_t _b_10341, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control3_fun10641(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control3_fun10641__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control3_fun10641__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control3_fun10641, kk_context());
  _self->op = op;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_control3_fun10642__t {
  struct kk_function_s _base;
  kk_box_t _b_10341;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_control3_fun10642(kk_function_t _fself, kk_function_t k0, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control3_fun10642(kk_box_t _b_10341, kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control3_fun10642__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control3_fun10642__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control3_fun10642, kk_context());
  _self->_b_10341 = _b_10341;
  _self->op = op;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_control3_fun10649__t {
  struct kk_function_s _base;
  kk_function_t k0;
  kk_ref_t resumed;
};
static kk_box_t kk_std_core_hnd_clause_control3_fun10649(kk_function_t _fself, kk_box_t ret, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control3_fun10649(kk_function_t k0, kk_ref_t resumed, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control3_fun10649__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control3_fun10649__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control3_fun10649, kk_context());
  _self->k0 = k0;
  _self->resumed = resumed;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_control3_fun10650__t {
  struct kk_function_s _base;
  kk_box_t ret;
};
static kk_box_t kk_std_core_hnd_clause_control3_fun10650(kk_function_t _fself, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control3_fun10650(kk_box_t ret, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control3_fun10650__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control3_fun10650__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control3_fun10650, kk_context());
  _self->ret = ret;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_control3_fun10650(kk_function_t _fself, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control3_fun10650__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control3_fun10650__t*, _fself);
  kk_box_t ret = _self->ret; /* 7581 */
  kk_drop_match(_self, {kk_box_dup(ret);}, {}, _ctx)
  return ret;
}
static kk_box_t kk_std_core_hnd_clause_control3_fun10649(kk_function_t _fself, kk_box_t ret, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control3_fun10649__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control3_fun10649__t*, _fself);
  kk_function_t k0 = _self->k0; /* (() -> 7581) -> 7582 7584 */
  kk_ref_t resumed = _self->resumed; /* ref<global,bool> */
  kk_drop_match(_self, {kk_function_dup(k0);kk_ref_dup(resumed);}, {}, _ctx)
  kk_unit_t __ = kk_Unit;
  kk_ref_set(resumed,(kk_bool_box(true)),kk_context());
  return kk_function_call(kk_box_t, (kk_function_t, kk_function_t, kk_context_t*), k0, (k0, kk_std_core_hnd_new_clause_control3_fun10650(ret, _ctx), _ctx));
}


// lift anonymous function
struct kk_std_core_hnd_clause_control3_fun10651__t {
  struct kk_function_s _base;
  kk_function_t k0;
  kk_ref_t resumed;
};
static kk_box_t kk_std_core_hnd_clause_control3_fun10651(kk_function_t _fself, kk_box_t xres, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control3_fun10651(kk_function_t k0, kk_ref_t resumed, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control3_fun10651__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control3_fun10651__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control3_fun10651, kk_context());
  _self->k0 = k0;
  _self->resumed = resumed;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_control3_fun10651(kk_function_t _fself, kk_box_t xres, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control3_fun10651__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control3_fun10651__t*, _fself);
  kk_function_t k0 = _self->k0; /* (() -> 7581) -> 7582 7584 */
  kk_ref_t resumed = _self->resumed; /* ref<global,bool> */
  kk_drop_match(_self, {kk_function_dup(k0);kk_ref_dup(resumed);}, {}, _ctx)
  return kk_std_core_hnd_protect_check(resumed, k0, xres, _ctx);
}
static kk_box_t kk_std_core_hnd_clause_control3_fun10642(kk_function_t _fself, kk_function_t k0, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control3_fun10642__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control3_fun10642__t*, _fself);
  kk_box_t _b_10341 = _self->_b_10341; /* 52 */
  kk_function_t op = _self->op; /* (x1 : 7578, x2 : 7579, x3 : 7580, k : (7581) -> 7582 7584) -> 7582 7584 */
  kk_drop_match(_self, {kk_box_dup(_b_10341);kk_function_dup(op);}, {}, _ctx)
  kk_ref_t resumed = kk_ref_alloc((kk_bool_box(false)),kk_context()); /*ref<global,bool>*/;
  kk_box_t res;
  kk_box_t _x10643;
  kk_std_core_types__tuple3_ _match_10464;
  kk_box_t _x10644 = kk_box_dup(_b_10341); /*52*/
  _match_10464 = kk_std_core_types__tuple3__unbox(_x10644, _ctx); /*(7578, 7579, 7580)*/
  kk_box_t _x = _match_10464.fst;
  kk_box_dup(_x);
  kk_std_core_types__tuple3__drop(_match_10464, _ctx);
  _x10643 = _x; /*7578*/
  kk_box_t _x10645;
  kk_std_core_types__tuple3_ _match_10463;
  kk_box_t _x10646 = kk_box_dup(_b_10341); /*52*/
  _match_10463 = kk_std_core_types__tuple3__unbox(_x10646, _ctx); /*(7578, 7579, 7580)*/
  kk_box_t _x0 = _match_10463.snd;
  kk_box_dup(_x0);
  kk_std_core_types__tuple3__drop(_match_10463, _ctx);
  _x10645 = _x0; /*7579*/
  kk_box_t _x10647;
  kk_std_core_types__tuple3_ _match_10462 = kk_std_core_types__tuple3__unbox(_b_10341, _ctx); /*(7578, 7579, 7580)*/;
  kk_box_t _x1 = _match_10462.thd;
  kk_box_dup(_x1);
  kk_std_core_types__tuple3__drop(_match_10462, _ctx);
  _x10647 = _x1; /*7580*/
  kk_function_t _x10648;
  kk_function_dup(k0);
  kk_ref_dup(resumed);
  _x10648 = kk_std_core_hnd_new_clause_control3_fun10649(k0, resumed, _ctx); /*(ret : 7581) -> 7582 7584*/
  res = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_box_t, kk_function_t, kk_context_t*), op, (op, _x10643, _x10645, _x10647, _x10648, _ctx)); /*7584*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(res, _ctx);
    return kk_std_core_hnd_yield_extend(kk_std_core_hnd_new_clause_control3_fun10651(k0, resumed, _ctx), _ctx);
  }
  return kk_std_core_hnd_protect_check(resumed, k0, res, _ctx);
}
static kk_box_t kk_std_core_hnd_clause_control3_fun10641(kk_function_t _fself, kk_std_core_hnd__marker _b_10339, kk_std_core_hnd__ev _b_10340, kk_box_t _b_10341, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control3_fun10641__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control3_fun10641__t*, _fself);
  kk_function_t op = _self->op; /* (x1 : 7578, x2 : 7579, x3 : 7580, k : (7581) -> 7582 7584) -> 7582 7584 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(_b_10340, ((int32_t)3), _ctx);
  return kk_std_core_hnd_yield_to(_b_10339, kk_std_core_hnd_new_clause_control3_fun10642(_b_10341, op, _ctx), _ctx);
}

kk_std_core_hnd__clause1 kk_std_core_hnd_clause_control3(kk_function_t op, kk_context_t* _ctx) { /* forall<a,b,c,d,e,a1,b1> (op : (x1 : a, x2 : b, x3 : c, k : (d) -> e b1) -> e b1) -> clause1<(a, b, c),d,a1,e,b1> */ 
  return kk_std_core_hnd__new_Clause1(kk_std_core_hnd_new_clause_control3_fun10641(op, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_clause_control4_fun10652__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_control4_fun10652(kk_function_t _fself, kk_std_core_hnd__marker _b_10353, kk_std_core_hnd__ev _b_10354, kk_box_t _b_10355, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control4_fun10652(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control4_fun10652__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control4_fun10652__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control4_fun10652, kk_context());
  _self->op = op;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_control4_fun10653__t {
  struct kk_function_s _base;
  kk_box_t _b_10355;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_control4_fun10653(kk_function_t _fself, kk_function_t k0, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control4_fun10653(kk_box_t _b_10355, kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control4_fun10653__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control4_fun10653__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control4_fun10653, kk_context());
  _self->_b_10355 = _b_10355;
  _self->op = op;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_control4_fun10666__t {
  struct kk_function_s _base;
  kk_function_t k0;
  kk_ref_t resumed;
};
static kk_box_t kk_std_core_hnd_clause_control4_fun10666(kk_function_t _fself, kk_box_t ret, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control4_fun10666(kk_function_t k0, kk_ref_t resumed, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control4_fun10666__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control4_fun10666__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control4_fun10666, kk_context());
  _self->k0 = k0;
  _self->resumed = resumed;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_control4_fun10667__t {
  struct kk_function_s _base;
  kk_box_t ret;
};
static kk_box_t kk_std_core_hnd_clause_control4_fun10667(kk_function_t _fself, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control4_fun10667(kk_box_t ret, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control4_fun10667__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control4_fun10667__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control4_fun10667, kk_context());
  _self->ret = ret;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_control4_fun10667(kk_function_t _fself, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control4_fun10667__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control4_fun10667__t*, _fself);
  kk_box_t ret = _self->ret; /* 7915 */
  kk_drop_match(_self, {kk_box_dup(ret);}, {}, _ctx)
  return ret;
}
static kk_box_t kk_std_core_hnd_clause_control4_fun10666(kk_function_t _fself, kk_box_t ret, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control4_fun10666__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control4_fun10666__t*, _fself);
  kk_function_t k0 = _self->k0; /* (() -> 7915) -> 7916 7918 */
  kk_ref_t resumed = _self->resumed; /* ref<global,bool> */
  kk_drop_match(_self, {kk_function_dup(k0);kk_ref_dup(resumed);}, {}, _ctx)
  kk_unit_t __ = kk_Unit;
  kk_ref_set(resumed,(kk_bool_box(true)),kk_context());
  return kk_function_call(kk_box_t, (kk_function_t, kk_function_t, kk_context_t*), k0, (k0, kk_std_core_hnd_new_clause_control4_fun10667(ret, _ctx), _ctx));
}


// lift anonymous function
struct kk_std_core_hnd_clause_control4_fun10668__t {
  struct kk_function_s _base;
  kk_function_t k0;
  kk_ref_t resumed;
};
static kk_box_t kk_std_core_hnd_clause_control4_fun10668(kk_function_t _fself, kk_box_t xres, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_control4_fun10668(kk_function_t k0, kk_ref_t resumed, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control4_fun10668__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_control4_fun10668__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_control4_fun10668, kk_context());
  _self->k0 = k0;
  _self->resumed = resumed;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_control4_fun10668(kk_function_t _fself, kk_box_t xres, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control4_fun10668__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control4_fun10668__t*, _fself);
  kk_function_t k0 = _self->k0; /* (() -> 7915) -> 7916 7918 */
  kk_ref_t resumed = _self->resumed; /* ref<global,bool> */
  kk_drop_match(_self, {kk_function_dup(k0);kk_ref_dup(resumed);}, {}, _ctx)
  return kk_std_core_hnd_protect_check(resumed, k0, xres, _ctx);
}
static kk_box_t kk_std_core_hnd_clause_control4_fun10653(kk_function_t _fself, kk_function_t k0, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control4_fun10653__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control4_fun10653__t*, _fself);
  kk_box_t _b_10355 = _self->_b_10355; /* 52 */
  kk_function_t op = _self->op; /* (x1 : 7911, x2 : 7912, x3 : 7913, x4 : 7914, k : (7915) -> 7916 7918) -> 7916 7918 */
  kk_drop_match(_self, {kk_box_dup(_b_10355);kk_function_dup(op);}, {}, _ctx)
  kk_ref_t resumed = kk_ref_alloc((kk_bool_box(false)),kk_context()); /*ref<global,bool>*/;
  kk_box_t res;
  kk_box_t _x10654;
  kk_std_core_types__tuple4_ _match_10460;
  kk_box_t _x10655 = kk_box_dup(_b_10355); /*52*/
  _match_10460 = kk_std_core_types__tuple4__unbox(_x10655, _ctx); /*(7911, 7912, 7913, 7914)*/
  struct kk_std_core_types__Tuple4_* _con10656 = kk_std_core_types__as_dash__lp__comma__comma__comma__rp_(_match_10460);
  kk_box_t _x = _con10656->fst;
  kk_box_t _pat00 = _con10656->snd;
  kk_box_t _pat10 = _con10656->thd;
  kk_box_t _pat2 = _con10656->field4;
  if (kk_std_core_types__tuple4__is_unique(_match_10460)) {
    kk_box_drop(_pat2, _ctx);
    kk_box_drop(_pat10, _ctx);
    kk_box_drop(_pat00, _ctx);
    kk_std_core_types__tuple4__free(_match_10460);
  }
  else {
    kk_box_dup(_x);
    kk_std_core_types__tuple4__decref(_match_10460, _ctx);
  }
  _x10654 = _x; /*7911*/
  kk_box_t _x10657;
  kk_std_core_types__tuple4_ _match_10459;
  kk_box_t _x10658 = kk_box_dup(_b_10355); /*52*/
  _match_10459 = kk_std_core_types__tuple4__unbox(_x10658, _ctx); /*(7911, 7912, 7913, 7914)*/
  struct kk_std_core_types__Tuple4_* _con10659 = kk_std_core_types__as_dash__lp__comma__comma__comma__rp_(_match_10459);
  kk_box_t _pat01 = _con10659->fst;
  kk_box_t _x0 = _con10659->snd;
  kk_box_t _pat11 = _con10659->thd;
  kk_box_t _pat20 = _con10659->field4;
  if (kk_std_core_types__tuple4__is_unique(_match_10459)) {
    kk_box_drop(_pat20, _ctx);
    kk_box_drop(_pat11, _ctx);
    kk_box_drop(_pat01, _ctx);
    kk_std_core_types__tuple4__free(_match_10459);
  }
  else {
    kk_box_dup(_x0);
    kk_std_core_types__tuple4__decref(_match_10459, _ctx);
  }
  _x10657 = _x0; /*7912*/
  kk_box_t _x10660;
  kk_std_core_types__tuple4_ _match_10458;
  kk_box_t _x10661 = kk_box_dup(_b_10355); /*52*/
  _match_10458 = kk_std_core_types__tuple4__unbox(_x10661, _ctx); /*(7911, 7912, 7913, 7914)*/
  struct kk_std_core_types__Tuple4_* _con10662 = kk_std_core_types__as_dash__lp__comma__comma__comma__rp_(_match_10458);
  kk_box_t _pat02 = _con10662->fst;
  kk_box_t _pat12 = _con10662->snd;
  kk_box_t _x1 = _con10662->thd;
  kk_box_t _pat21 = _con10662->field4;
  if (kk_std_core_types__tuple4__is_unique(_match_10458)) {
    kk_box_drop(_pat21, _ctx);
    kk_box_drop(_pat12, _ctx);
    kk_box_drop(_pat02, _ctx);
    kk_std_core_types__tuple4__free(_match_10458);
  }
  else {
    kk_box_dup(_x1);
    kk_std_core_types__tuple4__decref(_match_10458, _ctx);
  }
  _x10660 = _x1; /*7913*/
  kk_box_t _x10663;
  kk_std_core_types__tuple4_ _match_10457 = kk_std_core_types__tuple4__unbox(_b_10355, _ctx); /*(7911, 7912, 7913, 7914)*/;
  struct kk_std_core_types__Tuple4_* _con10664 = kk_std_core_types__as_dash__lp__comma__comma__comma__rp_(_match_10457);
  kk_box_t _pat03 = _con10664->fst;
  kk_box_t _pat13 = _con10664->snd;
  kk_box_t _pat22 = _con10664->thd;
  kk_box_t _x2 = _con10664->field4;
  if (kk_std_core_types__tuple4__is_unique(_match_10457)) {
    kk_box_drop(_pat22, _ctx);
    kk_box_drop(_pat13, _ctx);
    kk_box_drop(_pat03, _ctx);
    kk_std_core_types__tuple4__free(_match_10457);
  }
  else {
    kk_box_dup(_x2);
    kk_std_core_types__tuple4__decref(_match_10457, _ctx);
  }
  _x10663 = _x2; /*7914*/
  kk_function_t _x10665;
  kk_function_dup(k0);
  kk_ref_dup(resumed);
  _x10665 = kk_std_core_hnd_new_clause_control4_fun10666(k0, resumed, _ctx); /*(ret : 7915) -> 7916 7918*/
  res = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_box_t, kk_box_t, kk_function_t, kk_context_t*), op, (op, _x10654, _x10657, _x10660, _x10663, _x10665, _ctx)); /*7918*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(res, _ctx);
    return kk_std_core_hnd_yield_extend(kk_std_core_hnd_new_clause_control4_fun10668(k0, resumed, _ctx), _ctx);
  }
  return kk_std_core_hnd_protect_check(resumed, k0, res, _ctx);
}
static kk_box_t kk_std_core_hnd_clause_control4_fun10652(kk_function_t _fself, kk_std_core_hnd__marker _b_10353, kk_std_core_hnd__ev _b_10354, kk_box_t _b_10355, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_control4_fun10652__t* _self = kk_function_as(struct kk_std_core_hnd_clause_control4_fun10652__t*, _fself);
  kk_function_t op = _self->op; /* (x1 : 7911, x2 : 7912, x3 : 7913, x4 : 7914, k : (7915) -> 7916 7918) -> 7916 7918 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(_b_10354, ((int32_t)3), _ctx);
  return kk_std_core_hnd_yield_to(_b_10353, kk_std_core_hnd_new_clause_control4_fun10653(_b_10355, op, _ctx), _ctx);
}

kk_std_core_hnd__clause1 kk_std_core_hnd_clause_control4(kk_function_t op, kk_context_t* _ctx) { /* forall<a,b,c,d,a1,e,b1,c1> (op : (x1 : a, x2 : b, x3 : c, x4 : d, k : (a1) -> e c1) -> e c1) -> clause1<(a, b, c, d),a1,b1,e,c1> */ 
  return kk_std_core_hnd__new_Clause1(kk_std_core_hnd_new_clause_control4_fun10652(op, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_clause_never0_fun10669__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_never0_fun10669(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev __ev, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_never0_fun10669(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never0_fun10669__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_never0_fun10669__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_never0_fun10669, kk_context());
  _self->op = op;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_never0_fun10670__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_never0_fun10670(kk_function_t _fself, kk_function_t __k, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_never0_fun10670(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never0_fun10670__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_never0_fun10670__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_never0_fun10670, kk_context());
  _self->op = op;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_never0_fun10670(kk_function_t _fself, kk_function_t __k, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never0_fun10670__t* _self = kk_function_as(struct kk_std_core_hnd_clause_never0_fun10670__t*, _fself);
  kk_function_t op = _self->op; /* () -> 7949 7951 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_function_drop(__k, _ctx);
  return kk_function_call(kk_box_t, (kk_function_t, kk_context_t*), op, (op, _ctx));
}
static kk_box_t kk_std_core_hnd_clause_never0_fun10669(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev __ev, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never0_fun10669__t* _self = kk_function_as(struct kk_std_core_hnd_clause_never0_fun10669__t*, _fself);
  kk_function_t op = _self->op; /* () -> 7949 7951 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(__ev, ((int32_t)3), _ctx);
  return kk_std_core_hnd_yield_to_final(m0, kk_std_core_hnd_new_clause_never0_fun10670(op, _ctx), _ctx);
}

kk_std_core_hnd__clause0 kk_std_core_hnd_clause_never0(kk_function_t op, kk_context_t* _ctx) { /* forall<a,e,b,c> (op : () -> e c) -> clause0<a,b,e,c> */ 
  return kk_std_core_hnd__new_Clause0(kk_std_core_hnd_new_clause_never0_fun10669(op, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_clause_never1_fun10671__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_never1_fun10671(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev __ev, kk_box_t x, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_never1_fun10671(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never1_fun10671__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_never1_fun10671__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_never1_fun10671, kk_context());
  _self->op = op;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_never1_fun10672__t {
  struct kk_function_s _base;
  kk_function_t op;
  kk_box_t x;
};
static kk_box_t kk_std_core_hnd_clause_never1_fun10672(kk_function_t _fself, kk_function_t __k, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_never1_fun10672(kk_function_t op, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never1_fun10672__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_never1_fun10672__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_never1_fun10672, kk_context());
  _self->op = op;
  _self->x = x;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_never1_fun10672(kk_function_t _fself, kk_function_t __k, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never1_fun10672__t* _self = kk_function_as(struct kk_std_core_hnd_clause_never1_fun10672__t*, _fself);
  kk_function_t op = _self->op; /* (7986) -> 7988 7990 */
  kk_box_t x = _self->x; /* 7986 */
  kk_drop_match(_self, {kk_function_dup(op);kk_box_dup(x);}, {}, _ctx)
  kk_function_drop(__k, _ctx);
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), op, (op, x, _ctx));
}
static kk_box_t kk_std_core_hnd_clause_never1_fun10671(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev __ev, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never1_fun10671__t* _self = kk_function_as(struct kk_std_core_hnd_clause_never1_fun10671__t*, _fself);
  kk_function_t op = _self->op; /* (7986) -> 7988 7990 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(__ev, ((int32_t)3), _ctx);
  return kk_std_core_hnd_yield_to_final(m0, kk_std_core_hnd_new_clause_never1_fun10672(op, x, _ctx), _ctx);
}

kk_std_core_hnd__clause1 kk_std_core_hnd_clause_never1(kk_function_t op, kk_context_t* _ctx) { /* forall<a,b,e,c,d> (op : (a) -> e d) -> clause1<a,b,c,e,d> */ 
  return kk_std_core_hnd__new_Clause1(kk_std_core_hnd_new_clause_never1_fun10671(op, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_clause_never2_fun10673__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_never2_fun10673(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev __ev, kk_box_t x1, kk_box_t x2, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_never2_fun10673(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never2_fun10673__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_never2_fun10673__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_never2_fun10673, kk_context());
  _self->op = op;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_never2_fun10674__t {
  struct kk_function_s _base;
  kk_function_t op;
  kk_box_t x1;
  kk_box_t x2;
};
static kk_box_t kk_std_core_hnd_clause_never2_fun10674(kk_function_t _fself, kk_function_t __k, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_never2_fun10674(kk_function_t op, kk_box_t x1, kk_box_t x2, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never2_fun10674__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_never2_fun10674__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_never2_fun10674, kk_context());
  _self->op = op;
  _self->x1 = x1;
  _self->x2 = x2;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_never2_fun10674(kk_function_t _fself, kk_function_t __k, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never2_fun10674__t* _self = kk_function_as(struct kk_std_core_hnd_clause_never2_fun10674__t*, _fself);
  kk_function_t op = _self->op; /* (8030, 8031) -> 8033 8035 */
  kk_box_t x1 = _self->x1; /* 8030 */
  kk_box_t x2 = _self->x2; /* 8031 */
  kk_drop_match(_self, {kk_function_dup(op);kk_box_dup(x1);kk_box_dup(x2);}, {}, _ctx)
  kk_function_drop(__k, _ctx);
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_context_t*), op, (op, x1, x2, _ctx));
}
static kk_box_t kk_std_core_hnd_clause_never2_fun10673(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev __ev, kk_box_t x1, kk_box_t x2, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never2_fun10673__t* _self = kk_function_as(struct kk_std_core_hnd_clause_never2_fun10673__t*, _fself);
  kk_function_t op = _self->op; /* (8030, 8031) -> 8033 8035 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(__ev, ((int32_t)3), _ctx);
  return kk_std_core_hnd_yield_to_final(m0, kk_std_core_hnd_new_clause_never2_fun10674(op, x1, x2, _ctx), _ctx);
}

kk_std_core_hnd__clause2 kk_std_core_hnd_clause_never2(kk_function_t op, kk_context_t* _ctx) { /* forall<a,b,c,e,d,a1> (op : (a, b) -> e a1) -> clause2<a,b,c,d,e,a1> */ 
  return kk_std_core_hnd__new_Clause2(kk_std_core_hnd_new_clause_never2_fun10673(op, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_clause_never3_fun10675__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_never3_fun10675(kk_function_t _fself, kk_std_core_hnd__marker _b_10361, kk_std_core_hnd__ev _b_10362, kk_box_t _b_10363, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_never3_fun10675(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never3_fun10675__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_never3_fun10675__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_never3_fun10675, kk_context());
  _self->op = op;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_clause_never3_fun10676__t {
  struct kk_function_s _base;
  kk_box_t _b_10363;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_never3_fun10676(kk_function_t _fself, kk_function_t __k, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_never3_fun10676(kk_box_t _b_10363, kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never3_fun10676__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_never3_fun10676__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_never3_fun10676, kk_context());
  _self->_b_10363 = _b_10363;
  _self->op = op;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_never3_fun10676(kk_function_t _fself, kk_function_t __k, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never3_fun10676__t* _self = kk_function_as(struct kk_std_core_hnd_clause_never3_fun10676__t*, _fself);
  kk_box_t _b_10363 = _self->_b_10363; /* 52 */
  kk_function_t op = _self->op; /* (8287, 8288, 8289) -> 8291 8293 */
  kk_drop_match(_self, {kk_box_dup(_b_10363);kk_function_dup(op);}, {}, _ctx)
  kk_function_drop(__k, _ctx);
  kk_box_t _x10677;
  kk_std_core_types__tuple3_ _match_10455;
  kk_box_t _x10678 = kk_box_dup(_b_10363); /*52*/
  _match_10455 = kk_std_core_types__tuple3__unbox(_x10678, _ctx); /*(8287, 8288, 8289)*/
  kk_box_t _x = _match_10455.fst;
  kk_box_dup(_x);
  kk_std_core_types__tuple3__drop(_match_10455, _ctx);
  _x10677 = _x; /*8287*/
  kk_box_t _x10679;
  kk_std_core_types__tuple3_ _match_10454;
  kk_box_t _x10680 = kk_box_dup(_b_10363); /*52*/
  _match_10454 = kk_std_core_types__tuple3__unbox(_x10680, _ctx); /*(8287, 8288, 8289)*/
  kk_box_t _x0 = _match_10454.snd;
  kk_box_dup(_x0);
  kk_std_core_types__tuple3__drop(_match_10454, _ctx);
  _x10679 = _x0; /*8288*/
  kk_box_t _x10681;
  kk_std_core_types__tuple3_ _match_10453 = kk_std_core_types__tuple3__unbox(_b_10363, _ctx); /*(8287, 8288, 8289)*/;
  kk_box_t _x1 = _match_10453.thd;
  kk_box_dup(_x1);
  kk_std_core_types__tuple3__drop(_match_10453, _ctx);
  _x10681 = _x1; /*8289*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_box_t, kk_context_t*), op, (op, _x10677, _x10679, _x10681, _ctx));
}
static kk_box_t kk_std_core_hnd_clause_never3_fun10675(kk_function_t _fself, kk_std_core_hnd__marker _b_10361, kk_std_core_hnd__ev _b_10362, kk_box_t _b_10363, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_never3_fun10675__t* _self = kk_function_as(struct kk_std_core_hnd_clause_never3_fun10675__t*, _fself);
  kk_function_t op = _self->op; /* (8287, 8288, 8289) -> 8291 8293 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(_b_10362, ((int32_t)3), _ctx);
  return kk_std_core_hnd_yield_to_final(_b_10361, kk_std_core_hnd_new_clause_never3_fun10676(_b_10363, op, _ctx), _ctx);
}

kk_std_core_hnd__clause1 kk_std_core_hnd_clause_never3(kk_function_t op, kk_context_t* _ctx) { /* forall<a,b,c,d,e,a1,b1> (op : (a, b, c) -> e b1) -> clause1<(a, b, c),d,a1,e,b1> */ 
  return kk_std_core_hnd__new_Clause1(kk_std_core_hnd_new_clause_never3_fun10675(op, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_clause_tail_noyield3_fun10682__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_tail_noyield3_fun10682(kk_function_t _fself, kk_std_core_hnd__marker _b_10369, kk_std_core_hnd__ev _b_10370, kk_box_t _b_10371, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_tail_noyield3_fun10682(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail_noyield3_fun10682__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_tail_noyield3_fun10682__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_tail_noyield3_fun10682, kk_context());
  _self->op = op;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_tail_noyield3_fun10682(kk_function_t _fself, kk_std_core_hnd__marker _b_10369, kk_std_core_hnd__ev _b_10370, kk_box_t _b_10371, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail_noyield3_fun10682__t* _self = kk_function_as(struct kk_std_core_hnd_clause_tail_noyield3_fun10682__t*, _fself);
  kk_function_t op = _self->op; /* (8545, 8546, 8547) -> 8549 8548 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(_b_10370, ((int32_t)3), _ctx);
  kk_box_t _x10683;
  kk_std_core_types__tuple3_ _match_10452;
  kk_box_t _x10684 = kk_box_dup(_b_10371); /*52*/
  _match_10452 = kk_std_core_types__tuple3__unbox(_x10684, _ctx); /*(8545, 8546, 8547)*/
  kk_box_t _x = _match_10452.fst;
  kk_box_dup(_x);
  kk_std_core_types__tuple3__drop(_match_10452, _ctx);
  _x10683 = _x; /*8545*/
  kk_box_t _x10685;
  kk_std_core_types__tuple3_ _match_10451;
  kk_box_t _x10686 = kk_box_dup(_b_10371); /*52*/
  _match_10451 = kk_std_core_types__tuple3__unbox(_x10686, _ctx); /*(8545, 8546, 8547)*/
  kk_box_t _x0 = _match_10451.snd;
  kk_box_dup(_x0);
  kk_std_core_types__tuple3__drop(_match_10451, _ctx);
  _x10685 = _x0; /*8546*/
  kk_box_t _x10687;
  kk_std_core_types__tuple3_ _match_10450 = kk_std_core_types__tuple3__unbox(_b_10371, _ctx); /*(8545, 8546, 8547)*/;
  kk_box_t _x1 = _match_10450.thd;
  kk_box_dup(_x1);
  kk_std_core_types__tuple3__drop(_match_10450, _ctx);
  _x10687 = _x1; /*8547*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_box_t, kk_context_t*), op, (op, _x10683, _x10685, _x10687, _ctx));
}

kk_std_core_hnd__clause1 kk_std_core_hnd_clause_tail_noyield3(kk_function_t op, kk_context_t* _ctx) { /* forall<a,b,c,d,e,a1,b1> (op : (a, b, c) -> e d) -> clause1<(a, b, c),d,a1,e,b1> */ 
  return kk_std_core_hnd__new_Clause1(kk_std_core_hnd_new_clause_tail_noyield3_fun10682(op, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_under1_fun10689__t {
  struct kk_function_s _base;
  kk_std_core_hnd__ev ev;
};
static kk_box_t kk_std_core_hnd_under1_fun10689(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_under1_fun10689(kk_std_core_hnd__ev ev, kk_context_t* _ctx) {
  struct kk_std_core_hnd_under1_fun10689__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_under1_fun10689__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_under1_fun10689, kk_context());
  _self->ev = ev;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_under1_fun10689(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd_under1_fun10689__t* _self = kk_function_as(struct kk_std_core_hnd_under1_fun10689__t*, _fself);
  kk_std_core_hnd__ev ev = _self->ev; /* std/core/hnd/ev<8607> */
  kk_drop_match(_self, {kk_std_core_hnd__ev_dup(ev);}, {}, _ctx)
  return kk_std_core_hnd_under1(ev, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd_under1(kk_std_core_hnd__ev ev, kk_function_t op, kk_box_t x, kk_context_t* _ctx) { /* forall<a,b,e,c> (ev : ev<c>, op : (a) -> e b, x : a) -> e b */ 
  kk_evv_t w0;
  kk_std_core_hnd__ev _x10688 = kk_std_core_hnd__ev_dup(ev); /*std/core/hnd/ev<8607>*/
  w0 = kk_std_core_hnd_evv_swap_with(_x10688, _ctx); /*std/core/hnd/evv<_8557>*/
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), op, (op, x, _ctx)); /*8605*/;
  if (kk_yielding(kk_context())) {
    kk_evv_drop(w0, _ctx);
    kk_box_drop(y, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd_new_under1_fun10689(ev, _ctx), _ctx);
  }
  kk_std_core_hnd__ev_dropn(ev, ((int32_t)3), _ctx);
  kk_unit_t __0 = kk_Unit;
  kk_evv_set(w0,kk_context());
  return y;
}


// lift anonymous function
struct kk_std_core_hnd_under0_fun10691__t {
  struct kk_function_s _base;
  kk_std_core_hnd__ev ev;
};
static kk_box_t kk_std_core_hnd_under0_fun10691(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_under0_fun10691(kk_std_core_hnd__ev ev, kk_context_t* _ctx) {
  struct kk_std_core_hnd_under0_fun10691__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_under0_fun10691__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_under0_fun10691, kk_context());
  _self->ev = ev;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_under0_fun10691(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd_under0_fun10691__t* _self = kk_function_as(struct kk_std_core_hnd_under0_fun10691__t*, _fself);
  kk_std_core_hnd__ev ev = _self->ev; /* std/core/hnd/ev<8655> */
  kk_drop_match(_self, {kk_std_core_hnd__ev_dup(ev);}, {}, _ctx)
  return kk_std_core_hnd_under1(ev, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd_under0(kk_std_core_hnd__ev ev, kk_function_t op, kk_context_t* _ctx) { /* forall<a,e,b> (ev : ev<b>, op : () -> e a) -> e a */ 
  kk_evv_t w0;
  kk_std_core_hnd__ev _x10690 = kk_std_core_hnd__ev_dup(ev); /*std/core/hnd/ev<8655>*/
  w0 = kk_std_core_hnd_evv_swap_with(_x10690, _ctx); /*std/core/hnd/evv<_8612>*/
  kk_box_t y = kk_function_call(kk_box_t, (kk_function_t, kk_context_t*), op, (op, _ctx)); /*8653*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w0,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd_new_under0_fun10691(ev, _ctx), _ctx);
  }
  kk_std_core_hnd__ev_dropn(ev, ((int32_t)3), _ctx);
  return y;
}
extern kk_box_t kk_std_core_hnd_clause_tail0_fun10692(kk_function_t _fself, kk_std_core_hnd__marker __m, kk_std_core_hnd__ev ev, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail0_fun10692__t* _self = kk_function_as(struct kk_std_core_hnd_clause_tail0_fun10692__t*, _fself);
  kk_function_t op = _self->op; /* () -> 8683 8685 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  return kk_std_core_hnd_under0(ev, op, _ctx);
}
extern kk_box_t kk_std_core_hnd_clause_tail1_fun10693(kk_function_t _fself, kk_std_core_hnd__marker __m, kk_std_core_hnd__ev ev, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail1_fun10693__t* _self = kk_function_as(struct kk_std_core_hnd_clause_tail1_fun10693__t*, _fself);
  kk_function_t op = _self->op; /* (8722) -> 8720 8723 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  return kk_std_core_hnd_under1(ev, op, x, _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_under2_fun10695__t {
  struct kk_function_s _base;
  kk_std_core_hnd__ev ev;
};
static kk_box_t kk_std_core_hnd_under2_fun10695(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_under2_fun10695(kk_std_core_hnd__ev ev, kk_context_t* _ctx) {
  struct kk_std_core_hnd_under2_fun10695__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_under2_fun10695__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_under2_fun10695, kk_context());
  _self->ev = ev;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_under2_fun10695(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd_under2_fun10695__t* _self = kk_function_as(struct kk_std_core_hnd_under2_fun10695__t*, _fself);
  kk_std_core_hnd__ev ev = _self->ev; /* std/core/hnd/ev<8782> */
  kk_drop_match(_self, {kk_std_core_hnd__ev_dup(ev);}, {}, _ctx)
  return kk_std_core_hnd_under1(ev, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd_under2(kk_std_core_hnd__ev ev, kk_function_t op, kk_box_t x1, kk_box_t x2, kk_context_t* _ctx) { /* forall<a,b,c,e,d> (ev : ev<d>, op : (a, b) -> e c, x1 : a, x2 : b) -> e c */ 
  kk_evv_t w0;
  kk_std_core_hnd__ev _x10694 = kk_std_core_hnd__ev_dup(ev); /*std/core/hnd/ev<8782>*/
  w0 = kk_std_core_hnd_evv_swap_with(_x10694, _ctx); /*std/core/hnd/evv<_8731>*/
  kk_box_t z = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_context_t*), op, (op, x1, x2, _ctx)); /*8780*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w0,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(z, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd_new_under2_fun10695(ev, _ctx), _ctx);
  }
  kk_std_core_hnd__ev_dropn(ev, ((int32_t)3), _ctx);
  return z;
}
extern kk_box_t kk_std_core_hnd_clause_tail2_fun10696(kk_function_t _fself, kk_std_core_hnd__marker m0, kk_std_core_hnd__ev ev, kk_box_t x1, kk_box_t x2, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail2_fun10696__t* _self = kk_function_as(struct kk_std_core_hnd_clause_tail2_fun10696__t*, _fself);
  kk_function_t op = _self->op; /* (8824, 8825) -> 8822 8826 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  return kk_std_core_hnd_under2(ev, op, x1, x2, _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_clause_tail3_fun10697__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_tail3_fun10697(kk_function_t _fself, kk_std_core_hnd__marker _b_10377, kk_std_core_hnd__ev _b_10378, kk_box_t _b_10379, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_tail3_fun10697(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail3_fun10697__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_tail3_fun10697__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_tail3_fun10697, kk_context());
  _self->op = op;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_tail3_fun10697(kk_function_t _fself, kk_std_core_hnd__marker _b_10377, kk_std_core_hnd__ev _b_10378, kk_box_t _b_10379, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail3_fun10697__t* _self = kk_function_as(struct kk_std_core_hnd_clause_tail3_fun10697__t*, _fself);
  kk_function_t op = _self->op; /* (9079, 9080, 9081) -> 9083 9082 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(_b_10378, ((int32_t)3), _ctx);
  kk_box_t _x10698;
  kk_std_core_types__tuple3_ _match_10446;
  kk_box_t _x10699 = kk_box_dup(_b_10379); /*52*/
  _match_10446 = kk_std_core_types__tuple3__unbox(_x10699, _ctx); /*(9079, 9080, 9081)*/
  kk_box_t _x = _match_10446.fst;
  kk_box_dup(_x);
  kk_std_core_types__tuple3__drop(_match_10446, _ctx);
  _x10698 = _x; /*9079*/
  kk_box_t _x10700;
  kk_std_core_types__tuple3_ _match_10445;
  kk_box_t _x10701 = kk_box_dup(_b_10379); /*52*/
  _match_10445 = kk_std_core_types__tuple3__unbox(_x10701, _ctx); /*(9079, 9080, 9081)*/
  kk_box_t _x0 = _match_10445.snd;
  kk_box_dup(_x0);
  kk_std_core_types__tuple3__drop(_match_10445, _ctx);
  _x10700 = _x0; /*9080*/
  kk_box_t _x10702;
  kk_std_core_types__tuple3_ _match_10444 = kk_std_core_types__tuple3__unbox(_b_10379, _ctx); /*(9079, 9080, 9081)*/;
  kk_box_t _x1 = _match_10444.thd;
  kk_box_dup(_x1);
  kk_std_core_types__tuple3__drop(_match_10444, _ctx);
  _x10702 = _x1; /*9081*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_box_t, kk_context_t*), op, (op, _x10698, _x10700, _x10702, _ctx));
}

kk_std_core_hnd__clause1 kk_std_core_hnd_clause_tail3(kk_function_t op, kk_context_t* _ctx) { /* forall<a,b,c,d,e,a1,b1> (op : (a, b, c) -> e d) -> clause1<(a, b, c),d,a1,e,b1> */ 
  return kk_std_core_hnd__new_Clause1(kk_std_core_hnd_new_clause_tail3_fun10697(op, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_clause_tail4_fun10703__t {
  struct kk_function_s _base;
  kk_function_t op;
};
static kk_box_t kk_std_core_hnd_clause_tail4_fun10703(kk_function_t _fself, kk_std_core_hnd__marker _b_10385, kk_std_core_hnd__ev _b_10386, kk_box_t _b_10387, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_clause_tail4_fun10703(kk_function_t op, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail4_fun10703__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_clause_tail4_fun10703__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_clause_tail4_fun10703, kk_context());
  _self->op = op;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_clause_tail4_fun10703(kk_function_t _fself, kk_std_core_hnd__marker _b_10385, kk_std_core_hnd__ev _b_10386, kk_box_t _b_10387, kk_context_t* _ctx) {
  struct kk_std_core_hnd_clause_tail4_fun10703__t* _self = kk_function_as(struct kk_std_core_hnd_clause_tail4_fun10703__t*, _fself);
  kk_function_t op = _self->op; /* (9411, 9412, 9413, 9414) -> 9416 9415 */
  kk_drop_match(_self, {kk_function_dup(op);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(_b_10386, ((int32_t)3), _ctx);
  kk_box_t _x10704;
  kk_std_core_types__tuple4_ _match_10443;
  kk_box_t _x10705 = kk_box_dup(_b_10387); /*52*/
  _match_10443 = kk_std_core_types__tuple4__unbox(_x10705, _ctx); /*(9411, 9412, 9413, 9414)*/
  struct kk_std_core_types__Tuple4_* _con10706 = kk_std_core_types__as_dash__lp__comma__comma__comma__rp_(_match_10443);
  kk_box_t _x = _con10706->fst;
  kk_box_t _pat0 = _con10706->snd;
  kk_box_t _pat1 = _con10706->thd;
  kk_box_t _pat2 = _con10706->field4;
  if (kk_std_core_types__tuple4__is_unique(_match_10443)) {
    kk_box_drop(_pat2, _ctx);
    kk_box_drop(_pat1, _ctx);
    kk_box_drop(_pat0, _ctx);
    kk_std_core_types__tuple4__free(_match_10443);
  }
  else {
    kk_box_dup(_x);
    kk_std_core_types__tuple4__decref(_match_10443, _ctx);
  }
  _x10704 = _x; /*9411*/
  kk_box_t _x10707;
  kk_std_core_types__tuple4_ _match_10442;
  kk_box_t _x10708 = kk_box_dup(_b_10387); /*52*/
  _match_10442 = kk_std_core_types__tuple4__unbox(_x10708, _ctx); /*(9411, 9412, 9413, 9414)*/
  struct kk_std_core_types__Tuple4_* _con10709 = kk_std_core_types__as_dash__lp__comma__comma__comma__rp_(_match_10442);
  kk_box_t _pat00 = _con10709->fst;
  kk_box_t _x0 = _con10709->snd;
  kk_box_t _pat10 = _con10709->thd;
  kk_box_t _pat20 = _con10709->field4;
  if (kk_std_core_types__tuple4__is_unique(_match_10442)) {
    kk_box_drop(_pat20, _ctx);
    kk_box_drop(_pat10, _ctx);
    kk_box_drop(_pat00, _ctx);
    kk_std_core_types__tuple4__free(_match_10442);
  }
  else {
    kk_box_dup(_x0);
    kk_std_core_types__tuple4__decref(_match_10442, _ctx);
  }
  _x10707 = _x0; /*9412*/
  kk_box_t _x10710;
  kk_std_core_types__tuple4_ _match_10441;
  kk_box_t _x10711 = kk_box_dup(_b_10387); /*52*/
  _match_10441 = kk_std_core_types__tuple4__unbox(_x10711, _ctx); /*(9411, 9412, 9413, 9414)*/
  struct kk_std_core_types__Tuple4_* _con10712 = kk_std_core_types__as_dash__lp__comma__comma__comma__rp_(_match_10441);
  kk_box_t _pat01 = _con10712->fst;
  kk_box_t _pat11 = _con10712->snd;
  kk_box_t _x1 = _con10712->thd;
  kk_box_t _pat21 = _con10712->field4;
  if (kk_std_core_types__tuple4__is_unique(_match_10441)) {
    kk_box_drop(_pat21, _ctx);
    kk_box_drop(_pat11, _ctx);
    kk_box_drop(_pat01, _ctx);
    kk_std_core_types__tuple4__free(_match_10441);
  }
  else {
    kk_box_dup(_x1);
    kk_std_core_types__tuple4__decref(_match_10441, _ctx);
  }
  _x10710 = _x1; /*9413*/
  kk_box_t _x10713;
  kk_std_core_types__tuple4_ _match_10440 = kk_std_core_types__tuple4__unbox(_b_10387, _ctx); /*(9411, 9412, 9413, 9414)*/;
  struct kk_std_core_types__Tuple4_* _con10714 = kk_std_core_types__as_dash__lp__comma__comma__comma__rp_(_match_10440);
  kk_box_t _pat02 = _con10714->fst;
  kk_box_t _pat12 = _con10714->snd;
  kk_box_t _pat22 = _con10714->thd;
  kk_box_t _x2 = _con10714->field4;
  if (kk_std_core_types__tuple4__is_unique(_match_10440)) {
    kk_box_drop(_pat22, _ctx);
    kk_box_drop(_pat12, _ctx);
    kk_box_drop(_pat02, _ctx);
    kk_std_core_types__tuple4__free(_match_10440);
  }
  else {
    kk_box_dup(_x2);
    kk_std_core_types__tuple4__decref(_match_10440, _ctx);
  }
  _x10713 = _x2; /*9414*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_box_t, kk_box_t, kk_context_t*), op, (op, _x10704, _x10707, _x10710, _x10713, _ctx));
}

kk_std_core_hnd__clause1 kk_std_core_hnd_clause_tail4(kk_function_t op, kk_context_t* _ctx) { /* forall<a,b,c,d,a1,e,b1,c1> (op : (a, b, c, d) -> e a1) -> clause1<(a, b, c, d),a1,b1,e,c1> */ 
  return kk_std_core_hnd__new_Clause1(kk_std_core_hnd_new_clause_tail4_fun10703(op, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_finally_prompt_fun10715__t {
  struct kk_function_s _base;
  kk_function_t fin;
};
static kk_box_t kk_std_core_hnd_finally_prompt_fun10715(kk_function_t _fself, kk_function_t cont, kk_box_t x, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_finally_prompt_fun10715(kk_function_t fin, kk_context_t* _ctx) {
  struct kk_std_core_hnd_finally_prompt_fun10715__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_finally_prompt_fun10715__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_finally_prompt_fun10715, kk_context());
  _self->fin = fin;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_finally_prompt_fun10715(kk_function_t _fself, kk_function_t cont, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_hnd_finally_prompt_fun10715__t* _self = kk_function_as(struct kk_std_core_hnd_finally_prompt_fun10715__t*, _fself);
  kk_function_t fin = _self->fin; /* () -> 9508 () */
  kk_drop_match(_self, {kk_function_dup(fin);}, {}, _ctx)
  kk_box_t _x10716 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), cont, (cont, x, _ctx)); /*9507*/
  return kk_std_core_hnd_finally_prompt(fin, _x10716, _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_finally_prompt_fun10717__t {
  struct kk_function_s _base;
  kk_std_core_hnd__yield_info yld;
};
static kk_box_t kk_std_core_hnd_finally_prompt_fun10717(kk_function_t _fself, kk_box_t __x, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_finally_prompt_fun10717(kk_std_core_hnd__yield_info yld, kk_context_t* _ctx) {
  struct kk_std_core_hnd_finally_prompt_fun10717__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_finally_prompt_fun10717__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_finally_prompt_fun10717, kk_context());
  _self->yld = yld;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_finally_prompt_fun10717(kk_function_t _fself, kk_box_t __x, kk_context_t* _ctx) {
  struct kk_std_core_hnd_finally_prompt_fun10717__t* _self = kk_function_as(struct kk_std_core_hnd_finally_prompt_fun10717__t*, _fself);
  kk_std_core_hnd__yield_info yld = _self->yld; /* std/core/hnd/yield-info */
  kk_drop_match(_self, {kk_std_core_hnd__yield_info_dup(yld);}, {}, _ctx)
  kk_box_drop(__x, _ctx);
  return kk_std_core_hnd_unsafe_reyield(yld, _ctx);
}

kk_box_t kk_std_core_hnd_finally_prompt(kk_function_t fin, kk_box_t res, kk_context_t* _ctx) { /* forall<a,e> (fin : () -> e (), res : a) -> e a */ 
  bool _match_10437 = !((kk_yielding(kk_context()))); /*bool*/;
  if (_match_10437) {
    kk_unit_t __ = kk_Unit;
    kk_function_call(kk_unit_t, (kk_function_t, kk_context_t*), fin, (fin, _ctx));
    return res;
  }
  kk_box_drop(res, _ctx);
  bool _match_10438 = kk_yielding_non_final(kk_context()); /*bool*/;
  if (_match_10438) {
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd_new_finally_prompt_fun10715(fin, _ctx), _ctx);
  }
  kk_std_core_hnd__yield_info yld = kk_std_core_hnd_yield_capture(_ctx); /*std/core/hnd/yield-info*/;
  kk_unit_t __0 = kk_Unit;
  kk_function_call(kk_unit_t, (kk_function_t, kk_context_t*), fin, (fin, _ctx));
  if (kk_yielding(kk_context())) {
    return kk_std_core_hnd_yield_extend(kk_std_core_hnd_new_finally_prompt_fun10717(yld, _ctx), _ctx);
  }
  return kk_std_core_hnd_unsafe_reyield(yld, _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_initially_prompt_fun10719__t {
  struct kk_function_s _base;
  kk_ref_t count;
  kk_function_t init;
};
static kk_box_t kk_std_core_hnd_initially_prompt_fun10719(kk_function_t _fself, kk_function_t cont, kk_box_t x, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_initially_prompt_fun10719(kk_ref_t count, kk_function_t init, kk_context_t* _ctx) {
  struct kk_std_core_hnd_initially_prompt_fun10719__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_initially_prompt_fun10719__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_initially_prompt_fun10719, kk_context());
  _self->count = count;
  _self->init = init;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_hnd_initially_prompt_fun10726__t {
  struct kk_function_s _base;
  kk_function_t cont;
  kk_function_t init;
  kk_box_t x;
};
static kk_box_t kk_std_core_hnd_initially_prompt_fun10726(kk_function_t _fself, kk_box_t __ret, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_initially_prompt_fun10726(kk_function_t cont, kk_function_t init, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_hnd_initially_prompt_fun10726__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_initially_prompt_fun10726__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_initially_prompt_fun10726, kk_context());
  _self->cont = cont;
  _self->init = init;
  _self->x = x;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_initially_prompt_fun10726(kk_function_t _fself, kk_box_t __ret, kk_context_t* _ctx) {
  struct kk_std_core_hnd_initially_prompt_fun10726__t* _self = kk_function_as(struct kk_std_core_hnd_initially_prompt_fun10726__t*, _fself);
  kk_function_t cont = _self->cont; /* (9660) -> 9661 9660 */
  kk_function_t init = _self->init; /* (int) -> 9661 () */
  kk_box_t x = _self->x; /* 9660 */
  kk_drop_match(_self, {kk_function_dup(cont);kk_function_dup(init);kk_box_dup(x);}, {}, _ctx)
  kk_box_drop(__ret, _ctx);
  kk_box_t _x10727 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), cont, (cont, x, _ctx)); /*9660*/
  return kk_std_core_hnd_initially_prompt(init, _x10727, _ctx);
}
static kk_box_t kk_std_core_hnd_initially_prompt_fun10719(kk_function_t _fself, kk_function_t cont, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_hnd_initially_prompt_fun10719__t* _self = kk_function_as(struct kk_std_core_hnd_initially_prompt_fun10719__t*, _fself);
  kk_ref_t count = _self->count; /* ref<global,int> */
  kk_function_t init = _self->init; /* (int) -> 9661 () */
  kk_drop_match(_self, {kk_ref_dup(count);kk_function_dup(init);}, {}, _ctx)
  kk_integer_t cnt;
  kk_box_t _x10720;
  kk_ref_t _x10721 = kk_ref_dup(count); /*ref<global,int>*/
  _x10720 = kk_ref_get(_x10721,kk_context()); /*179*/
  cnt = kk_integer_unbox(_x10720); /*int*/
  kk_unit_t __ = kk_Unit;
  kk_integer_t _b_10399_10397;
  kk_integer_t _x10722 = kk_integer_dup(cnt); /*int*/
  _b_10399_10397 = kk_integer_add(_x10722,(kk_integer_from_small(1)),kk_context()); /*int*/
  kk_ref_set(count,(kk_integer_box(_b_10399_10397)),kk_context());
  bool _match_10435;
  kk_integer_t _x10723 = kk_integer_dup(cnt); /*int*/
  _match_10435 = kk_integer_eq(_x10723,(kk_integer_from_small(0)),kk_context()); /*bool*/
  if (_match_10435) {
    kk_integer_drop(cnt, _ctx);
    kk_box_t _x10724 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), cont, (cont, x, _ctx)); /*9660*/
    return kk_std_core_hnd_initially_prompt(init, _x10724, _ctx);
  }
  kk_unit_t r = kk_Unit;
  kk_function_t _x10725 = kk_function_dup(init); /*(int) -> 9661 ()*/
  kk_function_call(kk_unit_t, (kk_function_t, kk_integer_t, kk_context_t*), _x10725, (_x10725, cnt, _ctx));
  if (kk_yielding(kk_context())) {
    return kk_std_core_hnd_yield_extend(kk_std_core_hnd_new_initially_prompt_fun10726(cont, init, x, _ctx), _ctx);
  }
  kk_box_t _x10728 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), cont, (cont, x, _ctx)); /*9660*/
  return kk_std_core_hnd_initially_prompt(init, _x10728, _ctx);
}

kk_box_t kk_std_core_hnd_initially_prompt(kk_function_t init, kk_box_t res, kk_context_t* _ctx) { /* forall<a,e> (init : (int) -> e (), res : a) -> e a */ 
  bool _match_10434 = kk_yielding_non_final(kk_context()); /*bool*/;
  if (_match_10434) {
    kk_box_drop(res, _ctx);
    kk_ref_t count = kk_ref_alloc((kk_integer_box(kk_integer_from_small(0))),kk_context()); /*ref<global,int>*/;
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd_new_initially_prompt_fun10719(count, init, _ctx), _ctx);
  }
  kk_function_drop(init, _ctx);
  return res;
}


// lift anonymous function
struct kk_std_core_hnd_initially_fun10730__t {
  struct kk_function_s _base;
  kk_function_t action;
  kk_function_t init;
};
static kk_box_t kk_std_core_hnd_initially_fun10730(kk_function_t _fself, kk_box_t __ret, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_initially_fun10730(kk_function_t action, kk_function_t init, kk_context_t* _ctx) {
  struct kk_std_core_hnd_initially_fun10730__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_initially_fun10730__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_initially_fun10730, kk_context());
  _self->action = action;
  _self->init = init;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_initially_fun10730(kk_function_t _fself, kk_box_t __ret, kk_context_t* _ctx) {
  struct kk_std_core_hnd_initially_fun10730__t* _self = kk_function_as(struct kk_std_core_hnd_initially_fun10730__t*, _fself);
  kk_function_t action = _self->action; /* () -> 9698 9697 */
  kk_function_t init = _self->init; /* (int) -> 9698 () */
  kk_drop_match(_self, {kk_function_dup(action);kk_function_dup(init);}, {}, _ctx)
  kk_box_drop(__ret, _ctx);
  kk_box_t _x10731 = kk_function_call(kk_box_t, (kk_function_t, kk_context_t*), action, (action, _ctx)); /*9697*/
  return kk_std_core_hnd_initially_prompt(init, _x10731, _ctx);
}

kk_box_t kk_std_core_hnd_initially(kk_function_t init, kk_function_t action, kk_context_t* _ctx) { /* forall<a,e> (init : (int) -> e (), action : () -> e a) -> e a */ 
  kk_unit_t __ = kk_Unit;
  kk_function_t _x10729 = kk_function_dup(init); /*(int) -> 9698 ()*/
  kk_function_call(kk_unit_t, (kk_function_t, kk_integer_t, kk_context_t*), _x10729, (_x10729, kk_integer_from_small(0), _ctx));
  if (kk_yielding(kk_context())) {
    return kk_std_core_hnd_yield_extend(kk_std_core_hnd_new_initially_fun10730(action, init, _ctx), _ctx);
  }
  kk_box_t _x10732 = kk_function_call(kk_box_t, (kk_function_t, kk_context_t*), action, (action, _ctx)); /*9697*/
  return kk_std_core_hnd_initially_prompt(init, _x10732, _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_prompt_local_var_fun10734__t {
  struct kk_function_s _base;
  kk_ref_t loc;
  kk_box_t v;
};
static kk_box_t kk_std_core_hnd_prompt_local_var_fun10734(kk_function_t _fself, kk_function_t cont, kk_box_t x, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_prompt_local_var_fun10734(kk_ref_t loc, kk_box_t v, kk_context_t* _ctx) {
  struct kk_std_core_hnd_prompt_local_var_fun10734__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_prompt_local_var_fun10734__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_prompt_local_var_fun10734, kk_context());
  _self->loc = loc;
  _self->v = v;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_prompt_local_var_fun10734(kk_function_t _fself, kk_function_t cont, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_hnd_prompt_local_var_fun10734__t* _self = kk_function_as(struct kk_std_core_hnd_prompt_local_var_fun10734__t*, _fself);
  kk_ref_t loc = _self->loc; /* local-var<9790,9787> */
  kk_box_t v = _self->v; /* 9787 */
  kk_drop_match(_self, {kk_ref_dup(loc);kk_box_dup(v);}, {}, _ctx)
  kk_unit_t __0 = kk_Unit;
  kk_ref_t _x10735 = kk_ref_dup(loc); /*local-var<9790,9787>*/
  (kk_ref_set(_x10735,v,kk_context()));
  kk_box_t _x10736 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), cont, (cont, x, _ctx)); /*9788*/
  return kk_std_core_hnd_prompt_local_var(loc, _x10736, _ctx);
}

kk_box_t kk_std_core_hnd_prompt_local_var(kk_ref_t loc, kk_box_t res, kk_context_t* _ctx) { /* forall<a,b,h> (loc : local-var<h,a>, res : b) -> <div,local<h>> b */ 
  bool _match_10432 = !((kk_yielding(kk_context()))); /*bool*/;
  if (_match_10432) {
    kk_ref_drop(loc, _ctx);
    return res;
  }
  kk_box_drop(res, _ctx);
  kk_box_t v;
  kk_ref_t _x10733 = kk_ref_dup(loc); /*local-var<9790,9787>*/
  v = (kk_ref_get(_x10733,kk_context())); /*9787*/
  return kk_std_core_hnd_yield_cont(kk_std_core_hnd_new_prompt_local_var_fun10734(loc, v, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_try_finalize_prompt_fun10739__t {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_hnd_try_finalize_prompt_fun10739(kk_function_t _fself, kk_function_t _b_10401, kk_box_t _b_10402, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_try_finalize_prompt_fun10739(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_hnd_try_finalize_prompt_fun10739, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_hnd_try_finalize_prompt_fun10739(kk_function_t _fself, kk_function_t _b_10401, kk_box_t _b_10402, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  kk_std_core_types__either _x10740;
  kk_box_t _x10741 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _b_10401, (_b_10401, _b_10402, _ctx)); /*3818*/
  _x10740 = kk_std_core_hnd_try_finalize_prompt(_x10741, _ctx); /*either<std/core/hnd/yield-info,1802>*/
  return kk_std_core_types__either_box(_x10740, _ctx);
}

kk_std_core_types__either kk_std_core_hnd_try_finalize_prompt(kk_box_t res, kk_context_t* _ctx) { /* forall<a,e> (res : a) -> e either<yield-info,a> */ 
  bool _match_10430 = kk_yielding_non_final(kk_context()); /*bool*/;
  if (_match_10430) {
    kk_box_drop(res, _ctx);
    kk_box_t _x10738 = kk_std_core_hnd_yield_cont(kk_std_core_hnd_new_try_finalize_prompt_fun10739(_ctx), _ctx); /*3820*/
    return kk_std_core_types__either_unbox(_x10738, _ctx);
  }
  bool _match_10431 = !((kk_yielding(kk_context()))); /*bool*/;
  if (_match_10431) {
    return kk_std_core_types__new_Right(res, _ctx);
  }
  kk_box_drop(res, _ctx);
  kk_std_core_hnd__yield_info _b_10407_10403 = kk_std_core_hnd_yield_capture(_ctx); /*std/core/hnd/yield-info*/;
  return kk_std_core_types__new_Left(kk_std_core_hnd__yield_info_box(_b_10407_10403, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_hnd_under3_fun10743__t {
  struct kk_function_s _base;
  kk_std_core_hnd__ev ev;
};
static kk_box_t kk_std_core_hnd_under3_fun10743(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_under3_fun10743(kk_std_core_hnd__ev ev, kk_context_t* _ctx) {
  struct kk_std_core_hnd_under3_fun10743__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_under3_fun10743__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_under3_fun10743, kk_context());
  _self->ev = ev;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_under3_fun10743(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd_under3_fun10743__t* _self = kk_function_as(struct kk_std_core_hnd_under3_fun10743__t*, _fself);
  kk_std_core_hnd__ev ev = _self->ev; /* std/core/hnd/ev<10027> */
  kk_drop_match(_self, {kk_std_core_hnd__ev_dup(ev);}, {}, _ctx)
  return kk_std_core_hnd_under1(ev, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd_under3(kk_std_core_hnd__ev ev, kk_function_t op, kk_box_t x1, kk_box_t x2, kk_box_t x3, kk_context_t* _ctx) { /* forall<a,b,c,d,e,a1> (ev : ev<a1>, op : (a, b, c) -> e d, x1 : a, x2 : b, x3 : c) -> e d */ 
  kk_evv_t w0;
  kk_std_core_hnd__ev _x10742 = kk_std_core_hnd__ev_dup(ev); /*std/core/hnd/ev<10027>*/
  w0 = kk_std_core_hnd_evv_swap_with(_x10742, _ctx); /*std/core/hnd/evv<_9972>*/
  kk_box_t z = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_box_t, kk_context_t*), op, (op, x1, x2, x3, _ctx)); /*10025*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w0,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(z, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd_new_under3_fun10743(ev, _ctx), _ctx);
  }
  kk_std_core_hnd__ev_dropn(ev, ((int32_t)3), _ctx);
  return z;
}


// lift anonymous function
struct kk_std_core_hnd_under4_fun10745__t {
  struct kk_function_s _base;
  kk_std_core_hnd__ev ev;
};
static kk_box_t kk_std_core_hnd_under4_fun10745(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_hnd_new_under4_fun10745(kk_std_core_hnd__ev ev, kk_context_t* _ctx) {
  struct kk_std_core_hnd_under4_fun10745__t* _self = kk_function_alloc_as(struct kk_std_core_hnd_under4_fun10745__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_hnd_under4_fun10745, kk_context());
  _self->ev = ev;
  return &_self->_base;
}

static kk_box_t kk_std_core_hnd_under4_fun10745(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_hnd_under4_fun10745__t* _self = kk_function_as(struct kk_std_core_hnd_under4_fun10745__t*, _fself);
  kk_std_core_hnd__ev ev = _self->ev; /* std/core/hnd/ev<10095> */
  kk_drop_match(_self, {kk_std_core_hnd__ev_dup(ev);}, {}, _ctx)
  return kk_std_core_hnd_under1(ev, cont, res, _ctx);
}

kk_box_t kk_std_core_hnd_under4(kk_std_core_hnd__ev ev, kk_function_t op, kk_box_t x1, kk_box_t x2, kk_box_t x3, kk_box_t x4, kk_context_t* _ctx) { /* forall<a,b,c,d,a1,e,b1> (ev : ev<b1>, op : (a, b, c, d) -> e a1, x1 : a, x2 : b, x3 : c, x4 : d) -> e a1 */ 
  kk_evv_t w0;
  kk_std_core_hnd__ev _x10744 = kk_std_core_hnd__ev_dup(ev); /*std/core/hnd/ev<10095>*/
  w0 = kk_std_core_hnd_evv_swap_with(_x10744, _ctx); /*std/core/hnd/evv<_10036>*/
  kk_box_t z = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_box_t, kk_box_t, kk_context_t*), op, (op, x1, x2, x3, x4, _ctx)); /*10093*/;
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w0,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(z, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_hnd_new_under4_fun10745(ev, _ctx), _ctx);
  }
  kk_std_core_hnd__ev_dropn(ev, ((int32_t)3), _ctx);
  return z;
}

// initialization
void kk_std_core_hnd__init(kk_context_t* _ctx){
  static bool _initialized = false;
  if (_initialized) return;
  _initialized = true;
  kk_std_core_types__init(_ctx);
}
