// Koka generated module: "std/core", koka version: 2.0.14
#include "std_core.h"
/*---------------------------------------------------------------------------
  Copyright 2020 Microsoft Corporation.

  This is free software; you can redistribute it and/or modify it under the
  terms of the Apache License, Version 2.0. A copy of the License can be
  found in the file "license.txt" at the root of this distribution.
---------------------------------------------------------------------------*/

kk_std_core__list kk_vector_to_list(kk_vector_t v, kk_std_core__list tail, kk_context_t* ctx) {
  // todo: avoid boxed_dup if v is unique
  size_t n;
  kk_box_t* p = kk_vector_buf(v, &n);
  if (n == 0) {
    kk_vector_drop(v,ctx);
    return tail;
  }
  kk_std_core__list nil  = kk_std_core__new_Nil(ctx);
  struct kk_std_core_Cons* cons = NULL;
  kk_std_core__list list = kk_std_core__new_Nil(ctx);
  for( size_t i = 0; i < n; i++ ) {
    kk_std_core__list hd = kk_std_core__new_Cons(kk_reuse_null,kk_box_dup(p[i]), nil, ctx);
    if (cons==NULL) {
      list = hd;
    }
    else {
      cons->tail = hd;
    }
    cons = kk_std_core__as_Cons(hd);
  }
  cons->tail = tail;
  kk_vector_drop(v,ctx);
  return list;
}

kk_vector_t kk_list_to_vector(kk_std_core__list xs, kk_context_t* ctx) {
  // todo: avoid boxed_dup if xs is unique
  // find the length
  size_t len = 0;
  kk_std_core__list ys = xs;
  while (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* cons = kk_std_core__as_Cons(ys);
    len++;
    ys = cons->tail;
  }
  // alloc the vector and copy
  kk_vector_t v = kk_vector_alloc(len,kk_box_null,ctx);
  kk_box_t* p = kk_vector_buf(v,NULL);
  ys = xs;
  for( size_t i = 0; i < len; i++) {
    struct kk_std_core_Cons* cons = kk_std_core__as_Cons(ys);
    ys = cons->tail;
    p[i] = kk_box_dup(cons->head);
  }
  return v;
}

kk_vector_t kk_vector_init( size_t n, kk_function_t init, kk_context_t* ctx) {
  kk_vector_t v = kk_vector_alloc(n,kk_box_null,ctx);
  kk_box_t* p = kk_vector_buf(v,NULL);
  for(size_t i = 0; i < n; i++) {
    kk_function_dup(init);
    p[i] = kk_function_call(kk_box_t,(kk_function_t,size_t,kk_context_t*),init,(init,i,ctx));
  }
  kk_function_drop(init,ctx);
  return v;
}

kk_box_t kk_main_console( kk_function_t action, kk_context_t* ctx ) {
  return kk_function_call(kk_box_t,(kk_function_t,kk_unit_t,kk_context_t*),action,(action,kk_Unit,ctx));
}


kk_std_core__list kk_string_to_list(kk_string_t s, kk_context_t* ctx) {
  const uint8_t* p = kk_string_buf_borrow(s);
  kk_std_core__list nil  = kk_std_core__new_Nil(ctx);
  kk_std_core__list list = nil;
  struct kk_std_core_Cons* tl = NULL;
  size_t count;
  kk_char_t c;
  while( ((void)(c = kk_utf8_read(p,&count)), c != 0) ) {
    p += count;
    kk_std_core__list cons = kk_std_core__new_Cons(kk_reuse_null,kk_char_box(c,ctx), nil, ctx);
    if (tl!=NULL) {
      tl->tail = cons;
    }
    else {
      list = cons;
    }
    tl = kk_std_core__as_Cons(cons);
  }
  return list;
}

kk_string_t kk_string_from_list(kk_std_core__list cs, kk_context_t* ctx) {
  // TODO: optimize for short strings to write directly into a local buffer?
  // find total UTF8 length
  size_t len = 0;
  kk_std_core__list xs = cs;
  while (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* cons = kk_std_core__as_Cons(xs);
    len += kk_utf8_len(kk_char_unbox(cons->head,ctx));
    xs = cons->tail;
  }
  // allocate and copy the characters
  kk_string_t s = kk_string_alloc_buf(len,ctx);
  uint8_t* p = (uint8_t*)kk_string_buf_borrow(s);
  xs = cs;
  while (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* cons = kk_std_core__as_Cons(xs);
    size_t count;
    kk_utf8_write( kk_char_unbox(cons->head,ctx), p, &count );
    p += count;
    xs = cons->tail;
  }
  kk_assert_internal(*p == 0);
  kk_std_core__list_drop(cs,ctx);  // todo: drop while visiting
  return s;
}

static inline void kk_sslice_start_end_borrow( kk_std_core__sslice sslice, const uint8_t** start, const uint8_t** end) {
  const uint8_t* s = kk_string_buf_borrow(sslice.str);
  *start = s + sslice.start;
  *end = s + sslice.start + sslice.len;
}

kk_integer_t kk_slice_count( kk_std_core__sslice sslice, kk_context_t* ctx ) {
  // TODO: optimize this by extending kk_string_count
  const uint8_t* start;
  const uint8_t* end;
  kk_sslice_start_end_borrow(sslice, &start, &end);
  size_t count = 0;
  while( start < end && *start != 0 ) {
    const uint8_t* next = kk_utf8_next(start);
    count++;
    start = next;
  }
  return kk_integer_from_size_t(count,ctx);
}

kk_string_t kk_slice_to_string( kk_std_core__sslice  sslice, kk_context_t* ctx ) {
  const uint8_t* start;
  const uint8_t* end;
  kk_sslice_start_end_borrow(sslice, &start, &end);
  // is it the full string?
  if (sslice.start == 0 && sslice.len == kk_string_len_borrow(sslice.str)) {
    // TODO: drop sslice and dup sslice.str?
    return sslice.str;
  }
  else {
    // if not, we copy len bytes
    kk_string_t s = kk_string_alloc_dupn(sslice.len, (const char*)start, ctx);
    kk_std_core__sslice_drop(sslice,ctx);
    return s;
  }
}

kk_std_core__sslice kk_slice_first( kk_string_t str, kk_context_t* ctx ) {
  const uint8_t* s = kk_string_buf_borrow(str);
  const uint8_t* next = kk_utf8_next(s);
  return kk_std_core__new_Sslice(str, 0, (size_t)(next - s), ctx);
}

kk_std_core__sslice kk_slice_last( kk_string_t str, kk_context_t* ctx ) {
  const uint8_t* s = kk_string_buf_borrow(str);
  const uint8_t* end = s + kk_string_len_borrow(str);
  const uint8_t* prev = (s==end ? s : kk_utf8_prev(end));
  return kk_std_core__new_Sslice(str, (size_t)(prev - s), (size_t)(end - prev), ctx);
}

kk_std_core_types__maybe kk_slice_next( struct kk_std_core_Sslice slice, kk_context_t* ctx ) {
  if (slice.len == 0) {
    kk_std_core__sslice_drop(slice,ctx);
    return kk_std_core_types__new_Nothing(ctx);
  }
  const uint8_t* start;
  const uint8_t* end;
  kk_sslice_start_end_borrow(slice, &start, &end);
  size_t clen;
  const kk_char_t c = kk_utf8_read(start,&clen);
  kk_assert_internal(clen > 0 && clen <= slice.len);
  if (clen > slice.len) clen = slice.len;
  // TODO: specialize type to avoid boxing
  kk_std_core__sslice snext = kk_std_core__new_Sslice(slice.str, slice.start + clen, slice.len - clen, ctx);
  kk_std_core_types__tuple2_ res = kk_std_core_types__new_dash__lp__comma__rp_( kk_char_box(c,ctx), kk_std_core__sslice_box(snext,ctx), ctx);
  return kk_std_core_types__new_Just( kk_std_core_types__tuple2__box(res,ctx), ctx );
}

struct kk_std_core_Sslice kk_slice_extend( struct kk_std_core_Sslice slice, kk_integer_t count, kk_context_t* ctx ) {
  ptrdiff_t cnt = kk_integer_clamp(count,ctx);
  if (cnt==0 || (slice.len == 0 && cnt<0)) return slice;
  const uint8_t* const s0 = kk_string_buf_borrow(slice.str);  // start
  const uint8_t* const s1 = s0 + slice.start + slice.len;  // end
  const uint8_t* t  = s1;
  if (cnt >= 0) {
    do {
      t = kk_utf8_next(t);
      cnt--;
    } while (cnt > 0 && *t != 0);
  }
  else {  // cnt < 0
    const uint8_t* sstart = s0 - slice.start;
    do {
      t = kk_utf8_prev(t);
      cnt++;
    } while (cnt < 0 && t > sstart);
  }
  if (t == s1) return slice;  // length is unchanged
  return kk_std_core__new_Sslice(slice.str, slice.start, (t < s0 ? 0 : (size_t)(t - s0)), ctx);
}

struct kk_std_core_Sslice kk_slice_advance( struct kk_std_core_Sslice slice, kk_integer_t count, kk_context_t* ctx ) {
  const ptrdiff_t cnt0 = kk_integer_clamp(count,ctx);
  ptrdiff_t cnt = cnt0;
  if (cnt==0 || (slice.start == 0 && cnt<0)) return slice;
  const uint8_t* const s0 = kk_string_buf_borrow(slice.str);  // start
  const uint8_t* const s1 = s0 + slice.start + slice.len;  // end
  const uint8_t* const sstart = s0 - slice.start;
  kk_assert_internal(sstart == kk_string_buf_borrow(slice.str));
  // advance the start
  const uint8_t* t0  = s0;
  if (cnt >= 0) {
    do {
      t0 = kk_utf8_next(t0);
      cnt--;
    } while (cnt > 0 && *t0 != 0);
  }
  else {  // cnt < 0
    do {
      t0 = kk_utf8_prev(t0);
      cnt++;
    } while (cnt < 0 && t0 > sstart);
  }
  if (t0 == s0) return slice;  // start is unchanged
  // "t0" points to the new start, now advance the end by the same amount of codepoints
  const uint8_t* t1 = s1;
  cnt = cnt0;
  if (cnt >= 0) {
    do {
      t1 = kk_utf8_next(t1);
      cnt--;
    } while (cnt > 0 && *t1 != 0);
  }
  else {  // cnt < 0
    do {
      t1 = kk_utf8_prev(t1);
      cnt++;
    } while (cnt < 0 && t1 > sstart);
  }
  // t1 points to the new end
  kk_assert_internal(t1 >= t0);
  return kk_std_core__new_Sslice(slice.str, (size_t)(t0 - sstart), (size_t)(t1 - t0), ctx);
}

struct kk_std_core_Sslice kk_slice_common_prefix( kk_string_t str1, kk_string_t str2, kk_integer_t iupto, kk_context_t* ctx ) {
  const uint8_t* s1 = kk_string_buf_borrow(str1);
  const uint8_t* s2 = kk_string_buf_borrow(str1);
  size_t upto = kk_integer_clamp_size_t(iupto,ctx);
  size_t count;
  for(count = 0; count < upto && *s1 != 0 && *s2 != 0; count++, s1++, s2++ ) {
    if (*s1 != *s2) break;
  }
  kk_string_drop(str2,ctx);
  return kk_std_core__new_Sslice(str1, 0, count, ctx);
}



kk_std_core__error kk_error_from_errno( int err, kk_box_t result, kk_context_t* ctx ) {
  if (err==0) {
    return kk_std_core__new_Ok( result, ctx );
  }
  else {
    kk_box_drop(result, ctx);
    kk_string_t msg;
    #if defined(_GNU_SOURCE) && !defined(__APPLE__)
      // GNU version of strerror_r
      char buf[256];
      char* serr = strerror_r(err, buf, 255); buf[255] = 0;
      msg = kk_string_alloc_dup( serr, ctx );
    #elif (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600 || defined(__APPLE__))
      // XSI version of strerror_r
      char buf[256];
      strerror_r(err, buf, 255); buf[255] = 0;
      msg = kk_string_alloc_dup( buf, ctx );
    #elif defined(_MSC_VER) || (__STDC_VERSION__ >= 201112L || __cplusplus >= 201103L)
      // MSVC, or C/C++ 11
      char buf[256];
      strerror_s(buf, 255, err); buf[255] = 0;
      msg = kk_string_alloc_dup( buf, ctx );
    #else
      // Old style
      msg = kk_string_alloc_dup( strerror(err), ctx );
    #endif
    return kk_std_core__new_Error( kk_std_core__new_Exception( msg, kk_std_core__new_ExnSystem(kk_reuse_null, kk_integer_from_int(err,ctx), ctx), ctx), ctx );
  }
}

kk_define_string_literal(, kk_std_core__tag_ExnError, 17, "std/core/ExnError")
kk_define_string_literal(, kk_std_core__tag_ExnAssert, 18, "std/core/ExnAssert")
kk_define_string_literal(, kk_std_core__tag_ExnTodo, 16, "std/core/ExnTodo")
kk_define_string_literal(, kk_std_core__tag_ExnRange, 17, "std/core/ExnRange")
kk_define_string_literal(, kk_std_core__tag_ExnPattern, 19, "std/core/ExnPattern")
kk_define_string_literal(, kk_std_core__tag_ExnSystem, 18, "std/core/ExnSystem")
kk_define_string_literal(, kk_std_core__tag_ExnInternal, 20, "std/core/ExnInternal")

kk_std_core__exception kk_std_core__copy(kk_std_core__exception _this, kk_std_core_types__optional message0, kk_std_core_types__optional info0, kk_context_t* _ctx) { /* (exception, message : optional<string>, info : optional<exception-info>) -> exception */ 
  kk_string_t _x25394;
  if (kk_std_core_types__is_Optional(message0)) {
    kk_box_t _box_x23596 = message0._cons.Optional.value;
    kk_string_t _message_1672 = kk_string_unbox(_box_x23596);
    _x25394 = _message_1672; /*string*/
    goto _match25395;
  }
  kk_string_t _x = _this.message;
  kk_string_dup(_x);
  _x25394 = _x; /*string*/
  _match25395: ;
  kk_std_core__exception_info _x25397;
  if (kk_std_core_types__is_Optional(info0)) {
    kk_box_t _box_x23597 = info0._cons.Optional.value;
    kk_std_core__exception_info _info_1678 = kk_std_core__exception_info_unbox(_box_x23597, NULL);
    kk_std_core__exception_drop(_this, _ctx);
    _x25397 = _info_1678; /*exception-info*/
    goto _match25398;
  }
  kk_std_core__exception_info _x0 = _this.info;
  kk_std_core__exception_info_dup(_x0);
  kk_std_core__exception_drop(_this, _ctx);
  _x25397 = _x0; /*exception-info*/
  _match25398: ;
  return kk_std_core__new_Exception(_x25394, _x25397, _ctx);
}
 
// monadic lift

kk_std_core__delayed kk_std_core__mlift22848_op(kk_ref_t _c_0, kk_context_t* _ctx) { /* forall<e,a> (ref<global,either<() -> e a,a>>) -> delayed<e,a> */ 
  return kk_std_core__new_Delay(_c_0, _ctx);
}


// lift anonymous function
struct kk_std_core__copy_fun25401__t_1 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core__copy_fun25401_1(kk_function_t _fself, kk_box_t _b_23599, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_copy_fun25401_1(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core__copy_fun25401_1, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core__copy_fun25401_1(kk_function_t _fself, kk_box_t _b_23599, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  kk_std_core__delayed _x25402;
  kk_ref_t _x25403 = kk_ref_unbox(_b_23599, _ctx); /*ref<global,either<() -> 1723 1724,1724>>*/
  _x25402 = kk_std_core__mlift22848_op(_x25403, _ctx); /*delayed<1723,1724>*/
  return kk_std_core__delayed_box(_x25402, _ctx);
}

kk_std_core__delayed kk_std_core__copy_1(kk_std_core__delayed _this, kk_std_core_types__optional dref0, kk_context_t* _ctx) { /* forall<e,a> (delayed<e,a>, dref : optional<ref<global,either<() -> e a,a>>>) -> delayed<e,a> */ 
  if (kk_yielding(kk_context())) {
    kk_std_core__delayed_drop(_this, _ctx);
    kk_std_core_types__optional_drop(dref0, _ctx);
    kk_box_t _x25400 = kk_std_core_hnd_yield_extend(kk_std_core__new_copy_fun25401_1(_ctx), _ctx); /*3991*/
    return kk_std_core__delayed_unbox(_x25400, _ctx);
  }
  kk_ref_t _x25404;
  if (kk_std_core_types__is_Optional(dref0)) {
    kk_box_t _box_x23600 = dref0._cons.Optional.value;
    kk_ref_t _dref_1711 = kk_ref_unbox(_box_x23600, NULL);
    kk_std_core__delayed_drop(_this, _ctx);
    _x25404 = _dref_1711; /*ref<global,either<() -> 1723 1724,1724>>*/
    goto _match25405;
  }
  kk_ref_t _x = _this.dref;
  _x25404 = _x; /*ref<global,either<() -> 1723 1724,1724>>*/
  _match25405: ;
  return kk_std_core__new_Delay(_x25404, _ctx);
}

kk_std_core__sslice kk_std_core__copy_2(kk_std_core__sslice _this, kk_std_core_types__optional str0, kk_std_core_types__optional start0, kk_std_core_types__optional len0, kk_context_t* _ctx) { /* (sslice, str : optional<string>, start : optional<size_t>, len : optional<size_t>) -> sslice */ 
  kk_string_t _x25408;
  if (kk_std_core_types__is_Optional(str0)) {
    kk_box_t _box_x23603 = str0._cons.Optional.value;
    kk_string_t _str_1812 = kk_string_unbox(_box_x23603);
    _x25408 = _str_1812; /*string*/
    goto _match25409;
  }
  kk_string_t _x = _this.str;
  kk_string_dup(_x);
  _x25408 = _x; /*string*/
  _match25409: ;
  size_t _x25411;
  if (kk_std_core_types__is_Optional(start0)) {
    kk_box_t _box_x23604 = start0._cons.Optional.value;
    size_t _start_1818 = kk_size_unbox(_box_x23604, NULL);
    _x25411 = _start_1818; /*size_t*/
    goto _match25412;
  }
  size_t _x0 = _this.start;
  _x25411 = _x0; /*size_t*/
  _match25412: ;
  size_t _x25414;
  if (kk_std_core_types__is_Optional(len0)) {
    kk_box_t _box_x23605 = len0._cons.Optional.value;
    size_t _len_1824 = kk_size_unbox(_box_x23605, NULL);
    kk_std_core__sslice_drop(_this, _ctx);
    _x25414 = _len_1824; /*size_t*/
    goto _match25415;
  }
  size_t _x1 = _this.len;
  kk_std_core__sslice_drop(_this, _ctx);
  _x25414 = _x1; /*size_t*/
  _match25415: ;
  return kk_std_core__new_Sslice(_x25408, _x25411, _x25414, _ctx);
}

kk_std_core__stream kk_std_core__copy_3(kk_std_core__stream _this, kk_std_core_types__optional head0, kk_std_core_types__optional tail0, kk_context_t* _ctx) { /* forall<a> (stream<a>, head : optional<a>, tail : optional<stream<a>>) -> stream<a> */ 
  kk_box_t _x25419;
  if (kk_std_core_types__is_Optional(head0)) {
    kk_box_t _head_1874 = head0._cons.Optional.value;
    _x25419 = _head_1874; /*1891*/
  }
  else {
    struct kk_std_core_Next* _con25420 = kk_std_core__as_Next(_this);
    kk_box_t _x = _con25420->head;
    kk_box_dup(_x);
    _x25419 = _x; /*1891*/
  }
  kk_std_core__stream _x25421;
  if (kk_std_core_types__is_Optional(tail0)) {
    kk_box_t _box_x23606 = tail0._cons.Optional.value;
    kk_std_core__stream _tail_1881 = kk_std_core__stream_unbox(_box_x23606, NULL);
    kk_std_core__stream_drop(_this, _ctx);
    _x25421 = _tail_1881; /*stream<1891>*/
    goto _match25422;
  }
  struct kk_std_core_Next* _con25424 = kk_std_core__as_Next(_this);
  kk_box_t _pat01 = _con25424->head;
  kk_std_core__stream _x0 = _con25424->tail;
  if (kk_std_core__stream_is_unique(_this)) {
    kk_box_drop(_pat01, _ctx);
    kk_std_core__stream_free(_this);
  }
  else {
    kk_std_core__stream_dup(_x0);
    kk_std_core__stream_decref(_this, _ctx);
  }
  _x25421 = _x0; /*stream<1891>*/
  _match25422: ;
  return kk_std_core__new_Next(kk_reuse_null, _x25419, _x25421, _ctx);
}
 
// runtime tag for the `:exn` effect

kk_std_core_hnd__htag kk_std_core__tag_exn;
 
// handler for the `:exn` effect

kk_box_t kk_std_core__handle_exn(int32_t cfc, kk_std_core__hnd_exn hnd, kk_function_t ret, kk_function_t action, kk_context_t* _ctx) { /* forall<a,e,b> (cfc : int32, hnd : .hnd-exn<e,b>, ret : (res : a) -> e b, action : () -> <exn|e> a) -> e b */ 
  kk_std_core_hnd__htag _x25428 = kk_std_core_hnd__htag_dup(kk_std_core__tag_exn); /*std/core/hnd/htag<.hnd-exn>*/
  return kk_std_core_hnd__hhandle(_x25428, cfc, kk_std_core__hnd_exn_box(hnd, _ctx), ret, action, _ctx);
}
 
// Unsafe: transform any type to a `null` type; used internally by the compiler.

kk_std_core__null kk_std_core__null_any(kk_box_t x, kk_context_t* _ctx) { /* forall<a> (x : a) -> null<a> */ 
  return ((x).box == kk_box_null.box ? kk_datatype_from_ptr(NULL) : kk_datatype_unbox(x));
}

kk_ref_t kk_std_core_redirect;
 
// Print a string to the console, including a final newline character.

kk_unit_t kk_std_core_xprintsln(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> console () */ 
  kk_println(s,kk_context()); return kk_Unit;
}

kk_integer_t kk_std_core_string_compare(kk_string_t x, kk_string_t y, kk_context_t* _ctx) { /* (x : string, y : string) -> int */ 
  return kk_string_cmp_int(x,y,kk_context());
}
 
// Convert an integer to an `:int32`. The number is _clamped_ to the maximal or minimum `:int32`
// value if it is outside the range of an `:int32`.

int32_t kk_std_core_int32(kk_integer_t _arg1, kk_context_t* _ctx) { /* (int) -> int32 */ 
  return kk_integer_clamp32(_arg1,kk_context());
}
 
// Convert an integer to an `:size_t`. The number is _clamped_ to the maximal or minimum `:size_t`
// value if it is outside the range of an `:size_t`.

size_t kk_std_core_size__t(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> size_t */ 
  return kk_integer_clamp_size_t(i,kk_context());
}
 
// Convert a character to a string

kk_string_t kk_std_core_string(kk_char_t c, kk_context_t* _ctx) { /* (c : char) -> string */ 
  return kk_string_from_char(c,kk_context());
}
 
// Convert a vector of characters to a string.

kk_string_t kk_std_core_string_1(kk_vector_t _arg1, kk_context_t* _ctx) { /* (vector<char>) -> string */ 
  return kk_string_from_chars(_arg1,kk_context());
}
 
// Convert a list of characters to a string

kk_string_t kk_std_core_string_2(kk_std_core__list cs, kk_context_t* _ctx) { /* (cs : list<char>) -> total string */ 
  return kk_string_from_list(cs,kk_context());
}
 
// O(n). Copy the `slice` argument into a fresh string.
// Takes O(1) time if the slice covers the entire string.

kk_string_t kk_std_core_string_3(kk_std_core__sslice slice0, kk_context_t* _ctx) { /* (slice : sslice) -> string */ 
  return kk_slice_to_string(slice0,kk_context());
}
 
// Convert a vector to a list with an optional tail.

kk_std_core__list kk_std_core_vlist(kk_vector_t v, kk_std_core_types__optional tail0, kk_context_t* _ctx) { /* forall<a> (v : vector<a>, tail : optional<list<a>>) -> list<a> */ 
  kk_std_core__list _x25431;
  if (kk_std_core_types__is_Optional(tail0)) {
    kk_box_t _box_x23620 = tail0._cons.Optional.value;
    kk_std_core__list _tail_2143 = kk_std_core__list_unbox(_box_x23620, NULL);
    _x25431 = _tail_2143; /*list<2153>*/
    goto _match25432;
  }
  _x25431 = kk_std_core__new_Nil(_ctx); /*list<2153>*/
  _match25432: ;
  return kk_vector_to_list(v,_x25431,kk_context());
}

kk_string_t kk_std_core_int_show_hex(kk_integer_t i, bool use_capitals, kk_context_t* _ctx) { /* (i : int, use-capitals : bool) -> string */ 
  return kk_integer_to_hex_string(i,use_capitals,kk_context());
}

kk_string_t kk_std_core_repeatz(kk_string_t s, size_t n, kk_context_t* _ctx) { /* (s : string, n : size_t) -> string */ 
  return kk_string_repeat(s,n,kk_context());
}

kk_string_t kk_std_core_show_expx(double d, int32_t prec, kk_context_t* _ctx) { /* (d : double, prec : int32) -> string */ 
  return kk_double_show_exp(d,prec,kk_context());
}

kk_string_t kk_std_core_show_fixedx(double d, int32_t prec, kk_context_t* _ctx) { /* (d : double, prec : int32) -> string */ 
  return kk_double_show_fixed(d,prec,kk_context());
}
 
// Print a string to the console

kk_unit_t kk_std_core_xprints(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> console () */ 
  kk_print(s,kk_context()); return kk_Unit;
}
 
// Raise an integer `i` to the power of `exp`.

kk_integer_t kk_std_core_pow(kk_integer_t i, kk_integer_t exp, kk_context_t* _ctx) { /* (i : int, exp : int) -> int */ 
  return kk_integer_pow(i,exp,kk_context());
}
 
// O(`count`). Advance the start position of a string slice by `count` characters
// up to the end of the string.
// A negative `count` advances the start position backwards upto the first position
// in a string.
// Maintains the character count of the original slice upto the end of the string.
// For example:
//
// * `"abc".first.advance(1).string == "b"`,
// * `"abc".first.advance(3).string == ""`,
// * `"abc".last.advance(-1).string == "b"`.
//

kk_std_core__sslice kk_std_core_advance(kk_std_core__sslice slice0, kk_integer_t count, kk_context_t* _ctx) { /* (slice : sslice, count : int) -> sslice */ 
  return kk_slice_advance(slice0,count,kk_context());
}
 
// O(`count`). Extend a string slice by `count` characters up to the end of the string.
// A negative `count` shrinks the slice up to the empty slice.
// For example:
//
// * `"abc".first.extend(1).string == "ab"`
// * `"abc".last.extend(-1).string == ""`
//

kk_std_core__sslice kk_std_core_extend(kk_std_core__sslice slice0, kk_integer_t count, kk_context_t* _ctx) { /* (slice : sslice, count : int) -> sslice */ 
  return kk_slice_extend(slice0,count,kk_context());
}

kk_std_core__sslice kk_std_core_first1(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> sslice */ 
  return kk_slice_first(s,kk_context());
}
 
// Convert a string to upper-case

kk_string_t kk_std_core_to_upper(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> string */ 
  return kk_string_to_upper(s,kk_context());
}

kk_integer_t kk_std_core_cdiv_exp10(kk_integer_t i, kk_integer_t n, kk_context_t* _ctx) { /* (i : int, n : int) -> int */ 
  return kk_integer_cdiv_pow10(i,n,kk_context());
}

kk_integer_t kk_std_core_mul_exp10(kk_integer_t i, kk_integer_t n, kk_context_t* _ctx) { /* (i : int, n : int) -> int */ 
  return kk_integer_mul_pow10(i,n,kk_context());
}
 
// Return the common prefix of two strings (upto `upto` characters (default is minimum length of the two strings))

kk_std_core__sslice kk_std_core_common_prefix(kk_string_t s, kk_string_t t, kk_std_core_types__optional upto, kk_context_t* _ctx) { /* (s : string, t : string, upto : optional<int>) -> sslice */ 
  kk_integer_t _x25435;
  if (kk_std_core_types__is_Optional(upto)) {
    kk_box_t _box_x23621 = upto._cons.Optional.value;
    kk_integer_t _upto_2252 = kk_integer_unbox(_box_x23621);
    _x25435 = _upto_2252; /*int*/
    goto _match25436;
  }
  _x25435 = kk_integer_from_small(-1); /*int*/
  _match25436: ;
  return kk_slice_common_prefix(s,t,_x25435,kk_context());
}
 
// lift

kk_std_core__list kk_std_core__ctail_lift22726_concat(kk_std_core__list ys, kk_std_core__list zss, kk_std_core_types__ctail _acc, kk_context_t* _ctx) { /* forall<a> (ys : list<a>, zss : list<list<a>>, ctail<list<a>>) -> list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con25438 = kk_std_core__as_Cons(ys);
    kk_box_t y = _con25438->head;
    kk_std_core__list yy = _con25438->tail;
    kk_reuse_t _ru_25059 = kk_reuse_null; /*reuse*/;
    if (kk_std_core__list_is_unique(ys)) {
      _ru_25059 = (kk_std_core__list_reuse(ys));
    }
    else {
      kk_box_dup(y);
      kk_std_core__list_dup(yy);
      kk_std_core__list_decref(ys, _ctx);
      _ru_25059 = kk_reuse_null;
    }
    kk_std_core__list _ctail_22788 = kk_std_core__list_hole(); /*list<2300>*/;
    kk_std_core__list _ctail_22789;
    bool _x25439 = _ru_25059!=NULL; /*bool*/
    if (_x25439) {
      struct kk_std_core_Cons* _con25440 = (struct kk_std_core_Cons*)_ru_25059;
      _con25440->tail = _ctail_22788;
      _ctail_22789 = kk_std_core__base_Cons(_con25440); /*list<2300>*/
    }
    else {
      _ctail_22789 = kk_std_core__new_Cons(kk_reuse_null, y, _ctail_22788, _ctx); /*list<2300>*/
    }
    { // tailcall
      kk_std_core_types__ctail _x25441;
      kk_box_t* field_22928 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_22789)->tail)); /*cfield<list<2300>>*/;
      if (kk_std_core_types__is_CTail(_acc)) {
        kk_box_t _box_x23628 = _acc._cons.CTail._field1;
        kk_box_t* last0 = _acc._cons.CTail._field2;
        kk_std_core__list head0 = kk_std_core__list_unbox(_box_x23628, NULL);
        kk_unit_t __ = kk_Unit;
        *(last0) = kk_std_core__list_box(_ctail_22789, _ctx);
        _x25441 = kk_std_core_types__new_CTail(kk_std_core__list_box(head0, _ctx), field_22928, _ctx); /*ctail<58>*/
        goto _match25442;
      }
      _x25441 = kk_std_core_types__new_CTail(kk_std_core__list_box(_ctail_22789, _ctx), field_22928, _ctx); /*ctail<58>*/
      _match25442: ;
      ys = yy;
      _acc = _x25441;
      goto kk__tailcall;
    }
  }
  if (kk_std_core__is_Cons(zss)) {
    struct kk_std_core_Cons* _con25444 = kk_std_core__as_Cons(zss);
    kk_box_t _box_x23637 = _con25444->head;
    kk_std_core__list zzs = _con25444->tail;
    kk_std_core__list zs = kk_std_core__list_unbox(_box_x23637, NULL);
    if (kk_std_core__list_is_unique(zss)) {
      kk_std_core__list_free(zss);
    }
    else {
      kk_std_core__list_dup(zs);
      kk_std_core__list_dup(zzs);
      kk_std_core__list_decref(zss, _ctx);
    }
    { // tailcall
      ys = zs;
      zss = zzs;
      goto kk__tailcall;
    }
  }
  if (kk_std_core_types__is_CTail(_acc)) {
    kk_box_t _box_x23638 = _acc._cons.CTail._field1;
    kk_box_t* last00 = _acc._cons.CTail._field2;
    kk_std_core__list head00 = kk_std_core__list_unbox(_box_x23638, NULL);
    kk_unit_t __0 = kk_Unit;
    *(last00) = kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx);
    return head00;
  }
  return kk_std_core__new_Nil(_ctx);
}
 
// lift

kk_std_core__list kk_std_core__lift22726_concat(kk_std_core__list ys0, kk_std_core__list zss0, kk_context_t* _ctx) { /* forall<a> (ys : list<a>, zss : list<list<a>>) -> list<a> */ 
  return kk_std_core__ctail_lift22726_concat(ys0, zss0, kk_std_core_types__new_CTailNil(_ctx), _ctx);
}
extern kk_box_t kk_std_core_const_fun25447_1(kk_function_t _fself, kk_box_t __l123_c7, kk_context_t* _ctx) {
  struct kk_std_core_const_fun25447__t_1* _self = kk_function_as(struct kk_std_core_const_fun25447__t_1*, _fself);
  kk_box_t default0 = _self->default0; /* 2328 */
  kk_drop_match(_self, {kk_box_dup(default0);}, {}, _ctx)
  kk_box_drop(__l123_c7, _ctx);
  return default0;
}
 
// If the slice is not empty,
// return the first character, and a new slice that is advanced by 1.

kk_std_core_types__maybe kk_std_core_next(kk_std_core__sslice slice0, kk_context_t* _ctx) { /* (slice : sslice) -> maybe<(char, sslice)> */ 
  return kk_slice_next(slice0,kk_context());
}
 
// Return the number of decimal digits of `i`. Return `0` when `i==0`.

kk_integer_t kk_std_core_count_digits(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> int */ 
  return kk_integer_count_digits(i,kk_context());
}
 
// Transform an `:error` type to an `:either` value.

kk_std_core_types__either kk_std_core_either(kk_std_core__error t, kk_context_t* _ctx) { /* forall<a> (t : error<a>) -> either<exception,a> */ 
  if (kk_std_core__is_Error(t)) {
    kk_std_core__exception exn0 = t._cons.Error.exception;
    kk_std_core__exception_dup(exn0);
    kk_std_core__error_drop(t, _ctx);
    return kk_std_core_types__new_Left(kk_std_core__exception_box(exn0, _ctx), _ctx);
  }
  kk_box_t x = t._cons.Ok.result;
  return kk_std_core_types__new_Right(x, _ctx);
}

bool kk_std_core_xends_with(kk_string_t s, kk_string_t post, kk_context_t* _ctx) { /* (s : string, post : string) -> bool */ 
  return kk_string_ends_with(s,post,kk_context());
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift22849_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_std_core__list zz, kk_std_core__list ys1_22746, kk_context_t* _ctx) { /* forall<a,b,e> (ctail<list<b>>, f : (a) -> e list<b>, zz : list<a>, ys1.22746 : list<b>) -> e list<b> */ 
  return kk_std_core__ctail_lift22727_flatmap(f, ys1_22746, zz, _acc, _ctx);
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift22850_op(kk_function_t _accm, kk_function_t f0, kk_std_core__list zz0, kk_std_core__list ys1_227460, kk_context_t* _ctx) { /* forall<a,b,e> ((list<b>) -> list<b>, f : (a) -> e list<b>, zz : list<a>, ys1.22746 : list<b>) -> e list<b> */ 
  return kk_std_core__ctailm_lift22727_flatmap(f0, ys1_227460, zz0, _accm, _ctx);
}
 
// lift


// lift anonymous function
struct kk_std_core__ctail_lift22727_flatmap_fun25457__t {
  struct kk_function_s _base;
  kk_function_t f1;
  kk_std_core__list zz1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_lift22727_flatmap_fun25457(kk_function_t _fself, kk_box_t _b_23665, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_lift22727_flatmap_fun25457(kk_function_t f1, kk_std_core__list zz1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_lift22727_flatmap_fun25457__t* _self = kk_function_alloc_as(struct kk_std_core__ctail_lift22727_flatmap_fun25457__t, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_lift22727_flatmap_fun25457, kk_context());
  _self->f1 = f1;
  _self->zz1 = zz1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_lift22727_flatmap_fun25457(kk_function_t _fself, kk_box_t _b_23665, kk_context_t* _ctx) {
  struct kk_std_core__ctail_lift22727_flatmap_fun25457__t* _self = kk_function_as(struct kk_std_core__ctail_lift22727_flatmap_fun25457__t*, _fself);
  kk_function_t f1 = _self->f1; /* (2430) -> 2432 list<2431> */
  kk_std_core__list zz1 = _self->zz1; /* list<2430> */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<2431>> */
  kk_drop_match(_self, {kk_function_dup(f1);kk_std_core__list_dup(zz1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  kk_std_core__list _x25458;
  kk_std_core__list _x25459 = kk_std_core__list_unbox(_b_23665, _ctx); /*list<2431>*/
  _x25458 = kk_std_core__mlift22849_op(_acc0, f1, zz1, _x25459, _ctx); /*list<2431>*/
  return kk_std_core__list_box(_x25458, _ctx);
}

kk_std_core__list kk_std_core__ctail_lift22727_flatmap(kk_function_t f1, kk_std_core__list ys, kk_std_core__list zs, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,b,e> (f : (a) -> e list<b>, ys : list<b>, zs : list<a>, ctail<list<b>>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con25448 = kk_std_core__as_Cons(ys);
    kk_box_t y = _con25448->head;
    kk_std_core__list yy = _con25448->tail;
    kk_reuse_t _ru_25061 = kk_reuse_null; /*reuse*/;
    if (kk_std_core__list_is_unique(ys)) {
      _ru_25061 = (kk_std_core__list_reuse(ys));
    }
    else {
      kk_box_dup(y);
      kk_std_core__list_dup(yy);
      kk_std_core__list_decref(ys, _ctx);
      _ru_25061 = kk_reuse_null;
    }
    kk_std_core__list _ctail_22790 = kk_std_core__list_hole(); /*list<2431>*/;
    kk_std_core__list _ctail_22791;
    bool _x25449 = _ru_25061!=NULL; /*bool*/
    if (_x25449) {
      struct kk_std_core_Cons* _con25450 = (struct kk_std_core_Cons*)_ru_25061;
      _con25450->tail = _ctail_22790;
      _ctail_22791 = kk_std_core__base_Cons(_con25450); /*list<2431>*/
    }
    else {
      _ctail_22791 = kk_std_core__new_Cons(kk_reuse_null, y, _ctail_22790, _ctx); /*list<2431>*/
    }
    { // tailcall
      kk_std_core_types__ctail _x25451;
      kk_box_t* field_22933 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_22791)->tail)); /*cfield<list<2431>>*/;
      if (kk_std_core_types__is_CTail(_acc0)) {
        kk_box_t _box_x23655 = _acc0._cons.CTail._field1;
        kk_box_t* last0 = _acc0._cons.CTail._field2;
        kk_std_core__list head0 = kk_std_core__list_unbox(_box_x23655, NULL);
        kk_unit_t __ = kk_Unit;
        *(last0) = kk_std_core__list_box(_ctail_22791, _ctx);
        _x25451 = kk_std_core_types__new_CTail(kk_std_core__list_box(head0, _ctx), field_22933, _ctx); /*ctail<58>*/
        goto _match25452;
      }
      _x25451 = kk_std_core_types__new_CTail(kk_std_core__list_box(_ctail_22791, _ctx), field_22933, _ctx); /*ctail<58>*/
      _match25452: ;
      ys = yy;
      _acc0 = _x25451;
      goto kk__tailcall;
    }
  }
  if (kk_std_core__is_Cons(zs)) {
    struct kk_std_core_Cons* _con25454 = kk_std_core__as_Cons(zs);
    kk_box_t z = _con25454->head;
    kk_std_core__list zz1 = _con25454->tail;
    if (kk_std_core__list_is_unique(zs)) {
      kk_std_core__list_free(zs);
    }
    else {
      kk_box_dup(z);
      kk_std_core__list_dup(zz1);
      kk_std_core__list_decref(zs, _ctx);
    }
    kk_std_core__list x_22934;
    kk_function_t _x25455 = kk_function_dup(f1); /*(2430) -> 2432 list<2431>*/
    x_22934 = kk_function_call(kk_std_core__list, (kk_function_t, kk_box_t, kk_context_t*), _x25455, (_x25455, z, _ctx)); /*list<2431>*/
    if (kk_yielding(kk_context())) {
      kk_std_core__list_drop(x_22934, _ctx);
      kk_box_t _x25456 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_lift22727_flatmap_fun25457(f1, zz1, _acc0, _ctx), _ctx); /*3991*/
      return kk_std_core__list_unbox(_x25456, _ctx);
    }
    { // tailcall
      ys = x_22934;
      zs = zz1;
      goto kk__tailcall;
    }
  }
  kk_function_drop(f1, _ctx);
  if (kk_std_core_types__is_CTail(_acc0)) {
    kk_box_t _box_x23666 = _acc0._cons.CTail._field1;
    kk_box_t* last00 = _acc0._cons.CTail._field2;
    kk_std_core__list head00 = kk_std_core__list_unbox(_box_x23666, NULL);
    kk_unit_t __0 = kk_Unit;
    *(last00) = kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx);
    return head00;
  }
  return kk_std_core__new_Nil(_ctx);
}
 
// lift


// lift anonymous function
struct kk_std_core__ctailm_lift22727_flatmap_fun25463__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t y0;
};
static kk_std_core__list kk_std_core__ctailm_lift22727_flatmap_fun25463(kk_function_t _fself, kk_std_core__list _ctail_22793, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_lift22727_flatmap_fun25463(kk_function_t _accm0, kk_box_t y0, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift22727_flatmap_fun25463__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_lift22727_flatmap_fun25463__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_lift22727_flatmap_fun25463, kk_context());
  _self->_accm0 = _accm0;
  _self->y0 = y0;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_lift22727_flatmap_fun25463(kk_function_t _fself, kk_std_core__list _ctail_22793, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift22727_flatmap_fun25463__t* _self = kk_function_as(struct kk_std_core__ctailm_lift22727_flatmap_fun25463__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<2431>) -> list<2431> */
  kk_box_t y0 = _self->y0; /* 2431 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(y0);}, {}, _ctx)
  kk_std_core__list _x25464 = kk_std_core__new_Cons(kk_reuse_null, y0, _ctail_22793, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x25464, _ctx));
}


// lift anonymous function
struct kk_std_core__ctailm_lift22727_flatmap_fun25468__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t f2;
  kk_std_core__list zz2;
};
static kk_box_t kk_std_core__ctailm_lift22727_flatmap_fun25468(kk_function_t _fself, kk_box_t _b_23678, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_lift22727_flatmap_fun25468(kk_function_t _accm0, kk_function_t f2, kk_std_core__list zz2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift22727_flatmap_fun25468__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_lift22727_flatmap_fun25468__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_lift22727_flatmap_fun25468, kk_context());
  _self->_accm0 = _accm0;
  _self->f2 = f2;
  _self->zz2 = zz2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_lift22727_flatmap_fun25468(kk_function_t _fself, kk_box_t _b_23678, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift22727_flatmap_fun25468__t* _self = kk_function_as(struct kk_std_core__ctailm_lift22727_flatmap_fun25468__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<2431>) -> list<2431> */
  kk_function_t f2 = _self->f2; /* (2430) -> 2432 list<2431> */
  kk_std_core__list zz2 = _self->zz2; /* list<2430> */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(f2);kk_std_core__list_dup(zz2);}, {}, _ctx)
  kk_std_core__list _x25469;
  kk_std_core__list _x25470 = kk_std_core__list_unbox(_b_23678, _ctx); /*list<2431>*/
  _x25469 = kk_std_core__mlift22850_op(_accm0, f2, zz2, _x25470, _ctx); /*list<2431>*/
  return kk_std_core__list_box(_x25469, _ctx);
}

kk_std_core__list kk_std_core__ctailm_lift22727_flatmap(kk_function_t f2, kk_std_core__list ys0, kk_std_core__list zs0, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,b,e> (f : (a) -> e list<b>, ys : list<b>, zs : list<a>, (list<b>) -> list<b>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys0)) {
    struct kk_std_core_Cons* _con25461 = kk_std_core__as_Cons(ys0);
    kk_box_t y0 = _con25461->head;
    kk_std_core__list yy0 = _con25461->tail;
    if (kk_std_core__list_is_unique(ys0)) {
      kk_std_core__list_free(ys0);
    }
    else {
      kk_box_dup(y0);
      kk_std_core__list_dup(yy0);
      kk_std_core__list_decref(ys0, _ctx);
    }
    { // tailcall
      kk_function_t _x25462 = kk_std_core__new_ctailm_lift22727_flatmap_fun25463(_accm0, y0, _ctx); /*(list<2431>) -> list<2431>*/
      ys0 = yy0;
      _accm0 = _x25462;
      goto kk__tailcall;
    }
  }
  if (kk_std_core__is_Cons(zs0)) {
    struct kk_std_core_Cons* _con25465 = kk_std_core__as_Cons(zs0);
    kk_box_t z0 = _con25465->head;
    kk_std_core__list zz2 = _con25465->tail;
    if (kk_std_core__list_is_unique(zs0)) {
      kk_std_core__list_free(zs0);
    }
    else {
      kk_box_dup(z0);
      kk_std_core__list_dup(zz2);
      kk_std_core__list_decref(zs0, _ctx);
    }
    kk_std_core__list x_22940;
    kk_function_t _x25466 = kk_function_dup(f2); /*(2430) -> 2432 list<2431>*/
    x_22940 = kk_function_call(kk_std_core__list, (kk_function_t, kk_box_t, kk_context_t*), _x25466, (_x25466, z0, _ctx)); /*list<2431>*/
    if (kk_yielding(kk_context())) {
      kk_std_core__list_drop(x_22940, _ctx);
      kk_box_t _x25467 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_lift22727_flatmap_fun25468(_accm0, f2, zz2, _ctx), _ctx); /*3991*/
      return kk_std_core__list_unbox(_x25467, _ctx);
    }
    { // tailcall
      ys0 = x_22940;
      zs0 = zz2;
      goto kk__tailcall;
    }
  }
  kk_function_drop(f2, _ctx);
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, kk_std_core__new_Nil(_ctx), _ctx));
}
 
// lift


// lift anonymous function
struct kk_std_core__lift22727_flatmap_fun25471__t {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core__lift22727_flatmap_fun25471(kk_function_t _fself, kk_std_core__list _ctail_22792, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift22727_flatmap_fun25471(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core__lift22727_flatmap_fun25471, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core__lift22727_flatmap_fun25471(kk_function_t _fself, kk_std_core__list _ctail_22792, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  return _ctail_22792;
}

kk_std_core__list kk_std_core__lift22727_flatmap(kk_function_t f3, kk_std_core__list ys1, kk_std_core__list zs1, kk_context_t* _ctx) { /* forall<a,b,e> (f : (a) -> e list<b>, ys : list<b>, zs : list<a>) -> e list<b> */ 
  bool _match_25381 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_25381) {
    return kk_std_core__ctail_lift22727_flatmap(f3, ys1, zs1, kk_std_core_types__new_CTailNil(_ctx), _ctx);
  }
  return kk_std_core__ctailm_lift22727_flatmap(f3, ys1, zs1, kk_std_core__new_lift22727_flatmap_fun25471(_ctx), _ctx);
}
 
// lift

kk_std_core__list kk_std_core__lift22728_reverse_append(kk_std_core__list acc, kk_std_core__list ys, kk_context_t* _ctx) { /* forall<a> (acc : list<a>, ys : list<a>) -> list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con25472 = kk_std_core__as_Cons(ys);
    kk_box_t x = _con25472->head;
    kk_std_core__list xx = _con25472->tail;
    kk_reuse_t _ru_25065 = kk_reuse_null; /*reuse*/;
    if (kk_std_core__list_is_unique(ys)) {
      _ru_25065 = (kk_std_core__list_reuse(ys));
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(ys, _ctx);
      _ru_25065 = kk_reuse_null;
    }
    { // tailcall
      kk_std_core__list _x25473;
      bool _x25474 = _ru_25065!=NULL; /*bool*/
      if (_x25474) {
        struct kk_std_core_Cons* _con25475 = (struct kk_std_core_Cons*)_ru_25065;
        _con25475->tail = acc;
        _x25473 = kk_std_core__base_Cons(_con25475); /*list<61>*/
      }
      else {
        _x25473 = kk_std_core__new_Cons(kk_reuse_null, x, acc, _ctx); /*list<61>*/
      }
      acc = _x25473;
      ys = xx;
      goto kk__tailcall;
    }
  }
  return acc;
}
 
// monadic lift

kk_box_t kk_std_core__mlift22851_force(kk_ref_t r, kk_box_t x0, kk_context_t* _ctx) { /* forall<a,e> (r : ref<global,either<() -> e a,a>>, x0 : a) -> <st<global>,div|e> a */ 
  kk_unit_t __ = kk_Unit;
  kk_std_core_types__either _b_23684_23682;
  kk_box_t _x25476 = kk_box_dup(x0); /*2619*/
  _b_23684_23682 = kk_std_core_types__new_Right(_x25476, _ctx); /*either<() -> 2620 2619,2619>*/
  kk_ref_set(r,(kk_std_core_types__either_box(_b_23684_23682, _ctx)),kk_context());
  return x0;
}
 
// Force a delayed value; the value is computed only on the first
// call to `force` and cached afterwards.


// lift anonymous function
struct kk_std_core_force_fun25480__t {
  struct kk_function_s _base;
  kk_std_core__delayed delayed;
};
static kk_box_t kk_std_core_force_fun25480(kk_function_t _fself, kk_box_t x00, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_force_fun25480(kk_std_core__delayed delayed, kk_context_t* _ctx) {
  struct kk_std_core_force_fun25480__t* _self = kk_function_alloc_as(struct kk_std_core_force_fun25480__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_force_fun25480, kk_context());
  _self->delayed = delayed;
  return &_self->_base;
}

static kk_box_t kk_std_core_force_fun25480(kk_function_t _fself, kk_box_t x00, kk_context_t* _ctx) {
  struct kk_std_core_force_fun25480__t* _self = kk_function_as(struct kk_std_core_force_fun25480__t*, _fself);
  kk_std_core__delayed delayed = _self->delayed; /* delayed<2620,2619> */
  kk_drop_match(_self, {kk_std_core__delayed_dup(delayed);}, {}, _ctx)
  kk_ref_t _x25481;
  kk_ref_t _x0 = delayed.dref;
  _x25481 = _x0; /*ref<global,either<() -> 2620 2619,2619>>*/
  return kk_std_core__mlift22851_force(_x25481, x00, _ctx);
}

kk_box_t kk_std_core_force(kk_std_core__delayed delayed, kk_context_t* _ctx) { /* forall<a,e> (delayed : delayed<e,a>) -> e a */ 
  kk_std_core_types__either _match_25379;
  kk_box_t _x25477;
  kk_ref_t _x25478;
  kk_ref_t _x = delayed.dref;
  kk_ref_dup(_x);
  _x25478 = _x; /*ref<global,either<() -> 2620 2619,2619>>*/
  _x25477 = kk_ref_get(_x25478,kk_context()); /*179*/
  _match_25379 = kk_std_core_types__either_unbox(_x25477, _ctx); /*either<() -> 2620 2619,2619>*/
  if (kk_std_core_types__is_Right(_match_25379)) {
    kk_box_t x = _match_25379._cons.Right.right;
    kk_std_core__delayed_drop(delayed, _ctx);
    return x;
  }
  kk_box_t _fun_unbox_x23687 = _match_25379._cons.Left.left;
  kk_box_t x0_22946;
  kk_function_t _x25479 = kk_function_unbox(_fun_unbox_x23687); /*() -> 2620 23688*/
  x0_22946 = kk_std_core_hnd__mask_builtin(_x25479, _ctx); /*2619*/
  kk_function_t next_22947 = kk_std_core_new_force_fun25480(delayed, _ctx); /*(2619) -> <st<global>,div|2620> 2619*/;
  if (kk_yielding(kk_context())) {
    kk_box_drop(x0_22946, _ctx);
    return kk_std_core_hnd_yield_extend(next_22947, _ctx);
  }
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), next_22947, (next_22947, x0_22946, _ctx));
}
 
// Generic show: shows the internal representation of an object as a string
// Note: this breaks parametricity so it should not be public

kk_string_t kk_std_core_gshow(kk_box_t _arg1, kk_context_t* _ctx) { /* forall<a> (a) -> string */ 
  return kk_show_any(_arg1,kk_context());
}
 
// Return the host environment: `dotnet`, `browser`, `webworker`, `node`, or `libc`.

kk_string_t kk_std_core_host(kk_context_t* _ctx) { /* () -> ndet string */ 
  return kk_get_host(kk_context());
}
 
// lift

kk_std_core__list kk_std_core__ctail_lift22729_intersperse(kk_std_core__list ys, kk_box_t s, kk_std_core_types__ctail _acc, kk_context_t* _ctx) { /* forall<a> (ys : list<a>, s : a, ctail<list<a>>) -> list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con25482 = kk_std_core__as_Cons(ys);
    kk_box_t y = _con25482->head;
    kk_std_core__list yy = _con25482->tail;
    kk_reuse_t _ru_25066 = kk_reuse_null; /*reuse*/;
    if (kk_std_core__list_is_unique(ys)) {
      _ru_25066 = (kk_std_core__list_reuse(ys));
    }
    else {
      kk_box_dup(y);
      kk_std_core__list_dup(yy);
      kk_std_core__list_decref(ys, _ctx);
      _ru_25066 = kk_reuse_null;
    }
    kk_std_core__list _ctail_22794;
    kk_std_core__list _ru_25385 = kk_std_core__list_hole(); /*list<2646>*/;
    bool _x25483 = _ru_25066!=NULL; /*bool*/
    if (_x25483) {
      struct kk_std_core_Cons* _con25484 = (struct kk_std_core_Cons*)_ru_25066;
      _con25484->tail = _ru_25385;
      _ctail_22794 = kk_std_core__base_Cons(_con25484); /*list<2646>*/
    }
    else {
      _ctail_22794 = kk_std_core__new_Cons(kk_reuse_null, y, _ru_25385, _ctx); /*list<2646>*/
    }
    kk_std_core__list _ctail_22795;
    kk_box_t _x25485 = kk_box_dup(s); /*2646*/
    kk_std_core__list _x25486 = kk_std_core__list_dup(_ctail_22794); /*list<2646>*/
    _ctail_22795 = kk_std_core__new_Cons(kk_reuse_null, _x25485, _x25486, _ctx); /*list<2646>*/
    { // tailcall
      kk_std_core_types__ctail _x25487;
      kk_box_t* field_22950;
      kk_std_core__list_drop(_ctail_22794, _ctx);
      field_22950 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_22794)->tail)); /*cfield<list<2646>>*/
      if (kk_std_core_types__is_CTail(_acc)) {
        kk_box_t _box_x23696 = _acc._cons.CTail._field1;
        kk_box_t* last0 = _acc._cons.CTail._field2;
        kk_std_core__list head0 = kk_std_core__list_unbox(_box_x23696, NULL);
        kk_unit_t __ = kk_Unit;
        *(last0) = kk_std_core__list_box(_ctail_22795, _ctx);
        _x25487 = kk_std_core_types__new_CTail(kk_std_core__list_box(head0, _ctx), field_22950, _ctx); /*ctail<58>*/
        goto _match25488;
      }
      _x25487 = kk_std_core_types__new_CTail(kk_std_core__list_box(_ctail_22795, _ctx), field_22950, _ctx); /*ctail<58>*/
      _match25488: ;
      ys = yy;
      _acc = _x25487;
      goto kk__tailcall;
    }
  }
  kk_box_drop(s, _ctx);
  if (kk_std_core_types__is_CTail(_acc)) {
    kk_box_t _box_x23705 = _acc._cons.CTail._field1;
    kk_box_t* last00 = _acc._cons.CTail._field2;
    kk_std_core__list head00 = kk_std_core__list_unbox(_box_x23705, NULL);
    kk_unit_t __0 = kk_Unit;
    *(last00) = kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx);
    return head00;
  }
  return kk_std_core__new_Nil(_ctx);
}
 
// lift

kk_std_core__list kk_std_core__lift22729_intersperse(kk_std_core__list ys0, kk_box_t s0, kk_context_t* _ctx) { /* forall<a> (ys : list<a>, s : a) -> list<a> */ 
  return kk_std_core__ctail_lift22729_intersperse(ys0, s0, kk_std_core_types__new_CTailNil(_ctx), _ctx);
}
 
// Insert a separator `sep`  between all elements of a list `xs` .

kk_std_core__list kk_std_core_intersperse(kk_std_core__list xs, kk_box_t sep, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, sep : a) -> list<a> */ 
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con25491 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con25491->head;
    kk_std_core__list xx = _con25491->tail;
    kk_reuse_t _ru_25067 = kk_reuse_null; /*reuse*/;
    if (kk_std_core__list_is_unique(xs)) {
      _ru_25067 = (kk_std_core__list_reuse(xs));
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
      _ru_25067 = kk_reuse_null;
    }
    kk_std_core__list _ru_25386 = kk_std_core__lift22729_intersperse(xx, sep, _ctx); /*list<2646>*/;
    bool _x25492 = _ru_25067!=NULL; /*bool*/
    if (_x25492) {
      struct kk_std_core_Cons* _con25493 = (struct kk_std_core_Cons*)_ru_25067;
      _con25493->tail = _ru_25386;
      return kk_std_core__base_Cons(_con25493);
    }
    return kk_std_core__new_Cons(kk_reuse_null, x, _ru_25386, _ctx);
  }
  kk_box_drop(sep, _ctx);
  return kk_std_core__new_Nil(_ctx);
}
 
// Return the number of ending `0` digits of `i`. Return `0` when `i==0`.

kk_integer_t kk_std_core_is_exp10(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> int */ 
  return kk_integer_ctz(i,kk_context());
}

kk_std_core__sslice kk_std_core_last1(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> sslice */ 
  return kk_slice_last(s,kk_context());
}
 
// Used by the compiler to wrap main console applications

kk_box_t kk_std_core_main_console(kk_function_t main, kk_context_t* _ctx) { /* forall<a,e> (main : () -> e a) -> e a */ 
  return kk_main_console(main,kk_context());
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift22852_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_std_core__list yy, kk_box_t _ctail_22796, kk_context_t* _ctx) { /* forall<a,b,e> (ctail<list<b>>, f : (value : a, rest : list<a>) -> e b, yy : list<a>, b) -> e list<b> */ 
  kk_std_core__list _ctail_22797 = kk_std_core__list_hole(); /*list<2747>*/;
  kk_std_core__list _ctail_22798 = kk_std_core__new_Cons(kk_reuse_null, _ctail_22796, _ctail_22797, _ctx); /*list<2747>*/;
  kk_std_core_types__ctail _x25495;
  kk_box_t* field_22955 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_22798)->tail)); /*cfield<list<2747>>*/;
  if (kk_std_core_types__is_CTail(_acc)) {
    kk_box_t _box_x23720 = _acc._cons.CTail._field1;
    kk_box_t* last0 = _acc._cons.CTail._field2;
    kk_std_core__list head0 = kk_std_core__list_unbox(_box_x23720, NULL);
    kk_unit_t __ = kk_Unit;
    *(last0) = kk_std_core__list_box(_ctail_22798, _ctx);
    _x25495 = kk_std_core_types__new_CTail(kk_std_core__list_box(head0, _ctx), field_22955, _ctx); /*ctail<58>*/
    goto _match25496;
  }
  _x25495 = kk_std_core_types__new_CTail(kk_std_core__list_box(_ctail_22798, _ctx), field_22955, _ctx); /*ctail<58>*/
  _match25496: ;
  return kk_std_core__ctail_lift22730_map_peek(f, yy, _x25495, _ctx);
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift22853_op_fun25498__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t _ctail_22801;
};
static kk_std_core__list kk_std_core__mlift22853_op_fun25498(kk_function_t _fself, kk_std_core__list _ctail_22800, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift22853_op_fun25498(kk_function_t _accm, kk_box_t _ctail_22801, kk_context_t* _ctx) {
  struct kk_std_core__mlift22853_op_fun25498__t* _self = kk_function_alloc_as(struct kk_std_core__mlift22853_op_fun25498__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift22853_op_fun25498, kk_context());
  _self->_accm = _accm;
  _self->_ctail_22801 = _ctail_22801;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift22853_op_fun25498(kk_function_t _fself, kk_std_core__list _ctail_22800, kk_context_t* _ctx) {
  struct kk_std_core__mlift22853_op_fun25498__t* _self = kk_function_as(struct kk_std_core__mlift22853_op_fun25498__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<2747>) -> list<2747> */
  kk_box_t _ctail_22801 = _self->_ctail_22801; /* 2747 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(_ctail_22801);}, {}, _ctx)
  kk_std_core__list _x25499 = kk_std_core__new_Cons(kk_reuse_null, _ctail_22801, _ctail_22800, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x25499, _ctx));
}

kk_std_core__list kk_std_core__mlift22853_op(kk_function_t _accm, kk_function_t f0, kk_std_core__list yy0, kk_box_t _ctail_22801, kk_context_t* _ctx) { /* forall<a,b,e> ((list<b>) -> list<b>, f : (value : a, rest : list<a>) -> e b, yy : list<a>, b) -> e list<b> */ 
  return kk_std_core__ctailm_lift22730_map_peek(f0, yy0, kk_std_core__new_mlift22853_op_fun25498(_accm, _ctail_22801, _ctx), _ctx);
}
 
// lift


// lift anonymous function
struct kk_std_core__ctail_lift22730_map_peek_fun25504__t {
  struct kk_function_s _base;
  kk_function_t f1;
  kk_std_core__list yy1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_lift22730_map_peek_fun25504(kk_function_t _fself, kk_box_t _b_23734, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_lift22730_map_peek_fun25504(kk_function_t f1, kk_std_core__list yy1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_lift22730_map_peek_fun25504__t* _self = kk_function_alloc_as(struct kk_std_core__ctail_lift22730_map_peek_fun25504__t, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_lift22730_map_peek_fun25504, kk_context());
  _self->f1 = f1;
  _self->yy1 = yy1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_lift22730_map_peek_fun25504(kk_function_t _fself, kk_box_t _b_23734, kk_context_t* _ctx) {
  struct kk_std_core__ctail_lift22730_map_peek_fun25504__t* _self = kk_function_as(struct kk_std_core__ctail_lift22730_map_peek_fun25504__t*, _fself);
  kk_function_t f1 = _self->f1; /* (value : 2746, rest : list<2746>) -> 2748 2747 */
  kk_std_core__list yy1 = _self->yy1; /* list<2746> */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<2747>> */
  kk_drop_match(_self, {kk_function_dup(f1);kk_std_core__list_dup(yy1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  kk_std_core__list _x25505 = kk_std_core__mlift22852_op(_acc0, f1, yy1, _b_23734, _ctx); /*list<2747>*/
  return kk_std_core__list_box(_x25505, _ctx);
}

kk_std_core__list kk_std_core__ctail_lift22730_map_peek(kk_function_t f1, kk_std_core__list ys, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,b,e> (f : (value : a, rest : list<a>) -> e b, ys : list<a>, ctail<list<b>>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con25500 = kk_std_core__as_Cons(ys);
    kk_box_t y = _con25500->head;
    kk_std_core__list yy1 = _con25500->tail;
    kk_reuse_t _ru_25068 = kk_reuse_null; /*reuse*/;
    if (kk_std_core__list_is_unique(ys)) {
      _ru_25068 = (kk_std_core__list_reuse(ys));
    }
    else {
      kk_box_dup(y);
      kk_std_core__list_dup(yy1);
      kk_std_core__list_decref(ys, _ctx);
      _ru_25068 = kk_reuse_null;
    }
    kk_box_t x_22956;
    kk_function_t _x25502 = kk_function_dup(f1); /*(value : 2746, rest : list<2746>) -> 2748 2747*/
    kk_std_core__list _x25501 = kk_std_core__list_dup(yy1); /*list<2746>*/
    x_22956 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_std_core__list, kk_context_t*), _x25502, (_x25502, y, _x25501, _ctx)); /*2747*/
    if (kk_yielding(kk_context())) {
      kk_reuse_drop(_ru_25068, _ctx);
      kk_box_drop(x_22956, _ctx);
      kk_box_t _x25503 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_lift22730_map_peek_fun25504(f1, yy1, _acc0, _ctx), _ctx); /*3991*/
      return kk_std_core__list_unbox(_x25503, _ctx);
    }
    kk_std_core__list _ctail_227970 = kk_std_core__list_hole(); /*list<2747>*/;
    kk_std_core__list _ctail_227980 = kk_std_core__new_Cons(_ru_25068, x_22956, _ctail_227970, _ctx); /*list<2747>*/;
    { // tailcall
      kk_std_core_types__ctail _x25506;
      kk_box_t* field_22962 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_227980)->tail)); /*cfield<list<2747>>*/;
      if (kk_std_core_types__is_CTail(_acc0)) {
        kk_box_t _box_x23741 = _acc0._cons.CTail._field1;
        kk_box_t* last2 = _acc0._cons.CTail._field2;
        kk_std_core__list head1 = kk_std_core__list_unbox(_box_x23741, NULL);
        kk_unit_t __0 = kk_Unit;
        *(last2) = kk_std_core__list_box(_ctail_227980, _ctx);
        _x25506 = kk_std_core_types__new_CTail(kk_std_core__list_box(head1, _ctx), field_22962, _ctx); /*ctail<58>*/
        goto _match25507;
      }
      _x25506 = kk_std_core_types__new_CTail(kk_std_core__list_box(_ctail_227980, _ctx), field_22962, _ctx); /*ctail<58>*/
      _match25507: ;
      ys = yy1;
      _acc0 = _x25506;
      goto kk__tailcall;
    }
  }
  kk_function_drop(f1, _ctx);
  if (kk_std_core_types__is_CTail(_acc0)) {
    kk_box_t _box_x23750 = _acc0._cons.CTail._field1;
    kk_box_t* last00 = _acc0._cons.CTail._field2;
    kk_std_core__list head00 = kk_std_core__list_unbox(_box_x23750, NULL);
    kk_unit_t __00 = kk_Unit;
    *(last00) = kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx);
    return head00;
  }
  return kk_std_core__new_Nil(_ctx);
}
 
// lift


// lift anonymous function
struct kk_std_core__ctailm_lift22730_map_peek_fun25514__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t f2;
  kk_std_core__list yy2;
};
static kk_box_t kk_std_core__ctailm_lift22730_map_peek_fun25514(kk_function_t _fself, kk_box_t _b_23762, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_lift22730_map_peek_fun25514(kk_function_t _accm0, kk_function_t f2, kk_std_core__list yy2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift22730_map_peek_fun25514__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_lift22730_map_peek_fun25514__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_lift22730_map_peek_fun25514, kk_context());
  _self->_accm0 = _accm0;
  _self->f2 = f2;
  _self->yy2 = yy2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_lift22730_map_peek_fun25514(kk_function_t _fself, kk_box_t _b_23762, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift22730_map_peek_fun25514__t* _self = kk_function_as(struct kk_std_core__ctailm_lift22730_map_peek_fun25514__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<2747>) -> list<2747> */
  kk_function_t f2 = _self->f2; /* (value : 2746, rest : list<2746>) -> 2748 2747 */
  kk_std_core__list yy2 = _self->yy2; /* list<2746> */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(f2);kk_std_core__list_dup(yy2);}, {}, _ctx)
  kk_std_core__list _x25515 = kk_std_core__mlift22853_op(_accm0, f2, yy2, _b_23762, _ctx); /*list<2747>*/
  return kk_std_core__list_box(_x25515, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_lift22730_map_peek_fun25517__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t x_22965;
};
static kk_std_core__list kk_std_core__ctailm_lift22730_map_peek_fun25517(kk_function_t _fself, kk_std_core__list _ctail_228000, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_lift22730_map_peek_fun25517(kk_function_t _accm0, kk_box_t x_22965, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift22730_map_peek_fun25517__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_lift22730_map_peek_fun25517__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_lift22730_map_peek_fun25517, kk_context());
  _self->_accm0 = _accm0;
  _self->x_22965 = x_22965;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_lift22730_map_peek_fun25517(kk_function_t _fself, kk_std_core__list _ctail_228000, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_lift22730_map_peek_fun25517__t* _self = kk_function_as(struct kk_std_core__ctailm_lift22730_map_peek_fun25517__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<2747>) -> list<2747> */
  kk_box_t x_22965 = _self->x_22965; /* 2747 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(x_22965);}, {}, _ctx)
  kk_std_core__list _x25518 = kk_std_core__new_Cons(kk_reuse_null, x_22965, _ctail_228000, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x25518, _ctx));
}

kk_std_core__list kk_std_core__ctailm_lift22730_map_peek(kk_function_t f2, kk_std_core__list ys0, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,b,e> (f : (value : a, rest : list<a>) -> e b, ys : list<a>, (list<b>) -> list<b>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys0)) {
    struct kk_std_core_Cons* _con25510 = kk_std_core__as_Cons(ys0);
    kk_box_t y0 = _con25510->head;
    kk_std_core__list yy2 = _con25510->tail;
    if (kk_std_core__list_is_unique(ys0)) {
      kk_std_core__list_free(ys0);
    }
    else {
      kk_box_dup(y0);
      kk_std_core__list_dup(yy2);
      kk_std_core__list_decref(ys0, _ctx);
    }
    kk_box_t x_22965;
    kk_function_t _x25512 = kk_function_dup(f2); /*(value : 2746, rest : list<2746>) -> 2748 2747*/
    kk_std_core__list _x25511 = kk_std_core__list_dup(yy2); /*list<2746>*/
    x_22965 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_std_core__list, kk_context_t*), _x25512, (_x25512, y0, _x25511, _ctx)); /*2747*/
    if (kk_yielding(kk_context())) {
      kk_box_drop(x_22965, _ctx);
      kk_box_t _x25513 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_lift22730_map_peek_fun25514(_accm0, f2, yy2, _ctx), _ctx); /*3991*/
      return kk_std_core__list_unbox(_x25513, _ctx);
    }
    { // tailcall
      kk_function_t _x25516 = kk_std_core__new_ctailm_lift22730_map_peek_fun25517(_accm0, x_22965, _ctx); /*(list<2747>) -> list<2747>*/
      ys0 = yy2;
      _accm0 = _x25516;
      goto kk__tailcall;
    }
  }
  kk_function_drop(f2, _ctx);
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, kk_std_core__new_Nil(_ctx), _ctx));
}
 
// lift


// lift anonymous function
struct kk_std_core__lift22730_map_peek_fun25519__t {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core__lift22730_map_peek_fun25519(kk_function_t _fself, kk_std_core__list _ctail_22799, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift22730_map_peek_fun25519(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core__lift22730_map_peek_fun25519, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core__lift22730_map_peek_fun25519(kk_function_t _fself, kk_std_core__list _ctail_22799, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  return _ctail_22799;
}

kk_std_core__list kk_std_core__lift22730_map_peek(kk_function_t f3, kk_std_core__list ys1, kk_context_t* _ctx) { /* forall<a,b,e> (f : (value : a, rest : list<a>) -> e b, ys : list<a>) -> e list<b> */ 
  bool _match_25376 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_25376) {
    return kk_std_core__ctail_lift22730_map_peek(f3, ys1, kk_std_core_types__new_CTailNil(_ctx), _ctx);
  }
  return kk_std_core__ctailm_lift22730_map_peek(f3, ys1, kk_std_core__new_lift22730_map_peek_fun25519(_ctx), _ctx);
}

kk_integer_t kk_std_core_maxListStack;

kk_ref_t kk_std_core_trace_enabled;
 
// Compose two funs `f` and `g`.


// lift anonymous function
struct kk_std_core_o_fun25521__t {
  struct kk_function_s _base;
  kk_function_t f;
  kk_function_t g;
};
static kk_box_t kk_std_core_o_fun25521(kk_function_t _fself, kk_box_t x, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_o_fun25521(kk_function_t f, kk_function_t g, kk_context_t* _ctx) {
  struct kk_std_core_o_fun25521__t* _self = kk_function_alloc_as(struct kk_std_core_o_fun25521__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_o_fun25521, kk_context());
  _self->f = f;
  _self->g = g;
  return &_self->_base;
}

static kk_box_t kk_std_core_o_fun25521(kk_function_t _fself, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_o_fun25521__t* _self = kk_function_as(struct kk_std_core_o_fun25521__t*, _fself);
  kk_function_t f = _self->f; /* (2831) -> 2828 2829 */
  kk_function_t g = _self->g; /* (2827) -> 2828 2831 */
  kk_drop_match(_self, {kk_function_dup(f);kk_function_dup(g);}, {}, _ctx)
  kk_box_t x0_22970 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), g, (g, x, _ctx)); /*2831*/;
  if (kk_yielding(kk_context())) {
    kk_box_drop(x0_22970, _ctx);
    return kk_std_core_hnd_yield_extend(f, _ctx);
  }
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, x0_22970, _ctx));
}

kk_function_t kk_std_core_o(kk_function_t f, kk_function_t g, kk_context_t* _ctx) { /* forall<a,b,c,e> (f : (a) -> e b, g : (c) -> e a) -> ((x : c) -> e b) */ 
  return kk_std_core_new_o_fun25521(f, g, _ctx);
}
 
// Given a total function to calculate a value `:a`, return
// a total function that only calculates the value once and then
// returns the cached result.


// lift anonymous function
struct kk_std_core_once_fun25524__t {
  struct kk_function_s _base;
  kk_function_t calc;
  kk_ref_t r;
};
static kk_box_t kk_std_core_once_fun25524(kk_function_t _fself, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_once_fun25524(kk_function_t calc, kk_ref_t r, kk_context_t* _ctx) {
  struct kk_std_core_once_fun25524__t* _self = kk_function_alloc_as(struct kk_std_core_once_fun25524__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_once_fun25524, kk_context());
  _self->calc = calc;
  _self->r = r;
  return &_self->_base;
}

static kk_box_t kk_std_core_once_fun25524(kk_function_t _fself, kk_context_t* _ctx) {
  struct kk_std_core_once_fun25524__t* _self = kk_function_as(struct kk_std_core_once_fun25524__t*, _fself);
  kk_function_t calc = _self->calc; /* () -> 2954 */
  kk_ref_t r = _self->r; /* ref<_2860,maybe<2954>> */
  kk_drop_match(_self, {kk_function_dup(calc);kk_ref_dup(r);}, {}, _ctx)
  kk_std_core_types__maybe _match_25374;
  kk_box_t _x25525;
  kk_ref_t _x25526 = kk_ref_dup(r); /*ref<_2860,maybe<2954>>*/
  _x25525 = kk_ref_get(_x25526,kk_context()); /*179*/
  _match_25374 = kk_std_core_types__maybe_unbox(_x25525, _ctx); /*maybe<2954>*/
  if (kk_std_core_types__is_Just(_match_25374)) {
    kk_box_t x = _match_25374._cons.Just.value;
    kk_function_drop(calc, _ctx);
    kk_ref_drop(r, _ctx);
    return x;
  }
  kk_box_t x0 = kk_function_call(kk_box_t, (kk_function_t, kk_context_t*), calc, (calc, _ctx)); /*2954*/;
  kk_unit_t __ = kk_Unit;
  kk_std_core_types__maybe _b_23774_23772;
  kk_box_t _x25527 = kk_box_dup(x0); /*2954*/
  _b_23774_23772 = kk_std_core_types__new_Just(_x25527, _ctx); /*maybe<2954>*/
  kk_ref_set(r,(kk_std_core_types__maybe_box(_b_23774_23772, _ctx)),kk_context());
  return x0;
}

kk_function_t kk_std_core_once(kk_function_t calc, kk_context_t* _ctx) { /* forall<a> (calc : () -> a) -> (() -> a) */ 
  kk_ref_t r = kk_ref_alloc((kk_std_core_types__maybe_box(kk_std_core_types__new_Nothing(_ctx), _ctx)),kk_context()); /*ref<_2860,maybe<2954>>*/;
  return kk_std_core_new_once_fun25524(calc, r, _ctx);
}

kk_std_core_types__maybe kk_std_core_xparse_int(kk_string_t s, bool hex, kk_context_t* _ctx) { /* (s : string, hex : bool) -> maybe<int> */ 
  return kk_integer_xparse(s,hex,kk_context());
}

kk_box_t kk_std_core_phantom(kk_context_t* _ctx) { /* forall<a> () -> a */ 
  return kk_box_null;
}
 
// Convert a string to lower-case

kk_string_t kk_std_core_to_lower(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> string */ 
  return kk_string_to_lower(s,kk_context());
}

kk_unit_t kk_std_core_xtrace(kk_string_t message0, kk_context_t* _ctx) { /* (message : string) -> () */ 
  kk_trace(message0,kk_context()); return kk_Unit;
}

kk_unit_t kk_std_core_xtrace_any(kk_string_t message0, kk_box_t x, kk_context_t* _ctx) { /* forall<a> (message : string, x : a) -> () */ 
  kk_trace_any(message0,x,kk_context()); return kk_Unit;
}

kk_ref_t kk_std_core_unique_count;

kk_vector_t kk_std_core_unvlist(kk_std_core__list xs, kk_context_t* _ctx) { /* forall<a> (xs : list<a>) -> vector<a> */ 
  return kk_list_to_vector(xs,kk_context());
}
 
// Create a new vector of length `n`  with initial elements given by function `f` .

kk_vector_t kk_std_core_vector_initz(size_t n, kk_function_t f, kk_context_t* _ctx) { /* forall<a> (n : size_t, f : (size_t) -> a) -> vector<a> */ 
  return kk_vector_init(n,f,kk_context());
}
 
// Append two lists.

kk_std_core__list kk_std_core__ctail_append(kk_std_core__list xs, kk_std_core__list ys, kk_std_core_types__ctail _acc, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, ys : list<a>, ctail<list<a>>) -> list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con25534 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con25534->head;
    kk_std_core__list xx = _con25534->tail;
    kk_reuse_t _ru_25070 = kk_reuse_null; /*reuse*/;
    if (kk_std_core__list_is_unique(xs)) {
      _ru_25070 = (kk_std_core__list_reuse(xs));
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
      _ru_25070 = kk_reuse_null;
    }
    kk_std_core__list _ctail_22802 = kk_std_core__list_hole(); /*list<3441>*/;
    kk_std_core__list _ctail_22803;
    bool _x25535 = _ru_25070!=NULL; /*bool*/
    if (_x25535) {
      struct kk_std_core_Cons* _con25536 = (struct kk_std_core_Cons*)_ru_25070;
      _con25536->tail = _ctail_22802;
      _ctail_22803 = kk_std_core__base_Cons(_con25536); /*list<3441>*/
    }
    else {
      _ctail_22803 = kk_std_core__new_Cons(kk_reuse_null, x, _ctail_22802, _ctx); /*list<3441>*/
    }
    { // tailcall
      kk_std_core_types__ctail _x25537;
      kk_box_t* field_22977 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_22803)->tail)); /*cfield<list<3441>>*/;
      if (kk_std_core_types__is_CTail(_acc)) {
        kk_box_t _box_x23784 = _acc._cons.CTail._field1;
        kk_box_t* last0 = _acc._cons.CTail._field2;
        kk_std_core__list head0 = kk_std_core__list_unbox(_box_x23784, NULL);
        kk_unit_t __ = kk_Unit;
        *(last0) = kk_std_core__list_box(_ctail_22803, _ctx);
        _x25537 = kk_std_core_types__new_CTail(kk_std_core__list_box(head0, _ctx), field_22977, _ctx); /*ctail<58>*/
        goto _match25538;
      }
      _x25537 = kk_std_core_types__new_CTail(kk_std_core__list_box(_ctail_22803, _ctx), field_22977, _ctx); /*ctail<58>*/
      _match25538: ;
      xs = xx;
      _acc = _x25537;
      goto kk__tailcall;
    }
  }
  kk_box_t _x25540 = kk_std_core_types__ctail_resolve(_acc, kk_std_core__list_box(ys, _ctx), _ctx); /*-1*/
  return kk_std_core__list_unbox(_x25540, _ctx);
}
 
// Append two lists.

kk_std_core__list kk_std_core_append(kk_std_core__list xs0, kk_std_core__list ys0, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, ys : list<a>) -> list<a> */ 
  return kk_std_core__ctail_append(xs0, ys0, kk_std_core_types__new_CTailNil(_ctx), _ctx);
}
 
// Append two strings.

kk_string_t kk_std_core__lp__plus__6_rp_(kk_string_t x, kk_string_t y, kk_context_t* _ctx) { /* (x : string, y : string) -> string */ 
  return kk_string_cat(x,y,kk_context());
}

kk_unit_t kk_std_core_printsln(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> console () */ 
  kk_std_core_types__maybe _match_25373;
  kk_box_t _x25546;
  kk_ref_t _x25547 = kk_ref_dup(kk_std_core_redirect); /*ref<global,maybe<(string) -> console ()>>*/
  _x25546 = kk_ref_get(_x25547,kk_context()); /*179*/
  _match_25373 = kk_std_core_types__maybe_unbox(_x25546, _ctx); /*maybe<(string) -> console ()>*/
  if (kk_std_core_types__is_Nothing(_match_25373)) {
    kk_std_core_xprintsln(s, _ctx); return kk_Unit;
  }
  kk_box_t _fun_unbox_x23805 = _match_25373._cons.Just.value;
  kk_string_t _b_23810_23808;
  kk_string_t _x25548;
  kk_define_string_literal(, _s25549, 1, "\n")
  _x25548 = kk_string_dup(_s25549); /*string*/
  _b_23810_23808 = kk_std_core__lp__plus__6_rp_(s, _x25548, _ctx); /*string*/
  kk_box_t _x25550;
  kk_function_t _x25551 = kk_function_unbox(_fun_unbox_x23805); /*(23806) -> console 23807*/
  _x25550 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x25551, (_x25551, kk_string_box(_b_23810_23808), _ctx)); /*23807*/
  kk_unit_unbox(_x25550); return kk_Unit;
}

kk_std_core_types__order kk_std_core_order(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> order */ 
  bool _match_25370;
  kk_integer_t _x25556 = kk_integer_dup(i); /*int*/
  _match_25370 = kk_integer_lt(_x25556,(kk_integer_from_small(0)),kk_context()); /*bool*/
  if (_match_25370) {
    kk_integer_drop(i, _ctx);
    return kk_std_core_types__new_Lt(_ctx);
  }
  bool _match_25371 = kk_integer_gt(i,(kk_integer_from_small(0)),kk_context()); /*bool*/;
  if (_match_25371) {
    return kk_std_core_types__new_Gt(_ctx);
  }
  return kk_std_core_types__new_Eq(_ctx);
}
 
// Compare two strings.
// Uses the character codes directly for comparison

kk_std_core_types__order kk_std_core_compare_4(kk_string_t x, kk_string_t y, kk_context_t* _ctx) { /* (x : string, y : string) -> order */ 
  kk_integer_t i_22978 = kk_std_core_string_compare(x, y, _ctx); /*int*/;
  bool _match_25368;
  kk_integer_t _x25557 = kk_integer_dup(i_22978); /*int*/
  _match_25368 = kk_integer_lt(_x25557,(kk_integer_from_small(0)),kk_context()); /*bool*/
  if (_match_25368) {
    kk_integer_drop(i_22978, _ctx);
    return kk_std_core_types__new_Lt(_ctx);
  }
  bool _match_25369 = kk_integer_gt(i_22978,(kk_integer_from_small(0)),kk_context()); /*bool*/;
  if (_match_25369) {
    return kk_std_core_types__new_Gt(_ctx);
  }
  return kk_std_core_types__new_Eq(_ctx);
}

bool kk_std_core__lp__lt__7_rp_(kk_string_t x, kk_string_t y, kk_context_t* _ctx) { /* (x : string, y : string) -> bool */ 
  kk_std_core_types__order x0_22979 = kk_std_core_compare_4(x, y, _ctx); /*order*/;
  kk_integer_t _x25558 = kk_std_core_int_5(x0_22979, _ctx); /*int*/
  kk_integer_t _x25559;
  kk_std_core_types__order _x25560 = kk_std_core_types__new_Lt(_ctx); /*order*/
  if (kk_std_core_types__is_Lt(_x25560)) {
    _x25559 = kk_integer_sub((kk_integer_from_small(0)),(kk_integer_from_small(1)),kk_context()); /*int*/
    goto _match25561;
  }
  if (kk_std_core_types__is_Eq(_x25560)) {
    _x25559 = kk_integer_from_small(0); /*int*/
    goto _match25561;
  }
  _x25559 = kk_integer_from_small(1); /*int*/
  _match25561: ;
  return kk_integer_eq(_x25558,_x25559,kk_context());
}

bool kk_std_core__lp__gt__5_rp_(kk_string_t x, kk_string_t y, kk_context_t* _ctx) { /* (x : string, y : string) -> bool */ 
  kk_std_core_types__order x0_22981 = kk_std_core_compare_4(x, y, _ctx); /*order*/;
  kk_integer_t _x25564 = kk_std_core_int_5(x0_22981, _ctx); /*int*/
  kk_integer_t _x25565;
  kk_std_core_types__order _x25566 = kk_std_core_types__new_Gt(_ctx); /*order*/
  if (kk_std_core_types__is_Lt(_x25566)) {
    _x25565 = kk_integer_sub((kk_integer_from_small(0)),(kk_integer_from_small(1)),kk_context()); /*int*/
    goto _match25567;
  }
  if (kk_std_core_types__is_Eq(_x25566)) {
    _x25565 = kk_integer_from_small(0); /*int*/
    goto _match25567;
  }
  _x25565 = kk_integer_from_small(1); /*int*/
  _match25567: ;
  return kk_integer_eq(_x25564,_x25565,kk_context());
}

bool kk_std_core__lp__gt__eq__5_rp_(kk_string_t x, kk_string_t y, kk_context_t* _ctx) { /* (x : string, y : string) -> bool */ 
  kk_std_core_types__order x0_22983 = kk_std_core_compare_4(x, y, _ctx); /*order*/;
  kk_integer_t _x25571 = kk_std_core_int_5(x0_22983, _ctx); /*int*/
  kk_integer_t _x25572;
  kk_std_core_types__order _x25573 = kk_std_core_types__new_Lt(_ctx); /*order*/
  if (kk_std_core_types__is_Lt(_x25573)) {
    _x25572 = kk_integer_sub((kk_integer_from_small(0)),(kk_integer_from_small(1)),kk_context()); /*int*/
    goto _match25574;
  }
  if (kk_std_core_types__is_Eq(_x25573)) {
    _x25572 = kk_integer_from_small(0); /*int*/
    goto _match25574;
  }
  _x25572 = kk_integer_from_small(1); /*int*/
  _match25574: ;
  return kk_integer_gt(_x25571,_x25572,kk_context());
}
 
// lift

kk_string_t kk_std_core__lift22731_joinsep(kk_string_t sep, kk_std_core__list ys, kk_string_t acc, kk_context_t* _ctx) { /* (sep : string, ys : list<string>, acc : string) -> string */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con25575 = kk_std_core__as_Cons(ys);
    kk_box_t _box_x23811 = _con25575->head;
    kk_std_core__list yy = _con25575->tail;
    kk_string_t y = kk_string_unbox(_box_x23811);
    if (kk_std_core__list_is_unique(ys)) {
      kk_std_core__list_free(ys);
    }
    else {
      kk_string_dup(y);
      kk_std_core__list_dup(yy);
      kk_std_core__list_decref(ys, _ctx);
    }
    kk_string_t acc0_22753;
    kk_string_t _x25577;
    kk_string_t _x25578 = kk_string_dup(sep); /*string*/
    _x25577 = kk_std_core__lp__plus__6_rp_(acc, _x25578, _ctx); /*string*/
    acc0_22753 = kk_std_core__lp__plus__6_rp_(_x25577, y, _ctx); /*string*/
    { // tailcall
      ys = yy;
      acc = acc0_22753;
      goto kk__tailcall;
    }
  }
  kk_string_drop(sep, _ctx);
  return acc;
}
 
// Concatenate all strings in a list

kk_string_t kk_std_core_joinsep(kk_std_core__list xs, kk_string_t sep, kk_context_t* _ctx) { /* (xs : list<string>, sep : string) -> string */ 
  if (kk_std_core__is_Nil(xs)) {
    kk_string_drop(sep, _ctx);
    return kk_string_empty();
  }
  struct kk_std_core_Cons* _con25580 = kk_std_core__as_Cons(xs);
  kk_box_t _box_x23812 = _con25580->head;
  kk_std_core__list xx = _con25580->tail;
  kk_string_t x = kk_string_unbox(_box_x23812);
  if (kk_std_core__list_is_unique(xs)) {
    kk_std_core__list_free(xs);
  }
  else {
    kk_string_dup(x);
    kk_std_core__list_dup(xx);
    kk_std_core__list_decref(xs, _ctx);
  }
  return kk_std_core__lift22731_joinsep(sep, xx, x, _ctx);
}
 
// Concatenate all strings in a list

kk_string_t kk_std_core_join_2(kk_std_core__list xs, kk_context_t* _ctx) { /* (xs : list<string>) -> string */ 
  if (kk_std_core__is_Nil(xs)) {
    return kk_string_empty();
  }
  struct kk_std_core_Cons* _con25583 = kk_std_core__as_Cons(xs);
  kk_box_t _box_x23813 = _con25583->head;
  kk_std_core__list xx = _con25583->tail;
  kk_string_t x = kk_string_unbox(_box_x23813);
  if (kk_std_core__list_is_unique(xs)) {
    kk_std_core__list_free(xs);
  }
  else {
    kk_string_dup(x);
    kk_std_core__list_dup(xx);
    kk_std_core__list_decref(xs, _ctx);
  }
  kk_string_t _x25585 = kk_string_empty(); /*string*/
  return kk_std_core__lift22731_joinsep(_x25585, xx, x, _ctx);
}

size_t kk_std_core_decr_1(size_t i, kk_context_t* _ctx) { /* (i : size_t) -> size_t */ 
  return (i - 1);
}

bool kk_std_core__lp__lt__eq__7_rp_(kk_string_t x, kk_string_t y, kk_context_t* _ctx) { /* (x : string, y : string) -> bool */ 
  kk_std_core_types__order x0_22987 = kk_std_core_compare_4(x, y, _ctx); /*order*/;
  kk_integer_t _x25592 = kk_std_core_int_5(x0_22987, _ctx); /*int*/
  kk_integer_t _x25593;
  kk_std_core_types__order _x25594 = kk_std_core_types__new_Gt(_ctx); /*order*/
  if (kk_std_core_types__is_Lt(_x25594)) {
    _x25593 = kk_integer_sub((kk_integer_from_small(0)),(kk_integer_from_small(1)),kk_context()); /*int*/
    goto _match25595;
  }
  if (kk_std_core_types__is_Eq(_x25594)) {
    _x25593 = kk_integer_from_small(0); /*int*/
    goto _match25595;
  }
  _x25593 = kk_integer_from_small(1); /*int*/
  _match25595: ;
  return kk_integer_lt(_x25592,_x25593,kk_context());
}

size_t kk_std_core_incr_1(size_t i, kk_context_t* _ctx) { /* (i : size_t) -> size_t */ 
  return (i + 1);
}
 
// monadic lift

kk_unit_t kk_std_core__mlift22854_op(kk_function_t action, size_t end, size_t i, kk_unit_t wild__, kk_context_t* _ctx) { /* forall<e> (action : (size_t) -> e (), end : size_t, i : size_t, wild_ : ()) -> e () */ 
  size_t i0_22754 = kk_std_core_incr_1(i, _ctx); /*size_t*/;
  kk_std_core__lift22732_forz(action, end, i0_22754, _ctx); return kk_Unit;
}
 
// lift


// lift anonymous function
struct kk_std_core__lift22732_forz_fun25598__t {
  struct kk_function_s _base;
  kk_function_t action0;
  size_t end0;
  size_t i0;
};
static kk_box_t kk_std_core__lift22732_forz_fun25598(kk_function_t _fself, kk_box_t _b_23815, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift22732_forz_fun25598(kk_function_t action0, size_t end0, size_t i0, kk_context_t* _ctx) {
  struct kk_std_core__lift22732_forz_fun25598__t* _self = kk_function_alloc_as(struct kk_std_core__lift22732_forz_fun25598__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift22732_forz_fun25598, kk_context());
  _self->action0 = action0;
  _self->end0 = end0;
  _self->i0 = i0;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift22732_forz_fun25598(kk_function_t _fself, kk_box_t _b_23815, kk_context_t* _ctx) {
  struct kk_std_core__lift22732_forz_fun25598__t* _self = kk_function_as(struct kk_std_core__lift22732_forz_fun25598__t*, _fself);
  kk_function_t action0 = _self->action0; /* (size_t) -> 5684 () */
  size_t end0 = _self->end0; /* size_t */
  size_t i0 = _self->i0; /* size_t */
  kk_drop_match(_self, {kk_function_dup(action0);;;}, {}, _ctx)
  kk_unit_t _x25599 = kk_Unit;
  kk_unit_t _x25600 = kk_Unit;
  kk_unit_unbox(_b_23815);
  kk_std_core__mlift22854_op(action0, end0, i0, _x25600, _ctx);
  return kk_unit_box(_x25599);
}

kk_unit_t kk_std_core__lift22732_forz(kk_function_t action0, size_t end0, size_t i0, kk_context_t* _ctx) { /* forall<e> (action : (size_t) -> e (), end : size_t, i : size_t) -> e () */ 
  kk__tailcall: ;
  bool _match_25356 = (i0 <= end0); /*bool*/;
  if (_match_25356) {
    kk_unit_t x_22989 = kk_Unit;
    kk_function_t _x25596 = kk_function_dup(action0); /*(size_t) -> 5684 ()*/
    kk_function_call(kk_unit_t, (kk_function_t, size_t, kk_context_t*), _x25596, (_x25596, i0, _ctx));
    if (kk_yielding(kk_context())) {
      kk_box_t _x25597 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift22732_forz_fun25598(action0, end0, i0, _ctx), _ctx); /*3991*/
      kk_unit_unbox(_x25597); return kk_Unit;
    }
    size_t i0_227540 = kk_std_core_incr_1(i0, _ctx); /*size_t*/;
    { // tailcall
      i0 = i0_227540;
      goto kk__tailcall;
    }
  }
  kk_function_drop(action0, _ctx);
  kk_Unit; return kk_Unit;
}


// lift anonymous function
struct kk_std_core_foreach_indexedz_fun25603__t {
  struct kk_function_s _base;
  kk_function_t f;
  kk_vector_t v;
};
static kk_unit_t kk_std_core_foreach_indexedz_fun25603(kk_function_t _fself, size_t i, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_indexedz_fun25603(kk_function_t f, kk_vector_t v, kk_context_t* _ctx) {
  struct kk_std_core_foreach_indexedz_fun25603__t* _self = kk_function_alloc_as(struct kk_std_core_foreach_indexedz_fun25603__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_foreach_indexedz_fun25603, kk_context());
  _self->f = f;
  _self->v = v;
  return &_self->_base;
}

static kk_unit_t kk_std_core_foreach_indexedz_fun25603(kk_function_t _fself, size_t i, kk_context_t* _ctx) {
  struct kk_std_core_foreach_indexedz_fun25603__t* _self = kk_function_as(struct kk_std_core_foreach_indexedz_fun25603__t*, _fself);
  kk_function_t f = _self->f; /* (5781, size_t) -> 5782 () */
  kk_vector_t v = _self->v; /* vector<5781> */
  kk_drop_match(_self, {kk_function_dup(f);kk_vector_dup(v);}, {}, _ctx)
  kk_box_t _x25604 = kk_vector_at(v,i); /*216*/
  return kk_function_call(kk_unit_t, (kk_function_t, kk_box_t, size_t, kk_context_t*), f, (f, _x25604, i, _ctx));
}

kk_unit_t kk_std_core_foreach_indexedz(kk_vector_t v, kk_function_t f, kk_context_t* _ctx) { /* forall<a,e> (v : vector<a>, f : (a, size_t) -> e ()) -> e () */ 
  size_t start_22993 = ((size_t)0); /*size_t*/;
  size_t end_22994;
  size_t _x25601;
  kk_vector_t _x25602 = kk_vector_dup(v); /*vector<5781>*/
  _x25601 = kk_vector_len(_x25602); /*size_t*/
  end_22994 = kk_std_core_decr_1(_x25601, _ctx); /*size_t*/
  kk_std_core__lift22732_forz(kk_std_core_new_foreach_indexedz_fun25603(f, v, _ctx), end_22994, start_22993, _ctx); return kk_Unit;
}
 
// lift

kk_integer_t kk_std_core__lift22733_length_1(kk_std_core__list ys, kk_integer_t acc, kk_context_t* _ctx) { /* forall<a> (ys : list<a>, acc : int) -> int */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con25605 = kk_std_core__as_Cons(ys);
    kk_box_t _pat0 = _con25605->head;
    kk_std_core__list yy = _con25605->tail;
    if (kk_std_core__list_is_unique(ys)) {
      kk_box_drop(_pat0, _ctx);
      kk_std_core__list_free(ys);
    }
    else {
      kk_std_core__list_dup(yy);
      kk_std_core__list_decref(ys, _ctx);
    }
    { // tailcall
      kk_integer_t _x25606 = kk_integer_add(acc,(kk_integer_from_small(1)),kk_context()); /*int*/
      ys = yy;
      acc = _x25606;
      goto kk__tailcall;
    }
  }
  return acc;
}
 
// lift

kk_std_core__list kk_std_core__lift22734_list(kk_integer_t low, kk_integer_t high, kk_std_core__list acc, kk_context_t* _ctx) { /* (low : int, high : int, acc : list<int>) -> list<int> */ 
  kk__tailcall: ;
  bool _match_25355;
  kk_integer_t _x25608 = kk_integer_dup(low); /*int*/
  kk_integer_t _x25609 = kk_integer_dup(high); /*int*/
  _match_25355 = kk_integer_gt(_x25608,_x25609,kk_context()); /*bool*/
  if (_match_25355) {
    kk_integer_drop(high, _ctx);
    kk_integer_drop(low, _ctx);
    return acc;
  }
  { // tailcall
    kk_integer_t _x25610;
    kk_integer_t _x25611 = kk_integer_dup(high); /*int*/
    _x25610 = kk_integer_sub(_x25611,(kk_integer_from_small(1)),kk_context()); /*int*/
    kk_std_core__list _x25612 = kk_std_core__new_Cons(kk_reuse_null, kk_integer_box(high), acc, _ctx); /*list<61>*/
    high = _x25610;
    acc = _x25612;
    goto kk__tailcall;
  }
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift22855_op(kk_std_core__list acc, kk_function_t f, kk_integer_t high0_22757, kk_integer_t low, kk_box_t _y_46, kk_context_t* _ctx) { /* forall<a,e> (acc : list<a>, f : (int) -> e a, high0.22757 : int, low : int, a) -> e list<a> */ 
  kk_std_core__list acc0_22758 = kk_std_core__new_Cons(kk_reuse_null, _y_46, acc, _ctx); /*list<6191>*/;
  return kk_std_core__lift22735_list_1(f, low, high0_22757, acc0_22758, _ctx);
}
 
// lift


// lift anonymous function
struct kk_std_core__lift22735_list_fun25618__t_1 {
  struct kk_function_s _base;
  kk_std_core__list acc0;
  kk_function_t f0;
  kk_integer_t high0_227570;
  kk_integer_t low0;
};
static kk_box_t kk_std_core__lift22735_list_fun25618_1(kk_function_t _fself, kk_box_t _b_23823, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift22735_list_fun25618_1(kk_std_core__list acc0, kk_function_t f0, kk_integer_t high0_227570, kk_integer_t low0, kk_context_t* _ctx) {
  struct kk_std_core__lift22735_list_fun25618__t_1* _self = kk_function_alloc_as(struct kk_std_core__lift22735_list_fun25618__t_1, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift22735_list_fun25618_1, kk_context());
  _self->acc0 = acc0;
  _self->f0 = f0;
  _self->high0_227570 = high0_227570;
  _self->low0 = low0;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift22735_list_fun25618_1(kk_function_t _fself, kk_box_t _b_23823, kk_context_t* _ctx) {
  struct kk_std_core__lift22735_list_fun25618__t_1* _self = kk_function_as(struct kk_std_core__lift22735_list_fun25618__t_1*, _fself);
  kk_std_core__list acc0 = _self->acc0; /* list<6191> */
  kk_function_t f0 = _self->f0; /* (int) -> 6192 6191 */
  kk_integer_t high0_227570 = _self->high0_227570; /* int */
  kk_integer_t low0 = _self->low0; /* int */
  kk_drop_match(_self, {kk_std_core__list_dup(acc0);kk_function_dup(f0);kk_integer_dup(high0_227570);kk_integer_dup(low0);}, {}, _ctx)
  kk_std_core__list _x25619 = kk_std_core__mlift22855_op(acc0, f0, high0_227570, low0, _b_23823, _ctx); /*list<6191>*/
  return kk_std_core__list_box(_x25619, _ctx);
}

kk_std_core__list kk_std_core__lift22735_list_1(kk_function_t f0, kk_integer_t low0, kk_integer_t high, kk_std_core__list acc0, kk_context_t* _ctx) { /* forall<a,e> (f : (int) -> e a, low : int, high : int, acc : list<a>) -> e list<a> */ 
  kk__tailcall: ;
  bool _match_25353;
  kk_integer_t _x25613 = kk_integer_dup(low0); /*int*/
  kk_integer_t _x25614 = kk_integer_dup(high); /*int*/
  _match_25353 = kk_integer_gt(_x25613,_x25614,kk_context()); /*bool*/
  if (_match_25353) {
    kk_function_drop(f0, _ctx);
    kk_integer_drop(high, _ctx);
    kk_integer_drop(low0, _ctx);
    return acc0;
  }
  kk_integer_t high0_227570;
  kk_integer_t _x25615 = kk_integer_dup(high); /*int*/
  high0_227570 = kk_integer_sub(_x25615,(kk_integer_from_small(1)),kk_context()); /*int*/
  kk_box_t x_22996;
  kk_function_t _x25616 = kk_function_dup(f0); /*(int) -> 6192 6191*/
  x_22996 = kk_function_call(kk_box_t, (kk_function_t, kk_integer_t, kk_context_t*), _x25616, (_x25616, high, _ctx)); /*6191*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x_22996, _ctx);
    kk_box_t _x25617 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift22735_list_fun25618_1(acc0, f0, high0_227570, low0, _ctx), _ctx); /*3991*/
    return kk_std_core__list_unbox(_x25617, _ctx);
  }
  kk_std_core__list acc0_227580 = kk_std_core__new_Cons(kk_reuse_null, x_22996, acc0, _ctx); /*list<6191>*/;
  { // tailcall
    high = high0_227570;
    acc0 = acc0_227580;
    goto kk__tailcall;
  }
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift22856_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_std_core__list xx, kk_box_t _ctail_22804, kk_context_t* _ctx) { /* forall<a,b,e> (ctail<list<b>>, f : (a) -> e b, xx : list<a>, b) -> e list<b> */ 
  kk_std_core__list _ctail_22805 = kk_std_core__list_hole(); /*list<7496>*/;
  kk_std_core__list _ctail_22806 = kk_std_core__new_Cons(kk_reuse_null, _ctail_22804, _ctail_22805, _ctx); /*list<7496>*/;
  kk_std_core_types__ctail _x25620;
  kk_box_t* field_23002 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_22806)->tail)); /*cfield<list<7496>>*/;
  if (kk_std_core_types__is_CTail(_acc)) {
    kk_box_t _box_x23832 = _acc._cons.CTail._field1;
    kk_box_t* last0 = _acc._cons.CTail._field2;
    kk_std_core__list head0 = kk_std_core__list_unbox(_box_x23832, NULL);
    kk_unit_t __ = kk_Unit;
    *(last0) = kk_std_core__list_box(_ctail_22806, _ctx);
    _x25620 = kk_std_core_types__new_CTail(kk_std_core__list_box(head0, _ctx), field_23002, _ctx); /*ctail<58>*/
    goto _match25621;
  }
  _x25620 = kk_std_core_types__new_CTail(kk_std_core__list_box(_ctail_22806, _ctx), field_23002, _ctx); /*ctail<58>*/
  _match25621: ;
  return kk_std_core__ctail_map_5(xx, f, _x25620, _ctx);
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift22857_op_fun25623__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t _ctail_22809;
};
static kk_std_core__list kk_std_core__mlift22857_op_fun25623(kk_function_t _fself, kk_std_core__list _ctail_22808, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift22857_op_fun25623(kk_function_t _accm, kk_box_t _ctail_22809, kk_context_t* _ctx) {
  struct kk_std_core__mlift22857_op_fun25623__t* _self = kk_function_alloc_as(struct kk_std_core__mlift22857_op_fun25623__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift22857_op_fun25623, kk_context());
  _self->_accm = _accm;
  _self->_ctail_22809 = _ctail_22809;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift22857_op_fun25623(kk_function_t _fself, kk_std_core__list _ctail_22808, kk_context_t* _ctx) {
  struct kk_std_core__mlift22857_op_fun25623__t* _self = kk_function_as(struct kk_std_core__mlift22857_op_fun25623__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<7496>) -> list<7496> */
  kk_box_t _ctail_22809 = _self->_ctail_22809; /* 7496 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(_ctail_22809);}, {}, _ctx)
  kk_std_core__list _x25624 = kk_std_core__new_Cons(kk_reuse_null, _ctail_22809, _ctail_22808, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x25624, _ctx));
}

kk_std_core__list kk_std_core__mlift22857_op(kk_function_t _accm, kk_function_t f0, kk_std_core__list xx0, kk_box_t _ctail_22809, kk_context_t* _ctx) { /* forall<a,b,e> ((list<b>) -> list<b>, f : (a) -> e b, xx : list<a>, b) -> e list<b> */ 
  return kk_std_core__ctailm_map_5(xx0, f0, kk_std_core__new_mlift22857_op_fun25623(_accm, _ctail_22809, _ctx), _ctx);
}
 
// Apply a function `f`  to each element of the input list in sequence.


// lift anonymous function
struct kk_std_core__ctail_map_fun25628__t_5 {
  struct kk_function_s _base;
  kk_function_t f1;
  kk_std_core__list xx1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_map_fun25628_5(kk_function_t _fself, kk_box_t _b_23846, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_map_fun25628_5(kk_function_t f1, kk_std_core__list xx1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_map_fun25628__t_5* _self = kk_function_alloc_as(struct kk_std_core__ctail_map_fun25628__t_5, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_map_fun25628_5, kk_context());
  _self->f1 = f1;
  _self->xx1 = xx1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_map_fun25628_5(kk_function_t _fself, kk_box_t _b_23846, kk_context_t* _ctx) {
  struct kk_std_core__ctail_map_fun25628__t_5* _self = kk_function_as(struct kk_std_core__ctail_map_fun25628__t_5*, _fself);
  kk_function_t f1 = _self->f1; /* (7495) -> 7497 7496 */
  kk_std_core__list xx1 = _self->xx1; /* list<7495> */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<7496>> */
  kk_drop_match(_self, {kk_function_dup(f1);kk_std_core__list_dup(xx1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  kk_std_core__list _x25629 = kk_std_core__mlift22856_op(_acc0, f1, xx1, _b_23846, _ctx); /*list<7496>*/
  return kk_std_core__list_box(_x25629, _ctx);
}

kk_std_core__list kk_std_core__ctail_map_5(kk_std_core__list xs, kk_function_t f1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, f : (a) -> e b, ctail<list<b>>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con25625 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con25625->head;
    kk_std_core__list xx1 = _con25625->tail;
    kk_reuse_t _ru_25075 = kk_reuse_null; /*reuse*/;
    if (kk_std_core__list_is_unique(xs)) {
      _ru_25075 = (kk_std_core__list_reuse(xs));
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx1);
      kk_std_core__list_decref(xs, _ctx);
      _ru_25075 = kk_reuse_null;
    }
    kk_box_t x0_23003;
    kk_function_t _x25626 = kk_function_dup(f1); /*(7495) -> 7497 7496*/
    x0_23003 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x25626, (_x25626, x, _ctx)); /*7496*/
    if (kk_yielding(kk_context())) {
      kk_reuse_drop(_ru_25075, _ctx);
      kk_box_drop(x0_23003, _ctx);
      kk_box_t _x25627 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_map_fun25628_5(f1, xx1, _acc0, _ctx), _ctx); /*3991*/
      return kk_std_core__list_unbox(_x25627, _ctx);
    }
    kk_std_core__list _ctail_228050 = kk_std_core__list_hole(); /*list<7496>*/;
    kk_std_core__list _ctail_228060 = kk_std_core__new_Cons(_ru_25075, x0_23003, _ctail_228050, _ctx); /*list<7496>*/;
    { // tailcall
      kk_std_core_types__ctail _x25630;
      kk_box_t* field_23009 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_228060)->tail)); /*cfield<list<7496>>*/;
      if (kk_std_core_types__is_CTail(_acc0)) {
        kk_box_t _box_x23853 = _acc0._cons.CTail._field1;
        kk_box_t* last2 = _acc0._cons.CTail._field2;
        kk_std_core__list head1 = kk_std_core__list_unbox(_box_x23853, NULL);
        kk_unit_t __0 = kk_Unit;
        *(last2) = kk_std_core__list_box(_ctail_228060, _ctx);
        _x25630 = kk_std_core_types__new_CTail(kk_std_core__list_box(head1, _ctx), field_23009, _ctx); /*ctail<58>*/
        goto _match25631;
      }
      _x25630 = kk_std_core_types__new_CTail(kk_std_core__list_box(_ctail_228060, _ctx), field_23009, _ctx); /*ctail<58>*/
      _match25631: ;
      xs = xx1;
      _acc0 = _x25630;
      goto kk__tailcall;
    }
  }
  kk_function_drop(f1, _ctx);
  if (kk_std_core_types__is_CTail(_acc0)) {
    kk_box_t _box_x23862 = _acc0._cons.CTail._field1;
    kk_box_t* last00 = _acc0._cons.CTail._field2;
    kk_std_core__list head00 = kk_std_core__list_unbox(_box_x23862, NULL);
    kk_unit_t __00 = kk_Unit;
    *(last00) = kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx);
    return head00;
  }
  return kk_std_core__new_Nil(_ctx);
}
 
// Apply a function `f`  to each element of the input list in sequence.


// lift anonymous function
struct kk_std_core__ctailm_map_fun25637__t_5 {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t f2;
  kk_std_core__list xx2;
};
static kk_box_t kk_std_core__ctailm_map_fun25637_5(kk_function_t _fself, kk_box_t _b_23874, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_map_fun25637_5(kk_function_t _accm0, kk_function_t f2, kk_std_core__list xx2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_map_fun25637__t_5* _self = kk_function_alloc_as(struct kk_std_core__ctailm_map_fun25637__t_5, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_map_fun25637_5, kk_context());
  _self->_accm0 = _accm0;
  _self->f2 = f2;
  _self->xx2 = xx2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_map_fun25637_5(kk_function_t _fself, kk_box_t _b_23874, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_map_fun25637__t_5* _self = kk_function_as(struct kk_std_core__ctailm_map_fun25637__t_5*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<7496>) -> list<7496> */
  kk_function_t f2 = _self->f2; /* (7495) -> 7497 7496 */
  kk_std_core__list xx2 = _self->xx2; /* list<7495> */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(f2);kk_std_core__list_dup(xx2);}, {}, _ctx)
  kk_std_core__list _x25638 = kk_std_core__mlift22857_op(_accm0, f2, xx2, _b_23874, _ctx); /*list<7496>*/
  return kk_std_core__list_box(_x25638, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_map_fun25640__t_5 {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t x0_23012;
};
static kk_std_core__list kk_std_core__ctailm_map_fun25640_5(kk_function_t _fself, kk_std_core__list _ctail_228080, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_map_fun25640_5(kk_function_t _accm0, kk_box_t x0_23012, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_map_fun25640__t_5* _self = kk_function_alloc_as(struct kk_std_core__ctailm_map_fun25640__t_5, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_map_fun25640_5, kk_context());
  _self->_accm0 = _accm0;
  _self->x0_23012 = x0_23012;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_map_fun25640_5(kk_function_t _fself, kk_std_core__list _ctail_228080, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_map_fun25640__t_5* _self = kk_function_as(struct kk_std_core__ctailm_map_fun25640__t_5*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<7496>) -> list<7496> */
  kk_box_t x0_23012 = _self->x0_23012; /* 7496 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(x0_23012);}, {}, _ctx)
  kk_std_core__list _x25641 = kk_std_core__new_Cons(kk_reuse_null, x0_23012, _ctail_228080, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x25641, _ctx));
}

kk_std_core__list kk_std_core__ctailm_map_5(kk_std_core__list xs0, kk_function_t f2, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, f : (a) -> e b, (list<b>) -> list<b>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs0)) {
    struct kk_std_core_Cons* _con25634 = kk_std_core__as_Cons(xs0);
    kk_box_t x0 = _con25634->head;
    kk_std_core__list xx2 = _con25634->tail;
    if (kk_std_core__list_is_unique(xs0)) {
      kk_std_core__list_free(xs0);
    }
    else {
      kk_box_dup(x0);
      kk_std_core__list_dup(xx2);
      kk_std_core__list_decref(xs0, _ctx);
    }
    kk_box_t x0_23012;
    kk_function_t _x25635 = kk_function_dup(f2); /*(7495) -> 7497 7496*/
    x0_23012 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x25635, (_x25635, x0, _ctx)); /*7496*/
    if (kk_yielding(kk_context())) {
      kk_box_drop(x0_23012, _ctx);
      kk_box_t _x25636 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_map_fun25637_5(_accm0, f2, xx2, _ctx), _ctx); /*3991*/
      return kk_std_core__list_unbox(_x25636, _ctx);
    }
    { // tailcall
      kk_function_t _x25639 = kk_std_core__new_ctailm_map_fun25640_5(_accm0, x0_23012, _ctx); /*(list<7496>) -> list<7496>*/
      xs0 = xx2;
      _accm0 = _x25639;
      goto kk__tailcall;
    }
  }
  kk_function_drop(f2, _ctx);
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, kk_std_core__new_Nil(_ctx), _ctx));
}
 
// Apply a function `f`  to each element of the input list in sequence.


// lift anonymous function
struct kk_std_core_map_fun25642__t_5 {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core_map_fun25642_5(kk_function_t _fself, kk_std_core__list _ctail_22807, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun25642_5(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_map_fun25642_5, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core_map_fun25642_5(kk_function_t _fself, kk_std_core__list _ctail_22807, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  return _ctail_22807;
}

kk_std_core__list kk_std_core_map_5(kk_std_core__list xs1, kk_function_t f3, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, f : (a) -> e b) -> e list<b> */ 
  bool _match_25350 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_25350) {
    return kk_std_core__ctail_map_5(xs1, f3, kk_std_core_types__new_CTailNil(_ctx), _ctx);
  }
  return kk_std_core__ctailm_map_5(xs1, f3, kk_std_core_new_map_fun25642_5(_ctx), _ctx);
}
 
// Create a list of characters from `lo`  to `hi`  (inclusive).


// lift anonymous function
struct kk_std_core_list_fun25643__t_2 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_list_fun25643_2(kk_function_t _fself, kk_box_t _b_23879, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_list_fun25643_2(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_list_fun25643_2, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_list_fun25643_2(kk_function_t _fself, kk_box_t _b_23879, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  kk_char_t _x25644;
  kk_integer_t _x25645 = kk_integer_unbox(_b_23879); /*int*/
  _x25644 = kk_integer_clamp32(_x25645,kk_context()); /*char*/
  return kk_char_box(_x25644, _ctx);
}

kk_std_core__list kk_std_core_list_2(kk_char_t lo, kk_char_t hi, kk_context_t* _ctx) { /* (lo : char, hi : char) -> total list<char> */ 
  kk_std_core__list _b_23880_23877;
  kk_integer_t lo0_23016 = kk_integer_from_int(lo,kk_context()); /*int*/;
  kk_integer_t hi0_23017 = kk_integer_from_int(hi,kk_context()); /*int*/;
  _b_23880_23877 = kk_std_core__lift22734_list(lo0_23016, hi0_23017, kk_std_core__new_Nil(_ctx), _ctx); /*list<int>*/
  return kk_std_core_map_5(_b_23880_23877, kk_std_core_new_list_fun25643_2(_ctx), _ctx);
}
 
// Convert a string to a list of characters

kk_std_core__list kk_std_core_list_4(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> total list<char> */ 
  return kk_string_to_list(s,kk_context());
}
 
// monadic lift

kk_std_core_types__maybe kk_std_core__mlift22858_map(kk_box_t _y_59, kk_context_t* _ctx) { /* forall<a,e> (a) -> e maybe<a> */ 
  return kk_std_core_types__new_Just(_y_59, _ctx);
}


// lift anonymous function
struct kk_std_core_map_fun25647__t {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_map_fun25647(kk_function_t _fself, kk_box_t _b_23883, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun25647(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_map_fun25647, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_map_fun25647(kk_function_t _fself, kk_box_t _b_23883, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  kk_std_core_types__maybe _x25648 = kk_std_core__mlift22858_map(_b_23883, _ctx); /*maybe<6612>*/
  return kk_std_core_types__maybe_box(_x25648, _ctx);
}

kk_std_core_types__maybe kk_std_core_map(kk_std_core_types__maybe m, kk_function_t f, kk_context_t* _ctx) { /* forall<a,b,e> (m : maybe<a>, f : (a) -> e b) -> e maybe<b> */ 
  if (kk_std_core_types__is_Nothing(m)) {
    kk_function_drop(f, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
  kk_box_t x = m._cons.Just.value;
  kk_box_t x0_23018 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, x, _ctx)); /*6612*/;
  if (kk_yielding(kk_context())) {
    kk_box_drop(x0_23018, _ctx);
    kk_box_t _x25646 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun25647(_ctx), _ctx); /*3991*/
    return kk_std_core_types__maybe_unbox(_x25646, _ctx);
  }
  return kk_std_core_types__new_Just(x0_23018, _ctx);
}
 
// monadic lift

kk_std_core_types__either kk_std_core__mlift22859_map_1(kk_box_t _y_61, kk_context_t* _ctx) { /* forall<a,b,e> (b) -> e either<a,b> */ 
  return kk_std_core_types__new_Right(_y_61, _ctx);
}
 
// Map over the `Right` component of an `:either` type.


// lift anonymous function
struct kk_std_core_map_fun25650__t_1 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_map_fun25650_1(kk_function_t _fself, kk_box_t _b_23887, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun25650_1(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_map_fun25650_1, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_map_fun25650_1(kk_function_t _fself, kk_box_t _b_23887, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  kk_std_core_types__either _x25651 = kk_std_core__mlift22859_map_1(_b_23887, _ctx); /*either<6648,6650>*/
  return kk_std_core_types__either_box(_x25651, _ctx);
}

kk_std_core_types__either kk_std_core_map_1(kk_std_core_types__either e, kk_function_t f, kk_context_t* _ctx) { /* forall<a,b,c,e> (e : either<a,b>, f : (b) -> e c) -> e either<a,c> */ 
  if (kk_std_core_types__is_Right(e)) {
    kk_box_t x = e._cons.Right.right;
    kk_box_t x0_23022 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, x, _ctx)); /*6650*/;
    if (kk_yielding(kk_context())) {
      kk_box_drop(x0_23022, _ctx);
      kk_box_t _x25649 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun25650_1(_ctx), _ctx); /*3991*/
      return kk_std_core_types__either_unbox(_x25649, _ctx);
    }
    return kk_std_core_types__new_Right(x0_23022, _ctx);
  }
  kk_box_t x00 = e._cons.Left.left;
  kk_function_drop(f, _ctx);
  return kk_std_core_types__new_Left(x00, _ctx);
}
 
// monadic lift

kk_std_core_types__tuple2_ kk_std_core__mlift22860_map_2(kk_box_t _y_63, kk_box_t _y_64, kk_context_t* _ctx) { /* forall<a,e> (a, a) -> e (a, a) */ 
  return kk_std_core_types__new_dash__lp__comma__rp_(_y_63, _y_64, _ctx);
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift22861_map_fun25654__t_2 {
  struct kk_function_s _base;
  kk_box_t _y_63;
};
static kk_box_t kk_std_core__mlift22861_map_fun25654_2(kk_function_t _fself, kk_box_t _b_23891, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift22861_map_fun25654_2(kk_box_t _y_63, kk_context_t* _ctx) {
  struct kk_std_core__mlift22861_map_fun25654__t_2* _self = kk_function_alloc_as(struct kk_std_core__mlift22861_map_fun25654__t_2, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift22861_map_fun25654_2, kk_context());
  _self->_y_63 = _y_63;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift22861_map_fun25654_2(kk_function_t _fself, kk_box_t _b_23891, kk_context_t* _ctx) {
  struct kk_std_core__mlift22861_map_fun25654__t_2* _self = kk_function_as(struct kk_std_core__mlift22861_map_fun25654__t_2*, _fself);
  kk_box_t _y_63 = _self->_y_63; /* 6811 */
  kk_drop_match(_self, {kk_box_dup(_y_63);}, {}, _ctx)
  kk_std_core_types__tuple2_ _x25655 = kk_std_core__mlift22860_map_2(_y_63, _b_23891, _ctx); /*(6811, 6811)*/
  return kk_std_core_types__tuple2__box(_x25655, _ctx);
}

kk_std_core_types__tuple2_ kk_std_core__mlift22861_map_2(kk_function_t f, kk_std_core_types__tuple2_ t, kk_box_t _y_63, kk_context_t* _ctx) { /* forall<a,b,e> (f : (a) -> e b, t : (a, a), b) -> e (b, b) */ 
  kk_box_t x_23026;
  kk_box_t _x25652;
  kk_box_t _x = t.snd;
  kk_box_dup(_x);
  kk_std_core_types__tuple2__drop(t, _ctx);
  _x25652 = _x; /*6810*/
  x_23026 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, _x25652, _ctx)); /*6811*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x_23026, _ctx);
    kk_box_t _x25653 = kk_std_core_hnd_yield_extend(kk_std_core__new_mlift22861_map_fun25654_2(_y_63, _ctx), _ctx); /*3991*/
    return kk_std_core_types__tuple2__unbox(_x25653, _ctx);
  }
  return kk_std_core__mlift22860_map_2(_y_63, x_23026, _ctx);
}


// lift anonymous function
struct kk_std_core_map_fun25659__t_2 {
  struct kk_function_s _base;
  kk_function_t f;
  kk_std_core_types__tuple2_ t;
};
static kk_box_t kk_std_core_map_fun25659_2(kk_function_t _fself, kk_box_t _b_23895, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun25659_2(kk_function_t f, kk_std_core_types__tuple2_ t, kk_context_t* _ctx) {
  struct kk_std_core_map_fun25659__t_2* _self = kk_function_alloc_as(struct kk_std_core_map_fun25659__t_2, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun25659_2, kk_context());
  _self->f = f;
  _self->t = t;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun25659_2(kk_function_t _fself, kk_box_t _b_23895, kk_context_t* _ctx) {
  struct kk_std_core_map_fun25659__t_2* _self = kk_function_as(struct kk_std_core_map_fun25659__t_2*, _fself);
  kk_function_t f = _self->f; /* (6810) -> 6812 6811 */
  kk_std_core_types__tuple2_ t = _self->t; /* (6810, 6810) */
  kk_drop_match(_self, {kk_function_dup(f);kk_std_core_types__tuple2__dup(t);}, {}, _ctx)
  kk_std_core_types__tuple2_ _x25660 = kk_std_core__mlift22861_map_2(f, t, _b_23895, _ctx); /*(6811, 6811)*/
  return kk_std_core_types__tuple2__box(_x25660, _ctx);
}


// lift anonymous function
struct kk_std_core_map_fun25663__t_2 {
  struct kk_function_s _base;
  kk_box_t x_23029;
};
static kk_box_t kk_std_core_map_fun25663_2(kk_function_t _fself, kk_box_t _b_23897, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun25663_2(kk_box_t x_23029, kk_context_t* _ctx) {
  struct kk_std_core_map_fun25663__t_2* _self = kk_function_alloc_as(struct kk_std_core_map_fun25663__t_2, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun25663_2, kk_context());
  _self->x_23029 = x_23029;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun25663_2(kk_function_t _fself, kk_box_t _b_23897, kk_context_t* _ctx) {
  struct kk_std_core_map_fun25663__t_2* _self = kk_function_as(struct kk_std_core_map_fun25663__t_2*, _fself);
  kk_box_t x_23029 = _self->x_23029; /* 6811 */
  kk_drop_match(_self, {kk_box_dup(x_23029);}, {}, _ctx)
  kk_std_core_types__tuple2_ _x25664 = kk_std_core__mlift22860_map_2(x_23029, _b_23897, _ctx); /*(6811, 6811)*/
  return kk_std_core_types__tuple2__box(_x25664, _ctx);
}

kk_std_core_types__tuple2_ kk_std_core_map_2(kk_std_core_types__tuple2_ t, kk_function_t f, kk_context_t* _ctx) { /* forall<a,b,e> (t : (a, a), f : (a) -> e b) -> e (b, b) */ 
  kk_box_t x_23029;
  kk_function_t _x25657 = kk_function_dup(f); /*(6810) -> 6812 6811*/
  kk_box_t _x25656;
  kk_box_t _x = t.fst;
  kk_box_dup(_x);
  _x25656 = _x; /*6810*/
  x_23029 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x25657, (_x25657, _x25656, _ctx)); /*6811*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x_23029, _ctx);
    kk_box_t _x25658 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun25659_2(f, t, _ctx), _ctx); /*3991*/
    return kk_std_core_types__tuple2__unbox(_x25658, _ctx);
  }
  kk_box_t x0_23034;
  kk_box_t _x25661;
  kk_box_t _x0 = t.snd;
  kk_box_dup(_x0);
  kk_std_core_types__tuple2__drop(t, _ctx);
  _x25661 = _x0; /*6810*/
  x0_23034 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, _x25661, _ctx)); /*6811*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x0_23034, _ctx);
    kk_box_t _x25662 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun25663_2(x_23029, _ctx), _ctx); /*3991*/
    return kk_std_core_types__tuple2__unbox(_x25662, _ctx);
  }
  return kk_std_core_types__new_dash__lp__comma__rp_(x_23029, x0_23034, _ctx);
}
 
// monadic lift

kk_std_core_types__tuple3_ kk_std_core__mlift22862_map_3(kk_box_t _y_65, kk_box_t _y_66, kk_box_t _y_67, kk_context_t* _ctx) { /* forall<a,e> (a, a, a) -> e (a, a, a) */ 
  return kk_std_core_types__new_dash__lp__comma__comma__rp_(_y_65, _y_66, _y_67, _ctx);
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift22863_map_fun25667__t_3 {
  struct kk_function_s _base;
  kk_box_t _y_65;
  kk_box_t _y_66;
};
static kk_box_t kk_std_core__mlift22863_map_fun25667_3(kk_function_t _fself, kk_box_t _b_23903, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift22863_map_fun25667_3(kk_box_t _y_65, kk_box_t _y_66, kk_context_t* _ctx) {
  struct kk_std_core__mlift22863_map_fun25667__t_3* _self = kk_function_alloc_as(struct kk_std_core__mlift22863_map_fun25667__t_3, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift22863_map_fun25667_3, kk_context());
  _self->_y_65 = _y_65;
  _self->_y_66 = _y_66;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift22863_map_fun25667_3(kk_function_t _fself, kk_box_t _b_23903, kk_context_t* _ctx) {
  struct kk_std_core__mlift22863_map_fun25667__t_3* _self = kk_function_as(struct kk_std_core__mlift22863_map_fun25667__t_3*, _fself);
  kk_box_t _y_65 = _self->_y_65; /* 7051 */
  kk_box_t _y_66 = _self->_y_66; /* 7051 */
  kk_drop_match(_self, {kk_box_dup(_y_65);kk_box_dup(_y_66);}, {}, _ctx)
  kk_std_core_types__tuple3_ _x25668 = kk_std_core__mlift22862_map_3(_y_65, _y_66, _b_23903, _ctx); /*(7051, 7051, 7051)*/
  return kk_std_core_types__tuple3__box(_x25668, _ctx);
}

kk_std_core_types__tuple3_ kk_std_core__mlift22863_map_3(kk_box_t _y_65, kk_function_t f, kk_std_core_types__tuple3_ t, kk_box_t _y_66, kk_context_t* _ctx) { /* forall<a,b,e> (b, f : (a) -> e b, t : (a, a, a), b) -> e (b, b, b) */ 
  kk_box_t x_23039;
  kk_box_t _x25665;
  kk_box_t _x = t.thd;
  kk_box_dup(_x);
  kk_std_core_types__tuple3__drop(t, _ctx);
  _x25665 = _x; /*7050*/
  x_23039 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, _x25665, _ctx)); /*7051*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x_23039, _ctx);
    kk_box_t _x25666 = kk_std_core_hnd_yield_extend(kk_std_core__new_mlift22863_map_fun25667_3(_y_65, _y_66, _ctx), _ctx); /*3991*/
    return kk_std_core_types__tuple3__unbox(_x25666, _ctx);
  }
  return kk_std_core__mlift22862_map_3(_y_65, _y_66, x_23039, _ctx);
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift22864_map_fun25672__t_3 {
  struct kk_function_s _base;
  kk_box_t _y_65;
  kk_function_t f;
  kk_std_core_types__tuple3_ t;
};
static kk_box_t kk_std_core__mlift22864_map_fun25672_3(kk_function_t _fself, kk_box_t _b_23907, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift22864_map_fun25672_3(kk_box_t _y_65, kk_function_t f, kk_std_core_types__tuple3_ t, kk_context_t* _ctx) {
  struct kk_std_core__mlift22864_map_fun25672__t_3* _self = kk_function_alloc_as(struct kk_std_core__mlift22864_map_fun25672__t_3, 6, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift22864_map_fun25672_3, kk_context());
  _self->_y_65 = _y_65;
  _self->f = f;
  _self->t = t;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift22864_map_fun25672_3(kk_function_t _fself, kk_box_t _b_23907, kk_context_t* _ctx) {
  struct kk_std_core__mlift22864_map_fun25672__t_3* _self = kk_function_as(struct kk_std_core__mlift22864_map_fun25672__t_3*, _fself);
  kk_box_t _y_65 = _self->_y_65; /* 7051 */
  kk_function_t f = _self->f; /* (7050) -> 7052 7051 */
  kk_std_core_types__tuple3_ t = _self->t; /* (7050, 7050, 7050) */
  kk_drop_match(_self, {kk_box_dup(_y_65);kk_function_dup(f);kk_std_core_types__tuple3__dup(t);}, {}, _ctx)
  kk_std_core_types__tuple3_ _x25673 = kk_std_core__mlift22863_map_3(_y_65, f, t, _b_23907, _ctx); /*(7051, 7051, 7051)*/
  return kk_std_core_types__tuple3__box(_x25673, _ctx);
}

kk_std_core_types__tuple3_ kk_std_core__mlift22864_map_3(kk_function_t f, kk_std_core_types__tuple3_ t, kk_box_t _y_65, kk_context_t* _ctx) { /* forall<a,b,e> (f : (a) -> e b, t : (a, a, a), b) -> e (b, b, b) */ 
  kk_box_t x_23042;
  kk_function_t _x25670 = kk_function_dup(f); /*(7050) -> 7052 7051*/
  kk_box_t _x25669;
  kk_box_t _x = t.snd;
  kk_box_dup(_x);
  _x25669 = _x; /*7050*/
  x_23042 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x25670, (_x25670, _x25669, _ctx)); /*7051*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x_23042, _ctx);
    kk_box_t _x25671 = kk_std_core_hnd_yield_extend(kk_std_core__new_mlift22864_map_fun25672_3(_y_65, f, t, _ctx), _ctx); /*3991*/
    return kk_std_core_types__tuple3__unbox(_x25671, _ctx);
  }
  return kk_std_core__mlift22863_map_3(_y_65, f, t, x_23042, _ctx);
}


// lift anonymous function
struct kk_std_core_map_fun25677__t_3 {
  struct kk_function_s _base;
  kk_function_t f;
  kk_std_core_types__tuple3_ t;
};
static kk_box_t kk_std_core_map_fun25677_3(kk_function_t _fself, kk_box_t _b_23911, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun25677_3(kk_function_t f, kk_std_core_types__tuple3_ t, kk_context_t* _ctx) {
  struct kk_std_core_map_fun25677__t_3* _self = kk_function_alloc_as(struct kk_std_core_map_fun25677__t_3, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun25677_3, kk_context());
  _self->f = f;
  _self->t = t;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun25677_3(kk_function_t _fself, kk_box_t _b_23911, kk_context_t* _ctx) {
  struct kk_std_core_map_fun25677__t_3* _self = kk_function_as(struct kk_std_core_map_fun25677__t_3*, _fself);
  kk_function_t f = _self->f; /* (7050) -> 7052 7051 */
  kk_std_core_types__tuple3_ t = _self->t; /* (7050, 7050, 7050) */
  kk_drop_match(_self, {kk_function_dup(f);kk_std_core_types__tuple3__dup(t);}, {}, _ctx)
  kk_std_core_types__tuple3_ _x25678 = kk_std_core__mlift22864_map_3(f, t, _b_23911, _ctx); /*(7051, 7051, 7051)*/
  return kk_std_core_types__tuple3__box(_x25678, _ctx);
}


// lift anonymous function
struct kk_std_core_map_fun25682__t_3 {
  struct kk_function_s _base;
  kk_function_t f;
  kk_std_core_types__tuple3_ t;
  kk_box_t x_23045;
};
static kk_box_t kk_std_core_map_fun25682_3(kk_function_t _fself, kk_box_t _b_23913, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun25682_3(kk_function_t f, kk_std_core_types__tuple3_ t, kk_box_t x_23045, kk_context_t* _ctx) {
  struct kk_std_core_map_fun25682__t_3* _self = kk_function_alloc_as(struct kk_std_core_map_fun25682__t_3, 6, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun25682_3, kk_context());
  _self->f = f;
  _self->t = t;
  _self->x_23045 = x_23045;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun25682_3(kk_function_t _fself, kk_box_t _b_23913, kk_context_t* _ctx) {
  struct kk_std_core_map_fun25682__t_3* _self = kk_function_as(struct kk_std_core_map_fun25682__t_3*, _fself);
  kk_function_t f = _self->f; /* (7050) -> 7052 7051 */
  kk_std_core_types__tuple3_ t = _self->t; /* (7050, 7050, 7050) */
  kk_box_t x_23045 = _self->x_23045; /* 7051 */
  kk_drop_match(_self, {kk_function_dup(f);kk_std_core_types__tuple3__dup(t);kk_box_dup(x_23045);}, {}, _ctx)
  kk_std_core_types__tuple3_ _x25683 = kk_std_core__mlift22863_map_3(x_23045, f, t, _b_23913, _ctx); /*(7051, 7051, 7051)*/
  return kk_std_core_types__tuple3__box(_x25683, _ctx);
}


// lift anonymous function
struct kk_std_core_map_fun25686__t_3 {
  struct kk_function_s _base;
  kk_box_t x_23045;
  kk_box_t x0_23050;
};
static kk_box_t kk_std_core_map_fun25686_3(kk_function_t _fself, kk_box_t _b_23915, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun25686_3(kk_box_t x_23045, kk_box_t x0_23050, kk_context_t* _ctx) {
  struct kk_std_core_map_fun25686__t_3* _self = kk_function_alloc_as(struct kk_std_core_map_fun25686__t_3, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun25686_3, kk_context());
  _self->x_23045 = x_23045;
  _self->x0_23050 = x0_23050;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun25686_3(kk_function_t _fself, kk_box_t _b_23915, kk_context_t* _ctx) {
  struct kk_std_core_map_fun25686__t_3* _self = kk_function_as(struct kk_std_core_map_fun25686__t_3*, _fself);
  kk_box_t x_23045 = _self->x_23045; /* 7051 */
  kk_box_t x0_23050 = _self->x0_23050; /* 7051 */
  kk_drop_match(_self, {kk_box_dup(x_23045);kk_box_dup(x0_23050);}, {}, _ctx)
  kk_std_core_types__tuple3_ _x25687 = kk_std_core__mlift22862_map_3(x_23045, x0_23050, _b_23915, _ctx); /*(7051, 7051, 7051)*/
  return kk_std_core_types__tuple3__box(_x25687, _ctx);
}

kk_std_core_types__tuple3_ kk_std_core_map_3(kk_std_core_types__tuple3_ t, kk_function_t f, kk_context_t* _ctx) { /* forall<a,b,e> (t : (a, a, a), f : (a) -> e b) -> e (b, b, b) */ 
  kk_box_t x_23045;
  kk_function_t _x25675 = kk_function_dup(f); /*(7050) -> 7052 7051*/
  kk_box_t _x25674;
  kk_box_t _x = t.fst;
  kk_box_dup(_x);
  _x25674 = _x; /*7050*/
  x_23045 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x25675, (_x25675, _x25674, _ctx)); /*7051*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x_23045, _ctx);
    kk_box_t _x25676 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun25677_3(f, t, _ctx), _ctx); /*3991*/
    return kk_std_core_types__tuple3__unbox(_x25676, _ctx);
  }
  kk_box_t x0_23050;
  kk_function_t _x25680 = kk_function_dup(f); /*(7050) -> 7052 7051*/
  kk_box_t _x25679;
  kk_box_t _x0 = t.snd;
  kk_box_dup(_x0);
  _x25679 = _x0; /*7050*/
  x0_23050 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x25680, (_x25680, _x25679, _ctx)); /*7051*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x0_23050, _ctx);
    kk_box_t _x25681 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun25682_3(f, t, x_23045, _ctx), _ctx); /*3991*/
    return kk_std_core_types__tuple3__unbox(_x25681, _ctx);
  }
  kk_box_t x1_23055;
  kk_box_t _x25684;
  kk_box_t _x1 = t.thd;
  kk_box_dup(_x1);
  kk_std_core_types__tuple3__drop(t, _ctx);
  _x25684 = _x1; /*7050*/
  x1_23055 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, _x25684, _ctx)); /*7051*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x1_23055, _ctx);
    kk_box_t _x25685 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun25686_3(x_23045, x0_23050, _ctx), _ctx); /*3991*/
    return kk_std_core_types__tuple3__unbox(_x25685, _ctx);
  }
  return kk_std_core_types__new_dash__lp__comma__comma__rp_(x_23045, x0_23050, x1_23055, _ctx);
}
 
// monadic lift

kk_std_core_types__tuple4_ kk_std_core__mlift22865_map_4(kk_box_t _y_68, kk_box_t _y_69, kk_box_t _y_70, kk_box_t _y_71, kk_context_t* _ctx) { /* forall<a,e> (a, a, a, a) -> e (a, a, a, a) */ 
  return kk_std_core_types__new_dash__lp__comma__comma__comma__rp_(kk_reuse_null, _y_68, _y_69, _y_70, _y_71, _ctx);
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift22866_map_fun25691__t_4 {
  struct kk_function_s _base;
  kk_box_t _y_68;
  kk_box_t _y_69;
  kk_box_t _y_70;
};
static kk_box_t kk_std_core__mlift22866_map_fun25691_4(kk_function_t _fself, kk_box_t _b_23923, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift22866_map_fun25691_4(kk_box_t _y_68, kk_box_t _y_69, kk_box_t _y_70, kk_context_t* _ctx) {
  struct kk_std_core__mlift22866_map_fun25691__t_4* _self = kk_function_alloc_as(struct kk_std_core__mlift22866_map_fun25691__t_4, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift22866_map_fun25691_4, kk_context());
  _self->_y_68 = _y_68;
  _self->_y_69 = _y_69;
  _self->_y_70 = _y_70;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift22866_map_fun25691_4(kk_function_t _fself, kk_box_t _b_23923, kk_context_t* _ctx) {
  struct kk_std_core__mlift22866_map_fun25691__t_4* _self = kk_function_as(struct kk_std_core__mlift22866_map_fun25691__t_4*, _fself);
  kk_box_t _y_68 = _self->_y_68; /* 7364 */
  kk_box_t _y_69 = _self->_y_69; /* 7364 */
  kk_box_t _y_70 = _self->_y_70; /* 7364 */
  kk_drop_match(_self, {kk_box_dup(_y_68);kk_box_dup(_y_69);kk_box_dup(_y_70);}, {}, _ctx)
  kk_std_core_types__tuple4_ _x25692 = kk_std_core__mlift22865_map_4(_y_68, _y_69, _y_70, _b_23923, _ctx); /*(7364, 7364, 7364, 7364)*/
  return kk_std_core_types__tuple4__box(_x25692, _ctx);
}

kk_std_core_types__tuple4_ kk_std_core__mlift22866_map_4(kk_box_t _y_68, kk_box_t _y_69, kk_function_t f, kk_std_core_types__tuple4_ t, kk_box_t _y_70, kk_context_t* _ctx) { /* forall<a,b,e> (b, b, f : (a) -> e b, t : (a, a, a, a), b) -> e (b, b, b, b) */ 
  kk_box_t x_23060;
  kk_box_t _x25688;
  struct kk_std_core_types__Tuple4_* _con25689 = kk_std_core_types__as_dash__lp__comma__comma__comma__rp_(t);
  kk_box_t _pat00 = _con25689->fst;
  kk_box_t _pat10 = _con25689->snd;
  kk_box_t _pat2 = _con25689->thd;
  kk_box_t _x = _con25689->field4;
  if (kk_std_core_types__tuple4__is_unique(t)) {
    kk_box_drop(_pat2, _ctx);
    kk_box_drop(_pat10, _ctx);
    kk_box_drop(_pat00, _ctx);
    kk_std_core_types__tuple4__free(t);
  }
  else {
    kk_box_dup(_x);
    kk_std_core_types__tuple4__decref(t, _ctx);
  }
  _x25688 = _x; /*7363*/
  x_23060 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, _x25688, _ctx)); /*7364*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x_23060, _ctx);
    kk_box_t _x25690 = kk_std_core_hnd_yield_extend(kk_std_core__new_mlift22866_map_fun25691_4(_y_68, _y_69, _y_70, _ctx), _ctx); /*3991*/
    return kk_std_core_types__tuple4__unbox(_x25690, _ctx);
  }
  return kk_std_core__mlift22865_map_4(_y_68, _y_69, _y_70, x_23060, _ctx);
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift22867_map_fun25697__t_4 {
  struct kk_function_s _base;
  kk_box_t _y_68;
  kk_box_t _y_69;
  kk_function_t f;
  kk_std_core_types__tuple4_ t;
};
static kk_box_t kk_std_core__mlift22867_map_fun25697_4(kk_function_t _fself, kk_box_t _b_23927, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift22867_map_fun25697_4(kk_box_t _y_68, kk_box_t _y_69, kk_function_t f, kk_std_core_types__tuple4_ t, kk_context_t* _ctx) {
  struct kk_std_core__mlift22867_map_fun25697__t_4* _self = kk_function_alloc_as(struct kk_std_core__mlift22867_map_fun25697__t_4, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift22867_map_fun25697_4, kk_context());
  _self->_y_68 = _y_68;
  _self->_y_69 = _y_69;
  _self->f = f;
  _self->t = t;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift22867_map_fun25697_4(kk_function_t _fself, kk_box_t _b_23927, kk_context_t* _ctx) {
  struct kk_std_core__mlift22867_map_fun25697__t_4* _self = kk_function_as(struct kk_std_core__mlift22867_map_fun25697__t_4*, _fself);
  kk_box_t _y_68 = _self->_y_68; /* 7364 */
  kk_box_t _y_69 = _self->_y_69; /* 7364 */
  kk_function_t f = _self->f; /* (7363) -> 7365 7364 */
  kk_std_core_types__tuple4_ t = _self->t; /* (7363, 7363, 7363, 7363) */
  kk_drop_match(_self, {kk_box_dup(_y_68);kk_box_dup(_y_69);kk_function_dup(f);kk_std_core_types__tuple4__dup(t);}, {}, _ctx)
  kk_std_core_types__tuple4_ _x25698 = kk_std_core__mlift22866_map_4(_y_68, _y_69, f, t, _b_23927, _ctx); /*(7364, 7364, 7364, 7364)*/
  return kk_std_core_types__tuple4__box(_x25698, _ctx);
}

kk_std_core_types__tuple4_ kk_std_core__mlift22867_map_4(kk_box_t _y_68, kk_function_t f, kk_std_core_types__tuple4_ t, kk_box_t _y_69, kk_context_t* _ctx) { /* forall<a,b,e> (b, f : (a) -> e b, t : (a, a, a, a), b) -> e (b, b, b, b) */ 
  kk_box_t x_23063;
  kk_function_t _x25695 = kk_function_dup(f); /*(7363) -> 7365 7364*/
  kk_box_t _x25693;
  struct kk_std_core_types__Tuple4_* _con25694 = kk_std_core_types__as_dash__lp__comma__comma__comma__rp_(t);
  kk_box_t _x = _con25694->thd;
  kk_box_dup(_x);
  _x25693 = _x; /*7363*/
  x_23063 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x25695, (_x25695, _x25693, _ctx)); /*7364*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x_23063, _ctx);
    kk_box_t _x25696 = kk_std_core_hnd_yield_extend(kk_std_core__new_mlift22867_map_fun25697_4(_y_68, _y_69, f, t, _ctx), _ctx); /*3991*/
    return kk_std_core_types__tuple4__unbox(_x25696, _ctx);
  }
  return kk_std_core__mlift22866_map_4(_y_68, _y_69, f, t, x_23063, _ctx);
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift22868_map_fun25703__t_4 {
  struct kk_function_s _base;
  kk_box_t _y_68;
  kk_function_t f;
  kk_std_core_types__tuple4_ t;
};
static kk_box_t kk_std_core__mlift22868_map_fun25703_4(kk_function_t _fself, kk_box_t _b_23931, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift22868_map_fun25703_4(kk_box_t _y_68, kk_function_t f, kk_std_core_types__tuple4_ t, kk_context_t* _ctx) {
  struct kk_std_core__mlift22868_map_fun25703__t_4* _self = kk_function_alloc_as(struct kk_std_core__mlift22868_map_fun25703__t_4, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift22868_map_fun25703_4, kk_context());
  _self->_y_68 = _y_68;
  _self->f = f;
  _self->t = t;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift22868_map_fun25703_4(kk_function_t _fself, kk_box_t _b_23931, kk_context_t* _ctx) {
  struct kk_std_core__mlift22868_map_fun25703__t_4* _self = kk_function_as(struct kk_std_core__mlift22868_map_fun25703__t_4*, _fself);
  kk_box_t _y_68 = _self->_y_68; /* 7364 */
  kk_function_t f = _self->f; /* (7363) -> 7365 7364 */
  kk_std_core_types__tuple4_ t = _self->t; /* (7363, 7363, 7363, 7363) */
  kk_drop_match(_self, {kk_box_dup(_y_68);kk_function_dup(f);kk_std_core_types__tuple4__dup(t);}, {}, _ctx)
  kk_std_core_types__tuple4_ _x25704 = kk_std_core__mlift22867_map_4(_y_68, f, t, _b_23931, _ctx); /*(7364, 7364, 7364, 7364)*/
  return kk_std_core_types__tuple4__box(_x25704, _ctx);
}

kk_std_core_types__tuple4_ kk_std_core__mlift22868_map_4(kk_function_t f, kk_std_core_types__tuple4_ t, kk_box_t _y_68, kk_context_t* _ctx) { /* forall<a,b,e> (f : (a) -> e b, t : (a, a, a, a), b) -> e (b, b, b, b) */ 
  kk_box_t x_23066;
  kk_function_t _x25701 = kk_function_dup(f); /*(7363) -> 7365 7364*/
  kk_box_t _x25699;
  struct kk_std_core_types__Tuple4_* _con25700 = kk_std_core_types__as_dash__lp__comma__comma__comma__rp_(t);
  kk_box_t _x = _con25700->snd;
  kk_box_dup(_x);
  _x25699 = _x; /*7363*/
  x_23066 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x25701, (_x25701, _x25699, _ctx)); /*7364*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x_23066, _ctx);
    kk_box_t _x25702 = kk_std_core_hnd_yield_extend(kk_std_core__new_mlift22868_map_fun25703_4(_y_68, f, t, _ctx), _ctx); /*3991*/
    return kk_std_core_types__tuple4__unbox(_x25702, _ctx);
  }
  return kk_std_core__mlift22867_map_4(_y_68, f, t, x_23066, _ctx);
}


// lift anonymous function
struct kk_std_core_map_fun25709__t_4 {
  struct kk_function_s _base;
  kk_function_t f;
  kk_std_core_types__tuple4_ t;
};
static kk_box_t kk_std_core_map_fun25709_4(kk_function_t _fself, kk_box_t _b_23935, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun25709_4(kk_function_t f, kk_std_core_types__tuple4_ t, kk_context_t* _ctx) {
  struct kk_std_core_map_fun25709__t_4* _self = kk_function_alloc_as(struct kk_std_core_map_fun25709__t_4, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun25709_4, kk_context());
  _self->f = f;
  _self->t = t;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun25709_4(kk_function_t _fself, kk_box_t _b_23935, kk_context_t* _ctx) {
  struct kk_std_core_map_fun25709__t_4* _self = kk_function_as(struct kk_std_core_map_fun25709__t_4*, _fself);
  kk_function_t f = _self->f; /* (7363) -> 7365 7364 */
  kk_std_core_types__tuple4_ t = _self->t; /* (7363, 7363, 7363, 7363) */
  kk_drop_match(_self, {kk_function_dup(f);kk_std_core_types__tuple4__dup(t);}, {}, _ctx)
  kk_std_core_types__tuple4_ _x25710 = kk_std_core__mlift22868_map_4(f, t, _b_23935, _ctx); /*(7364, 7364, 7364, 7364)*/
  return kk_std_core_types__tuple4__box(_x25710, _ctx);
}


// lift anonymous function
struct kk_std_core_map_fun25715__t_4 {
  struct kk_function_s _base;
  kk_function_t f;
  kk_std_core_types__tuple4_ t;
  kk_box_t x_23069;
};
static kk_box_t kk_std_core_map_fun25715_4(kk_function_t _fself, kk_box_t _b_23937, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun25715_4(kk_function_t f, kk_std_core_types__tuple4_ t, kk_box_t x_23069, kk_context_t* _ctx) {
  struct kk_std_core_map_fun25715__t_4* _self = kk_function_alloc_as(struct kk_std_core_map_fun25715__t_4, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun25715_4, kk_context());
  _self->f = f;
  _self->t = t;
  _self->x_23069 = x_23069;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun25715_4(kk_function_t _fself, kk_box_t _b_23937, kk_context_t* _ctx) {
  struct kk_std_core_map_fun25715__t_4* _self = kk_function_as(struct kk_std_core_map_fun25715__t_4*, _fself);
  kk_function_t f = _self->f; /* (7363) -> 7365 7364 */
  kk_std_core_types__tuple4_ t = _self->t; /* (7363, 7363, 7363, 7363) */
  kk_box_t x_23069 = _self->x_23069; /* 7364 */
  kk_drop_match(_self, {kk_function_dup(f);kk_std_core_types__tuple4__dup(t);kk_box_dup(x_23069);}, {}, _ctx)
  kk_std_core_types__tuple4_ _x25716 = kk_std_core__mlift22867_map_4(x_23069, f, t, _b_23937, _ctx); /*(7364, 7364, 7364, 7364)*/
  return kk_std_core_types__tuple4__box(_x25716, _ctx);
}


// lift anonymous function
struct kk_std_core_map_fun25721__t_4 {
  struct kk_function_s _base;
  kk_function_t f;
  kk_std_core_types__tuple4_ t;
  kk_box_t x_23069;
  kk_box_t x0_23074;
};
static kk_box_t kk_std_core_map_fun25721_4(kk_function_t _fself, kk_box_t _b_23939, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun25721_4(kk_function_t f, kk_std_core_types__tuple4_ t, kk_box_t x_23069, kk_box_t x0_23074, kk_context_t* _ctx) {
  struct kk_std_core_map_fun25721__t_4* _self = kk_function_alloc_as(struct kk_std_core_map_fun25721__t_4, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun25721_4, kk_context());
  _self->f = f;
  _self->t = t;
  _self->x_23069 = x_23069;
  _self->x0_23074 = x0_23074;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun25721_4(kk_function_t _fself, kk_box_t _b_23939, kk_context_t* _ctx) {
  struct kk_std_core_map_fun25721__t_4* _self = kk_function_as(struct kk_std_core_map_fun25721__t_4*, _fself);
  kk_function_t f = _self->f; /* (7363) -> 7365 7364 */
  kk_std_core_types__tuple4_ t = _self->t; /* (7363, 7363, 7363, 7363) */
  kk_box_t x_23069 = _self->x_23069; /* 7364 */
  kk_box_t x0_23074 = _self->x0_23074; /* 7364 */
  kk_drop_match(_self, {kk_function_dup(f);kk_std_core_types__tuple4__dup(t);kk_box_dup(x_23069);kk_box_dup(x0_23074);}, {}, _ctx)
  kk_std_core_types__tuple4_ _x25722 = kk_std_core__mlift22866_map_4(x_23069, x0_23074, f, t, _b_23939, _ctx); /*(7364, 7364, 7364, 7364)*/
  return kk_std_core_types__tuple4__box(_x25722, _ctx);
}


// lift anonymous function
struct kk_std_core_map_fun25726__t_4 {
  struct kk_function_s _base;
  kk_box_t x_23069;
  kk_box_t x0_23074;
  kk_box_t x1_23079;
};
static kk_box_t kk_std_core_map_fun25726_4(kk_function_t _fself, kk_box_t _b_23941, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun25726_4(kk_box_t x_23069, kk_box_t x0_23074, kk_box_t x1_23079, kk_context_t* _ctx) {
  struct kk_std_core_map_fun25726__t_4* _self = kk_function_alloc_as(struct kk_std_core_map_fun25726__t_4, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun25726_4, kk_context());
  _self->x_23069 = x_23069;
  _self->x0_23074 = x0_23074;
  _self->x1_23079 = x1_23079;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun25726_4(kk_function_t _fself, kk_box_t _b_23941, kk_context_t* _ctx) {
  struct kk_std_core_map_fun25726__t_4* _self = kk_function_as(struct kk_std_core_map_fun25726__t_4*, _fself);
  kk_box_t x_23069 = _self->x_23069; /* 7364 */
  kk_box_t x0_23074 = _self->x0_23074; /* 7364 */
  kk_box_t x1_23079 = _self->x1_23079; /* 7364 */
  kk_drop_match(_self, {kk_box_dup(x_23069);kk_box_dup(x0_23074);kk_box_dup(x1_23079);}, {}, _ctx)
  kk_std_core_types__tuple4_ _x25727 = kk_std_core__mlift22865_map_4(x_23069, x0_23074, x1_23079, _b_23941, _ctx); /*(7364, 7364, 7364, 7364)*/
  return kk_std_core_types__tuple4__box(_x25727, _ctx);
}

kk_std_core_types__tuple4_ kk_std_core_map_4(kk_std_core_types__tuple4_ t, kk_function_t f, kk_context_t* _ctx) { /* forall<a,b,e> (t : (a, a, a, a), f : (a) -> e b) -> e (b, b, b, b) */ 
  kk_box_t x_23069;
  kk_function_t _x25707 = kk_function_dup(f); /*(7363) -> 7365 7364*/
  kk_box_t _x25705;
  struct kk_std_core_types__Tuple4_* _con25706 = kk_std_core_types__as_dash__lp__comma__comma__comma__rp_(t);
  kk_box_t _x = _con25706->fst;
  kk_box_dup(_x);
  _x25705 = _x; /*7363*/
  x_23069 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x25707, (_x25707, _x25705, _ctx)); /*7364*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x_23069, _ctx);
    kk_box_t _x25708 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun25709_4(f, t, _ctx), _ctx); /*3991*/
    return kk_std_core_types__tuple4__unbox(_x25708, _ctx);
  }
  kk_box_t x0_23074;
  kk_function_t _x25713 = kk_function_dup(f); /*(7363) -> 7365 7364*/
  kk_box_t _x25711;
  struct kk_std_core_types__Tuple4_* _con25712 = kk_std_core_types__as_dash__lp__comma__comma__comma__rp_(t);
  kk_box_t _x0 = _con25712->snd;
  kk_box_dup(_x0);
  _x25711 = _x0; /*7363*/
  x0_23074 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x25713, (_x25713, _x25711, _ctx)); /*7364*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x0_23074, _ctx);
    kk_box_t _x25714 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun25715_4(f, t, x_23069, _ctx), _ctx); /*3991*/
    return kk_std_core_types__tuple4__unbox(_x25714, _ctx);
  }
  kk_box_t x1_23079;
  kk_function_t _x25719 = kk_function_dup(f); /*(7363) -> 7365 7364*/
  kk_box_t _x25717;
  struct kk_std_core_types__Tuple4_* _con25718 = kk_std_core_types__as_dash__lp__comma__comma__comma__rp_(t);
  kk_box_t _x1 = _con25718->thd;
  kk_box_dup(_x1);
  _x25717 = _x1; /*7363*/
  x1_23079 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x25719, (_x25719, _x25717, _ctx)); /*7364*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x1_23079, _ctx);
    kk_box_t _x25720 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun25721_4(f, t, x_23069, x0_23074, _ctx), _ctx); /*3991*/
    return kk_std_core_types__tuple4__unbox(_x25720, _ctx);
  }
  kk_box_t x2_23084;
  kk_box_t _x25723;
  struct kk_std_core_types__Tuple4_* _con25724 = kk_std_core_types__as_dash__lp__comma__comma__comma__rp_(t);
  kk_box_t _pat06 = _con25724->fst;
  kk_box_t _pat13 = _con25724->snd;
  kk_box_t _pat22 = _con25724->thd;
  kk_box_t _x2 = _con25724->field4;
  if (kk_std_core_types__tuple4__is_unique(t)) {
    kk_box_drop(_pat22, _ctx);
    kk_box_drop(_pat13, _ctx);
    kk_box_drop(_pat06, _ctx);
    kk_std_core_types__tuple4__free(t);
  }
  else {
    kk_box_dup(_x2);
    kk_std_core_types__tuple4__decref(t, _ctx);
  }
  _x25723 = _x2; /*7363*/
  x2_23084 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, _x25723, _ctx)); /*7364*/
  if (kk_yielding(kk_context())) {
    kk_box_drop(x2_23084, _ctx);
    kk_box_t _x25725 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun25726_4(x_23069, x0_23074, x1_23079, _ctx), _ctx); /*3991*/
    return kk_std_core_types__tuple4__unbox(_x25725, _ctx);
  }
  return kk_std_core_types__new_dash__lp__comma__comma__comma__rp_(kk_reuse_null, x_23069, x0_23074, x1_23079, x2_23084, _ctx);
}
 
// Apply a function `f` to each character in a string


// lift anonymous function
struct kk_std_core_map_fun25728__t_6 {
  struct kk_function_s _base;
  kk_function_t f;
};
static kk_box_t kk_std_core_map_fun25728_6(kk_function_t _fself, kk_box_t _b_23952, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun25728_6(kk_function_t f, kk_context_t* _ctx) {
  struct kk_std_core_map_fun25728__t_6* _self = kk_function_alloc_as(struct kk_std_core_map_fun25728__t_6, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun25728_6, kk_context());
  _self->f = f;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun25728_6(kk_function_t _fself, kk_box_t _b_23952, kk_context_t* _ctx) {
  struct kk_std_core_map_fun25728__t_6* _self = kk_function_as(struct kk_std_core_map_fun25728__t_6*, _fself);
  kk_function_t f = _self->f; /* (char) -> 7854 char */
  kk_drop_match(_self, {kk_function_dup(f);}, {}, _ctx)
  kk_char_t _x25729;
  kk_char_t _x25730 = kk_char_unbox(_b_23952, _ctx); /*char*/
  _x25729 = kk_function_call(kk_char_t, (kk_function_t, kk_char_t, kk_context_t*), f, (f, _x25730, _ctx)); /*char*/
  return kk_char_box(_x25729, _ctx);
}


// lift anonymous function
struct kk_std_core_map_fun25732__t_6 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_map_fun25732_6(kk_function_t _fself, kk_box_t _b_23956, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun25732_6(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_map_fun25732_6, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_map_fun25732_6(kk_function_t _fself, kk_box_t _b_23956, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  kk_string_t _x25733;
  kk_std_core__list _x25734 = kk_std_core__list_unbox(_b_23956, _ctx); /*list<char>*/
  _x25733 = kk_std_core_string_2(_x25734, _ctx); /*string*/
  return kk_string_box(_x25733);
}

kk_string_t kk_std_core_map_6(kk_string_t s, kk_function_t f, kk_context_t* _ctx) { /* forall<e> (s : string, f : (char) -> e char) -> e string */ 
  kk_std_core__list x_23089;
  kk_std_core__list _b_23953_23950 = kk_std_core_list_4(s, _ctx); /*list<char>*/;
  x_23089 = kk_std_core_map_5(_b_23953_23950, kk_std_core_new_map_fun25728_6(f, _ctx), _ctx); /*list<char>*/
  if (kk_yielding(kk_context())) {
    kk_std_core__list_drop(x_23089, _ctx);
    kk_box_t _x25731 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun25732_6(_ctx), _ctx); /*3991*/
    return kk_string_unbox(_x25731);
  }
  return kk_std_core_string_2(x_23089, _ctx);
}
 
// monadic lift

kk_unit_t kk_std_core__mlift22869_map_7(size_t i, kk_vector_t w, kk_box_t _y_73, kk_context_t* _ctx) { /* forall<a,e> (i : size_t, w : vector<a>, a) -> e () */ 
  kk_vector_unsafe_assign(w,i,_y_73,kk_context()); return kk_Unit;
}
 
// monadic lift

kk_vector_t kk_std_core__mlift22870_map_7(kk_vector_t w, kk_unit_t wild__, kk_context_t* _ctx) { /* forall<a,e> (w : vector<a>, wild_ : ()) -> e vector<a> */ 
  return w;
}
 
// Apply a total function `f` to each element in a vector `v`


// lift anonymous function
struct kk_std_core_map_fun25741__t_7 {
  struct kk_function_s _base;
  kk_function_t f;
  kk_vector_t v;
  kk_vector_t w;
};
static kk_unit_t kk_std_core_map_fun25741_7(kk_function_t _fself, size_t i, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun25741_7(kk_function_t f, kk_vector_t v, kk_vector_t w, kk_context_t* _ctx) {
  struct kk_std_core_map_fun25741__t_7* _self = kk_function_alloc_as(struct kk_std_core_map_fun25741__t_7, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun25741_7, kk_context());
  _self->f = f;
  _self->v = v;
  _self->w = w;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_map_fun25743__t_7 {
  struct kk_function_s _base;
  kk_vector_t w;
  size_t i;
};
static kk_box_t kk_std_core_map_fun25743_7(kk_function_t _fself, kk_box_t _b_23959, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun25743_7(kk_vector_t w, size_t i, kk_context_t* _ctx) {
  struct kk_std_core_map_fun25743__t_7* _self = kk_function_alloc_as(struct kk_std_core_map_fun25743__t_7, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun25743_7, kk_context());
  _self->w = w;
  _self->i = i;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun25743_7(kk_function_t _fself, kk_box_t _b_23959, kk_context_t* _ctx) {
  struct kk_std_core_map_fun25743__t_7* _self = kk_function_as(struct kk_std_core_map_fun25743__t_7*, _fself);
  kk_vector_t w = _self->w; /* vector<7909> */
  size_t i = _self->i; /* size_t */
  kk_drop_match(_self, {kk_vector_dup(w);;}, {}, _ctx)
  kk_unit_t _x25744 = kk_Unit;
  kk_std_core__mlift22869_map_7(i, w, _b_23959, _ctx);
  return kk_unit_box(_x25744);
}
static kk_unit_t kk_std_core_map_fun25741_7(kk_function_t _fself, size_t i, kk_context_t* _ctx) {
  struct kk_std_core_map_fun25741__t_7* _self = kk_function_as(struct kk_std_core_map_fun25741__t_7*, _fself);
  kk_function_t f = _self->f; /* (7908) -> 7910 7909 */
  kk_vector_t v = _self->v; /* vector<7908> */
  kk_vector_t w = _self->w; /* vector<7909> */
  kk_drop_match(_self, {kk_function_dup(f);kk_vector_dup(v);kk_vector_dup(w);}, {}, _ctx)
  kk_box_t x0_23099 = kk_vector_at(v,i); /*7908*/;
  kk_box_t x1_23101 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, x0_23099, _ctx)); /*7909*/;
  if (kk_yielding(kk_context())) {
    kk_box_drop(x1_23101, _ctx);
    kk_box_t _x25742 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun25743_7(w, i, _ctx), _ctx); /*3991*/
    return kk_unit_unbox(_x25742);
  }
  return kk_std_core__mlift22869_map_7(i, w, x1_23101, _ctx);
}


// lift anonymous function
struct kk_std_core_map_fun25746__t_7 {
  struct kk_function_s _base;
  kk_vector_t w;
};
static kk_box_t kk_std_core_map_fun25746_7(kk_function_t _fself, kk_box_t _b_23963, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_fun25746_7(kk_vector_t w, kk_context_t* _ctx) {
  struct kk_std_core_map_fun25746__t_7* _self = kk_function_alloc_as(struct kk_std_core_map_fun25746__t_7, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_map_fun25746_7, kk_context());
  _self->w = w;
  return &_self->_base;
}

static kk_box_t kk_std_core_map_fun25746_7(kk_function_t _fself, kk_box_t _b_23963, kk_context_t* _ctx) {
  struct kk_std_core_map_fun25746__t_7* _self = kk_function_as(struct kk_std_core_map_fun25746__t_7*, _fself);
  kk_vector_t w = _self->w; /* vector<7909> */
  kk_drop_match(_self, {kk_vector_dup(w);}, {}, _ctx)
  kk_vector_t _x25747;
  kk_unit_t _x25748 = kk_Unit;
  kk_unit_unbox(_b_23963);
  _x25747 = kk_std_core__mlift22870_map_7(w, _x25748, _ctx); /*vector<7909>*/
  return kk_vector_box(_x25747, _ctx);
}

kk_vector_t kk_std_core_map_7(kk_vector_t v, kk_function_t f, kk_context_t* _ctx) { /* forall<a,b,e> (v : vector<a>, f : (a) -> e b) -> e vector<b> */ 
  kk_vector_t w;
  size_t _x25735;
  kk_integer_t _x25736;
  kk_vector_t _x25737 = kk_vector_dup(v); /*vector<7908>*/
  _x25736 = kk_std_core_length_2(_x25737, _ctx); /*int*/
  _x25735 = kk_std_core_size__t(_x25736, _ctx); /*size_t*/
  w = kk_vector_allocz(_x25735,kk_context()); /*vector<7909>*/
  kk_unit_t x_23091 = kk_Unit;
  size_t start_23096 = ((size_t)0); /*size_t*/;
  size_t end_23097;
  size_t _x25738;
  kk_vector_t _x25739 = kk_vector_dup(v); /*vector<7908>*/
  _x25738 = kk_vector_len(_x25739); /*size_t*/
  end_23097 = kk_std_core_decr_1(_x25738, _ctx); /*size_t*/
  kk_function_t _x25740;
  kk_vector_dup(w);
  _x25740 = kk_std_core_new_map_fun25741_7(f, v, w, _ctx); /*(i : size_t) -> 7910 ()*/
  kk_std_core__lift22732_forz(_x25740, end_23097, start_23096, _ctx);
  if (kk_yielding(kk_context())) {
    kk_box_t _x25745 = kk_std_core_hnd_yield_extend(kk_std_core_new_map_fun25746_7(w, _ctx), _ctx); /*3991*/
    return kk_vector_unbox(_x25745, _ctx);
  }
  return w;
}
 
// Right-align a string to width `width`  using `fill`  (default is a space) to fill from the left.

kk_string_t kk_std_core_pad_left(kk_string_t s, kk_integer_t width, kk_std_core_types__optional fill, kk_context_t* _ctx) { /* (s : string, width : int, fill : optional<char>) -> string */ 
  size_t w = kk_std_core_size__t(width, _ctx); /*size_t*/;
  size_t n;
  kk_string_t _x25749 = kk_string_dup(s); /*string*/
  n = kk_string_len(_x25749,kk_context()); /*size_t*/
  bool _match_25329 = (w <= n); /*bool*/;
  if (_match_25329) {
    kk_std_core_types__optional_drop(fill, _ctx);
    return s;
  }
  kk_string_t _x25750;
  kk_string_t _x25751;
  kk_char_t _x25752;
  if (kk_std_core_types__is_Optional(fill)) {
    kk_box_t _box_x23966 = fill._cons.Optional.value;
    kk_char_t _fill_8593 = kk_char_unbox(_box_x23966, NULL);
    _x25752 = _fill_8593; /*char*/
    goto _match25753;
  }
  _x25752 = ' '; /*char*/
  _match25753: ;
  _x25751 = kk_std_core_string(_x25752, _ctx); /*string*/
  size_t _x25755 = (w - n); /*size_t*/
  _x25750 = kk_std_core_repeatz(_x25751, _x25755, _ctx); /*string*/
  return kk_std_core__lp__plus__6_rp_(_x25750, s, _ctx);
}
 
// Show an `:int` as a hexadecimal value.
// The `width`  parameter specifies how wide the hex value is where `"0"`  is used to align.
// The `use-capitals` parameter (= `True`) determines if captical letters should be used to display the hexadecimal digits.
// The `pre` (=`"0x"`) is an optional prefix for the number (goes between the sign and the number).

kk_string_t kk_std_core_show_hex(kk_integer_t i, kk_std_core_types__optional width, kk_std_core_types__optional use_capitals, kk_std_core_types__optional pre, kk_context_t* _ctx) { /* (i : int, width : optional<int>, use-capitals : optional<bool>, pre : optional<string>) -> string */ 
  kk_string_t _x25756;
  kk_string_t _x25757;
  bool _match_25328;
  kk_integer_t _x25758 = kk_integer_dup(i); /*int*/
  _match_25328 = kk_integer_lt(_x25758,(kk_integer_from_small(0)),kk_context()); /*bool*/
  if (_match_25328) {
    kk_define_string_literal(, _s25759, 1, "-")
    _x25757 = kk_string_dup(_s25759); /*string*/
  }
  else {
    _x25757 = kk_string_empty(); /*string*/
  }
  kk_string_t _x25761;
  if (kk_std_core_types__is_Optional(pre)) {
    kk_box_t _box_x23967 = pre._cons.Optional.value;
    kk_string_t _pre_8782 = kk_string_unbox(_box_x23967);
    _x25761 = _pre_8782; /*string*/
    goto _match25762;
  }
  kk_define_string_literal(, _s25764, 2, "0x")
  _x25761 = kk_string_dup(_s25764); /*string*/
  _match25762: ;
  _x25756 = kk_std_core__lp__plus__6_rp_(_x25757, _x25761, _ctx); /*string*/
  kk_string_t _x25765;
  kk_string_t s_23104;
  kk_integer_t _x25766 = kk_integer_abs(i,kk_context()); /*int*/
  bool _x25767;
  if (kk_std_core_types__is_Optional(use_capitals)) {
    kk_box_t _box_x23968 = use_capitals._cons.Optional.value;
    bool _use_capitals_8778 = kk_bool_unbox(_box_x23968);
    _x25767 = _use_capitals_8778; /*bool*/
    goto _match25768;
  }
  _x25767 = true; /*bool*/
  _match25768: ;
  s_23104 = kk_std_core_int_show_hex(_x25766, _x25767, _ctx); /*string*/
  kk_std_core_types__optional fill_23106 = kk_std_core_types__new_Optional(kk_char_box('0', _ctx), _ctx); /*optional<char>*/;
  size_t w;
  kk_integer_t _x25770;
  if (kk_std_core_types__is_Optional(width)) {
    kk_box_t _box_x23971 = width._cons.Optional.value;
    kk_integer_t _width_8774 = kk_integer_unbox(_box_x23971);
    _x25770 = _width_8774; /*int*/
    goto _match25771;
  }
  _x25770 = kk_integer_from_small(1); /*int*/
  _match25771: ;
  w = kk_std_core_size__t(_x25770, _ctx); /*size_t*/
  size_t n;
  kk_string_t _x25773 = kk_string_dup(s_23104); /*string*/
  n = kk_string_len(_x25773,kk_context()); /*size_t*/
  bool _match_25327 = (w <= n); /*bool*/;
  if (_match_25327) {
    kk_std_core_types__optional_drop(fill_23106, _ctx);
    _x25765 = s_23104; /*string*/
  }
  else {
    kk_string_t _x25774;
    kk_string_t _x25775;
    kk_char_t _x25776;
    if (kk_std_core_types__is_Optional(fill_23106)) {
      kk_box_t _box_x23972 = fill_23106._cons.Optional.value;
      kk_char_t _fill_8593 = kk_char_unbox(_box_x23972, NULL);
      _x25776 = _fill_8593; /*char*/
      goto _match25777;
    }
    _x25776 = ' '; /*char*/
    _match25777: ;
    _x25775 = kk_std_core_string(_x25776, _ctx); /*string*/
    size_t _x25779 = (w - n); /*size_t*/
    _x25774 = kk_std_core_repeatz(_x25775, _x25779, _ctx); /*string*/
    _x25765 = kk_std_core__lp__plus__6_rp_(_x25774, s_23104, _ctx); /*string*/
  }
  return kk_std_core__lp__plus__6_rp_(_x25756, _x25765, _ctx);
}
 
// Is the integer positive (stricly greater than zero)

bool kk_std_core_is_pos_2(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> bool */ 
  kk_std_core_types__order x_23107 = kk_int_as_order(kk_integer_signum(i,kk_context()),kk_context()); /*order*/;
  kk_integer_t _x25780 = kk_std_core_int_5(x_23107, _ctx); /*int*/
  kk_integer_t _x25781;
  kk_std_core_types__order _x25782 = kk_std_core_types__new_Gt(_ctx); /*order*/
  if (kk_std_core_types__is_Lt(_x25782)) {
    _x25781 = kk_integer_sub((kk_integer_from_small(0)),(kk_integer_from_small(1)),kk_context()); /*int*/
    goto _match25783;
  }
  if (kk_std_core_types__is_Eq(_x25782)) {
    _x25781 = kk_integer_from_small(0); /*int*/
    goto _match25783;
  }
  _x25781 = kk_integer_from_small(1); /*int*/
  _match25783: ;
  return kk_integer_eq(_x25780,_x25781,kk_context());
}
 
// Show a character as a string

kk_string_t kk_std_core_show_char(kk_char_t c, kk_context_t* _ctx) { /* (c : char) -> string */ 
  bool _match_25304;
  bool _match_25322 = (c < (' ')); /*bool*/;
  if (_match_25322) {
    _match_25304 = true; /*bool*/
  }
  else {
    _match_25304 = (c > ('~')); /*bool*/
  }
  if (_match_25304) {
    bool _match_25308 = (c == 0x000A); /*bool*/;
    if (_match_25308) {
      kk_define_string_literal(, _s25789, 2, "\\n")
      return kk_string_dup(_s25789);
    }
    bool _match_25309 = (c == 0x000D); /*bool*/;
    if (_match_25309) {
      kk_define_string_literal(, _s25790, 2, "\\r")
      return kk_string_dup(_s25790);
    }
    bool _match_25310 = (c == 0x0009); /*bool*/;
    if (_match_25310) {
      kk_define_string_literal(, _s25791, 2, "\\t")
      return kk_string_dup(_s25791);
    }
    bool _match_25311;
    kk_integer_t _x25792 = kk_integer_from_int(c,kk_context()); /*int*/
    _match_25311 = kk_integer_lte(_x25792,(kk_integer_from_small(255)),kk_context()); /*bool*/
    if (_match_25311) {
      kk_string_t _x25793;
      kk_define_string_literal(, _s25794, 2, "\\x")
      _x25793 = kk_string_dup(_s25794); /*string*/
      kk_string_t _x25795;
      kk_integer_t _arg_9686 = kk_integer_from_int(c,kk_context()); /*int*/;
      kk_std_core_types__optional _arg_9687 = kk_std_core_types__new_Optional(kk_integer_box(kk_integer_from_small(2)), _ctx); /*optional<int>*/;
      kk_std_core_types__optional _arg_9689;
      kk_box_t _x25796;
      kk_string_t _x25797 = kk_string_empty(); /*string*/
      _x25796 = kk_string_box(_x25797); /*108*/
      _arg_9689 = kk_std_core_types__new_Optional(_x25796, _ctx); /*optional<string>*/
      kk_string_t _x25799;
      kk_string_t _x25800;
      bool _match_25321;
      kk_integer_t _x25801 = kk_integer_dup(_arg_9686); /*int*/
      _match_25321 = kk_integer_lt(_x25801,(kk_integer_from_small(0)),kk_context()); /*bool*/
      if (_match_25321) {
        kk_define_string_literal(, _s25802, 1, "-")
        _x25800 = kk_string_dup(_s25802); /*string*/
      }
      else {
        _x25800 = kk_string_empty(); /*string*/
      }
      kk_string_t _x25804;
      if (kk_std_core_types__is_Optional(_arg_9689)) {
        kk_box_t _box_x23977 = _arg_9689._cons.Optional.value;
        kk_string_t _pre_8782 = kk_string_unbox(_box_x23977);
        _x25804 = _pre_8782; /*string*/
        goto _match25805;
      }
      kk_define_string_literal(, _s25807, 2, "0x")
      _x25804 = kk_string_dup(_s25807); /*string*/
      _match25805: ;
      _x25799 = kk_std_core__lp__plus__6_rp_(_x25800, _x25804, _ctx); /*string*/
      kk_string_t _x25808;
      kk_string_t s_23114;
      kk_integer_t _x25809 = kk_integer_abs(_arg_9686,kk_context()); /*int*/
      bool _x25810;
      kk_std_core_types__optional _x25811 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
      if (kk_std_core_types__is_Optional(_x25811)) {
        kk_box_t _box_x23978 = _x25811._cons.Optional.value;
        bool _use_capitals_8778 = kk_bool_unbox(_box_x23978);
        _x25810 = _use_capitals_8778; /*bool*/
        goto _match25812;
      }
      _x25810 = true; /*bool*/
      _match25812: ;
      s_23114 = kk_std_core_int_show_hex(_x25809, _x25810, _ctx); /*string*/
      kk_std_core_types__optional fill_23116 = kk_std_core_types__new_Optional(kk_char_box('0', _ctx), _ctx); /*optional<char>*/;
      size_t w;
      kk_integer_t _x25814;
      if (kk_std_core_types__is_Optional(_arg_9687)) {
        kk_box_t _box_x23981 = _arg_9687._cons.Optional.value;
        kk_integer_t _width_8774 = kk_integer_unbox(_box_x23981);
        _x25814 = _width_8774; /*int*/
        goto _match25815;
      }
      _x25814 = kk_integer_from_small(1); /*int*/
      _match25815: ;
      w = kk_std_core_size__t(_x25814, _ctx); /*size_t*/
      size_t n;
      kk_string_t _x25817 = kk_string_dup(s_23114); /*string*/
      n = kk_string_len(_x25817,kk_context()); /*size_t*/
      bool _match_25319 = (w <= n); /*bool*/;
      if (_match_25319) {
        kk_std_core_types__optional_drop(fill_23116, _ctx);
        _x25808 = s_23114; /*string*/
      }
      else {
        kk_string_t _x25818;
        kk_string_t _x25819;
        kk_char_t _x25820;
        if (kk_std_core_types__is_Optional(fill_23116)) {
          kk_box_t _box_x23982 = fill_23116._cons.Optional.value;
          kk_char_t _fill_8593 = kk_char_unbox(_box_x23982, NULL);
          _x25820 = _fill_8593; /*char*/
          goto _match25821;
        }
        _x25820 = ' '; /*char*/
        _match25821: ;
        _x25819 = kk_std_core_string(_x25820, _ctx); /*string*/
        size_t _x25823 = (w - n); /*size_t*/
        _x25818 = kk_std_core_repeatz(_x25819, _x25823, _ctx); /*string*/
        _x25808 = kk_std_core__lp__plus__6_rp_(_x25818, s_23114, _ctx); /*string*/
      }
      _x25795 = kk_std_core__lp__plus__6_rp_(_x25799, _x25808, _ctx); /*string*/
      return kk_std_core__lp__plus__6_rp_(_x25793, _x25795, _ctx);
    }
    bool _match_25312;
    kk_integer_t _x25824 = kk_integer_from_int(c,kk_context()); /*int*/
    _match_25312 = kk_integer_lte(_x25824,(kk_integer_from_int(65535, _ctx)),kk_context()); /*bool*/
    if (_match_25312) {
      kk_string_t _x25825;
      kk_define_string_literal(, _s25826, 2, "\\u")
      _x25825 = kk_string_dup(_s25826); /*string*/
      kk_string_t _x25827;
      kk_integer_t _arg_9833 = kk_integer_from_int(c,kk_context()); /*int*/;
      kk_std_core_types__optional _arg_9834 = kk_std_core_types__new_Optional(kk_integer_box(kk_integer_from_small(4)), _ctx); /*optional<int>*/;
      kk_std_core_types__optional _arg_9836;
      kk_box_t _x25828;
      kk_string_t _x25829 = kk_string_empty(); /*string*/
      _x25828 = kk_string_box(_x25829); /*108*/
      _arg_9836 = kk_std_core_types__new_Optional(_x25828, _ctx); /*optional<string>*/
      kk_string_t _x25831;
      kk_string_t _x25832;
      bool _match_25318;
      kk_integer_t _x25833 = kk_integer_dup(_arg_9833); /*int*/
      _match_25318 = kk_integer_lt(_x25833,(kk_integer_from_small(0)),kk_context()); /*bool*/
      if (_match_25318) {
        kk_define_string_literal(, _s25834, 1, "-")
        _x25832 = kk_string_dup(_s25834); /*string*/
      }
      else {
        _x25832 = kk_string_empty(); /*string*/
      }
      kk_string_t _x25836;
      if (kk_std_core_types__is_Optional(_arg_9836)) {
        kk_box_t _box_x23987 = _arg_9836._cons.Optional.value;
        kk_string_t _pre_87820 = kk_string_unbox(_box_x23987);
        _x25836 = _pre_87820; /*string*/
        goto _match25837;
      }
      kk_define_string_literal(, _s25839, 2, "0x")
      _x25836 = kk_string_dup(_s25839); /*string*/
      _match25837: ;
      _x25831 = kk_std_core__lp__plus__6_rp_(_x25832, _x25836, _ctx); /*string*/
      kk_string_t _x25840;
      kk_string_t s0_23121;
      kk_integer_t _x25841 = kk_integer_abs(_arg_9833,kk_context()); /*int*/
      bool _x25842;
      kk_std_core_types__optional _x25843 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
      if (kk_std_core_types__is_Optional(_x25843)) {
        kk_box_t _box_x23988 = _x25843._cons.Optional.value;
        bool _use_capitals_87780 = kk_bool_unbox(_box_x23988);
        _x25842 = _use_capitals_87780; /*bool*/
        goto _match25844;
      }
      _x25842 = true; /*bool*/
      _match25844: ;
      s0_23121 = kk_std_core_int_show_hex(_x25841, _x25842, _ctx); /*string*/
      kk_std_core_types__optional fill0_23123 = kk_std_core_types__new_Optional(kk_char_box('0', _ctx), _ctx); /*optional<char>*/;
      size_t w0;
      kk_integer_t _x25846;
      if (kk_std_core_types__is_Optional(_arg_9834)) {
        kk_box_t _box_x23991 = _arg_9834._cons.Optional.value;
        kk_integer_t _width_87740 = kk_integer_unbox(_box_x23991);
        _x25846 = _width_87740; /*int*/
        goto _match25847;
      }
      _x25846 = kk_integer_from_small(1); /*int*/
      _match25847: ;
      w0 = kk_std_core_size__t(_x25846, _ctx); /*size_t*/
      size_t n0;
      kk_string_t _x25849 = kk_string_dup(s0_23121); /*string*/
      n0 = kk_string_len(_x25849,kk_context()); /*size_t*/
      bool _match_25316 = (w0 <= n0); /*bool*/;
      if (_match_25316) {
        kk_std_core_types__optional_drop(fill0_23123, _ctx);
        _x25840 = s0_23121; /*string*/
      }
      else {
        kk_string_t _x25850;
        kk_string_t _x25851;
        kk_char_t _x25852;
        if (kk_std_core_types__is_Optional(fill0_23123)) {
          kk_box_t _box_x23992 = fill0_23123._cons.Optional.value;
          kk_char_t _fill_85930 = kk_char_unbox(_box_x23992, NULL);
          _x25852 = _fill_85930; /*char*/
          goto _match25853;
        }
        _x25852 = ' '; /*char*/
        _match25853: ;
        _x25851 = kk_std_core_string(_x25852, _ctx); /*string*/
        size_t _x25855 = (w0 - n0); /*size_t*/
        _x25850 = kk_std_core_repeatz(_x25851, _x25855, _ctx); /*string*/
        _x25840 = kk_std_core__lp__plus__6_rp_(_x25850, s0_23121, _ctx); /*string*/
      }
      _x25827 = kk_std_core__lp__plus__6_rp_(_x25831, _x25840, _ctx); /*string*/
      return kk_std_core__lp__plus__6_rp_(_x25825, _x25827, _ctx);
    }
    kk_string_t _x25856;
    kk_define_string_literal(, _s25857, 2, "\\U")
    _x25856 = kk_string_dup(_s25857); /*string*/
    kk_string_t _x25858;
    kk_integer_t _arg_9904 = kk_integer_from_int(c,kk_context()); /*int*/;
    kk_std_core_types__optional _arg_9905 = kk_std_core_types__new_Optional(kk_integer_box(kk_integer_from_small(6)), _ctx); /*optional<int>*/;
    kk_std_core_types__optional _arg_9907;
    kk_box_t _x25859;
    kk_string_t _x25860 = kk_string_empty(); /*string*/
    _x25859 = kk_string_box(_x25860); /*108*/
    _arg_9907 = kk_std_core_types__new_Optional(_x25859, _ctx); /*optional<string>*/
    kk_string_t _x25862;
    kk_string_t _x25863;
    bool _match_25315;
    kk_integer_t _x25864 = kk_integer_dup(_arg_9904); /*int*/
    _match_25315 = kk_integer_lt(_x25864,(kk_integer_from_small(0)),kk_context()); /*bool*/
    if (_match_25315) {
      kk_define_string_literal(, _s25865, 1, "-")
      _x25863 = kk_string_dup(_s25865); /*string*/
    }
    else {
      _x25863 = kk_string_empty(); /*string*/
    }
    kk_string_t _x25867;
    if (kk_std_core_types__is_Optional(_arg_9907)) {
      kk_box_t _box_x23997 = _arg_9907._cons.Optional.value;
      kk_string_t _pre_87821 = kk_string_unbox(_box_x23997);
      _x25867 = _pre_87821; /*string*/
      goto _match25868;
    }
    kk_define_string_literal(, _s25870, 2, "0x")
    _x25867 = kk_string_dup(_s25870); /*string*/
    _match25868: ;
    _x25862 = kk_std_core__lp__plus__6_rp_(_x25863, _x25867, _ctx); /*string*/
    kk_string_t _x25871;
    kk_string_t s1_23128;
    kk_integer_t _x25872 = kk_integer_abs(_arg_9904,kk_context()); /*int*/
    bool _x25873;
    kk_std_core_types__optional _x25874 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
    if (kk_std_core_types__is_Optional(_x25874)) {
      kk_box_t _box_x23998 = _x25874._cons.Optional.value;
      bool _use_capitals_87781 = kk_bool_unbox(_box_x23998);
      _x25873 = _use_capitals_87781; /*bool*/
      goto _match25875;
    }
    _x25873 = true; /*bool*/
    _match25875: ;
    s1_23128 = kk_std_core_int_show_hex(_x25872, _x25873, _ctx); /*string*/
    kk_std_core_types__optional fill1_23130 = kk_std_core_types__new_Optional(kk_char_box('0', _ctx), _ctx); /*optional<char>*/;
    size_t w1;
    kk_integer_t _x25877;
    if (kk_std_core_types__is_Optional(_arg_9905)) {
      kk_box_t _box_x24001 = _arg_9905._cons.Optional.value;
      kk_integer_t _width_87741 = kk_integer_unbox(_box_x24001);
      _x25877 = _width_87741; /*int*/
      goto _match25878;
    }
    _x25877 = kk_integer_from_small(1); /*int*/
    _match25878: ;
    w1 = kk_std_core_size__t(_x25877, _ctx); /*size_t*/
    size_t n1;
    kk_string_t _x25880 = kk_string_dup(s1_23128); /*string*/
    n1 = kk_string_len(_x25880,kk_context()); /*size_t*/
    bool _match_25313 = (w1 <= n1); /*bool*/;
    if (_match_25313) {
      kk_std_core_types__optional_drop(fill1_23130, _ctx);
      _x25871 = s1_23128; /*string*/
    }
    else {
      kk_string_t _x25881;
      kk_string_t _x25882;
      kk_char_t _x25883;
      if (kk_std_core_types__is_Optional(fill1_23130)) {
        kk_box_t _box_x24002 = fill1_23130._cons.Optional.value;
        kk_char_t _fill_85931 = kk_char_unbox(_box_x24002, NULL);
        _x25883 = _fill_85931; /*char*/
        goto _match25884;
      }
      _x25883 = ' '; /*char*/
      _match25884: ;
      _x25882 = kk_std_core_string(_x25883, _ctx); /*string*/
      size_t _x25886 = (w1 - n1); /*size_t*/
      _x25881 = kk_std_core_repeatz(_x25882, _x25886, _ctx); /*string*/
      _x25871 = kk_std_core__lp__plus__6_rp_(_x25881, s1_23128, _ctx); /*string*/
    }
    _x25858 = kk_std_core__lp__plus__6_rp_(_x25862, _x25871, _ctx); /*string*/
    return kk_std_core__lp__plus__6_rp_(_x25856, _x25858, _ctx);
  }
  bool _match_25305 = (c == ('\'')); /*bool*/;
  if (_match_25305) {
    kk_define_string_literal(, _s25887, 2, "\\\'")
    return kk_string_dup(_s25887);
  }
  bool _match_25306 = (c == ('"')); /*bool*/;
  if (_match_25306) {
    kk_define_string_literal(, _s25888, 2, "\\\"")
    return kk_string_dup(_s25888);
  }
  bool _match_25307 = (c == ('\\')); /*bool*/;
  if (_match_25307) {
    kk_define_string_literal(, _s25889, 2, "\\\\")
    return kk_string_dup(_s25889);
  }
  return kk_std_core_string(c, _ctx);
}
 
// Show a `:double` fixed-point notation.
// The optional `precision` (= `-2`) specifies the maximum precision.
// If `>=0` it specifies the number of digits behind the dot (up to `20` max).
// If negative, then at most the absolute value of `precision` digits behind the dot are used.
// This may still show a number in exponential notation if the it is too small or large,
// in particular, for  a `d` where `d > 1.0e21` or `d < 1.0e-15`, or if
// `precision.abs > 17`, the `show-exp` routine is used.

kk_string_t kk_std_core_show_fixed(double d, kk_std_core_types__optional precision, kk_context_t* _ctx) { /* (d : double, precision : optional<int>) -> string */ 
  double dabs = kk_double_abs(d); /*double*/;
  bool _match_25302;
  bool _match_25303 = (dabs < (0x1.203af9ee75616p-50)); /*bool*/;
  if (_match_25303) {
    _match_25302 = true; /*bool*/
  }
  else {
    _match_25302 = (dabs > (0x1.b1ae4d6e2ef5p69)); /*bool*/
  }
  if (_match_25302) {
    kk_std_core_types__optional precision0_23132;
    kk_box_t _x25894;
    kk_integer_t _x25895;
    if (kk_std_core_types__is_Optional(precision)) {
      kk_box_t _box_x24004 = precision._cons.Optional.value;
      kk_integer_t _precision_10039 = kk_integer_unbox(_box_x24004);
      _x25895 = _precision_10039; /*int*/
      goto _match25896;
    }
    _x25895 = kk_integer_from_small(-2); /*int*/
    _match25896: ;
    _x25894 = kk_integer_box(_x25895); /*108*/
    precision0_23132 = kk_std_core_types__new_Optional(_x25894, _ctx); /*optional<int>*/
    int32_t _x25898;
    kk_integer_t _x25899;
    if (kk_std_core_types__is_Optional(precision0_23132)) {
      kk_box_t _box_x24007 = precision0_23132._cons.Optional.value;
      kk_integer_t _precision_10028 = kk_integer_unbox(_box_x24007);
      _x25899 = _precision_10028; /*int*/
      goto _match25900;
    }
    _x25899 = kk_integer_from_small(-17); /*int*/
    _match25900: ;
    _x25898 = kk_std_core_int32(_x25899, _ctx); /*int32*/
    return kk_std_core_show_expx(d, _x25898, _ctx);
  }
  int32_t _x25902;
  kk_integer_t _x25903;
  if (kk_std_core_types__is_Optional(precision)) {
    kk_box_t _box_x24008 = precision._cons.Optional.value;
    kk_integer_t _precision_100390 = kk_integer_unbox(_box_x24008);
    _x25903 = _precision_100390; /*int*/
    goto _match25904;
  }
  _x25903 = kk_integer_from_small(-2); /*int*/
  _match25904: ;
  _x25902 = kk_std_core_int32(_x25903, _ctx); /*int32*/
  return kk_std_core_show_fixedx(d, _x25902, _ctx);
}
 
// monadic lift

kk_string_t kk_std_core__mlift22871_show_list(kk_std_core__list _y_75, kk_context_t* _ctx) { /* forall<e> (list<string>) -> e string */ 
  kk_string_t _x25906;
  kk_string_t _x25907;
  kk_define_string_literal(, _s25908, 1, "[")
  _x25907 = kk_string_dup(_s25908); /*string*/
  kk_string_t _x25909;
  if (kk_std_core__is_Nil(_y_75)) {
    _x25909 = kk_string_empty(); /*string*/
  }
  else {
    struct kk_std_core_Cons* _con25911 = kk_std_core__as_Cons(_y_75);
    kk_box_t _box_x24009 = _con25911->head;
    kk_std_core__list xx = _con25911->tail;
    kk_string_t x = kk_string_unbox(_box_x24009);
    if (kk_std_core__list_is_unique(_y_75)) {
      kk_std_core__list_free(_y_75);
    }
    else {
      kk_string_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(_y_75, _ctx);
    }
    kk_string_t _x25913;
    kk_define_string_literal(, _s25914, 1, ",")
    _x25913 = kk_string_dup(_s25914); /*string*/
    _x25909 = kk_std_core__lift22731_joinsep(_x25913, xx, x, _ctx); /*string*/
  }
  _x25906 = kk_std_core__lp__plus__6_rp_(_x25907, _x25909, _ctx); /*string*/
  kk_string_t _x25915;
  kk_define_string_literal(, _s25916, 1, "]")
  _x25915 = kk_string_dup(_s25916); /*string*/
  return kk_std_core__lp__plus__6_rp_(_x25906, _x25915, _ctx);
}
 
// Convert a list to a string


// lift anonymous function
struct kk_std_core_show_list_fun25917__t {
  struct kk_function_s _base;
  kk_function_t show_elem;
};
static kk_box_t kk_std_core_show_list_fun25917(kk_function_t _fself, kk_box_t _b_24012, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_show_list_fun25917(kk_function_t show_elem, kk_context_t* _ctx) {
  struct kk_std_core_show_list_fun25917__t* _self = kk_function_alloc_as(struct kk_std_core_show_list_fun25917__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_show_list_fun25917, kk_context());
  _self->show_elem = show_elem;
  return &_self->_base;
}

static kk_box_t kk_std_core_show_list_fun25917(kk_function_t _fself, kk_box_t _b_24012, kk_context_t* _ctx) {
  struct kk_std_core_show_list_fun25917__t* _self = kk_function_as(struct kk_std_core_show_list_fun25917__t*, _fself);
  kk_function_t show_elem = _self->show_elem; /* (10722) -> 10723 string */
  kk_drop_match(_self, {kk_function_dup(show_elem);}, {}, _ctx)
  kk_string_t _x25918 = kk_function_call(kk_string_t, (kk_function_t, kk_box_t, kk_context_t*), show_elem, (show_elem, _b_24012, _ctx)); /*string*/
  return kk_string_box(_x25918);
}


// lift anonymous function
struct kk_std_core_show_list_fun25920__t {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_show_list_fun25920(kk_function_t _fself, kk_box_t _b_24016, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_show_list_fun25920(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_show_list_fun25920, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_show_list_fun25920(kk_function_t _fself, kk_box_t _b_24016, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  kk_string_t _x25921;
  kk_std_core__list _x25922 = kk_std_core__list_unbox(_b_24016, _ctx); /*list<string>*/
  _x25921 = kk_std_core__mlift22871_show_list(_x25922, _ctx); /*string*/
  return kk_string_box(_x25921);
}

kk_string_t kk_std_core_show_list(kk_std_core__list xs, kk_function_t show_elem, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, show-elem : (a) -> e string) -> e string */ 
  kk_std_core__list x_23135 = kk_std_core_map_5(xs, kk_std_core_new_show_list_fun25917(show_elem, _ctx), _ctx); /*list<string>*/;
  if (kk_yielding(kk_context())) {
    kk_std_core__list_drop(x_23135, _ctx);
    kk_box_t _x25919 = kk_std_core_hnd_yield_extend(kk_std_core_new_show_list_fun25920(_ctx), _ctx); /*3991*/
    return kk_string_unbox(_x25919);
  }
  kk_string_t _x25923;
  kk_string_t _x25924;
  kk_define_string_literal(, _s25925, 1, "[")
  _x25924 = kk_string_dup(_s25925); /*string*/
  kk_string_t _x25926;
  if (kk_std_core__is_Nil(x_23135)) {
    _x25926 = kk_string_empty(); /*string*/
  }
  else {
    struct kk_std_core_Cons* _con25928 = kk_std_core__as_Cons(x_23135);
    kk_box_t _box_x24017 = _con25928->head;
    kk_std_core__list xx = _con25928->tail;
    kk_string_t x = kk_string_unbox(_box_x24017);
    if (kk_std_core__list_is_unique(x_23135)) {
      kk_std_core__list_free(x_23135);
    }
    else {
      kk_string_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(x_23135, _ctx);
    }
    kk_string_t _x25930;
    kk_define_string_literal(, _s25931, 1, ",")
    _x25930 = kk_string_dup(_s25931); /*string*/
    _x25926 = kk_std_core__lift22731_joinsep(_x25930, xx, x, _ctx); /*string*/
  }
  _x25923 = kk_std_core__lp__plus__6_rp_(_x25924, _x25926, _ctx); /*string*/
  kk_string_t _x25932;
  kk_define_string_literal(, _s25933, 1, "]")
  _x25932 = kk_string_dup(_s25933); /*string*/
  return kk_std_core__lp__plus__6_rp_(_x25923, _x25932, _ctx);
}
 
// Convert an `:int` to a string

kk_string_t kk_std_core_show(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> string */ 
  return kk_integer_to_string(i,kk_context());
}
 
// Show a `:double` as a string.
// If `d >= 1.0e-5` and `d < 1.0e+21`, `show-fixed` is used and otherwise `show-exp`.
// Default `precision` is `-17`.

kk_string_t kk_std_core_show_1(double d, kk_std_core_types__optional precision, kk_context_t* _ctx) { /* (d : double, precision : optional<int>) -> string */ 
  double dabs = kk_double_abs(d); /*double*/;
  bool _match_25297;
  bool _match_25300 = (dabs >= (0x1.4f8b588e368f1p-17)); /*bool*/;
  if (_match_25300) {
    _match_25297 = (dabs < (0x1.b1ae4d6e2ef5p69)); /*bool*/
  }
  else {
    _match_25297 = false; /*bool*/
  }
  if (_match_25297) {
    kk_std_core_types__optional precision0_23142;
    kk_box_t _x25934;
    kk_integer_t _x25935;
    if (kk_std_core_types__is_Optional(precision)) {
      kk_box_t _box_x24020 = precision._cons.Optional.value;
      kk_integer_t _precision_10842 = kk_integer_unbox(_box_x24020);
      _x25935 = _precision_10842; /*int*/
      goto _match25936;
    }
    _x25935 = kk_integer_from_small(-17); /*int*/
    _match25936: ;
    _x25934 = kk_integer_box(_x25935); /*108*/
    precision0_23142 = kk_std_core_types__new_Optional(_x25934, _ctx); /*optional<int>*/
    double dabs0 = kk_double_abs(d); /*double*/;
    bool _match_25298;
    bool _match_25299 = (dabs0 < (0x1.203af9ee75616p-50)); /*bool*/;
    if (_match_25299) {
      _match_25298 = true; /*bool*/
    }
    else {
      _match_25298 = (dabs0 > (0x1.b1ae4d6e2ef5p69)); /*bool*/
    }
    if (_match_25298) {
      kk_std_core_types__optional precision1_23144;
      kk_box_t _x25938;
      kk_integer_t _x25939;
      if (kk_std_core_types__is_Optional(precision0_23142)) {
        kk_box_t _box_x24023 = precision0_23142._cons.Optional.value;
        kk_integer_t _precision_10039 = kk_integer_unbox(_box_x24023);
        _x25939 = _precision_10039; /*int*/
        goto _match25940;
      }
      _x25939 = kk_integer_from_small(-2); /*int*/
      _match25940: ;
      _x25938 = kk_integer_box(_x25939); /*108*/
      precision1_23144 = kk_std_core_types__new_Optional(_x25938, _ctx); /*optional<int>*/
      int32_t _x25942;
      kk_integer_t _x25943;
      if (kk_std_core_types__is_Optional(precision1_23144)) {
        kk_box_t _box_x24026 = precision1_23144._cons.Optional.value;
        kk_integer_t _precision_10028 = kk_integer_unbox(_box_x24026);
        _x25943 = _precision_10028; /*int*/
        goto _match25944;
      }
      _x25943 = kk_integer_from_small(-17); /*int*/
      _match25944: ;
      _x25942 = kk_std_core_int32(_x25943, _ctx); /*int32*/
      return kk_std_core_show_expx(d, _x25942, _ctx);
    }
    int32_t _x25946;
    kk_integer_t _x25947;
    if (kk_std_core_types__is_Optional(precision0_23142)) {
      kk_box_t _box_x24027 = precision0_23142._cons.Optional.value;
      kk_integer_t _precision_100390 = kk_integer_unbox(_box_x24027);
      _x25947 = _precision_100390; /*int*/
      goto _match25948;
    }
    _x25947 = kk_integer_from_small(-2); /*int*/
    _match25948: ;
    _x25946 = kk_std_core_int32(_x25947, _ctx); /*int32*/
    return kk_std_core_show_fixedx(d, _x25946, _ctx);
  }
  kk_std_core_types__optional precision2_23146;
  kk_box_t _x25950;
  kk_integer_t _x25951;
  if (kk_std_core_types__is_Optional(precision)) {
    kk_box_t _box_x24028 = precision._cons.Optional.value;
    kk_integer_t _precision_108420 = kk_integer_unbox(_box_x24028);
    _x25951 = _precision_108420; /*int*/
    goto _match25952;
  }
  _x25951 = kk_integer_from_small(-17); /*int*/
  _match25952: ;
  _x25950 = kk_integer_box(_x25951); /*108*/
  precision2_23146 = kk_std_core_types__new_Optional(_x25950, _ctx); /*optional<int>*/
  int32_t _x25954;
  kk_integer_t _x25955;
  if (kk_std_core_types__is_Optional(precision2_23146)) {
    kk_box_t _box_x24031 = precision2_23146._cons.Optional.value;
    kk_integer_t _precision_100280 = kk_integer_unbox(_box_x24031);
    _x25955 = _precision_100280; /*int*/
    goto _match25956;
  }
  _x25955 = kk_integer_from_small(-17); /*int*/
  _match25956: ;
  _x25954 = kk_std_core_int32(_x25955, _ctx); /*int32*/
  return kk_std_core_show_expx(d, _x25954, _ctx);
}
 
// Show a string as a string literal


// lift anonymous function
struct kk_std_core_show_fun25968__t_3 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_show_fun25968_3(kk_function_t _fself, kk_box_t _b_24034, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_show_fun25968_3(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_show_fun25968_3, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_show_fun25968_3(kk_function_t _fself, kk_box_t _b_24034, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  kk_string_t _x25969;
  kk_char_t _x25970 = kk_char_unbox(_b_24034, _ctx); /*char*/
  _x25969 = kk_std_core_show_char(_x25970, _ctx); /*string*/
  return kk_string_box(_x25969);
}

kk_string_t kk_std_core_show_3(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> string */ 
  kk_string_t _x25964;
  kk_string_t _x25965;
  kk_define_string_literal(, _s25966, 1, "\"")
  _x25965 = kk_string_dup(_s25966); /*string*/
  kk_string_t _x25967;
  kk_std_core__list xs_23148;
  kk_std_core__list _b_24035_24032 = kk_std_core_list_4(s, _ctx); /*list<char>*/;
  xs_23148 = kk_std_core_map_5(_b_24035_24032, kk_std_core_new_show_fun25968_3(_ctx), _ctx); /*list<string>*/
  if (kk_std_core__is_Nil(xs_23148)) {
    _x25967 = kk_string_empty(); /*string*/
  }
  else {
    struct kk_std_core_Cons* _con25972 = kk_std_core__as_Cons(xs_23148);
    kk_box_t _box_x24037 = _con25972->head;
    kk_std_core__list xx = _con25972->tail;
    kk_string_t x = kk_string_unbox(_box_x24037);
    if (kk_std_core__list_is_unique(xs_23148)) {
      kk_std_core__list_free(xs_23148);
    }
    else {
      kk_string_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs_23148, _ctx);
    }
    kk_string_t _x25974 = kk_string_empty(); /*string*/
    _x25967 = kk_std_core__lift22731_joinsep(_x25974, xx, x, _ctx); /*string*/
  }
  _x25964 = kk_std_core__lp__plus__6_rp_(_x25965, _x25967, _ctx); /*string*/
  kk_string_t _x25976;
  kk_define_string_literal(, _s25977, 1, "\"")
  _x25976 = kk_string_dup(_s25977); /*string*/
  return kk_std_core__lp__plus__6_rp_(_x25964, _x25976, _ctx);
}
 
// Show an `:sslice` as a string literal


// lift anonymous function
struct kk_std_core_show_fun25985__t_6 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_show_fun25985_6(kk_function_t _fself, kk_box_t _b_24040, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_show_fun25985_6(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_show_fun25985_6, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_show_fun25985_6(kk_function_t _fself, kk_box_t _b_24040, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  kk_string_t _x25986;
  kk_char_t _x25987 = kk_char_unbox(_b_24040, _ctx); /*char*/
  _x25986 = kk_std_core_show_char(_x25987, _ctx); /*string*/
  return kk_string_box(_x25986);
}

kk_string_t kk_std_core_show_6(kk_std_core__sslice s, kk_context_t* _ctx) { /* (s : sslice) -> string */ 
  kk_string_t s0_23151 = kk_std_core_string_3(s, _ctx); /*string*/;
  kk_string_t _x25981;
  kk_string_t _x25982;
  kk_define_string_literal(, _s25983, 1, "\"")
  _x25982 = kk_string_dup(_s25983); /*string*/
  kk_string_t _x25984;
  kk_std_core__list xs_23152;
  kk_std_core__list _b_24041_24038 = kk_std_core_list_4(s0_23151, _ctx); /*list<char>*/;
  xs_23152 = kk_std_core_map_5(_b_24041_24038, kk_std_core_new_show_fun25985_6(_ctx), _ctx); /*list<string>*/
  if (kk_std_core__is_Nil(xs_23152)) {
    _x25984 = kk_string_empty(); /*string*/
  }
  else {
    struct kk_std_core_Cons* _con25989 = kk_std_core__as_Cons(xs_23152);
    kk_box_t _box_x24043 = _con25989->head;
    kk_std_core__list xx = _con25989->tail;
    kk_string_t x = kk_string_unbox(_box_x24043);
    if (kk_std_core__list_is_unique(xs_23152)) {
      kk_std_core__list_free(xs_23152);
    }
    else {
      kk_string_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs_23152, _ctx);
    }
    kk_string_t _x25991 = kk_string_empty(); /*string*/
    _x25984 = kk_std_core__lift22731_joinsep(_x25991, xx, x, _ctx); /*string*/
  }
  _x25981 = kk_std_core__lp__plus__6_rp_(_x25982, _x25984, _ctx); /*string*/
  kk_string_t _x25993;
  kk_define_string_literal(, _s25994, 1, "\"")
  _x25993 = kk_string_dup(_s25994); /*string*/
  return kk_std_core__lp__plus__6_rp_(_x25981, _x25993, _ctx);
}
extern kk_string_t kk_std_core_show_fun25995_7(kk_function_t _fself, kk_box_t _b_24046, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  kk_string_t _x25996 = kk_string_unbox(_b_24046); /*string*/
  return kk_std_core_show_3(_x25996, _ctx);
}
extern kk_string_t kk_std_core_show_fun25997_8(kk_function_t _fself, kk_box_t _b_24051, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  kk_integer_t _x25998 = kk_integer_unbox(_b_24051); /*int*/
  return kk_std_core_show(_x25998, _ctx);
}
extern kk_string_t kk_std_core_show_fun25999_9(kk_function_t _fself, kk_box_t _b_24056, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  bool _x26000 = kk_bool_unbox(_b_24056); /*bool*/
  return kk_std_core_show_4(_x26000, _ctx);
}

kk_unit_t kk_std_core_prints(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> console () */ 
  kk_std_core_types__maybe _match_25296;
  kk_box_t _x26008;
  kk_ref_t _x26009 = kk_ref_dup(kk_std_core_redirect); /*ref<global,maybe<(string) -> console ()>>*/
  _x26008 = kk_ref_get(_x26009,kk_context()); /*179*/
  _match_25296 = kk_std_core_types__maybe_unbox(_x26008, _ctx); /*maybe<(string) -> console ()>*/
  if (kk_std_core_types__is_Nothing(_match_25296)) {
    kk_std_core_xprints(s, _ctx); return kk_Unit;
  }
  kk_box_t _fun_unbox_x24063 = _match_25296._cons.Just.value;
  kk_box_t _x26010;
  kk_function_t _x26011 = kk_function_unbox(_fun_unbox_x24063); /*(24064) -> console 24065*/
  _x26010 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_context_t*), _x26011, (_x26011, kk_string_box(s), _ctx)); /*24065*/
  kk_unit_unbox(_x26010); return kk_Unit;
}


// lift anonymous function
struct kk_std_core__default_exn_fun26017__t {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core__default_exn_fun26017(kk_function_t _fself, kk_std_core_hnd__marker _b_24085, kk_std_core_hnd__ev _b_24086, kk_box_t _b_24087, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_default_exn_fun26017(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core__default_exn_fun26017, _ctx)
  return kk_function_dup(_fself);
}



// lift anonymous function
struct kk_std_core__default_exn_fun26018__t {
  struct kk_function_s _base;
  kk_box_t _b_24087;
};
static kk_box_t kk_std_core__default_exn_fun26018(kk_function_t _fself, kk_function_t _b_24082, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_default_exn_fun26018(kk_box_t _b_24087, kk_context_t* _ctx) {
  struct kk_std_core__default_exn_fun26018__t* _self = kk_function_alloc_as(struct kk_std_core__default_exn_fun26018__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__default_exn_fun26018, kk_context());
  _self->_b_24087 = _b_24087;
  return &_self->_base;
}

static kk_box_t kk_std_core__default_exn_fun26018(kk_function_t _fself, kk_function_t _b_24082, kk_context_t* _ctx) {
  struct kk_std_core__default_exn_fun26018__t* _self = kk_function_as(struct kk_std_core__default_exn_fun26018__t*, _fself);
  kk_box_t _b_24087 = _self->_b_24087; /* 52 */
  kk_drop_match(_self, {kk_box_dup(_b_24087);}, {}, _ctx)
  kk_function_drop(_b_24082, _ctx);
  kk_unit_t _x26019 = kk_Unit;
  kk_unit_t __ = kk_Unit;
  kk_string_t _x26020;
  kk_define_string_literal(, _s26021, 20, "uncaught exception: ")
  _x26020 = kk_string_dup(_s26021); /*string*/
  kk_std_core_prints(_x26020, _ctx);
  kk_string_t _x26022;
  kk_std_core__exception _match_25294 = kk_std_core__exception_unbox(_b_24087, _ctx); /*exception*/;
  kk_string_t _x = _match_25294.message;
  kk_string_dup(_x);
  kk_std_core__exception_drop(_match_25294, _ctx);
  _x26022 = _x; /*string*/
  kk_std_core_printsln(_x26022, _ctx);
  return kk_unit_box(_x26019);
}
static kk_box_t kk_std_core__default_exn_fun26017(kk_function_t _fself, kk_std_core_hnd__marker _b_24085, kk_std_core_hnd__ev _b_24086, kk_box_t _b_24087, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  kk_std_core_hnd__ev_dropn(_b_24086, ((int32_t)3), _ctx);
  return kk_std_core_hnd_yield_to_final(_b_24085, kk_std_core__new_default_exn_fun26018(_b_24087, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core__default_exn_fun26024__t {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core__default_exn_fun26024(kk_function_t _fself, kk_box_t _b_24092, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_default_exn_fun26024(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core__default_exn_fun26024, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core__default_exn_fun26024(kk_function_t _fself, kk_box_t _b_24092, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  return _b_24092;
}


// lift anonymous function
struct kk_std_core__default_exn_fun26025__t {
  struct kk_function_s _base;
  kk_function_t action;
};
static kk_box_t kk_std_core__default_exn_fun26025(kk_function_t _fself, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_default_exn_fun26025(kk_function_t action, kk_context_t* _ctx) {
  struct kk_std_core__default_exn_fun26025__t* _self = kk_function_alloc_as(struct kk_std_core__default_exn_fun26025__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__default_exn_fun26025, kk_context());
  _self->action = action;
  return &_self->_base;
}

static kk_box_t kk_std_core__default_exn_fun26025(kk_function_t _fself, kk_context_t* _ctx) {
  struct kk_std_core__default_exn_fun26025__t* _self = kk_function_as(struct kk_std_core__default_exn_fun26025__t*, _fself);
  kk_function_t action = _self->action; /* () -> <exn,console|12426> () */
  kk_drop_match(_self, {kk_function_dup(action);}, {}, _ctx)
  kk_unit_t _x26026 = kk_Unit;
  kk_function_call(kk_unit_t, (kk_function_t, kk_context_t*), action, (action, _ctx));
  return kk_unit_box(_x26026);
}

kk_unit_t kk_std_core__default_exn(kk_function_t action, kk_context_t* _ctx) { /* forall<e> (action : () -> <console,exn|e> ()) -> <console|e> () */ 
  int32_t _b_24093_24088 = ((int32_t)0); /*int32*/;
  kk_std_core__hnd_exn _b_24094_24089;
  kk_std_core_hnd__clause1 _x26016 = kk_std_core_hnd__new_Clause1(kk_std_core__new_default_exn_fun26017(_ctx), _ctx); /*std/core/hnd/clause1<52,53,54,55,56>*/
  _b_24094_24089 = kk_std_core__new_Hnd_exn(kk_reuse_null, _x26016, _ctx); /*.hnd-exn<<console|12426>,()>*/
  kk_box_t _x26023 = kk_std_core__handle_exn(_b_24093_24088, _b_24094_24089, kk_std_core__new_default_exn_fun26024(_ctx), kk_std_core__new_default_exn_fun26025(action, _ctx), _ctx); /*1965*/
  kk_unit_unbox(_x26023); return kk_Unit;
}
 
// Get (zero-based) element `n`  of a list. Return a `:maybe` type.

kk_std_core_types__maybe kk_std_core__lp__lb__rb__2_rp_(kk_std_core__list xs, kk_integer_t n, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, n : int) -> maybe<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con26027 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con26027->head;
    kk_std_core__list xx = _con26027->tail;
    if (kk_std_core__list_is_unique(xs)) {
      kk_std_core__list_free(xs);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    bool _match_25292;
    kk_integer_t _x26028 = kk_integer_dup(n); /*int*/
    _match_25292 = kk_integer_gt(_x26028,(kk_integer_from_small(0)),kk_context()); /*bool*/
    if (_match_25292) {
      kk_box_drop(x, _ctx);
      { // tailcall
        kk_integer_t _x26029 = kk_integer_sub(n,(kk_integer_from_small(1)),kk_context()); /*int*/
        xs = xx;
        n = _x26029;
        goto kk__tailcall;
      }
    }
    kk_std_core__list_drop(xx, _ctx);
    bool _match_25293 = kk_integer_eq(n,(kk_integer_from_small(0)),kk_context()); /*bool*/;
    if (_match_25293) {
      return kk_std_core_types__new_Just(x, _ctx);
    }
    kk_box_drop(x, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
  kk_integer_drop(n, _ctx);
  return kk_std_core_types__new_Nothing(_ctx);
}
 
// O(1). Return the string slice from the end of `slice` argument
// to the end of the string.

kk_std_core__sslice kk_std_core_after(kk_std_core__sslice slice0, kk_context_t* _ctx) { /* (slice : sslice) -> sslice */ 
  kk_string_t s = slice0.str;
  size_t start0 = slice0.start;
  size_t len0 = slice0.len;
  kk_string_dup(s);
  kk_std_core__sslice_drop(slice0, _ctx);
  kk_string_t _x26032 = kk_string_dup(s); /*string*/
  size_t _x26033 = (start0 + len0); /*size_t*/
  size_t _x26034;
  size_t _x26035 = kk_string_len(s,kk_context()); /*size_t*/
  size_t _x26036 = (start0 + len0); /*size_t*/
  _x26034 = (_x26035 - _x26036); /*size_t*/
  return kk_std_core__new_Sslice(_x26032, _x26033, _x26034, _ctx);
}
 
// monadic lift

bool kk_std_core__mlift22872_all(kk_function_t predicate, kk_std_core__list xx, bool _y_78, kk_context_t* _ctx) { /* forall<a,e> (predicate : (a) -> e bool, xx : list<a>, bool) -> e bool */ 
  if (_y_78) {
    return kk_std_core_all(xx, predicate, _ctx);
  }
  kk_function_drop(predicate, _ctx);
  kk_std_core__list_drop(xx, _ctx);
  return false;
}
 
// Do all elements satisfy a predicate ?


// lift anonymous function
struct kk_std_core_all_fun26040__t {
  struct kk_function_s _base;
  kk_function_t predicate0;
  kk_std_core__list xx0;
};
static kk_box_t kk_std_core_all_fun26040(kk_function_t _fself, kk_box_t _b_24106, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_all_fun26040(kk_function_t predicate0, kk_std_core__list xx0, kk_context_t* _ctx) {
  struct kk_std_core_all_fun26040__t* _self = kk_function_alloc_as(struct kk_std_core_all_fun26040__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_all_fun26040, kk_context());
  _self->predicate0 = predicate0;
  _self->xx0 = xx0;
  return &_self->_base;
}

static kk_box_t kk_std_core_all_fun26040(kk_function_t _fself, kk_box_t _b_24106, kk_context_t* _ctx) {
  struct kk_std_core_all_fun26040__t* _self = kk_function_as(struct kk_std_core_all_fun26040__t*, _fself);
  kk_function_t predicate0 = _self->predicate0; /* (12769) -> 12770 bool */
  kk_std_core__list xx0 = _self->xx0; /* list<12769> */
  kk_drop_match(_self, {kk_function_dup(predicate0);kk_std_core__list_dup(xx0);}, {}, _ctx)
  bool _x26041;
  bool _x26042 = kk_bool_unbox(_b_24106); /*bool*/
  _x26041 = kk_std_core__mlift22872_all(predicate0, xx0, _x26042, _ctx); /*bool*/
  return kk_bool_box(_x26041);
}

bool kk_std_core_all(kk_std_core__list xs, kk_function_t predicate0, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, predicate : (a) -> e bool) -> e bool */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con26037 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con26037->head;
    kk_std_core__list xx0 = _con26037->tail;
    if (kk_std_core__list_is_unique(xs)) {
      kk_std_core__list_free(xs);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx0);
      kk_std_core__list_decref(xs, _ctx);
    }
    bool x0_23162;
    kk_function_t _x26038 = kk_function_dup(predicate0); /*(12769) -> 12770 bool*/
    x0_23162 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), _x26038, (_x26038, x, _ctx)); /*bool*/
    if (kk_yielding(kk_context())) {
      kk_box_t _x26039 = kk_std_core_hnd_yield_extend(kk_std_core_new_all_fun26040(predicate0, xx0, _ctx), _ctx); /*3991*/
      return kk_bool_unbox(_x26039);
    }
    if (x0_23162) {
      { // tailcall
        xs = xx0;
        goto kk__tailcall;
      }
    }
    kk_function_drop(predicate0, _ctx);
    kk_std_core__list_drop(xx0, _ctx);
    return false;
  }
  kk_function_drop(predicate0, _ctx);
  return true;
}
 
// monadic lift

bool kk_std_core__mlift22873_any(kk_function_t predicate, kk_std_core__list xx, bool _y_82, kk_context_t* _ctx) { /* forall<a,e> (predicate : (a) -> e bool, xx : list<a>, bool) -> e bool */ 
  if (_y_82) {
    kk_function_drop(predicate, _ctx);
    kk_std_core__list_drop(xx, _ctx);
    return true;
  }
  return kk_std_core_any(xx, predicate, _ctx);
}
 
// Are there any elements in a list that satisfy a predicate ?


// lift anonymous function
struct kk_std_core_any_fun26046__t {
  struct kk_function_s _base;
  kk_function_t predicate0;
  kk_std_core__list xx0;
};
static kk_box_t kk_std_core_any_fun26046(kk_function_t _fself, kk_box_t _b_24110, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_any_fun26046(kk_function_t predicate0, kk_std_core__list xx0, kk_context_t* _ctx) {
  struct kk_std_core_any_fun26046__t* _self = kk_function_alloc_as(struct kk_std_core_any_fun26046__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_any_fun26046, kk_context());
  _self->predicate0 = predicate0;
  _self->xx0 = xx0;
  return &_self->_base;
}

static kk_box_t kk_std_core_any_fun26046(kk_function_t _fself, kk_box_t _b_24110, kk_context_t* _ctx) {
  struct kk_std_core_any_fun26046__t* _self = kk_function_as(struct kk_std_core_any_fun26046__t*, _fself);
  kk_function_t predicate0 = _self->predicate0; /* (12797) -> 12798 bool */
  kk_std_core__list xx0 = _self->xx0; /* list<12797> */
  kk_drop_match(_self, {kk_function_dup(predicate0);kk_std_core__list_dup(xx0);}, {}, _ctx)
  bool _x26047;
  bool _x26048 = kk_bool_unbox(_b_24110); /*bool*/
  _x26047 = kk_std_core__mlift22873_any(predicate0, xx0, _x26048, _ctx); /*bool*/
  return kk_bool_box(_x26047);
}

bool kk_std_core_any(kk_std_core__list xs, kk_function_t predicate0, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, predicate : (a) -> e bool) -> e bool */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con26043 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con26043->head;
    kk_std_core__list xx0 = _con26043->tail;
    if (kk_std_core__list_is_unique(xs)) {
      kk_std_core__list_free(xs);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx0);
      kk_std_core__list_decref(xs, _ctx);
    }
    bool x0_23166;
    kk_function_t _x26044 = kk_function_dup(predicate0); /*(12797) -> 12798 bool*/
    x0_23166 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), _x26044, (_x26044, x, _ctx)); /*bool*/
    if (kk_yielding(kk_context())) {
      kk_box_t _x26045 = kk_std_core_hnd_yield_extend(kk_std_core_new_any_fun26046(predicate0, xx0, _ctx), _ctx); /*3991*/
      return kk_bool_unbox(_x26045);
    }
    if (x0_23166) {
      kk_function_drop(predicate0, _ctx);
      kk_std_core__list_drop(xx0, _ctx);
      return true;
    }
    { // tailcall
      xs = xx0;
      goto kk__tailcall;
    }
  }
  kk_function_drop(predicate0, _ctx);
  return false;
}
 
// Throw an exception with a specified message.

kk_box_t kk_std_core_throw(kk_string_t message0, kk_std_core_types__optional info0, kk_context_t* _ctx) { /* forall<a> (message : string, info : optional<exception-info>) -> exn a */ 
  kk_std_core__exception exn_23170;
  kk_std_core__exception_info _x26049;
  if (kk_std_core_types__is_Optional(info0)) {
    kk_box_t _box_x24113 = info0._cons.Optional.value;
    kk_std_core__exception_info _info_12803 = kk_std_core__exception_info_unbox(_box_x24113, NULL);
    _x26049 = _info_12803; /*exception-info*/
    goto _match26050;
  }
  _x26049 = kk_std_core__new_ExnError(_ctx); /*exception-info*/
  _match26050: ;
  exn_23170 = kk_std_core__new_Exception(message0, _x26049, _ctx); /*exception*/
  kk_std_core_hnd__ev ev_23171;
  size_t _x26052 = ((size_t)0); /*size_t*/
  ev_23171 = kk_evv_at(_x26052,kk_context()); /*std/core/hnd/ev<.hnd-exn>*/
  struct kk_std_core_hnd_Ev* _con26053 = kk_std_core_hnd__as_Ev(ev_23171);
  kk_std_core_hnd__marker m = _con26053->marker;
  kk_box_t _box_x24114 = _con26053->hnd;
  kk_std_core__hnd_exn h = kk_std_core__hnd_exn_unbox(_box_x24114, NULL);
  kk_std_core__hnd_exn_dup(h);
  kk_std_core_hnd__clause1 _match_25289;
  struct kk_std_core__Hnd_exn* _con26055 = kk_std_core__as_Hnd_exn(h);
  kk_std_core_hnd__clause1 except_throw_exn = _con26055->except_throw_exn;
  if (kk_std_core__hnd_exn_is_unique(h)) {
    kk_std_core__hnd_exn_free(h);
  }
  else {
    kk_std_core_hnd__clause1_dup(except_throw_exn);
    kk_std_core__hnd_exn_decref(h, _ctx);
  }
  _match_25289 = except_throw_exn; /*forall<a> std/core/hnd/clause1<exception,a,.hnd-exn,3528,3529>*/
  kk_function_t _fun_unbox_x24118 = _match_25289.clause;
  return kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_box_t, kk_context_t*), _fun_unbox_x24118, (_fun_unbox_x24118, m, ev_23171, kk_std_core__exception_box(exn_23170, _ctx), _ctx));
}

kk_unit_t kk_std_core_assert(kk_string_t message0, bool condition, kk_context_t* _ctx) { /* (message : string, condition : bool) -> () */ 
  bool _match_25287 = !(condition); /*bool*/;
  if (_match_25287) {
    kk_std_core_types__optional info_23176 = kk_std_core_types__new_Optional(kk_std_core__exception_info_box(kk_std_core__new_ExnAssert(_ctx), _ctx), _ctx); /*optional<exception-info>*/;
    kk_std_core__exception exn_23177;
    kk_std_core__exception_info _x26056;
    if (kk_std_core_types__is_Optional(info_23176)) {
      kk_box_t _box_x24127 = info_23176._cons.Optional.value;
      kk_std_core__exception_info _info_12803 = kk_std_core__exception_info_unbox(_box_x24127, NULL);
      _x26056 = _info_12803; /*exception-info*/
      goto _match26057;
    }
    _x26056 = kk_std_core__new_ExnError(_ctx); /*exception-info*/
    _match26057: ;
    exn_23177 = kk_std_core__new_Exception(message0, _x26056, _ctx); /*exception*/
    kk_std_core_hnd__ev ev_23178;
    size_t _x26059 = ((size_t)0); /*size_t*/
    ev_23178 = kk_evv_at(_x26059,kk_context()); /*std/core/hnd/ev<.hnd-exn>*/
    kk_box_t _x26060;
    struct kk_std_core_hnd_Ev* _con26061 = kk_std_core_hnd__as_Ev(ev_23178);
    kk_std_core_hnd__marker m = _con26061->marker;
    kk_box_t _box_x24128 = _con26061->hnd;
    kk_std_core__hnd_exn h = kk_std_core__hnd_exn_unbox(_box_x24128, NULL);
    kk_std_core__hnd_exn_dup(h);
    kk_std_core_hnd__clause1 _match_25288;
    struct kk_std_core__Hnd_exn* _con26063 = kk_std_core__as_Hnd_exn(h);
    kk_std_core_hnd__clause1 except_throw_exn = _con26063->except_throw_exn;
    if (kk_std_core__hnd_exn_is_unique(h)) {
      kk_std_core__hnd_exn_free(h);
    }
    else {
      kk_std_core_hnd__clause1_dup(except_throw_exn);
      kk_std_core__hnd_exn_decref(h, _ctx);
    }
    _match_25288 = except_throw_exn; /*forall<a> std/core/hnd/clause1<exception,a,.hnd-exn,3528,3529>*/
    kk_function_t _fun_unbox_x24132 = _match_25288.clause;
    _x26060 = kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_box_t, kk_context_t*), _fun_unbox_x24132, (_fun_unbox_x24132, m, ev_23178, kk_std_core__exception_box(exn_23177, _ctx), _ctx)); /*53*/
    kk_unit_unbox(_x26060); return kk_Unit;
  }
  kk_string_drop(message0, _ctx);
  kk_Unit; return kk_Unit;
}
 
// Return the element at position `index` in vector `v`, or `Nothing` if out of bounds

kk_std_core_types__maybe kk_std_core_at(kk_vector_t v, kk_integer_t index, kk_context_t* _ctx) { /* forall<a> (v : vector<a>, index : int) -> maybe<a> */ 
  size_t idx = kk_std_core_size__t(index, _ctx); /*size_t*/;
  bool _match_25286;
  size_t _x26064;
  kk_vector_t _x26065 = kk_vector_dup(v); /*vector<12958>*/
  _x26064 = kk_vector_len(_x26065); /*size_t*/
  _match_25286 = (_x26064 <= idx); /*bool*/
  if (_match_25286) {
    kk_vector_drop(v, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
  kk_box_t _x26066 = kk_vector_at(v,idx); /*216*/
  return kk_std_core_types__new_Just(_x26066, _ctx);
}
 
// O(`n`). The first `n` (default = `1`) characters in a string.

kk_std_core__sslice kk_std_core_first(kk_string_t s, kk_std_core_types__optional n, kk_context_t* _ctx) { /* (s : string, n : optional<int>) -> sslice */ 
  kk_std_core__sslice slice0 = kk_std_core_first1(s, _ctx); /*sslice*/;
  bool _match_25285;
  kk_integer_t _x26070;
  if (kk_std_core_types__is_Optional(n)) {
    kk_box_t _box_x24139 = n._cons.Optional.value;
    kk_integer_t _n_13060 = kk_integer_unbox(_box_x24139);
    kk_integer_dup(_n_13060);
    _x26070 = _n_13060; /*int*/
    goto _match26071;
  }
  _x26070 = kk_integer_from_small(1); /*int*/
  _match26071: ;
  _match_25285 = kk_integer_eq(_x26070,(kk_integer_from_small(1)),kk_context()); /*bool*/
  if (_match_25285) {
    kk_std_core_types__optional_drop(n, _ctx);
    return slice0;
  }
  kk_integer_t _x26073;
  kk_integer_t _x26074;
  if (kk_std_core_types__is_Optional(n)) {
    kk_box_t _box_x24140 = n._cons.Optional.value;
    kk_integer_t _n_130600 = kk_integer_unbox(_box_x24140);
    _x26074 = _n_130600; /*int*/
    goto _match26075;
  }
  _x26074 = kk_integer_from_small(1); /*int*/
  _match26075: ;
  _x26073 = kk_integer_sub(_x26074,(kk_integer_from_small(1)),kk_context()); /*int*/
  return kk_std_core_extend(slice0, _x26073, _ctx);
}
 
// Convert the first character of a string to uppercase.

kk_string_t kk_std_core_capitalize(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> string */ 
  kk_string_t _x26077;
  kk_string_t _x26078;
  kk_std_core__sslice _x26079;
  kk_std_core__sslice slice0;
  kk_string_t _x26080 = kk_string_dup(s); /*string*/
  slice0 = kk_std_core_first1(_x26080, _ctx); /*sslice*/
  bool _match_25282;
  kk_integer_t _x26081;
  kk_std_core_types__optional _x26082 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
  if (kk_std_core_types__is_Optional(_x26082)) {
    kk_box_t _box_x24141 = _x26082._cons.Optional.value;
    kk_integer_t _n_13060 = kk_integer_unbox(_box_x24141);
    _x26081 = _n_13060; /*int*/
    goto _match26083;
  }
  _x26081 = kk_integer_from_small(1); /*int*/
  _match26083: ;
  _match_25282 = kk_integer_eq(_x26081,(kk_integer_from_small(1)),kk_context()); /*bool*/
  if (_match_25282) {
    _x26079 = slice0; /*sslice*/
  }
  else {
    kk_integer_t _x26085;
    kk_integer_t _x26086;
    kk_std_core_types__optional _x26087 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
    if (kk_std_core_types__is_Optional(_x26087)) {
      kk_box_t _box_x24142 = _x26087._cons.Optional.value;
      kk_integer_t _n_130600 = kk_integer_unbox(_box_x24142);
      _x26086 = _n_130600; /*int*/
      goto _match26088;
    }
    _x26086 = kk_integer_from_small(1); /*int*/
    _match26088: ;
    _x26085 = kk_integer_sub(_x26086,(kk_integer_from_small(1)),kk_context()); /*int*/
    _x26079 = kk_std_core_extend(slice0, _x26085, _ctx); /*sslice*/
  }
  _x26078 = kk_std_core_string_3(_x26079, _ctx); /*string*/
  _x26077 = kk_std_core_to_upper(_x26078, _ctx); /*string*/
  kk_string_t _x26090;
  kk_std_core__sslice _x26091;
  kk_std_core__sslice slice0_23183;
  kk_std_core__sslice slice1 = kk_std_core_first1(s, _ctx); /*sslice*/;
  bool _match_25279;
  kk_integer_t _x26092;
  kk_std_core_types__optional _x26093 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
  if (kk_std_core_types__is_Optional(_x26093)) {
    kk_box_t _box_x24143 = _x26093._cons.Optional.value;
    kk_integer_t _n_130601 = kk_integer_unbox(_box_x24143);
    _x26092 = _n_130601; /*int*/
    goto _match26094;
  }
  _x26092 = kk_integer_from_small(1); /*int*/
  _match26094: ;
  _match_25279 = kk_integer_eq(_x26092,(kk_integer_from_small(1)),kk_context()); /*bool*/
  if (_match_25279) {
    slice0_23183 = slice1; /*sslice*/
  }
  else {
    kk_integer_t _x26096;
    kk_integer_t _x26097;
    kk_std_core_types__optional _x26098 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
    if (kk_std_core_types__is_Optional(_x26098)) {
      kk_box_t _box_x24144 = _x26098._cons.Optional.value;
      kk_integer_t _n_130602 = kk_integer_unbox(_box_x24144);
      _x26097 = _n_130602; /*int*/
      goto _match26099;
    }
    _x26097 = kk_integer_from_small(1); /*int*/
    _match26099: ;
    _x26096 = kk_integer_sub(_x26097,(kk_integer_from_small(1)),kk_context()); /*int*/
    slice0_23183 = kk_std_core_extend(slice1, _x26096, _ctx); /*sslice*/
  }
  kk_string_t s1 = slice0_23183.str;
  size_t start0 = slice0_23183.start;
  size_t len0 = slice0_23183.len;
  kk_string_dup(s1);
  kk_std_core__sslice_drop(slice0_23183, _ctx);
  kk_string_t _x26101 = kk_string_dup(s1); /*string*/
  size_t _x26102 = (start0 + len0); /*size_t*/
  size_t _x26103;
  size_t _x26104 = kk_string_len(s1,kk_context()); /*size_t*/
  size_t _x26105 = (start0 + len0); /*size_t*/
  _x26103 = (_x26104 - _x26105); /*size_t*/
  _x26091 = kk_std_core__new_Sslice(_x26101, _x26102, _x26103, _ctx); /*sslice*/
  _x26090 = kk_std_core_string_3(_x26091, _ctx); /*string*/
  return kk_std_core__lp__plus__6_rp_(_x26077, _x26090, _ctx);
}
 
// Catch any exception raised in `action` and handle it.
// Use `on-exn` or `on-exit` when appropiate.


// lift anonymous function
struct kk_std_core_try_fun26108__t {
  struct kk_function_s _base;
  kk_function_t hndl;
};
static kk_box_t kk_std_core_try_fun26108(kk_function_t _fself, kk_box_t _b_24146, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_fun26108(kk_function_t hndl, kk_context_t* _ctx) {
  struct kk_std_core_try_fun26108__t* _self = kk_function_alloc_as(struct kk_std_core_try_fun26108__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_try_fun26108, kk_context());
  _self->hndl = hndl;
  return &_self->_base;
}

static kk_box_t kk_std_core_try_fun26108(kk_function_t _fself, kk_box_t _b_24146, kk_context_t* _ctx) {
  struct kk_std_core_try_fun26108__t* _self = kk_function_as(struct kk_std_core_try_fun26108__t*, _fself);
  kk_function_t hndl = _self->hndl; /* (exception) -> 13298 13297 */
  kk_drop_match(_self, {kk_function_dup(hndl);}, {}, _ctx)
  kk_std_core__exception _x26109 = kk_std_core__exception_unbox(_b_24146, _ctx); /*exception*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_std_core__exception, kk_context_t*), hndl, (hndl, _x26109, _ctx));
}


// lift anonymous function
struct kk_std_core_try_fun26110__t {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_try_fun26110(kk_function_t _fself, kk_box_t _x, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_fun26110(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_try_fun26110, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_try_fun26110(kk_function_t _fself, kk_box_t _x, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  return _x;
}

kk_box_t kk_std_core_try(kk_function_t action, kk_function_t hndl, kk_context_t* _ctx) { /* forall<a,e> (action : () -> <exn|e> a, hndl : (exception) -> e a) -> e a */ 
  kk_std_core__hnd_exn _x26106;
  kk_std_core_hnd__clause1 _x26107 = kk_std_core_hnd_clause_never1(kk_std_core_new_try_fun26108(hndl, _ctx), _ctx); /*std/core/hnd/clause1<7986,7987,7989,7988,7990>*/
  _x26106 = kk_std_core__new_Hnd_exn(kk_reuse_null, _x26107, _ctx); /*.hnd-exn<11,12>*/
  return kk_std_core__handle_exn(((int32_t)0), _x26106, kk_std_core_new_try_fun26110(_ctx), action, _ctx);
}
 
// monadic lift

kk_std_core__error kk_std_core__mlift22874_try_1(kk_box_t _y_92, kk_context_t* _ctx) { /* forall<a,e> (a) -> <exn|e> error<a> */ 
  return kk_std_core__new_Ok(_y_92, _ctx);
}
 
// Transform an exception effect to an  `:error` type.


// lift anonymous function
struct kk_std_core_try_fun26112__t_1 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_try_fun26112_1(kk_function_t _fself, kk_std_core_hnd__marker _b_24153, kk_std_core_hnd__ev _b_24154, kk_box_t _b_24155, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_fun26112_1(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_try_fun26112_1, _ctx)
  return kk_function_dup(_fself);
}



// lift anonymous function
struct kk_std_core_try_fun26113__t_1 {
  struct kk_function_s _base;
  kk_box_t _b_24155;
};
static kk_box_t kk_std_core_try_fun26113_1(kk_function_t _fself, kk_function_t _b_24150, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_fun26113_1(kk_box_t _b_24155, kk_context_t* _ctx) {
  struct kk_std_core_try_fun26113__t_1* _self = kk_function_alloc_as(struct kk_std_core_try_fun26113__t_1, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_try_fun26113_1, kk_context());
  _self->_b_24155 = _b_24155;
  return &_self->_base;
}

static kk_box_t kk_std_core_try_fun26113_1(kk_function_t _fself, kk_function_t _b_24150, kk_context_t* _ctx) {
  struct kk_std_core_try_fun26113__t_1* _self = kk_function_as(struct kk_std_core_try_fun26113__t_1*, _fself);
  kk_box_t _b_24155 = _self->_b_24155; /* 52 */
  kk_drop_match(_self, {kk_box_dup(_b_24155);}, {}, _ctx)
  kk_function_drop(_b_24150, _ctx);
  kk_std_core__error _x26114;
  kk_std_core__exception _x26115 = kk_std_core__exception_unbox(_b_24155, _ctx); /*exception*/
  _x26114 = kk_std_core__new_Error(_x26115, _ctx); /*error<30>*/
  return kk_std_core__error_box(_x26114, _ctx);
}
static kk_box_t kk_std_core_try_fun26112_1(kk_function_t _fself, kk_std_core_hnd__marker _b_24153, kk_std_core_hnd__ev _b_24154, kk_box_t _b_24155, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  kk_std_core_hnd__ev_dropn(_b_24154, ((int32_t)3), _ctx);
  return kk_std_core_hnd_yield_to_final(_b_24153, kk_std_core_new_try_fun26113_1(_b_24155, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_try_fun26117__t_1 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_try_fun26117_1(kk_function_t _fself, kk_box_t _b_24162, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_fun26117_1(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_try_fun26117_1, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_try_fun26117_1(kk_function_t _fself, kk_box_t _b_24162, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  return _b_24162;
}


// lift anonymous function
struct kk_std_core_try_fun26118__t_1 {
  struct kk_function_s _base;
  kk_function_t action;
};
static kk_box_t kk_std_core_try_fun26118_1(kk_function_t _fself, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_fun26118_1(kk_function_t action, kk_context_t* _ctx) {
  struct kk_std_core_try_fun26118__t_1* _self = kk_function_alloc_as(struct kk_std_core_try_fun26118__t_1, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_try_fun26118_1, kk_context());
  _self->action = action;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_try_fun26121__t_1 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_try_fun26121_1(kk_function_t _fself, kk_box_t _b_24157, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_fun26121_1(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_try_fun26121_1, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_try_fun26121_1(kk_function_t _fself, kk_box_t _b_24157, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  kk_std_core__error _x26122 = kk_std_core__mlift22874_try_1(_b_24157, _ctx); /*error<13336>*/
  return kk_std_core__error_box(_x26122, _ctx);
}
static kk_box_t kk_std_core_try_fun26118_1(kk_function_t _fself, kk_context_t* _ctx) {
  struct kk_std_core_try_fun26118__t_1* _self = kk_function_as(struct kk_std_core_try_fun26118__t_1*, _fself);
  kk_function_t action = _self->action; /* () -> <exn|13337> 13336 */
  kk_drop_match(_self, {kk_function_dup(action);}, {}, _ctx)
  kk_std_core__error _x26119;
  kk_box_t x_23188 = kk_function_call(kk_box_t, (kk_function_t, kk_context_t*), action, (action, _ctx)); /*13336*/;
  if (kk_yielding(kk_context())) {
    kk_box_drop(x_23188, _ctx);
    kk_box_t _x26120 = kk_std_core_hnd_yield_extend(kk_std_core_new_try_fun26121_1(_ctx), _ctx); /*3991*/
    _x26119 = kk_std_core__error_unbox(_x26120, _ctx); /*error<13336>*/
  }
  else {
    _x26119 = kk_std_core__mlift22874_try_1(x_23188, _ctx); /*error<13336>*/
  }
  return kk_std_core__error_box(_x26119, _ctx);
}

kk_std_core__error kk_std_core_try_1(kk_function_t action, kk_context_t* _ctx) { /* forall<a,e> (action : () -> <exn|e> a) -> e error<a> */ 
  int32_t _b_24163_24158 = ((int32_t)0); /*int32*/;
  kk_std_core__hnd_exn _b_24164_24159;
  kk_std_core_hnd__clause1 _x26111 = kk_std_core_hnd__new_Clause1(kk_std_core_new_try_fun26112_1(_ctx), _ctx); /*std/core/hnd/clause1<52,53,54,55,56>*/
  _b_24164_24159 = kk_std_core__new_Hnd_exn(kk_reuse_null, _x26111, _ctx); /*.hnd-exn<13337,error<13336>>*/
  kk_box_t _x26116 = kk_std_core__handle_exn(_b_24163_24158, _b_24164_24159, kk_std_core_new_try_fun26117_1(_ctx), kk_std_core_new_try_fun26118_1(action, _ctx), _ctx); /*1965*/
  return kk_std_core__error_unbox(_x26116, _ctx);
}

kk_std_core_types__tuple2_ kk_std_core_cdivmod_exp10(kk_integer_t i, kk_integer_t n, kk_context_t* _ctx) { /* (i : int, n : int) -> (int, int) */ 
  bool _match_25277;
  kk_integer_t _x26125 = kk_integer_dup(n); /*int*/
  _match_25277 = kk_integer_lte(_x26125,(kk_integer_from_small(0)),kk_context()); /*bool*/
  if (_match_25277) {
    kk_integer_drop(n, _ctx);
    return kk_std_core_types__new_dash__lp__comma__rp_(kk_integer_box(i), kk_integer_box(kk_integer_from_small(0)), _ctx);
  }
  kk_integer_t cq;
  kk_integer_t _x26126 = kk_integer_dup(i); /*int*/
  kk_integer_t _x26127 = kk_integer_dup(n); /*int*/
  cq = kk_std_core_cdiv_exp10(_x26126, _x26127, _ctx); /*int*/
  kk_integer_t cr;
  kk_integer_t _x26128;
  kk_integer_t _x26129 = kk_integer_dup(cq); /*int*/
  _x26128 = kk_std_core_mul_exp10(_x26129, n, _ctx); /*int*/
  cr = kk_integer_sub(i,_x26128,kk_context()); /*int*/
  return kk_std_core_types__new_dash__lp__comma__rp_(kk_integer_box(cq), kk_integer_box(cr), _ctx);
}
 
// Concatenate a list of `:maybe` values

kk_std_core__list kk_std_core__ctail_concat_maybe(kk_std_core__list xs, kk_std_core_types__ctail _acc, kk_context_t* _ctx) { /* forall<a> (xs : list<maybe<a>>, ctail<list<a>>) -> list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con26130 = kk_std_core__as_Cons(xs);
    kk_box_t _box_x24185 = _con26130->head;
    kk_std_core__list xx = _con26130->tail;
    kk_std_core_types__maybe x = kk_std_core_types__maybe_unbox(_box_x24185, NULL);
    kk_reuse_t _ru_25097 = kk_reuse_null; /*reuse*/;
    if (kk_std_core__list_is_unique(xs)) {
      kk_std_core_types__maybe_dup(x);
      kk_box_drop(_box_x24185, _ctx);
      _ru_25097 = (kk_std_core__list_reuse(xs));
    }
    else {
      kk_std_core_types__maybe_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
      _ru_25097 = kk_reuse_null;
    }
    if (kk_std_core_types__is_Just(x)) {
      kk_box_t y = x._cons.Just.value;
      kk_std_core__list _ctail_22810 = kk_std_core__list_hole(); /*list<13502>*/;
      kk_std_core__list _ctail_22811 = kk_std_core__new_Cons(_ru_25097, y, _ctail_22810, _ctx); /*list<13502>*/;
      { // tailcall
        kk_std_core_types__ctail _x26132;
        kk_box_t* field_23192 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_22811)->tail)); /*cfield<list<13502>>*/;
        if (kk_std_core_types__is_CTail(_acc)) {
          kk_box_t _box_x24192 = _acc._cons.CTail._field1;
          kk_box_t* last0 = _acc._cons.CTail._field2;
          kk_std_core__list head0 = kk_std_core__list_unbox(_box_x24192, NULL);
          kk_unit_t __ = kk_Unit;
          *(last0) = kk_std_core__list_box(_ctail_22811, _ctx);
          _x26132 = kk_std_core_types__new_CTail(kk_std_core__list_box(head0, _ctx), field_23192, _ctx); /*ctail<58>*/
          goto _match26133;
        }
        _x26132 = kk_std_core_types__new_CTail(kk_std_core__list_box(_ctail_22811, _ctx), field_23192, _ctx); /*ctail<58>*/
        _match26133: ;
        xs = xx;
        _acc = _x26132;
        goto kk__tailcall;
      }
    }
    kk_reuse_drop(_ru_25097, _ctx);
    { // tailcall
      xs = xx;
      goto kk__tailcall;
    }
  }
  if (kk_std_core_types__is_CTail(_acc)) {
    kk_box_t _box_x24201 = _acc._cons.CTail._field1;
    kk_box_t* last00 = _acc._cons.CTail._field2;
    kk_std_core__list head00 = kk_std_core__list_unbox(_box_x24201, NULL);
    kk_unit_t __0 = kk_Unit;
    *(last00) = kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx);
    return head00;
  }
  return kk_std_core__new_Nil(_ctx);
}
 
// Concatenate a list of `:maybe` values

kk_std_core__list kk_std_core_concat_maybe(kk_std_core__list xs0, kk_context_t* _ctx) { /* forall<a> (xs : list<maybe<a>>) -> list<a> */ 
  return kk_std_core__ctail_concat_maybe(xs0, kk_std_core_types__new_CTailNil(_ctx), _ctx);
}
 
// monadic lift

kk_std_core_types__maybe kk_std_core__mlift22875_op(kk_function_t action, size_t end, size_t i, kk_std_core_types__maybe _y_95, kk_context_t* _ctx) { /* forall<a,e> (action : (size_t) -> e maybe<a>, end : size_t, i : size_t, maybe<a>) -> e maybe<a> */ 
  if (kk_std_core_types__is_Nothing(_y_95)) {
    size_t i0_22764 = kk_std_core_incr_1(i, _ctx); /*size_t*/;
    return kk_std_core__lift22736_for_whilez(action, end, i0_22764, _ctx);
  }
  kk_box_t x = _y_95._cons.Just.value;
  kk_function_drop(action, _ctx);
  return kk_std_core_types__new_Just(x, _ctx);
}
 
// lift


// lift anonymous function
struct kk_std_core__lift22736_for_whilez_fun26138__t {
  struct kk_function_s _base;
  kk_function_t action0;
  size_t end0;
  size_t i0;
};
static kk_box_t kk_std_core__lift22736_for_whilez_fun26138(kk_function_t _fself, kk_box_t _b_24211, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift22736_for_whilez_fun26138(kk_function_t action0, size_t end0, size_t i0, kk_context_t* _ctx) {
  struct kk_std_core__lift22736_for_whilez_fun26138__t* _self = kk_function_alloc_as(struct kk_std_core__lift22736_for_whilez_fun26138__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift22736_for_whilez_fun26138, kk_context());
  _self->action0 = action0;
  _self->end0 = end0;
  _self->i0 = i0;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift22736_for_whilez_fun26138(kk_function_t _fself, kk_box_t _b_24211, kk_context_t* _ctx) {
  struct kk_std_core__lift22736_for_whilez_fun26138__t* _self = kk_function_as(struct kk_std_core__lift22736_for_whilez_fun26138__t*, _fself);
  kk_function_t action0 = _self->action0; /* (size_t) -> 13504 maybe<13503> */
  size_t end0 = _self->end0; /* size_t */
  size_t i0 = _self->i0; /* size_t */
  kk_drop_match(_self, {kk_function_dup(action0);;;}, {}, _ctx)
  kk_std_core_types__maybe _x26139;
  kk_std_core_types__maybe _x26140 = kk_std_core_types__maybe_unbox(_b_24211, _ctx); /*maybe<13503>*/
  _x26139 = kk_std_core__mlift22875_op(action0, end0, i0, _x26140, _ctx); /*maybe<13503>*/
  return kk_std_core_types__maybe_box(_x26139, _ctx);
}

kk_std_core_types__maybe kk_std_core__lift22736_for_whilez(kk_function_t action0, size_t end0, size_t i0, kk_context_t* _ctx) { /* forall<a,e> (action : (size_t) -> e maybe<a>, end : size_t, i : size_t) -> e maybe<a> */ 
  kk__tailcall: ;
  bool _match_25275 = (i0 <= end0); /*bool*/;
  if (_match_25275) {
    kk_std_core_types__maybe x_23195;
    kk_function_t _x26136 = kk_function_dup(action0); /*(size_t) -> 13504 maybe<13503>*/
    x_23195 = kk_function_call(kk_std_core_types__maybe, (kk_function_t, size_t, kk_context_t*), _x26136, (_x26136, i0, _ctx)); /*maybe<13503>*/
    if (kk_yielding(kk_context())) {
      kk_std_core_types__maybe_drop(x_23195, _ctx);
      kk_box_t _x26137 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift22736_for_whilez_fun26138(action0, end0, i0, _ctx), _ctx); /*3991*/
      return kk_std_core_types__maybe_unbox(_x26137, _ctx);
    }
    if (kk_std_core_types__is_Nothing(x_23195)) {
      size_t i0_227640 = kk_std_core_incr_1(i0, _ctx); /*size_t*/;
      { // tailcall
        i0 = i0_227640;
        goto kk__tailcall;
      }
    }
    kk_box_t x0 = x_23195._cons.Just.value;
    kk_function_drop(action0, _ctx);
    return kk_std_core_types__new_Just(x0, _ctx);
  }
  kk_function_drop(action0, _ctx);
  return kk_std_core_types__new_Nothing(_ctx);
}
 
// monadic lift

kk_std_core_types__maybe kk_std_core__mlift22876_foreach_while(kk_function_t action, kk_std_core__list xx, kk_std_core_types__maybe _y_100, kk_context_t* _ctx) { /* forall<a,b,e> (action : (a) -> e maybe<b>, xx : list<a>, maybe<b>) -> e maybe<b> */ 
  if (kk_std_core_types__is_Nothing(_y_100)) {
    return kk_std_core_foreach_while(xx, action, _ctx);
  }
  kk_function_drop(action, _ctx);
  kk_std_core__list_drop(xx, _ctx);
  return _y_100;
}
 
// Invoke `action` for each element of a list while `action` return `Nothing`


// lift anonymous function
struct kk_std_core_foreach_while_fun26147__t {
  struct kk_function_s _base;
  kk_function_t action0;
  kk_std_core__list xx0;
};
static kk_box_t kk_std_core_foreach_while_fun26147(kk_function_t _fself, kk_box_t _b_24215, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_while_fun26147(kk_function_t action0, kk_std_core__list xx0, kk_context_t* _ctx) {
  struct kk_std_core_foreach_while_fun26147__t* _self = kk_function_alloc_as(struct kk_std_core_foreach_while_fun26147__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_foreach_while_fun26147, kk_context());
  _self->action0 = action0;
  _self->xx0 = xx0;
  return &_self->_base;
}

static kk_box_t kk_std_core_foreach_while_fun26147(kk_function_t _fself, kk_box_t _b_24215, kk_context_t* _ctx) {
  struct kk_std_core_foreach_while_fun26147__t* _self = kk_function_as(struct kk_std_core_foreach_while_fun26147__t*, _fself);
  kk_function_t action0 = _self->action0; /* (13716) -> 13718 maybe<13717> */
  kk_std_core__list xx0 = _self->xx0; /* list<13716> */
  kk_drop_match(_self, {kk_function_dup(action0);kk_std_core__list_dup(xx0);}, {}, _ctx)
  kk_std_core_types__maybe _x26148;
  kk_std_core_types__maybe _x26149 = kk_std_core_types__maybe_unbox(_b_24215, _ctx); /*maybe<13717>*/
  _x26148 = kk_std_core__mlift22876_foreach_while(action0, xx0, _x26149, _ctx); /*maybe<13717>*/
  return kk_std_core_types__maybe_box(_x26148, _ctx);
}

kk_std_core_types__maybe kk_std_core_foreach_while(kk_std_core__list xs, kk_function_t action0, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, action : (a) -> e maybe<b>) -> e maybe<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Nil(xs)) {
    kk_function_drop(action0, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
  struct kk_std_core_Cons* _con26144 = kk_std_core__as_Cons(xs);
  kk_box_t x = _con26144->head;
  kk_std_core__list xx0 = _con26144->tail;
  if (kk_std_core__list_is_unique(xs)) {
    kk_std_core__list_free(xs);
  }
  else {
    kk_box_dup(x);
    kk_std_core__list_dup(xx0);
    kk_std_core__list_decref(xs, _ctx);
  }
  kk_std_core_types__maybe x0_23199;
  kk_function_t _x26145 = kk_function_dup(action0); /*(13716) -> 13718 maybe<13717>*/
  x0_23199 = kk_function_call(kk_std_core_types__maybe, (kk_function_t, kk_box_t, kk_context_t*), _x26145, (_x26145, x, _ctx)); /*maybe<13717>*/
  if (kk_yielding(kk_context())) {
    kk_std_core_types__maybe_drop(x0_23199, _ctx);
    kk_box_t _x26146 = kk_std_core_hnd_yield_extend(kk_std_core_new_foreach_while_fun26147(action0, xx0, _ctx), _ctx); /*3991*/
    return kk_std_core_types__maybe_unbox(_x26146, _ctx);
  }
  if (kk_std_core_types__is_Nothing(x0_23199)) {
    { // tailcall
      xs = xx0;
      goto kk__tailcall;
    }
  }
  kk_function_drop(action0, _ctx);
  kk_std_core__list_drop(xx0, _ctx);
  return x0_23199;
}
 
// monadic lift

kk_std_core_types__maybe kk_std_core__mlift22877_foreach_while_1(kk_function_t action, kk_std_core__sslice rest, kk_std_core_types__maybe _y_104, kk_context_t* _ctx) { /* forall<a,e> (action : (c : char) -> e maybe<a>, rest : sslice, maybe<a>) -> e maybe<a> */ 
  if (kk_std_core_types__is_Nothing(_y_104)) {
    return kk_std_core_foreach_while_1(rest, action, _ctx);
  }
  kk_function_drop(action, _ctx);
  kk_std_core__sslice_drop(rest, _ctx);
  return _y_104;
}
 
// Apply a function for each character in a string slice.
// If `action` returns `Just`, the function returns immediately with that result.


// lift anonymous function
struct kk_std_core_foreach_while_fun26155__t_1 {
  struct kk_function_s _base;
  kk_function_t action0;
  kk_std_core__sslice rest0;
};
static kk_box_t kk_std_core_foreach_while_fun26155_1(kk_function_t _fself, kk_box_t _b_24222, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_while_fun26155_1(kk_function_t action0, kk_std_core__sslice rest0, kk_context_t* _ctx) {
  struct kk_std_core_foreach_while_fun26155__t_1* _self = kk_function_alloc_as(struct kk_std_core_foreach_while_fun26155__t_1, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_foreach_while_fun26155_1, kk_context());
  _self->action0 = action0;
  _self->rest0 = rest0;
  return &_self->_base;
}

static kk_box_t kk_std_core_foreach_while_fun26155_1(kk_function_t _fself, kk_box_t _b_24222, kk_context_t* _ctx) {
  struct kk_std_core_foreach_while_fun26155__t_1* _self = kk_function_as(struct kk_std_core_foreach_while_fun26155__t_1*, _fself);
  kk_function_t action0 = _self->action0; /* (c : char) -> 13805 maybe<13804> */
  kk_std_core__sslice rest0 = _self->rest0; /* sslice */
  kk_drop_match(_self, {kk_function_dup(action0);kk_std_core__sslice_dup(rest0);}, {}, _ctx)
  kk_std_core_types__maybe _x26156;
  kk_std_core_types__maybe _x26157 = kk_std_core_types__maybe_unbox(_b_24222, _ctx); /*maybe<13804>*/
  _x26156 = kk_std_core__mlift22877_foreach_while_1(action0, rest0, _x26157, _ctx); /*maybe<13804>*/
  return kk_std_core_types__maybe_box(_x26156, _ctx);
}

kk_std_core_types__maybe kk_std_core_foreach_while_1(kk_std_core__sslice slice0, kk_function_t action0, kk_context_t* _ctx) { /* forall<a,e> (slice : sslice, action : (c : char) -> e maybe<a>) -> e maybe<a> */ 
  kk__tailcall: ;
  kk_std_core_types__maybe _match_25272 = kk_std_core_next(slice0, _ctx); /*maybe<(char, sslice)>*/;
  if (kk_std_core_types__is_Nothing(_match_25272)) {
    kk_function_drop(action0, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
  kk_box_t _box_x24218 = _match_25272._cons.Just.value;
  kk_std_core_types__tuple2_ _pat1 = kk_std_core_types__tuple2__unbox(_box_x24218, NULL);
  kk_box_t _box_x24219 = _pat1.fst;
  kk_box_t _box_x24220 = _pat1.snd;
  kk_char_t c = kk_char_unbox(_box_x24219, NULL);
  kk_std_core__sslice rest0 = kk_std_core__sslice_unbox(_box_x24220, NULL);
  kk_std_core__sslice_dup(rest0);
  kk_std_core_types__maybe_drop(_match_25272, _ctx);
  kk_std_core_types__maybe x_23203;
  kk_function_t _x26153 = kk_function_dup(action0); /*(c : char) -> 13805 maybe<13804>*/
  x_23203 = kk_function_call(kk_std_core_types__maybe, (kk_function_t, kk_char_t, kk_context_t*), _x26153, (_x26153, c, _ctx)); /*maybe<13804>*/
  if (kk_yielding(kk_context())) {
    kk_std_core_types__maybe_drop(x_23203, _ctx);
    kk_box_t _x26154 = kk_std_core_hnd_yield_extend(kk_std_core_new_foreach_while_fun26155_1(action0, rest0, _ctx), _ctx); /*3991*/
    return kk_std_core_types__maybe_unbox(_x26154, _ctx);
  }
  if (kk_std_core_types__is_Nothing(x_23203)) {
    { // tailcall
      slice0 = rest0;
      goto kk__tailcall;
    }
  }
  kk_function_drop(action0, _ctx);
  kk_std_core__sslice_drop(rest0, _ctx);
  return x_23203;
}
 
// Invoke a function `f` for each element in a vector `v`.
// If `f` returns `Just`, the iteration is stopped early and the result is returned.


// lift anonymous function
struct kk_std_core_foreach_while_fun26161__t_3 {
  struct kk_function_s _base;
  kk_function_t f;
  kk_vector_t v;
};
static kk_std_core_types__maybe kk_std_core_foreach_while_fun26161_3(kk_function_t _fself, size_t i, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_while_fun26161_3(kk_function_t f, kk_vector_t v, kk_context_t* _ctx) {
  struct kk_std_core_foreach_while_fun26161__t_3* _self = kk_function_alloc_as(struct kk_std_core_foreach_while_fun26161__t_3, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_foreach_while_fun26161_3, kk_context());
  _self->f = f;
  _self->v = v;
  return &_self->_base;
}

static kk_std_core_types__maybe kk_std_core_foreach_while_fun26161_3(kk_function_t _fself, size_t i, kk_context_t* _ctx) {
  struct kk_std_core_foreach_while_fun26161__t_3* _self = kk_function_as(struct kk_std_core_foreach_while_fun26161__t_3*, _fself);
  kk_function_t f = _self->f; /* (13907) -> 13909 maybe<13908> */
  kk_vector_t v = _self->v; /* vector<13907> */
  kk_drop_match(_self, {kk_function_dup(f);kk_vector_dup(v);}, {}, _ctx)
  kk_box_t _x26162 = kk_vector_at(v,i); /*216*/
  return kk_function_call(kk_std_core_types__maybe, (kk_function_t, kk_box_t, kk_context_t*), f, (f, _x26162, _ctx));
}

kk_std_core_types__maybe kk_std_core_foreach_while_3(kk_vector_t v, kk_function_t f, kk_context_t* _ctx) { /* forall<a,b,e> (v : vector<a>, f : (a) -> e maybe<b>) -> e maybe<b> */ 
  size_t start_23207 = ((size_t)0); /*size_t*/;
  size_t end_23208;
  size_t _x26159;
  kk_vector_t _x26160 = kk_vector_dup(v); /*vector<13907>*/
  _x26159 = kk_vector_len(_x26160); /*size_t*/
  end_23208 = kk_std_core_decr_1(_x26159, _ctx); /*size_t*/
  return kk_std_core__lift22736_for_whilez(kk_std_core_new_foreach_while_fun26161_3(f, v, _ctx), end_23208, start_23207, _ctx);
}
 
// monadic lift

kk_unit_t kk_std_core__mlift22878_foreach(kk_function_t action, kk_std_core__list xx, kk_unit_t wild__, kk_context_t* _ctx) { /* forall<a,e> (action : (a) -> e (), xx : list<a>, wild_ : ()) -> e () */ 
  kk_std_core_foreach(xx, action, _ctx); return kk_Unit;
}
 
// Invoke `action` for each element of a list


// lift anonymous function
struct kk_std_core_foreach_fun26166__t {
  struct kk_function_s _base;
  kk_function_t action0;
  kk_std_core__list xx0;
};
static kk_box_t kk_std_core_foreach_fun26166(kk_function_t _fself, kk_box_t _b_24226, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_fun26166(kk_function_t action0, kk_std_core__list xx0, kk_context_t* _ctx) {
  struct kk_std_core_foreach_fun26166__t* _self = kk_function_alloc_as(struct kk_std_core_foreach_fun26166__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_foreach_fun26166, kk_context());
  _self->action0 = action0;
  _self->xx0 = xx0;
  return &_self->_base;
}

static kk_box_t kk_std_core_foreach_fun26166(kk_function_t _fself, kk_box_t _b_24226, kk_context_t* _ctx) {
  struct kk_std_core_foreach_fun26166__t* _self = kk_function_as(struct kk_std_core_foreach_fun26166__t*, _fself);
  kk_function_t action0 = _self->action0; /* (14102) -> 14103 () */
  kk_std_core__list xx0 = _self->xx0; /* list<14102> */
  kk_drop_match(_self, {kk_function_dup(action0);kk_std_core__list_dup(xx0);}, {}, _ctx)
  kk_unit_t _x26167 = kk_Unit;
  kk_unit_t _x26168 = kk_Unit;
  kk_unit_unbox(_b_24226);
  kk_std_core__mlift22878_foreach(action0, xx0, _x26168, _ctx);
  return kk_unit_box(_x26167);
}

kk_unit_t kk_std_core_foreach(kk_std_core__list xs, kk_function_t action0, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, action : (a) -> e ()) -> e () */ 
  kk__tailcall: ;
  if (kk_std_core__is_Nil(xs)) {
    kk_function_drop(action0, _ctx);
    kk_Unit; return kk_Unit;
  }
  struct kk_std_core_Cons* _con26163 = kk_std_core__as_Cons(xs);
  kk_box_t x = _con26163->head;
  kk_std_core__list xx0 = _con26163->tail;
  if (kk_std_core__list_is_unique(xs)) {
    kk_std_core__list_free(xs);
  }
  else {
    kk_box_dup(x);
    kk_std_core__list_dup(xx0);
    kk_std_core__list_decref(xs, _ctx);
  }
  kk_unit_t x0_23210 = kk_Unit;
  kk_function_t _x26164 = kk_function_dup(action0); /*(14102) -> 14103 ()*/
  kk_function_call(kk_unit_t, (kk_function_t, kk_box_t, kk_context_t*), _x26164, (_x26164, x, _ctx));
  if (kk_yielding(kk_context())) {
    kk_box_t _x26165 = kk_std_core_hnd_yield_extend(kk_std_core_new_foreach_fun26166(action0, xx0, _ctx), _ctx); /*3991*/
    kk_unit_unbox(_x26165); return kk_Unit;
  }
  { // tailcall
    xs = xx0;
    goto kk__tailcall;
  }
}
 
// monadic lift

kk_std_core_types__maybe kk_std_core__mlift22879_foreach_1(kk_unit_t wild__, kk_context_t* _ctx) { /* forall<_a,e> (wild_ : ()) -> e maybe<_a> */ 
  return kk_std_core_types__new_Nothing(_ctx);
}
 
// monadic lift

kk_unit_t kk_std_core__mlift22880_foreach_1(kk_std_core_types__maybe wild__0, kk_context_t* _ctx) { /* forall<_a,e> (wild_0 : maybe<_a>) -> e () */ 
  kk_std_core_types__maybe_drop(wild__0, _ctx);
  kk_Unit; return kk_Unit;
}
 
// Apply a function for each character in a string slice.


// lift anonymous function
struct kk_std_core_foreach_fun26169__t_1 {
  struct kk_function_s _base;
  kk_function_t action;
};
static kk_std_core_types__maybe kk_std_core_foreach_fun26169_1(kk_function_t _fself, kk_char_t c, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_fun26169_1(kk_function_t action, kk_context_t* _ctx) {
  struct kk_std_core_foreach_fun26169__t_1* _self = kk_function_alloc_as(struct kk_std_core_foreach_fun26169__t_1, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_foreach_fun26169_1, kk_context());
  _self->action = action;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_foreach_fun26171__t_1 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_foreach_fun26171_1(kk_function_t _fself, kk_box_t _b_24230, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_fun26171_1(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_foreach_fun26171_1, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_foreach_fun26171_1(kk_function_t _fself, kk_box_t _b_24230, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  kk_std_core_types__maybe _x26172;
  kk_unit_t _x26173 = kk_Unit;
  kk_unit_unbox(_b_24230);
  _x26172 = kk_std_core__mlift22879_foreach_1(_x26173, _ctx); /*maybe<_14148>*/
  return kk_std_core_types__maybe_box(_x26172, _ctx);
}
static kk_std_core_types__maybe kk_std_core_foreach_fun26169_1(kk_function_t _fself, kk_char_t c, kk_context_t* _ctx) {
  struct kk_std_core_foreach_fun26169__t_1* _self = kk_function_as(struct kk_std_core_foreach_fun26169__t_1*, _fself);
  kk_function_t action = _self->action; /* (c : char) -> 14161 () */
  kk_drop_match(_self, {kk_function_dup(action);}, {}, _ctx)
  kk_unit_t x0_23217 = kk_Unit;
  kk_function_call(kk_unit_t, (kk_function_t, kk_char_t, kk_context_t*), action, (action, c, _ctx));
  if (kk_yielding(kk_context())) {
    kk_box_t _x26170 = kk_std_core_hnd_yield_extend(kk_std_core_new_foreach_fun26171_1(_ctx), _ctx); /*3991*/
    return kk_std_core_types__maybe_unbox(_x26170, _ctx);
  }
  return kk_std_core__mlift22879_foreach_1(x0_23217, _ctx);
}


// lift anonymous function
struct kk_std_core_foreach_fun26175__t_1 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_foreach_fun26175_1(kk_function_t _fself, kk_box_t _b_24234, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_fun26175_1(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_foreach_fun26175_1, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_foreach_fun26175_1(kk_function_t _fself, kk_box_t _b_24234, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  kk_unit_t _x26176 = kk_Unit;
  kk_std_core_types__maybe _x26177 = kk_std_core_types__maybe_unbox(_b_24234, _ctx); /*maybe<_14148>*/
  kk_std_core__mlift22880_foreach_1(_x26177, _ctx);
  return kk_unit_box(_x26176);
}

kk_unit_t kk_std_core_foreach_1(kk_std_core__sslice slice0, kk_function_t action, kk_context_t* _ctx) { /* forall<e> (slice : sslice, action : (c : char) -> e ()) -> e () */ 
  kk_std_core_types__maybe x_23214 = kk_std_core_foreach_while_1(slice0, kk_std_core_new_foreach_fun26169_1(action, _ctx), _ctx); /*maybe<_14148>*/;
  kk_std_core_types__maybe_drop(x_23214, _ctx);
  if (kk_yielding(kk_context())) {
    kk_box_t _x26174 = kk_std_core_hnd_yield_extend(kk_std_core_new_foreach_fun26175_1(_ctx), _ctx); /*3991*/
    kk_unit_unbox(_x26174); return kk_Unit;
  }
  kk_Unit; return kk_Unit;
}
 
// Invoke a function for each character in a string


// lift anonymous function
struct kk_std_core_foreach_fun26178__t_2 {
  struct kk_function_s _base;
  kk_function_t action;
};
static kk_std_core_types__maybe kk_std_core_foreach_fun26178_2(kk_function_t _fself, kk_char_t c, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_fun26178_2(kk_function_t action, kk_context_t* _ctx) {
  struct kk_std_core_foreach_fun26178__t_2* _self = kk_function_alloc_as(struct kk_std_core_foreach_fun26178__t_2, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_foreach_fun26178_2, kk_context());
  _self->action = action;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_foreach_fun26180__t_2 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_foreach_fun26180_2(kk_function_t _fself, kk_box_t _b_24238, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_fun26180_2(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_foreach_fun26180_2, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_foreach_fun26180_2(kk_function_t _fself, kk_box_t _b_24238, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  kk_std_core_types__maybe _x26181;
  kk_unit_t _x26182 = kk_Unit;
  kk_unit_unbox(_b_24238);
  _x26181 = kk_std_core__mlift22879_foreach_1(_x26182, _ctx); /*maybe<_14148>*/
  return kk_std_core_types__maybe_box(_x26181, _ctx);
}
static kk_std_core_types__maybe kk_std_core_foreach_fun26178_2(kk_function_t _fself, kk_char_t c, kk_context_t* _ctx) {
  struct kk_std_core_foreach_fun26178__t_2* _self = kk_function_as(struct kk_std_core_foreach_fun26178__t_2*, _fself);
  kk_function_t action = _self->action; /* (c : char) -> 14200 () */
  kk_drop_match(_self, {kk_function_dup(action);}, {}, _ctx)
  kk_unit_t x0_23225 = kk_Unit;
  kk_function_call(kk_unit_t, (kk_function_t, kk_char_t, kk_context_t*), action, (action, c, _ctx));
  if (kk_yielding(kk_context())) {
    kk_box_t _x26179 = kk_std_core_hnd_yield_extend(kk_std_core_new_foreach_fun26180_2(_ctx), _ctx); /*3991*/
    return kk_std_core_types__maybe_unbox(_x26179, _ctx);
  }
  return kk_std_core__mlift22879_foreach_1(x0_23225, _ctx);
}


// lift anonymous function
struct kk_std_core_foreach_fun26184__t_2 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_foreach_fun26184_2(kk_function_t _fself, kk_box_t _b_24242, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_fun26184_2(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_foreach_fun26184_2, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_foreach_fun26184_2(kk_function_t _fself, kk_box_t _b_24242, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  kk_unit_t _x26185 = kk_Unit;
  kk_std_core_types__maybe _x26186 = kk_std_core_types__maybe_unbox(_b_24242, _ctx); /*maybe<_14148>*/
  kk_std_core__mlift22880_foreach_1(_x26186, _ctx);
  return kk_unit_box(_x26185);
}

kk_unit_t kk_std_core_foreach_2(kk_string_t s, kk_function_t action, kk_context_t* _ctx) { /* forall<e> (s : string, action : (c : char) -> e ()) -> e () */ 
  kk_std_core__sslice slice_23220 = kk_std_core_slice(s, _ctx); /*sslice*/;
  kk_std_core_types__maybe x_23222 = kk_std_core_foreach_while_1(slice_23220, kk_std_core_new_foreach_fun26178_2(action, _ctx), _ctx); /*maybe<_14148>*/;
  kk_std_core_types__maybe_drop(x_23222, _ctx);
  if (kk_yielding(kk_context())) {
    kk_box_t _x26183 = kk_std_core_hnd_yield_extend(kk_std_core_new_foreach_fun26184_2(_ctx), _ctx); /*3991*/
    kk_unit_unbox(_x26183); return kk_Unit;
  }
  kk_Unit; return kk_Unit;
}
 
// Invoke a function `f` for each element in a vector `v`


// lift anonymous function
struct kk_std_core_foreach_fun26189__t_3 {
  struct kk_function_s _base;
  kk_function_t f;
  kk_vector_t v;
};
static kk_unit_t kk_std_core_foreach_fun26189_3(kk_function_t _fself, size_t i, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_fun26189_3(kk_function_t f, kk_vector_t v, kk_context_t* _ctx) {
  struct kk_std_core_foreach_fun26189__t_3* _self = kk_function_alloc_as(struct kk_std_core_foreach_fun26189__t_3, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_foreach_fun26189_3, kk_context());
  _self->f = f;
  _self->v = v;
  return &_self->_base;
}

static kk_unit_t kk_std_core_foreach_fun26189_3(kk_function_t _fself, size_t i, kk_context_t* _ctx) {
  struct kk_std_core_foreach_fun26189__t_3* _self = kk_function_as(struct kk_std_core_foreach_fun26189__t_3*, _fself);
  kk_function_t f = _self->f; /* (14215) -> 14216 () */
  kk_vector_t v = _self->v; /* vector<14215> */
  kk_drop_match(_self, {kk_function_dup(f);kk_vector_dup(v);}, {}, _ctx)
  kk_box_t x_23233 = kk_vector_at(v,i); /*14215*/;
  return kk_function_call(kk_unit_t, (kk_function_t, kk_box_t, kk_context_t*), f, (f, x_23233, _ctx));
}

kk_unit_t kk_std_core_foreach_3(kk_vector_t v, kk_function_t f, kk_context_t* _ctx) { /* forall<a,e> (v : vector<a>, f : (a) -> e ()) -> e () */ 
  size_t start_23230 = ((size_t)0); /*size_t*/;
  size_t end_23231;
  size_t _x26187;
  kk_vector_t _x26188 = kk_vector_dup(v); /*vector<14215>*/
  _x26187 = kk_vector_len(_x26188); /*size_t*/
  end_23231 = kk_std_core_decr_1(_x26187, _ctx); /*size_t*/
  kk_std_core__lift22732_forz(kk_std_core_new_foreach_fun26189_3(f, v, _ctx), end_23231, start_23230, _ctx); return kk_Unit;
}
 
// O(n). Return the number of characters in a string.

kk_integer_t kk_std_core_count_1(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> int */ 
  return kk_string_count_int(s,kk_context());
}
 
// O(n). Return the number of characters in a string slice

kk_integer_t kk_std_core_count_2(kk_std_core__sslice slice0, kk_context_t* _ctx) { /* (slice : sslice) -> int */ 
  return kk_slice_count(slice0,kk_context());
}
 
// Count the number of times a predicate is true for each character in a string


// lift anonymous function
struct kk_std_core_count_fun26191__t_3 {
  struct kk_function_s _base;
  kk_ref_t loc;
  kk_function_t pred;
};
static kk_std_core_types__maybe kk_std_core_count_fun26191_3(kk_function_t _fself, kk_char_t c, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_count_fun26191_3(kk_ref_t loc, kk_function_t pred, kk_context_t* _ctx) {
  struct kk_std_core_count_fun26191__t_3* _self = kk_function_alloc_as(struct kk_std_core_count_fun26191__t_3, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_count_fun26191_3, kk_context());
  _self->loc = loc;
  _self->pred = pred;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_count_fun26196__t_3 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_count_fun26196_3(kk_function_t _fself, kk_box_t _b_24254, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_count_fun26196_3(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_count_fun26196_3, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_count_fun26196_3(kk_function_t _fself, kk_box_t _b_24254, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  kk_std_core_types__maybe _x26197;
  kk_unit_t _x26198 = kk_Unit;
  kk_unit_unbox(_b_24254);
  _x26197 = kk_std_core__mlift22879_foreach_1(_x26198, _ctx); /*maybe<_14148>*/
  return kk_std_core_types__maybe_box(_x26197, _ctx);
}
static kk_std_core_types__maybe kk_std_core_count_fun26191_3(kk_function_t _fself, kk_char_t c, kk_context_t* _ctx) {
  struct kk_std_core_count_fun26191__t_3* _self = kk_function_as(struct kk_std_core_count_fun26191__t_3*, _fself);
  kk_ref_t loc = _self->loc; /* local-var<global,int> */
  kk_function_t pred = _self->pred; /* (char) -> bool */
  kk_drop_match(_self, {kk_ref_dup(loc);kk_function_dup(pred);}, {}, _ctx)
  kk_unit_t x0_23246 = kk_Unit;
  bool _match_25266 = kk_function_call(bool, (kk_function_t, kk_char_t, kk_context_t*), pred, (pred, c, _ctx)); /*bool*/;
  if (_match_25266) {
    kk_integer_t _b_24251_24249;
    kk_integer_t _x26192;
    kk_box_t _x26193;
    kk_ref_t _x26194 = kk_ref_dup(loc); /*local-var<global,int>*/
    _x26193 = (kk_ref_get(_x26194,kk_context())); /*228*/
    _x26192 = kk_integer_unbox(_x26193); /*int*/
    _b_24251_24249 = kk_integer_add(_x26192,(kk_integer_from_small(1)),kk_context()); /*int*/
    (kk_ref_set(loc,(kk_integer_box(_b_24251_24249)),kk_context()));
  }
  else {
    kk_ref_drop(loc, _ctx);
    kk_Unit;
  }
  if (kk_yielding(kk_context())) {
    kk_box_t _x26195 = kk_std_core_hnd_yield_extend(kk_std_core_new_count_fun26196_3(_ctx), _ctx); /*3991*/
    return kk_std_core_types__maybe_unbox(_x26195, _ctx);
  }
  return kk_std_core__mlift22879_foreach_1(x0_23246, _ctx);
}


// lift anonymous function
struct kk_std_core_count_fun26200__t_3 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_count_fun26200_3(kk_function_t _fself, kk_box_t _b_24258, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_count_fun26200_3(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_count_fun26200_3, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_count_fun26200_3(kk_function_t _fself, kk_box_t _b_24258, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  kk_unit_t _x26201 = kk_Unit;
  kk_std_core_types__maybe _x26202 = kk_std_core_types__maybe_unbox(_b_24258, _ctx); /*maybe<_14148>*/
  kk_std_core__mlift22880_foreach_1(_x26202, _ctx);
  return kk_unit_box(_x26201);
}

kk_integer_t kk_std_core_count_3(kk_string_t s, kk_function_t pred, kk_context_t* _ctx) { /* (s : string, pred : (char) -> bool) -> int */ 
  kk_ref_t loc = kk_ref_alloc((kk_integer_box(kk_integer_from_small(0))),kk_context()); /*local-var<global,int>*/;
  kk_integer_t res;
  kk_unit_t __ = kk_Unit;
  kk_std_core__sslice slice_23241 = kk_std_core_slice(s, _ctx); /*sslice*/;
  kk_std_core_types__maybe x_23243;
  kk_function_t _x26190;
  kk_ref_dup(loc);
  _x26190 = kk_std_core_new_count_fun26191_3(loc, pred, _ctx); /*(c : char) -> (local<global>) maybe<_14148>*/
  x_23243 = kk_std_core_foreach_while_1(slice_23241, _x26190, _ctx); /*maybe<_14148>*/
  kk_std_core_types__maybe_drop(x_23243, _ctx);
  if (kk_yielding(kk_context())) {
    kk_box_t _x26199 = kk_std_core_hnd_yield_extend(kk_std_core_new_count_fun26200_3(_ctx), _ctx); /*3991*/
    kk_unit_unbox(_x26199);
  }
  else {
    kk_Unit;
  }
  kk_box_t _x26203;
  kk_ref_t _x26204 = kk_ref_dup(loc); /*local-var<global,int>*/
  _x26203 = (kk_ref_get(_x26204,kk_context())); /*228*/
  res = kk_integer_unbox(_x26203); /*int*/
  kk_box_t _x26205 = kk_std_core_hnd_prompt_local_var(loc, kk_integer_box(res), _ctx); /*9788*/
  return kk_integer_unbox(_x26205);
}
 
// Is the integer negative (stricly smaller than zero)

bool kk_std_core_is_neg_2(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> bool */ 
  kk_std_core_types__order x_23251 = kk_int_as_order(kk_integer_signum(i,kk_context()),kk_context()); /*order*/;
  kk_integer_t _x26207 = kk_std_core_int_5(x_23251, _ctx); /*int*/
  kk_integer_t _x26208;
  kk_std_core_types__order _x26209 = kk_std_core_types__new_Lt(_ctx); /*order*/
  if (kk_std_core_types__is_Lt(_x26209)) {
    _x26208 = kk_integer_sub((kk_integer_from_small(0)),(kk_integer_from_small(1)),kk_context()); /*int*/
    goto _match26210;
  }
  if (kk_std_core_types__is_Eq(_x26209)) {
    _x26208 = kk_integer_from_small(0); /*int*/
    goto _match26210;
  }
  _x26208 = kk_integer_from_small(1); /*int*/
  _match26210: ;
  return kk_integer_eq(_x26207,_x26208,kk_context());
}

kk_std_core_types__tuple2_ kk_std_core_divmod_exp10(kk_integer_t i, kk_integer_t n, kk_context_t* _ctx) { /* (i : int, n : int) -> (int, int) */ 
  kk_std_core_types__tuple2_ _match_25261;
  kk_integer_t _x26211 = kk_integer_dup(n); /*int*/
  _match_25261 = kk_std_core_cdivmod_exp10(i, _x26211, _ctx); /*(int, int)*/
  kk_box_t _box_x24272 = _match_25261.fst;
  kk_box_t _box_x24273 = _match_25261.snd;
  kk_integer_t cq = kk_integer_unbox(_box_x24272);
  kk_integer_t cr = kk_integer_unbox(_box_x24273);
  bool _match_25262;
  bool _x26214;
  kk_integer_t _x26215 = kk_integer_dup(cr); /*int*/
  _x26214 = kk_std_core_is_neg_2(_x26215, _ctx); /*bool*/
  _match_25262 = !(_x26214); /*bool*/
  if (_match_25262) {
    kk_integer_drop(n, _ctx);
    return kk_std_core_types__new_dash__lp__comma__rp_(kk_integer_box(cq), kk_integer_box(cr), _ctx);
  }
  kk_integer_t _b_24280_24276 = kk_std_core_dec(cq, _ctx); /*int*/;
  kk_integer_t _b_24281_24277;
  kk_integer_t _x26216 = kk_std_core_exp10(n, _ctx); /*int*/
  _b_24281_24277 = kk_integer_add(cr,_x26216,kk_context()); /*int*/
  return kk_std_core_types__new_dash__lp__comma__rp_(kk_integer_box(_b_24280_24276), kk_integer_box(_b_24281_24277), _ctx);
}
 
// Drop the first `n` elements of a list (or fewer if the list is shorter than `n`)

kk_std_core__list kk_std_core_drop(kk_std_core__list xs, kk_integer_t n, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, n : int) -> list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con26217 = kk_std_core__as_Cons(xs);
    kk_box_t _pat0 = _con26217->head;
    kk_std_core__list xx = _con26217->tail;
    kk_integer_t _x26218 = kk_integer_dup(n); /*int*/
    if (kk_integer_gt(_x26218,(kk_integer_from_small(0)),kk_context())) {
      if (kk_std_core__list_is_unique(xs)) {
        kk_box_drop(_pat0, _ctx);
        kk_std_core__list_free(xs);
      }
      else {
        kk_std_core__list_dup(xx);
        kk_std_core__list_decref(xs, _ctx);
      }
      { // tailcall
        kk_integer_t _x26219 = kk_integer_sub(n,(kk_integer_from_small(1)),kk_context()); /*int*/
        xs = xx;
        n = _x26219;
        goto kk__tailcall;
      }
    }
  }
  kk_integer_drop(n, _ctx);
  return xs;
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift22881_drop_while(kk_function_t predicate, kk_std_core__list xs, kk_std_core__list xx, bool _y_120, kk_context_t* _ctx) { /* forall<a,e> (predicate : (a) -> e bool, xs : list<a>, xx : list<a>, bool) -> e list<a> */ 
  if (_y_120) {
    kk_std_core__list_drop(xs, _ctx);
    return kk_std_core_drop_while(xx, predicate, _ctx);
  }
  kk_function_drop(predicate, _ctx);
  kk_std_core__list_drop(xx, _ctx);
  return xs;
}
 
// Drop all initial elements that satisfy `predicate`


// lift anonymous function
struct kk_std_core_drop_while_fun26223__t {
  struct kk_function_s _base;
  kk_function_t predicate0;
  kk_std_core__list xs0;
  kk_std_core__list xx0;
};
static kk_box_t kk_std_core_drop_while_fun26223(kk_function_t _fself, kk_box_t _b_24283, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_drop_while_fun26223(kk_function_t predicate0, kk_std_core__list xs0, kk_std_core__list xx0, kk_context_t* _ctx) {
  struct kk_std_core_drop_while_fun26223__t* _self = kk_function_alloc_as(struct kk_std_core_drop_while_fun26223__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_drop_while_fun26223, kk_context());
  _self->predicate0 = predicate0;
  _self->xs0 = xs0;
  _self->xx0 = xx0;
  return &_self->_base;
}

static kk_box_t kk_std_core_drop_while_fun26223(kk_function_t _fself, kk_box_t _b_24283, kk_context_t* _ctx) {
  struct kk_std_core_drop_while_fun26223__t* _self = kk_function_as(struct kk_std_core_drop_while_fun26223__t*, _fself);
  kk_function_t predicate0 = _self->predicate0; /* (14833) -> 14834 bool */
  kk_std_core__list xs0 = _self->xs0; /* list<14833> */
  kk_std_core__list xx0 = _self->xx0; /* list<14833> */
  kk_drop_match(_self, {kk_function_dup(predicate0);kk_std_core__list_dup(xs0);kk_std_core__list_dup(xx0);}, {}, _ctx)
  kk_std_core__list _x26224;
  bool _x26225 = kk_bool_unbox(_b_24283); /*bool*/
  _x26224 = kk_std_core__mlift22881_drop_while(predicate0, xs0, xx0, _x26225, _ctx); /*list<14833>*/
  return kk_std_core__list_box(_x26224, _ctx);
}

kk_std_core__list kk_std_core_drop_while(kk_std_core__list xs0, kk_function_t predicate0, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, predicate : (a) -> e bool) -> e list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs0)) {
    struct kk_std_core_Cons* _con26220 = kk_std_core__as_Cons(xs0);
    kk_box_t x = _con26220->head;
    kk_std_core__list xx0 = _con26220->tail;
    kk_box_dup(x);
    kk_std_core__list_dup(xx0);
    bool x0_23253;
    kk_function_t _x26221 = kk_function_dup(predicate0); /*(14833) -> 14834 bool*/
    x0_23253 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), _x26221, (_x26221, x, _ctx)); /*bool*/
    if (kk_yielding(kk_context())) {
      kk_box_t _x26222 = kk_std_core_hnd_yield_extend(kk_std_core_new_drop_while_fun26223(predicate0, xs0, xx0, _ctx), _ctx); /*3991*/
      return kk_std_core__list_unbox(_x26222, _ctx);
    }
    if (x0_23253) {
      kk_std_core__list_dropn(xs0, ((int32_t)2), _ctx);
      { // tailcall
        xs0 = xx0;
        goto kk__tailcall;
      }
    }
    kk_function_drop(predicate0, _ctx);
    kk_std_core__list_drop(xx0, _ctx);
    return xs0;
  }
  kk_function_drop(predicate0, _ctx);
  return kk_std_core__new_Nil(_ctx);
}
 
// An empty slice

kk_std_core__sslice kk_std_core_empty;
 
// Does string `s`  end with `post`?
// If so, returns a slice of `s` from the start up to the `post` string at the end.

kk_std_core_types__maybe kk_std_core_ends_with(kk_string_t s, kk_string_t post, kk_context_t* _ctx) { /* (s : string, post : string) -> maybe<sslice> */ 
  bool _match_25259;
  kk_string_t _x26230 = kk_string_dup(s); /*string*/
  kk_string_t _x26231 = kk_string_dup(post); /*string*/
  _match_25259 = kk_std_core_xends_with(_x26230, _x26231, _ctx); /*bool*/
  if (_match_25259) {
    kk_std_core__sslice _b_24287_24286;
    kk_string_t _x26232 = kk_string_dup(s); /*string*/
    size_t _x26233 = ((size_t)0); /*size_t*/
    size_t _x26234;
    size_t _x26235 = kk_string_len(s,kk_context()); /*size_t*/
    size_t _x26236 = kk_string_len(post,kk_context()); /*size_t*/
    _x26234 = (_x26235 - _x26236); /*size_t*/
    _b_24287_24286 = kk_std_core__new_Sslice(_x26232, _x26233, _x26234, _ctx); /*sslice*/
    return kk_std_core_types__new_Just(kk_std_core__sslice_box(_b_24287_24286, _ctx), _ctx);
  }
  kk_string_drop(post, _ctx);
  kk_string_drop(s, _ctx);
  return kk_std_core_types__new_Nothing(_ctx);
}
 
// monadic lift

kk_box_t kk_std_core__mlift22882_error_pattern(kk_string_t definition, kk_string_t location, kk_string_t _c_124, kk_context_t* _ctx) { /* forall<a> (definition : string, location : string, string) -> a */ 
  kk_string_t message_23257;
  kk_string_t _x26237;
  kk_string_t _x26238 = kk_string_dup(location); /*string*/
  _x26237 = kk_std_core__lp__plus__6_rp_(_x26238, _c_124, _ctx); /*string*/
  kk_string_t _x26239;
  kk_define_string_literal(, _s26240, 23, ": pattern match failure")
  _x26239 = kk_string_dup(_s26240); /*string*/
  message_23257 = kk_std_core__lp__plus__6_rp_(_x26237, _x26239, _ctx); /*string*/
  kk_std_core_types__optional info_23258;
  kk_std_core__exception_info _b_24289_24288 = kk_std_core__new_ExnPattern(kk_reuse_null, location, definition, _ctx); /*exception-info*/;
  info_23258 = kk_std_core_types__new_Optional(kk_std_core__exception_info_box(_b_24289_24288, _ctx), _ctx); /*optional<exception-info>*/
  kk_std_core__exception exn_23259;
  kk_std_core__exception_info _x26241;
  if (kk_std_core_types__is_Optional(info_23258)) {
    kk_box_t _box_x24290 = info_23258._cons.Optional.value;
    kk_std_core__exception_info _info_12803 = kk_std_core__exception_info_unbox(_box_x24290, NULL);
    _x26241 = _info_12803; /*exception-info*/
    goto _match26242;
  }
  _x26241 = kk_std_core__new_ExnError(_ctx); /*exception-info*/
  _match26242: ;
  exn_23259 = kk_std_core__new_Exception(message_23257, _x26241, _ctx); /*exception*/
  kk_std_core_hnd__ev ev_23260;
  size_t _x26244 = ((size_t)0); /*size_t*/
  ev_23260 = kk_evv_at(_x26244,kk_context()); /*std/core/hnd/ev<.hnd-exn>*/
  struct kk_std_core_hnd_Ev* _con26245 = kk_std_core_hnd__as_Ev(ev_23260);
  kk_std_core_hnd__marker m = _con26245->marker;
  kk_box_t _box_x24291 = _con26245->hnd;
  kk_std_core__hnd_exn h = kk_std_core__hnd_exn_unbox(_box_x24291, NULL);
  kk_std_core__hnd_exn_dup(h);
  kk_std_core_hnd__clause1 _match_25258;
  struct kk_std_core__Hnd_exn* _con26247 = kk_std_core__as_Hnd_exn(h);
  kk_std_core_hnd__clause1 except_throw_exn = _con26247->except_throw_exn;
  if (kk_std_core__hnd_exn_is_unique(h)) {
    kk_std_core__hnd_exn_free(h);
  }
  else {
    kk_std_core_hnd__clause1_dup(except_throw_exn);
    kk_std_core__hnd_exn_decref(h, _ctx);
  }
  _match_25258 = except_throw_exn; /*forall<a> std/core/hnd/clause1<exception,a,.hnd-exn,3528,3529>*/
  kk_function_t _fun_unbox_x24295 = _match_25258.clause;
  return kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_box_t, kk_context_t*), _fun_unbox_x24295, (_fun_unbox_x24295, m, ev_23260, kk_std_core__exception_box(exn_23259, _ctx), _ctx));
}
 
// Raise a pattern match exception. This is function is used internally by the
// compiler to generate error messages on pattern match failures.


// lift anonymous function
struct kk_std_core_error_pattern_fun26253__t {
  struct kk_function_s _base;
  kk_string_t definition;
  kk_string_t location;
};
static kk_box_t kk_std_core_error_pattern_fun26253(kk_function_t _fself, kk_box_t _b_24303, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_error_pattern_fun26253(kk_string_t definition, kk_string_t location, kk_context_t* _ctx) {
  struct kk_std_core_error_pattern_fun26253__t* _self = kk_function_alloc_as(struct kk_std_core_error_pattern_fun26253__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_error_pattern_fun26253, kk_context());
  _self->definition = definition;
  _self->location = location;
  return &_self->_base;
}

static kk_box_t kk_std_core_error_pattern_fun26253(kk_function_t _fself, kk_box_t _b_24303, kk_context_t* _ctx) {
  struct kk_std_core_error_pattern_fun26253__t* _self = kk_function_as(struct kk_std_core_error_pattern_fun26253__t*, _fself);
  kk_string_t definition = _self->definition; /* string */
  kk_string_t location = _self->location; /* string */
  kk_drop_match(_self, {kk_string_dup(definition);kk_string_dup(location);}, {}, _ctx)
  kk_string_t _x26254 = kk_string_unbox(_b_24303); /*string*/
  return kk_std_core__mlift22882_error_pattern(definition, location, _x26254, _ctx);
}

kk_box_t kk_std_core_error_pattern(kk_string_t location, kk_string_t definition, kk_context_t* _ctx) { /* forall<a> (location : string, definition : string) -> exn a */ 
  kk_string_t x_23263;
  bool _match_25257;
  kk_string_t _x26248 = kk_string_dup(definition); /*string*/
  _match_25257 = kk_std_core_is_empty_2(_x26248, _ctx); /*bool*/
  if (_match_25257) {
    x_23263 = kk_string_empty(); /*string*/
  }
  else {
    kk_string_t _x26250;
    kk_define_string_literal(, _s26251, 2, ": ")
    _x26250 = kk_string_dup(_s26251); /*string*/
    kk_string_t _x26252 = kk_string_dup(definition); /*string*/
    x_23263 = kk_std_core__lp__plus__6_rp_(_x26250, _x26252, _ctx); /*string*/
  }
  if (kk_yielding(kk_context())) {
    kk_string_drop(x_23263, _ctx);
    return kk_std_core_hnd_yield_extend(kk_std_core_new_error_pattern_fun26253(definition, location, _ctx), _ctx);
  }
  kk_string_t message_23267;
  kk_string_t _x26255;
  kk_string_t _x26256 = kk_string_dup(location); /*string*/
  _x26255 = kk_std_core__lp__plus__6_rp_(_x26256, x_23263, _ctx); /*string*/
  kk_string_t _x26257;
  kk_define_string_literal(, _s26258, 23, ": pattern match failure")
  _x26257 = kk_string_dup(_s26258); /*string*/
  message_23267 = kk_std_core__lp__plus__6_rp_(_x26255, _x26257, _ctx); /*string*/
  kk_std_core_types__optional info_23268;
  kk_std_core__exception_info _b_24305_24304 = kk_std_core__new_ExnPattern(kk_reuse_null, location, definition, _ctx); /*exception-info*/;
  info_23268 = kk_std_core_types__new_Optional(kk_std_core__exception_info_box(_b_24305_24304, _ctx), _ctx); /*optional<exception-info>*/
  kk_std_core__exception exn_23269;
  kk_std_core__exception_info _x26259;
  if (kk_std_core_types__is_Optional(info_23268)) {
    kk_box_t _box_x24306 = info_23268._cons.Optional.value;
    kk_std_core__exception_info _info_12803 = kk_std_core__exception_info_unbox(_box_x24306, NULL);
    _x26259 = _info_12803; /*exception-info*/
    goto _match26260;
  }
  _x26259 = kk_std_core__new_ExnError(_ctx); /*exception-info*/
  _match26260: ;
  exn_23269 = kk_std_core__new_Exception(message_23267, _x26259, _ctx); /*exception*/
  kk_std_core_hnd__ev ev_23270;
  size_t _x26262 = ((size_t)0); /*size_t*/
  ev_23270 = kk_evv_at(_x26262,kk_context()); /*std/core/hnd/ev<.hnd-exn>*/
  struct kk_std_core_hnd_Ev* _con26263 = kk_std_core_hnd__as_Ev(ev_23270);
  kk_std_core_hnd__marker m = _con26263->marker;
  kk_box_t _box_x24307 = _con26263->hnd;
  kk_std_core__hnd_exn h = kk_std_core__hnd_exn_unbox(_box_x24307, NULL);
  kk_std_core__hnd_exn_dup(h);
  kk_std_core_hnd__clause1 _match_25256;
  struct kk_std_core__Hnd_exn* _con26265 = kk_std_core__as_Hnd_exn(h);
  kk_std_core_hnd__clause1 except_throw_exn = _con26265->except_throw_exn;
  if (kk_std_core__hnd_exn_is_unique(h)) {
    kk_std_core__hnd_exn_free(h);
  }
  else {
    kk_std_core_hnd__clause1_dup(except_throw_exn);
    kk_std_core__hnd_exn_decref(h, _ctx);
  }
  _match_25256 = except_throw_exn; /*forall<a> std/core/hnd/clause1<exception,a,.hnd-exn,3528,3529>*/
  kk_function_t _fun_unbox_x24311 = _match_25256.clause;
  return kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_box_t, kk_context_t*), _fun_unbox_x24311, (_fun_unbox_x24311, m, ev_23270, kk_std_core__exception_box(exn_23269, _ctx), _ctx));
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift22883_op(kk_std_core_types__ctail _acc, kk_function_t pred, kk_box_t x, kk_std_core__list xx, bool _y_127, kk_context_t* _ctx) { /* forall<a,e> (ctail<list<a>>, pred : (a) -> e bool, x : a, xx : list<a>, bool) -> e list<a> */ 
  if (_y_127) {
    kk_std_core__list _ctail_22812 = kk_std_core__list_hole(); /*list<15233>*/;
    kk_std_core__list _ctail_22813 = kk_std_core__new_Cons(kk_reuse_null, x, _ctail_22812, _ctx); /*list<15233>*/;
    kk_std_core_types__ctail _x26267;
    kk_box_t* field_23275 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_22813)->tail)); /*cfield<list<15233>>*/;
    if (kk_std_core_types__is_CTail(_acc)) {
      kk_box_t _box_x24326 = _acc._cons.CTail._field1;
      kk_box_t* last0 = _acc._cons.CTail._field2;
      kk_std_core__list head0 = kk_std_core__list_unbox(_box_x24326, NULL);
      kk_unit_t __ = kk_Unit;
      *(last0) = kk_std_core__list_box(_ctail_22813, _ctx);
      _x26267 = kk_std_core_types__new_CTail(kk_std_core__list_box(head0, _ctx), field_23275, _ctx); /*ctail<58>*/
      goto _match26268;
    }
    _x26267 = kk_std_core_types__new_CTail(kk_std_core__list_box(_ctail_22813, _ctx), field_23275, _ctx); /*ctail<58>*/
    _match26268: ;
    return kk_std_core__ctail_filter(xx, pred, _x26267, _ctx);
  }
  kk_box_drop(x, _ctx);
  return kk_std_core__ctail_filter(xx, pred, _acc, _ctx);
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift22884_op_fun26270__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t x0;
};
static kk_std_core__list kk_std_core__mlift22884_op_fun26270(kk_function_t _fself, kk_std_core__list _ctail_22815, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift22884_op_fun26270(kk_function_t _accm, kk_box_t x0, kk_context_t* _ctx) {
  struct kk_std_core__mlift22884_op_fun26270__t* _self = kk_function_alloc_as(struct kk_std_core__mlift22884_op_fun26270__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift22884_op_fun26270, kk_context());
  _self->_accm = _accm;
  _self->x0 = x0;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift22884_op_fun26270(kk_function_t _fself, kk_std_core__list _ctail_22815, kk_context_t* _ctx) {
  struct kk_std_core__mlift22884_op_fun26270__t* _self = kk_function_as(struct kk_std_core__mlift22884_op_fun26270__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<15233>) -> list<15233> */
  kk_box_t x0 = _self->x0; /* 15233 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(x0);}, {}, _ctx)
  kk_std_core__list _x26271 = kk_std_core__new_Cons(kk_reuse_null, x0, _ctail_22815, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x26271, _ctx));
}

kk_std_core__list kk_std_core__mlift22884_op(kk_function_t _accm, kk_function_t pred0, kk_box_t x0, kk_std_core__list xx0, bool _y_132, kk_context_t* _ctx) { /* forall<a,e> ((list<a>) -> list<a>, pred : (a) -> e bool, x : a, xx : list<a>, bool) -> e list<a> */ 
  if (_y_132) {
    return kk_std_core__ctailm_filter(xx0, pred0, kk_std_core__new_mlift22884_op_fun26270(_accm, x0, _ctx), _ctx);
  }
  kk_box_drop(x0, _ctx);
  return kk_std_core__ctailm_filter(xx0, pred0, _accm, _ctx);
}
 
// Retain only those elements of a list that satisfy the given predicate `pred`.
// For example: `filter([1,2,3],odd?) == [1,3]`


// lift anonymous function
struct kk_std_core__ctail_filter_fun26276__t {
  struct kk_function_s _base;
  kk_function_t pred1;
  kk_box_t x1;
  kk_std_core__list xx1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_filter_fun26276(kk_function_t _fself, kk_box_t _b_24340, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_filter_fun26276(kk_function_t pred1, kk_box_t x1, kk_std_core__list xx1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_filter_fun26276__t* _self = kk_function_alloc_as(struct kk_std_core__ctail_filter_fun26276__t, 6, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_filter_fun26276, kk_context());
  _self->pred1 = pred1;
  _self->x1 = x1;
  _self->xx1 = xx1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_filter_fun26276(kk_function_t _fself, kk_box_t _b_24340, kk_context_t* _ctx) {
  struct kk_std_core__ctail_filter_fun26276__t* _self = kk_function_as(struct kk_std_core__ctail_filter_fun26276__t*, _fself);
  kk_function_t pred1 = _self->pred1; /* (15233) -> 15234 bool */
  kk_box_t x1 = _self->x1; /* 15233 */
  kk_std_core__list xx1 = _self->xx1; /* list<15233> */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<15233>> */
  kk_drop_match(_self, {kk_function_dup(pred1);kk_box_dup(x1);kk_std_core__list_dup(xx1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  kk_std_core__list _x26277;
  bool _x26278 = kk_bool_unbox(_b_24340); /*bool*/
  _x26277 = kk_std_core__mlift22883_op(_acc0, pred1, x1, xx1, _x26278, _ctx); /*list<15233>*/
  return kk_std_core__list_box(_x26277, _ctx);
}

kk_std_core__list kk_std_core__ctail_filter(kk_std_core__list xs, kk_function_t pred1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, pred : (a) -> e bool, ctail<list<a>>) -> e list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con26272 = kk_std_core__as_Cons(xs);
    kk_box_t x1 = _con26272->head;
    kk_std_core__list xx1 = _con26272->tail;
    kk_reuse_t _ru_25106 = kk_reuse_null; /*reuse*/;
    if (kk_std_core__list_is_unique(xs)) {
      _ru_25106 = (kk_std_core__list_reuse(xs));
    }
    else {
      kk_box_dup(x1);
      kk_std_core__list_dup(xx1);
      kk_std_core__list_decref(xs, _ctx);
      _ru_25106 = kk_reuse_null;
    }
    bool x0_23276;
    kk_function_t _x26274 = kk_function_dup(pred1); /*(15233) -> 15234 bool*/
    kk_box_t _x26273 = kk_box_dup(x1); /*15233*/
    x0_23276 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), _x26274, (_x26274, _x26273, _ctx)); /*bool*/
    if (kk_yielding(kk_context())) {
      kk_reuse_drop(_ru_25106, _ctx);
      kk_box_t _x26275 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_filter_fun26276(pred1, x1, xx1, _acc0, _ctx), _ctx); /*3991*/
      return kk_std_core__list_unbox(_x26275, _ctx);
    }
    if (x0_23276) {
      kk_std_core__list _ctail_228120 = kk_std_core__list_hole(); /*list<15233>*/;
      kk_std_core__list _ctail_228130;
      bool _x26279 = _ru_25106!=NULL; /*bool*/
      if (_x26279) {
        struct kk_std_core_Cons* _con26280 = (struct kk_std_core_Cons*)_ru_25106;
        _con26280->tail = _ctail_228120;
        _ctail_228130 = kk_std_core__base_Cons(_con26280); /*list<15233>*/
      }
      else {
        _ctail_228130 = kk_std_core__new_Cons(kk_reuse_null, x1, _ctail_228120, _ctx); /*list<15233>*/
      }
      { // tailcall
        kk_std_core_types__ctail _x26281;
        kk_box_t* field_23282 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_228130)->tail)); /*cfield<list<15233>>*/;
        if (kk_std_core_types__is_CTail(_acc0)) {
          kk_box_t _box_x24347 = _acc0._cons.CTail._field1;
          kk_box_t* last2 = _acc0._cons.CTail._field2;
          kk_std_core__list head1 = kk_std_core__list_unbox(_box_x24347, NULL);
          kk_unit_t __0 = kk_Unit;
          *(last2) = kk_std_core__list_box(_ctail_228130, _ctx);
          _x26281 = kk_std_core_types__new_CTail(kk_std_core__list_box(head1, _ctx), field_23282, _ctx); /*ctail<58>*/
          goto _match26282;
        }
        _x26281 = kk_std_core_types__new_CTail(kk_std_core__list_box(_ctail_228130, _ctx), field_23282, _ctx); /*ctail<58>*/
        _match26282: ;
        xs = xx1;
        _acc0 = _x26281;
        goto kk__tailcall;
      }
    }
    kk_reuse_drop(_ru_25106, _ctx);
    kk_box_drop(x1, _ctx);
    { // tailcall
      xs = xx1;
      goto kk__tailcall;
    }
  }
  kk_function_drop(pred1, _ctx);
  if (kk_std_core_types__is_CTail(_acc0)) {
    kk_box_t _box_x24356 = _acc0._cons.CTail._field1;
    kk_box_t* last00 = _acc0._cons.CTail._field2;
    kk_std_core__list head00 = kk_std_core__list_unbox(_box_x24356, NULL);
    kk_unit_t __00 = kk_Unit;
    *(last00) = kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx);
    return head00;
  }
  return kk_std_core__new_Nil(_ctx);
}
 
// Retain only those elements of a list that satisfy the given predicate `pred`.
// For example: `filter([1,2,3],odd?) == [1,3]`


// lift anonymous function
struct kk_std_core__ctailm_filter_fun26289__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t pred2;
  kk_box_t x2;
  kk_std_core__list xx2;
};
static kk_box_t kk_std_core__ctailm_filter_fun26289(kk_function_t _fself, kk_box_t _b_24368, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_filter_fun26289(kk_function_t _accm0, kk_function_t pred2, kk_box_t x2, kk_std_core__list xx2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_filter_fun26289__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_filter_fun26289__t, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_filter_fun26289, kk_context());
  _self->_accm0 = _accm0;
  _self->pred2 = pred2;
  _self->x2 = x2;
  _self->xx2 = xx2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_filter_fun26289(kk_function_t _fself, kk_box_t _b_24368, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_filter_fun26289__t* _self = kk_function_as(struct kk_std_core__ctailm_filter_fun26289__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<15233>) -> list<15233> */
  kk_function_t pred2 = _self->pred2; /* (15233) -> 15234 bool */
  kk_box_t x2 = _self->x2; /* 15233 */
  kk_std_core__list xx2 = _self->xx2; /* list<15233> */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(pred2);kk_box_dup(x2);kk_std_core__list_dup(xx2);}, {}, _ctx)
  kk_std_core__list _x26290;
  bool _x26291 = kk_bool_unbox(_b_24368); /*bool*/
  _x26290 = kk_std_core__mlift22884_op(_accm0, pred2, x2, xx2, _x26291, _ctx); /*list<15233>*/
  return kk_std_core__list_box(_x26290, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_filter_fun26293__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t x2;
};
static kk_std_core__list kk_std_core__ctailm_filter_fun26293(kk_function_t _fself, kk_std_core__list _ctail_228150, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_filter_fun26293(kk_function_t _accm0, kk_box_t x2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_filter_fun26293__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_filter_fun26293__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_filter_fun26293, kk_context());
  _self->_accm0 = _accm0;
  _self->x2 = x2;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_filter_fun26293(kk_function_t _fself, kk_std_core__list _ctail_228150, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_filter_fun26293__t* _self = kk_function_as(struct kk_std_core__ctailm_filter_fun26293__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<15233>) -> list<15233> */
  kk_box_t x2 = _self->x2; /* 15233 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(x2);}, {}, _ctx)
  kk_std_core__list _x26294 = kk_std_core__new_Cons(kk_reuse_null, x2, _ctail_228150, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x26294, _ctx));
}

kk_std_core__list kk_std_core__ctailm_filter(kk_std_core__list xs0, kk_function_t pred2, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, pred : (a) -> e bool, (list<a>) -> list<a>) -> e list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs0)) {
    struct kk_std_core_Cons* _con26285 = kk_std_core__as_Cons(xs0);
    kk_box_t x2 = _con26285->head;
    kk_std_core__list xx2 = _con26285->tail;
    if (kk_std_core__list_is_unique(xs0)) {
      kk_std_core__list_free(xs0);
    }
    else {
      kk_box_dup(x2);
      kk_std_core__list_dup(xx2);
      kk_std_core__list_decref(xs0, _ctx);
    }
    bool x0_23285;
    kk_function_t _x26287 = kk_function_dup(pred2); /*(15233) -> 15234 bool*/
    kk_box_t _x26286 = kk_box_dup(x2); /*15233*/
    x0_23285 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), _x26287, (_x26287, _x26286, _ctx)); /*bool*/
    if (kk_yielding(kk_context())) {
      kk_box_t _x26288 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_filter_fun26289(_accm0, pred2, x2, xx2, _ctx), _ctx); /*3991*/
      return kk_std_core__list_unbox(_x26288, _ctx);
    }
    if (x0_23285) {
      { // tailcall
        kk_function_t _x26292 = kk_std_core__new_ctailm_filter_fun26293(_accm0, x2, _ctx); /*(list<15233>) -> list<15233>*/
        xs0 = xx2;
        _accm0 = _x26292;
        goto kk__tailcall;
      }
    }
    kk_box_drop(x2, _ctx);
    { // tailcall
      xs0 = xx2;
      goto kk__tailcall;
    }
  }
  kk_function_drop(pred2, _ctx);
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, kk_std_core__new_Nil(_ctx), _ctx));
}
 
// Retain only those elements of a list that satisfy the given predicate `pred`.
// For example: `filter([1,2,3],odd?) == [1,3]`


// lift anonymous function
struct kk_std_core_filter_fun26295__t {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core_filter_fun26295(kk_function_t _fself, kk_std_core__list _ctail_22814, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_filter_fun26295(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_filter_fun26295, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core_filter_fun26295(kk_function_t _fself, kk_std_core__list _ctail_22814, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  return _ctail_22814;
}

kk_std_core__list kk_std_core_filter(kk_std_core__list xs1, kk_function_t pred3, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, pred : (a) -> e bool) -> e list<a> */ 
  bool _match_25252 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_25252) {
    return kk_std_core__ctail_filter(xs1, pred3, kk_std_core_types__new_CTailNil(_ctx), _ctx);
  }
  return kk_std_core__ctailm_filter(xs1, pred3, kk_std_core_new_filter_fun26295(_ctx), _ctx);
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift22885_op(kk_std_core_types__ctail _acc, kk_function_t pred, kk_std_core__list xx, kk_std_core_types__maybe _y_140, kk_context_t* _ctx) { /* forall<a,b,e> (ctail<list<b>>, pred : (a) -> e maybe<b>, xx : list<a>, maybe<b>) -> e list<b> */ 
  if (kk_std_core_types__is_Nothing(_y_140)) {
    return kk_std_core__ctail_filter_map(xx, pred, _acc, _ctx);
  }
  kk_box_t y = _y_140._cons.Just.value;
  kk_std_core__list _ctail_22816 = kk_std_core__list_hole(); /*list<15279>*/;
  kk_std_core__list _ctail_22817 = kk_std_core__new_Cons(kk_reuse_null, y, _ctail_22816, _ctx); /*list<15279>*/;
  kk_std_core_types__ctail _x26296;
  kk_box_t* field_23291 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_22817)->tail)); /*cfield<list<15279>>*/;
  if (kk_std_core_types__is_CTail(_acc)) {
    kk_box_t _box_x24377 = _acc._cons.CTail._field1;
    kk_box_t* last0 = _acc._cons.CTail._field2;
    kk_std_core__list head0 = kk_std_core__list_unbox(_box_x24377, NULL);
    kk_unit_t __ = kk_Unit;
    *(last0) = kk_std_core__list_box(_ctail_22817, _ctx);
    _x26296 = kk_std_core_types__new_CTail(kk_std_core__list_box(head0, _ctx), field_23291, _ctx); /*ctail<58>*/
    goto _match26297;
  }
  _x26296 = kk_std_core_types__new_CTail(kk_std_core__list_box(_ctail_22817, _ctx), field_23291, _ctx); /*ctail<58>*/
  _match26297: ;
  return kk_std_core__ctail_filter_map(xx, pred, _x26296, _ctx);
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift22886_op_fun26299__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t y0;
};
static kk_std_core__list kk_std_core__mlift22886_op_fun26299(kk_function_t _fself, kk_std_core__list _ctail_22819, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift22886_op_fun26299(kk_function_t _accm, kk_box_t y0, kk_context_t* _ctx) {
  struct kk_std_core__mlift22886_op_fun26299__t* _self = kk_function_alloc_as(struct kk_std_core__mlift22886_op_fun26299__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift22886_op_fun26299, kk_context());
  _self->_accm = _accm;
  _self->y0 = y0;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift22886_op_fun26299(kk_function_t _fself, kk_std_core__list _ctail_22819, kk_context_t* _ctx) {
  struct kk_std_core__mlift22886_op_fun26299__t* _self = kk_function_as(struct kk_std_core__mlift22886_op_fun26299__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<15279>) -> list<15279> */
  kk_box_t y0 = _self->y0; /* 15279 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(y0);}, {}, _ctx)
  kk_std_core__list _x26300 = kk_std_core__new_Cons(kk_reuse_null, y0, _ctail_22819, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x26300, _ctx));
}

kk_std_core__list kk_std_core__mlift22886_op(kk_function_t _accm, kk_function_t pred0, kk_std_core__list xx0, kk_std_core_types__maybe _y_145, kk_context_t* _ctx) { /* forall<a,b,e> ((list<b>) -> list<b>, pred : (a) -> e maybe<b>, xx : list<a>, maybe<b>) -> e list<b> */ 
  if (kk_std_core_types__is_Nothing(_y_145)) {
    return kk_std_core__ctailm_filter_map(xx0, pred0, _accm, _ctx);
  }
  kk_box_t y0 = _y_145._cons.Just.value;
  return kk_std_core__ctailm_filter_map(xx0, pred0, kk_std_core__new_mlift22886_op_fun26299(_accm, y0, _ctx), _ctx);
}
 
// Retain only those elements of a list that satisfy the given predicate `pred`.
// For example: `filterMap([1,2,3],fn(i) { if (i.odd?) then Nothing else Just(i*i) }) == [4]`


// lift anonymous function
struct kk_std_core__ctail_filter_map_fun26305__t {
  struct kk_function_s _base;
  kk_function_t pred1;
  kk_std_core__list xx1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_filter_map_fun26305(kk_function_t _fself, kk_box_t _b_24396, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_filter_map_fun26305(kk_function_t pred1, kk_std_core__list xx1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_filter_map_fun26305__t* _self = kk_function_alloc_as(struct kk_std_core__ctail_filter_map_fun26305__t, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_filter_map_fun26305, kk_context());
  _self->pred1 = pred1;
  _self->xx1 = xx1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_filter_map_fun26305(kk_function_t _fself, kk_box_t _b_24396, kk_context_t* _ctx) {
  struct kk_std_core__ctail_filter_map_fun26305__t* _self = kk_function_as(struct kk_std_core__ctail_filter_map_fun26305__t*, _fself);
  kk_function_t pred1 = _self->pred1; /* (15278) -> 15280 maybe<15279> */
  kk_std_core__list xx1 = _self->xx1; /* list<15278> */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<15279>> */
  kk_drop_match(_self, {kk_function_dup(pred1);kk_std_core__list_dup(xx1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  kk_std_core__list _x26306;
  kk_std_core_types__maybe _x26307 = kk_std_core_types__maybe_unbox(_b_24396, _ctx); /*maybe<15279>*/
  _x26306 = kk_std_core__mlift22885_op(_acc0, pred1, xx1, _x26307, _ctx); /*list<15279>*/
  return kk_std_core__list_box(_x26306, _ctx);
}

kk_std_core__list kk_std_core__ctail_filter_map(kk_std_core__list xs, kk_function_t pred1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, pred : (a) -> e maybe<b>, ctail<list<b>>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Nil(xs)) {
    kk_function_drop(pred1, _ctx);
    if (kk_std_core_types__is_CTail(_acc0)) {
      kk_box_t _box_x24390 = _acc0._cons.CTail._field1;
      kk_box_t* last2 = _acc0._cons.CTail._field2;
      kk_std_core__list head1 = kk_std_core__list_unbox(_box_x24390, NULL);
      kk_unit_t __0 = kk_Unit;
      *(last2) = kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx);
      return head1;
    }
    return kk_std_core__new_Nil(_ctx);
  }
  struct kk_std_core_Cons* _con26302 = kk_std_core__as_Cons(xs);
  kk_box_t x = _con26302->head;
  kk_std_core__list xx1 = _con26302->tail;
  kk_reuse_t _ru_25108 = kk_reuse_null; /*reuse*/;
  if (kk_std_core__list_is_unique(xs)) {
    _ru_25108 = (kk_std_core__list_reuse(xs));
  }
  else {
    kk_box_dup(x);
    kk_std_core__list_dup(xx1);
    kk_std_core__list_decref(xs, _ctx);
    _ru_25108 = kk_reuse_null;
  }
  kk_std_core_types__maybe x0_23294;
  kk_function_t _x26303 = kk_function_dup(pred1); /*(15278) -> 15280 maybe<15279>*/
  x0_23294 = kk_function_call(kk_std_core_types__maybe, (kk_function_t, kk_box_t, kk_context_t*), _x26303, (_x26303, x, _ctx)); /*maybe<15279>*/
  if (kk_yielding(kk_context())) {
    kk_reuse_drop(_ru_25108, _ctx);
    kk_std_core_types__maybe_drop(x0_23294, _ctx);
    kk_box_t _x26304 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_filter_map_fun26305(pred1, xx1, _acc0, _ctx), _ctx); /*3991*/
    return kk_std_core__list_unbox(_x26304, _ctx);
  }
  if (kk_std_core_types__is_Nothing(x0_23294)) {
    kk_reuse_drop(_ru_25108, _ctx);
    { // tailcall
      xs = xx1;
      goto kk__tailcall;
    }
  }
  kk_box_t y1 = x0_23294._cons.Just.value;
  kk_std_core__list _ctail_228160 = kk_std_core__list_hole(); /*list<15279>*/;
  kk_std_core__list _ctail_228170 = kk_std_core__new_Cons(_ru_25108, y1, _ctail_228160, _ctx); /*list<15279>*/;
  { // tailcall
    kk_std_core_types__ctail _x26308;
    kk_box_t* field_23300 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_228170)->tail)); /*cfield<list<15279>>*/;
    if (kk_std_core_types__is_CTail(_acc0)) {
      kk_box_t _box_x24403 = _acc0._cons.CTail._field1;
      kk_box_t* last00 = _acc0._cons.CTail._field2;
      kk_std_core__list head00 = kk_std_core__list_unbox(_box_x24403, NULL);
      kk_unit_t __00 = kk_Unit;
      *(last00) = kk_std_core__list_box(_ctail_228170, _ctx);
      _x26308 = kk_std_core_types__new_CTail(kk_std_core__list_box(head00, _ctx), field_23300, _ctx); /*ctail<58>*/
      goto _match26309;
    }
    _x26308 = kk_std_core_types__new_CTail(kk_std_core__list_box(_ctail_228170, _ctx), field_23300, _ctx); /*ctail<58>*/
    _match26309: ;
    xs = xx1;
    _acc0 = _x26308;
    goto kk__tailcall;
  }
}
 
// Retain only those elements of a list that satisfy the given predicate `pred`.
// For example: `filterMap([1,2,3],fn(i) { if (i.odd?) then Nothing else Just(i*i) }) == [4]`


// lift anonymous function
struct kk_std_core__ctailm_filter_map_fun26314__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t pred2;
  kk_std_core__list xx2;
};
static kk_box_t kk_std_core__ctailm_filter_map_fun26314(kk_function_t _fself, kk_box_t _b_24419, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_filter_map_fun26314(kk_function_t _accm0, kk_function_t pred2, kk_std_core__list xx2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_filter_map_fun26314__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_filter_map_fun26314__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_filter_map_fun26314, kk_context());
  _self->_accm0 = _accm0;
  _self->pred2 = pred2;
  _self->xx2 = xx2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_filter_map_fun26314(kk_function_t _fself, kk_box_t _b_24419, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_filter_map_fun26314__t* _self = kk_function_as(struct kk_std_core__ctailm_filter_map_fun26314__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<15279>) -> list<15279> */
  kk_function_t pred2 = _self->pred2; /* (15278) -> 15280 maybe<15279> */
  kk_std_core__list xx2 = _self->xx2; /* list<15278> */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(pred2);kk_std_core__list_dup(xx2);}, {}, _ctx)
  kk_std_core__list _x26315;
  kk_std_core_types__maybe _x26316 = kk_std_core_types__maybe_unbox(_b_24419, _ctx); /*maybe<15279>*/
  _x26315 = kk_std_core__mlift22886_op(_accm0, pred2, xx2, _x26316, _ctx); /*list<15279>*/
  return kk_std_core__list_box(_x26315, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_filter_map_fun26318__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t y2;
};
static kk_std_core__list kk_std_core__ctailm_filter_map_fun26318(kk_function_t _fself, kk_std_core__list _ctail_228190, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_filter_map_fun26318(kk_function_t _accm0, kk_box_t y2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_filter_map_fun26318__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_filter_map_fun26318__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_filter_map_fun26318, kk_context());
  _self->_accm0 = _accm0;
  _self->y2 = y2;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_filter_map_fun26318(kk_function_t _fself, kk_std_core__list _ctail_228190, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_filter_map_fun26318__t* _self = kk_function_as(struct kk_std_core__ctailm_filter_map_fun26318__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<15279>) -> list<15279> */
  kk_box_t y2 = _self->y2; /* 15279 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(y2);}, {}, _ctx)
  kk_std_core__list _x26319 = kk_std_core__new_Cons(kk_reuse_null, y2, _ctail_228190, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x26319, _ctx));
}

kk_std_core__list kk_std_core__ctailm_filter_map(kk_std_core__list xs0, kk_function_t pred2, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, pred : (a) -> e maybe<b>, (list<b>) -> list<b>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Nil(xs0)) {
    kk_function_drop(pred2, _ctx);
    return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, kk_std_core__new_Nil(_ctx), _ctx));
  }
  struct kk_std_core_Cons* _con26311 = kk_std_core__as_Cons(xs0);
  kk_box_t x0 = _con26311->head;
  kk_std_core__list xx2 = _con26311->tail;
  if (kk_std_core__list_is_unique(xs0)) {
    kk_std_core__list_free(xs0);
  }
  else {
    kk_box_dup(x0);
    kk_std_core__list_dup(xx2);
    kk_std_core__list_decref(xs0, _ctx);
  }
  kk_std_core_types__maybe x0_23301;
  kk_function_t _x26312 = kk_function_dup(pred2); /*(15278) -> 15280 maybe<15279>*/
  x0_23301 = kk_function_call(kk_std_core_types__maybe, (kk_function_t, kk_box_t, kk_context_t*), _x26312, (_x26312, x0, _ctx)); /*maybe<15279>*/
  if (kk_yielding(kk_context())) {
    kk_std_core_types__maybe_drop(x0_23301, _ctx);
    kk_box_t _x26313 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_filter_map_fun26314(_accm0, pred2, xx2, _ctx), _ctx); /*3991*/
    return kk_std_core__list_unbox(_x26313, _ctx);
  }
  if (kk_std_core_types__is_Nothing(x0_23301)) {
    { // tailcall
      xs0 = xx2;
      goto kk__tailcall;
    }
  }
  kk_box_t y2 = x0_23301._cons.Just.value;
  { // tailcall
    kk_function_t _x26317 = kk_std_core__new_ctailm_filter_map_fun26318(_accm0, y2, _ctx); /*(list<15279>) -> list<15279>*/
    xs0 = xx2;
    _accm0 = _x26317;
    goto kk__tailcall;
  }
}
 
// Retain only those elements of a list that satisfy the given predicate `pred`.
// For example: `filterMap([1,2,3],fn(i) { if (i.odd?) then Nothing else Just(i*i) }) == [4]`


// lift anonymous function
struct kk_std_core_filter_map_fun26320__t {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core_filter_map_fun26320(kk_function_t _fself, kk_std_core__list _ctail_22818, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_filter_map_fun26320(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_filter_map_fun26320, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core_filter_map_fun26320(kk_function_t _fself, kk_std_core__list _ctail_22818, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  return _ctail_22818;
}

kk_std_core__list kk_std_core_filter_map(kk_std_core__list xs1, kk_function_t pred3, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, pred : (a) -> e maybe<b>) -> e list<b> */ 
  bool _match_25249 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_25249) {
    return kk_std_core__ctail_filter_map(xs1, pred3, kk_std_core_types__new_CTailNil(_ctx), _ctx);
  }
  return kk_std_core__ctailm_filter_map(xs1, pred3, kk_std_core_new_filter_map_fun26320(_ctx), _ctx);
}

bool kk_std_core_is_zero_1(size_t i, kk_context_t* _ctx) { /* (i : size_t) -> bool */ 
  return (i == 0);
}
 
// Find the first element satisfying some predicate


// lift anonymous function
struct kk_std_core_find_fun26321__t {
  struct kk_function_s _base;
  kk_function_t pred;
};
static kk_std_core_types__maybe kk_std_core_find_fun26321(kk_function_t _fself, kk_box_t x, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_find_fun26321(kk_function_t pred, kk_context_t* _ctx) {
  struct kk_std_core_find_fun26321__t* _self = kk_function_alloc_as(struct kk_std_core_find_fun26321__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_find_fun26321, kk_context());
  _self->pred = pred;
  return &_self->_base;
}

static kk_std_core_types__maybe kk_std_core_find_fun26321(kk_function_t _fself, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_find_fun26321__t* _self = kk_function_as(struct kk_std_core_find_fun26321__t*, _fself);
  kk_function_t pred = _self->pred; /* (15395) -> bool */
  kk_drop_match(_self, {kk_function_dup(pred);}, {}, _ctx)
  bool _match_25248;
  kk_box_t _x26322 = kk_box_dup(x); /*15395*/
  _match_25248 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), pred, (pred, _x26322, _ctx)); /*bool*/
  if (_match_25248) {
    return kk_std_core_types__new_Just(x, _ctx);
  }
  kk_box_drop(x, _ctx);
  return kk_std_core_types__new_Nothing(_ctx);
}

kk_std_core_types__maybe kk_std_core_find(kk_std_core__list xs, kk_function_t pred, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, pred : (a) -> bool) -> maybe<a> */ 
  return kk_std_core_foreach_while(xs, kk_std_core_new_find_fun26321(pred, _ctx), _ctx);
}
 
// O(n). If it occurs, return the position of substring `sub` in `s`, tupled with
// the position just following the substring `sub`.

kk_std_core_types__maybe kk_std_core_find_1(kk_string_t s, kk_string_t sub, kk_context_t* _ctx) { /* (s : string, sub : string) -> maybe<sslice> */ 
  size_t i;
  kk_string_t _x26323 = kk_string_dup(s); /*string*/
  kk_string_t _x26324 = kk_string_dup(sub); /*string*/
  i = kk_string_index_of1(_x26323,_x26324,kk_context()); /*size_t*/
  bool _match_25247 = kk_std_core_is_zero_1(i, _ctx); /*bool*/;
  if (_match_25247) {
    kk_string_drop(s, _ctx);
    kk_string_drop(sub, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
  kk_std_core__sslice _b_24423_24422;
  size_t _x26325 = kk_std_core_decr_1(i, _ctx); /*size_t*/
  size_t _x26326 = kk_string_len(sub,kk_context()); /*size_t*/
  _b_24423_24422 = kk_std_core__new_Sslice(s, _x26325, _x26326, _ctx); /*sslice*/
  return kk_std_core_types__new_Just(kk_std_core__sslice_box(_b_24423_24422, _ctx), _ctx);
}
 
// Return the last index of substring `sub` in `s` if it occurs.

kk_std_core_types__maybe kk_std_core_find_last(kk_string_t s, kk_string_t sub, kk_context_t* _ctx) { /* (s : string, sub : string) -> maybe<sslice> */ 
  size_t i;
  kk_string_t _x26327 = kk_string_dup(s); /*string*/
  kk_string_t _x26328 = kk_string_dup(sub); /*string*/
  i = kk_string_last_index_of1(_x26327,_x26328,kk_context()); /*size_t*/
  bool _match_25246 = kk_std_core_is_zero_1(i, _ctx); /*bool*/;
  if (_match_25246) {
    kk_string_drop(s, _ctx);
    kk_string_drop(sub, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
  kk_std_core__sslice _b_24425_24424;
  size_t _x26329 = kk_std_core_decr_1(i, _ctx); /*size_t*/
  size_t _x26330 = kk_string_len(sub,kk_context()); /*size_t*/
  _b_24425_24424 = kk_std_core__new_Sslice(s, _x26329, _x26330, _ctx); /*sslice*/
  return kk_std_core_types__new_Just(kk_std_core__sslice_box(_b_24425_24424, _ctx), _ctx);
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift22887_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_std_core__list xx, kk_std_core_types__maybe _y_153, kk_context_t* _ctx) { /* forall<a,b,e> (ctail<list<b>>, f : (a) -> e maybe<b>, xx : list<a>, maybe<b>) -> e list<b> */ 
  if (kk_std_core_types__is_Just(_y_153)) {
    kk_box_t y = _y_153._cons.Just.value;
    kk_std_core__list _ctail_22820 = kk_std_core__list_hole(); /*list<15635>*/;
    kk_std_core__list _ctail_22821 = kk_std_core__new_Cons(kk_reuse_null, y, _ctail_22820, _ctx); /*list<15635>*/;
    kk_std_core_types__ctail _x26333;
    kk_box_t* field_23307 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_22821)->tail)); /*cfield<list<15635>>*/;
    if (kk_std_core_types__is_CTail(_acc)) {
      kk_box_t _box_x24432 = _acc._cons.CTail._field1;
      kk_box_t* last0 = _acc._cons.CTail._field2;
      kk_std_core__list head0 = kk_std_core__list_unbox(_box_x24432, NULL);
      kk_unit_t __ = kk_Unit;
      *(last0) = kk_std_core__list_box(_ctail_22821, _ctx);
      _x26333 = kk_std_core_types__new_CTail(kk_std_core__list_box(head0, _ctx), field_23307, _ctx); /*ctail<58>*/
      goto _match26334;
    }
    _x26333 = kk_std_core_types__new_CTail(kk_std_core__list_box(_ctail_22821, _ctx), field_23307, _ctx); /*ctail<58>*/
    _match26334: ;
    return kk_std_core__ctail_flatmap_maybe(xx, f, _x26333, _ctx);
  }
  return kk_std_core__ctail_flatmap_maybe(xx, f, _acc, _ctx);
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift22888_op_fun26336__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t y0;
};
static kk_std_core__list kk_std_core__mlift22888_op_fun26336(kk_function_t _fself, kk_std_core__list _ctail_22823, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift22888_op_fun26336(kk_function_t _accm, kk_box_t y0, kk_context_t* _ctx) {
  struct kk_std_core__mlift22888_op_fun26336__t* _self = kk_function_alloc_as(struct kk_std_core__mlift22888_op_fun26336__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift22888_op_fun26336, kk_context());
  _self->_accm = _accm;
  _self->y0 = y0;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift22888_op_fun26336(kk_function_t _fself, kk_std_core__list _ctail_22823, kk_context_t* _ctx) {
  struct kk_std_core__mlift22888_op_fun26336__t* _self = kk_function_as(struct kk_std_core__mlift22888_op_fun26336__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<15635>) -> list<15635> */
  kk_box_t y0 = _self->y0; /* 15635 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(y0);}, {}, _ctx)
  kk_std_core__list _x26337 = kk_std_core__new_Cons(kk_reuse_null, y0, _ctail_22823, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x26337, _ctx));
}

kk_std_core__list kk_std_core__mlift22888_op(kk_function_t _accm, kk_function_t f0, kk_std_core__list xx0, kk_std_core_types__maybe _y_158, kk_context_t* _ctx) { /* forall<a,b,e> ((list<b>) -> list<b>, f : (a) -> e maybe<b>, xx : list<a>, maybe<b>) -> e list<b> */ 
  if (kk_std_core_types__is_Just(_y_158)) {
    kk_box_t y0 = _y_158._cons.Just.value;
    return kk_std_core__ctailm_flatmap_maybe(xx0, f0, kk_std_core__new_mlift22888_op_fun26336(_accm, y0, _ctx), _ctx);
  }
  return kk_std_core__ctailm_flatmap_maybe(xx0, f0, _accm, _ctx);
}
 
// Concatenate the `Just` result elements from applying a function to all elements.


// lift anonymous function
struct kk_std_core__ctail_flatmap_maybe_fun26341__t {
  struct kk_function_s _base;
  kk_function_t f1;
  kk_std_core__list xx1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_flatmap_maybe_fun26341(kk_function_t _fself, kk_box_t _b_24446, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_flatmap_maybe_fun26341(kk_function_t f1, kk_std_core__list xx1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_flatmap_maybe_fun26341__t* _self = kk_function_alloc_as(struct kk_std_core__ctail_flatmap_maybe_fun26341__t, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_flatmap_maybe_fun26341, kk_context());
  _self->f1 = f1;
  _self->xx1 = xx1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_flatmap_maybe_fun26341(kk_function_t _fself, kk_box_t _b_24446, kk_context_t* _ctx) {
  struct kk_std_core__ctail_flatmap_maybe_fun26341__t* _self = kk_function_as(struct kk_std_core__ctail_flatmap_maybe_fun26341__t*, _fself);
  kk_function_t f1 = _self->f1; /* (15634) -> 15636 maybe<15635> */
  kk_std_core__list xx1 = _self->xx1; /* list<15634> */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<15635>> */
  kk_drop_match(_self, {kk_function_dup(f1);kk_std_core__list_dup(xx1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  kk_std_core__list _x26342;
  kk_std_core_types__maybe _x26343 = kk_std_core_types__maybe_unbox(_b_24446, _ctx); /*maybe<15635>*/
  _x26342 = kk_std_core__mlift22887_op(_acc0, f1, xx1, _x26343, _ctx); /*list<15635>*/
  return kk_std_core__list_box(_x26342, _ctx);
}

kk_std_core__list kk_std_core__ctail_flatmap_maybe(kk_std_core__list xs, kk_function_t f1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, f : (a) -> e maybe<b>, ctail<list<b>>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con26338 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con26338->head;
    kk_std_core__list xx1 = _con26338->tail;
    kk_reuse_t _ru_25110 = kk_reuse_null; /*reuse*/;
    if (kk_std_core__list_is_unique(xs)) {
      _ru_25110 = (kk_std_core__list_reuse(xs));
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx1);
      kk_std_core__list_decref(xs, _ctx);
      _ru_25110 = kk_reuse_null;
    }
    kk_std_core_types__maybe x0_23308;
    kk_function_t _x26339 = kk_function_dup(f1); /*(15634) -> 15636 maybe<15635>*/
    x0_23308 = kk_function_call(kk_std_core_types__maybe, (kk_function_t, kk_box_t, kk_context_t*), _x26339, (_x26339, x, _ctx)); /*maybe<15635>*/
    if (kk_yielding(kk_context())) {
      kk_reuse_drop(_ru_25110, _ctx);
      kk_std_core_types__maybe_drop(x0_23308, _ctx);
      kk_box_t _x26340 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_flatmap_maybe_fun26341(f1, xx1, _acc0, _ctx), _ctx); /*3991*/
      return kk_std_core__list_unbox(_x26340, _ctx);
    }
    if (kk_std_core_types__is_Just(x0_23308)) {
      kk_box_t y1 = x0_23308._cons.Just.value;
      kk_std_core__list _ctail_228200 = kk_std_core__list_hole(); /*list<15635>*/;
      kk_std_core__list _ctail_228210 = kk_std_core__new_Cons(_ru_25110, y1, _ctail_228200, _ctx); /*list<15635>*/;
      { // tailcall
        kk_std_core_types__ctail _x26344;
        kk_box_t* field_23314 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_228210)->tail)); /*cfield<list<15635>>*/;
        if (kk_std_core_types__is_CTail(_acc0)) {
          kk_box_t _box_x24453 = _acc0._cons.CTail._field1;
          kk_box_t* last2 = _acc0._cons.CTail._field2;
          kk_std_core__list head1 = kk_std_core__list_unbox(_box_x24453, NULL);
          kk_unit_t __0 = kk_Unit;
          *(last2) = kk_std_core__list_box(_ctail_228210, _ctx);
          _x26344 = kk_std_core_types__new_CTail(kk_std_core__list_box(head1, _ctx), field_23314, _ctx); /*ctail<58>*/
          goto _match26345;
        }
        _x26344 = kk_std_core_types__new_CTail(kk_std_core__list_box(_ctail_228210, _ctx), field_23314, _ctx); /*ctail<58>*/
        _match26345: ;
        xs = xx1;
        _acc0 = _x26344;
        goto kk__tailcall;
      }
    }
    kk_reuse_drop(_ru_25110, _ctx);
    { // tailcall
      xs = xx1;
      goto kk__tailcall;
    }
  }
  kk_function_drop(f1, _ctx);
  if (kk_std_core_types__is_CTail(_acc0)) {
    kk_box_t _box_x24462 = _acc0._cons.CTail._field1;
    kk_box_t* last00 = _acc0._cons.CTail._field2;
    kk_std_core__list head00 = kk_std_core__list_unbox(_box_x24462, NULL);
    kk_unit_t __00 = kk_Unit;
    *(last00) = kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx);
    return head00;
  }
  return kk_std_core__new_Nil(_ctx);
}
 
// Concatenate the `Just` result elements from applying a function to all elements.


// lift anonymous function
struct kk_std_core__ctailm_flatmap_maybe_fun26351__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t f2;
  kk_std_core__list xx2;
};
static kk_box_t kk_std_core__ctailm_flatmap_maybe_fun26351(kk_function_t _fself, kk_box_t _b_24474, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_flatmap_maybe_fun26351(kk_function_t _accm0, kk_function_t f2, kk_std_core__list xx2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_flatmap_maybe_fun26351__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_flatmap_maybe_fun26351__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_flatmap_maybe_fun26351, kk_context());
  _self->_accm0 = _accm0;
  _self->f2 = f2;
  _self->xx2 = xx2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_flatmap_maybe_fun26351(kk_function_t _fself, kk_box_t _b_24474, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_flatmap_maybe_fun26351__t* _self = kk_function_as(struct kk_std_core__ctailm_flatmap_maybe_fun26351__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<15635>) -> list<15635> */
  kk_function_t f2 = _self->f2; /* (15634) -> 15636 maybe<15635> */
  kk_std_core__list xx2 = _self->xx2; /* list<15634> */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(f2);kk_std_core__list_dup(xx2);}, {}, _ctx)
  kk_std_core__list _x26352;
  kk_std_core_types__maybe _x26353 = kk_std_core_types__maybe_unbox(_b_24474, _ctx); /*maybe<15635>*/
  _x26352 = kk_std_core__mlift22888_op(_accm0, f2, xx2, _x26353, _ctx); /*list<15635>*/
  return kk_std_core__list_box(_x26352, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_flatmap_maybe_fun26355__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t y2;
};
static kk_std_core__list kk_std_core__ctailm_flatmap_maybe_fun26355(kk_function_t _fself, kk_std_core__list _ctail_228230, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_flatmap_maybe_fun26355(kk_function_t _accm0, kk_box_t y2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_flatmap_maybe_fun26355__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_flatmap_maybe_fun26355__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_flatmap_maybe_fun26355, kk_context());
  _self->_accm0 = _accm0;
  _self->y2 = y2;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_flatmap_maybe_fun26355(kk_function_t _fself, kk_std_core__list _ctail_228230, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_flatmap_maybe_fun26355__t* _self = kk_function_as(struct kk_std_core__ctailm_flatmap_maybe_fun26355__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<15635>) -> list<15635> */
  kk_box_t y2 = _self->y2; /* 15635 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(y2);}, {}, _ctx)
  kk_std_core__list _x26356 = kk_std_core__new_Cons(kk_reuse_null, y2, _ctail_228230, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x26356, _ctx));
}

kk_std_core__list kk_std_core__ctailm_flatmap_maybe(kk_std_core__list xs0, kk_function_t f2, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, f : (a) -> e maybe<b>, (list<b>) -> list<b>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs0)) {
    struct kk_std_core_Cons* _con26348 = kk_std_core__as_Cons(xs0);
    kk_box_t x0 = _con26348->head;
    kk_std_core__list xx2 = _con26348->tail;
    if (kk_std_core__list_is_unique(xs0)) {
      kk_std_core__list_free(xs0);
    }
    else {
      kk_box_dup(x0);
      kk_std_core__list_dup(xx2);
      kk_std_core__list_decref(xs0, _ctx);
    }
    kk_std_core_types__maybe x0_23317;
    kk_function_t _x26349 = kk_function_dup(f2); /*(15634) -> 15636 maybe<15635>*/
    x0_23317 = kk_function_call(kk_std_core_types__maybe, (kk_function_t, kk_box_t, kk_context_t*), _x26349, (_x26349, x0, _ctx)); /*maybe<15635>*/
    if (kk_yielding(kk_context())) {
      kk_std_core_types__maybe_drop(x0_23317, _ctx);
      kk_box_t _x26350 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_flatmap_maybe_fun26351(_accm0, f2, xx2, _ctx), _ctx); /*3991*/
      return kk_std_core__list_unbox(_x26350, _ctx);
    }
    if (kk_std_core_types__is_Just(x0_23317)) {
      kk_box_t y2 = x0_23317._cons.Just.value;
      { // tailcall
        kk_function_t _x26354 = kk_std_core__new_ctailm_flatmap_maybe_fun26355(_accm0, y2, _ctx); /*(list<15635>) -> list<15635>*/
        xs0 = xx2;
        _accm0 = _x26354;
        goto kk__tailcall;
      }
    }
    { // tailcall
      xs0 = xx2;
      goto kk__tailcall;
    }
  }
  kk_function_drop(f2, _ctx);
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, kk_std_core__new_Nil(_ctx), _ctx));
}
 
// Concatenate the `Just` result elements from applying a function to all elements.


// lift anonymous function
struct kk_std_core_flatmap_maybe_fun26357__t {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core_flatmap_maybe_fun26357(kk_function_t _fself, kk_std_core__list _ctail_22822, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_flatmap_maybe_fun26357(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_flatmap_maybe_fun26357, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core_flatmap_maybe_fun26357(kk_function_t _fself, kk_std_core__list _ctail_22822, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  return _ctail_22822;
}

kk_std_core__list kk_std_core_flatmap_maybe(kk_std_core__list xs1, kk_function_t f3, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, f : (a) -> e maybe<b>) -> e list<b> */ 
  bool _match_25243 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_25243) {
    return kk_std_core__ctail_flatmap_maybe(xs1, f3, kk_std_core_types__new_CTailNil(_ctx), _ctx);
  }
  return kk_std_core__ctailm_flatmap_maybe(xs1, f3, kk_std_core_new_flatmap_maybe_fun26357(_ctx), _ctx);
}

kk_box_t kk_std_core_fold_int(kk_integer_t start0, kk_integer_t end, kk_box_t init0, kk_function_t f, kk_context_t* _ctx) { /* forall<a> (start : int, end : int, init : a, f : (int, a) -> a) -> a */ 
  kk__tailcall: ;
  bool _match_25242;
  kk_integer_t _x26358 = kk_integer_dup(start0); /*int*/
  kk_integer_t _x26359 = kk_integer_dup(end); /*int*/
  _match_25242 = kk_integer_gte(_x26358,_x26359,kk_context()); /*bool*/
  if (_match_25242) {
    kk_integer_drop(end, _ctx);
    kk_function_drop(f, _ctx);
    kk_integer_drop(start0, _ctx);
    return init0;
  }
  kk_box_t x;
  kk_function_t _x26361 = kk_function_dup(f); /*(int, 15723) -> 15723*/
  kk_integer_t _x26360 = kk_integer_dup(start0); /*int*/
  x = kk_function_call(kk_box_t, (kk_function_t, kk_integer_t, kk_box_t, kk_context_t*), _x26361, (_x26361, _x26360, init0, _ctx)); /*15723*/
  { // tailcall
    kk_integer_t _x26362 = kk_std_core_inc(start0, _ctx); /*int*/
    start0 = _x26362;
    init0 = x;
    goto kk__tailcall;
  }
}
 
// monadic lift

kk_box_t kk_std_core__mlift22889_foldl(kk_function_t f, kk_std_core__list xx, kk_box_t _y_166, kk_context_t* _ctx) { /* forall<a,e,b> (f : (a, b) -> e a, xx : list<b>, a) -> e a */ 
  return kk_std_core_foldl(xx, _y_166, f, _ctx);
}
 
// Fold a list from the left, i.e. `foldl([1,2],0,(+)) == (0+1)+2`
// Since `foldl` is tail recursive, it is preferred over `foldr` when using an associative function `f`


// lift anonymous function
struct kk_std_core_foldl_fun26365__t {
  struct kk_function_s _base;
  kk_function_t f0;
  kk_std_core__list xx0;
};
static kk_box_t kk_std_core_foldl_fun26365(kk_function_t _fself, kk_box_t _y_1660, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foldl_fun26365(kk_function_t f0, kk_std_core__list xx0, kk_context_t* _ctx) {
  struct kk_std_core_foldl_fun26365__t* _self = kk_function_alloc_as(struct kk_std_core_foldl_fun26365__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_foldl_fun26365, kk_context());
  _self->f0 = f0;
  _self->xx0 = xx0;
  return &_self->_base;
}

static kk_box_t kk_std_core_foldl_fun26365(kk_function_t _fself, kk_box_t _y_1660, kk_context_t* _ctx) {
  struct kk_std_core_foldl_fun26365__t* _self = kk_function_as(struct kk_std_core_foldl_fun26365__t*, _fself);
  kk_function_t f0 = _self->f0; /* (15726, 15731) -> 15728 15726 */
  kk_std_core__list xx0 = _self->xx0; /* list<15731> */
  kk_drop_match(_self, {kk_function_dup(f0);kk_std_core__list_dup(xx0);}, {}, _ctx)
  return kk_std_core__mlift22889_foldl(f0, xx0, _y_1660, _ctx);
}

kk_box_t kk_std_core_foldl(kk_std_core__list xs, kk_box_t z, kk_function_t f0, kk_context_t* _ctx) { /* forall<a,b,e> (list<a>, b, (b, a) -> e b) -> e b */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con26363 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con26363->head;
    kk_std_core__list xx0 = _con26363->tail;
    if (kk_std_core__list_is_unique(xs)) {
      kk_std_core__list_free(xs);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx0);
      kk_std_core__list_decref(xs, _ctx);
    }
    kk_box_t x0_23321;
    kk_function_t _x26364 = kk_function_dup(f0); /*(15726, 15731) -> 15728 15726*/
    x0_23321 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_context_t*), _x26364, (_x26364, z, x, _ctx)); /*15726*/
    if (kk_yielding(kk_context())) {
      kk_box_drop(x0_23321, _ctx);
      return kk_std_core_hnd_yield_extend(kk_std_core_new_foldl_fun26365(f0, xx0, _ctx), _ctx);
    }
    { // tailcall
      xs = xx0;
      z = x0_23321;
      goto kk__tailcall;
    }
  }
  kk_function_drop(f0, _ctx);
  return z;
}


// lift anonymous function
struct kk_std_core_foldl1_fun26378__t {
  struct kk_function_s _base;
  size_t i_23325;
};
static kk_box_t kk_std_core_foldl1_fun26378(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foldl1_fun26378(size_t i_23325, kk_context_t* _ctx) {
  struct kk_std_core_foldl1_fun26378__t* _self = kk_function_alloc_as(struct kk_std_core_foldl1_fun26378__t, 1, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_foldl1_fun26378, kk_context());
  _self->i_23325 = i_23325;
  return &_self->_base;
}

static kk_box_t kk_std_core_foldl1_fun26378(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_foldl1_fun26378__t* _self = kk_function_as(struct kk_std_core_foldl1_fun26378__t*, _fself);
  size_t i_23325 = _self->i_23325; /* std/core/hnd/ev-index */
  kk_drop_match(_self, {;}, {}, _ctx)
  return kk_std_core_hnd_open_at1(i_23325, cont, res, _ctx);
}

kk_box_t kk_std_core_foldl1(kk_std_core__list xs, kk_function_t f, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, f : (a, a) -> <exn|e> a) -> <exn|e> a */ 
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con26366 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con26366->head;
    kk_std_core__list xx = _con26366->tail;
    if (kk_std_core__list_is_unique(xs)) {
      kk_std_core__list_free(xs);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    return kk_std_core_foldl(xx, x, f, _ctx);
  }
  kk_function_drop(f, _ctx);
  size_t i_23325;
  kk_std_core_hnd__htag _x26367 = kk_std_core_hnd__htag_dup(kk_std_core__tag_exn); /*std/core/hnd/htag<.hnd-exn>*/
  i_23325 = kk_std_core_hnd__evv_index(_x26367, _ctx); /*std/core/hnd/ev-index*/
  kk_evv_t w = kk_evv_swap_create1(i_23325,kk_context()); /*std/core/hnd/evv<<exn|15803>>*/;
  kk_box_t y;
  kk_std_core__exception exn_23170;
  kk_string_t _x26368;
  kk_define_string_literal(, _s26369, 33, "unexpected Nil in std/core/foldl1")
  _x26368 = kk_string_dup(_s26369); /*string*/
  kk_std_core__exception_info _x26370;
  kk_std_core_types__optional _x26371 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
  if (kk_std_core_types__is_Optional(_x26371)) {
    kk_box_t _box_x24477 = _x26371._cons.Optional.value;
    kk_std_core__exception_info _info_12803 = kk_std_core__exception_info_unbox(_box_x24477, NULL);
    _x26370 = _info_12803; /*exception-info*/
    goto _match26372;
  }
  _x26370 = kk_std_core__new_ExnError(_ctx); /*exception-info*/
  _match26372: ;
  exn_23170 = kk_std_core__new_Exception(_x26368, _x26370, _ctx); /*exception*/
  kk_std_core_hnd__ev ev_23171;
  size_t _x26374 = ((size_t)0); /*size_t*/
  ev_23171 = kk_evv_at(_x26374,kk_context()); /*std/core/hnd/ev<.hnd-exn>*/
  struct kk_std_core_hnd_Ev* _con26375 = kk_std_core_hnd__as_Ev(ev_23171);
  kk_std_core_hnd__marker m = _con26375->marker;
  kk_box_t _box_x24478 = _con26375->hnd;
  kk_std_core__hnd_exn h = kk_std_core__hnd_exn_unbox(_box_x24478, NULL);
  kk_std_core__hnd_exn_dup(h);
  kk_std_core_hnd__clause1 _match_25239;
  struct kk_std_core__Hnd_exn* _con26377 = kk_std_core__as_Hnd_exn(h);
  kk_std_core_hnd__clause1 except_throw_exn = _con26377->except_throw_exn;
  if (kk_std_core__hnd_exn_is_unique(h)) {
    kk_std_core__hnd_exn_free(h);
  }
  else {
    kk_std_core_hnd__clause1_dup(except_throw_exn);
    kk_std_core__hnd_exn_decref(h, _ctx);
  }
  _match_25239 = except_throw_exn; /*forall<a> std/core/hnd/clause1<exception,a,.hnd-exn,3528,3529>*/
  kk_function_t _fun_unbox_x24482 = _match_25239.clause;
  y = kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_box_t, kk_context_t*), _fun_unbox_x24482, (_fun_unbox_x24482, m, ev_23171, kk_std_core__exception_box(exn_23170, _ctx), _ctx)); /*15802*/
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_new_foldl1_fun26378(i_23325, _ctx), _ctx);
  }
  return y;
}
extern kk_box_t kk_std_core_foldr_fun26380(kk_function_t _fself, kk_box_t x, kk_box_t y, kk_context_t* _ctx) {
  struct kk_std_core_foldr_fun26380__t* _self = kk_function_as(struct kk_std_core_foldr_fun26380__t*, _fself);
  kk_function_t f = _self->f; /* (15821, 15817) -> 15819 15817 */
  kk_drop_match(_self, {kk_function_dup(f);}, {}, _ctx)
  return kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_context_t*), f, (f, y, x, _ctx));
}


// lift anonymous function
struct kk_std_core_foldr1_fun26393__t {
  struct kk_function_s _base;
  size_t i_23333;
};
static kk_box_t kk_std_core_foldr1_fun26393(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foldr1_fun26393(size_t i_23333, kk_context_t* _ctx) {
  struct kk_std_core_foldr1_fun26393__t* _self = kk_function_alloc_as(struct kk_std_core_foldr1_fun26393__t, 1, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_foldr1_fun26393, kk_context());
  _self->i_23333 = i_23333;
  return &_self->_base;
}

static kk_box_t kk_std_core_foldr1_fun26393(kk_function_t _fself, kk_function_t cont, kk_box_t res, kk_context_t* _ctx) {
  struct kk_std_core_foldr1_fun26393__t* _self = kk_function_as(struct kk_std_core_foldr1_fun26393__t*, _fself);
  size_t i_23333 = _self->i_23333; /* std/core/hnd/ev-index */
  kk_drop_match(_self, {;}, {}, _ctx)
  return kk_std_core_hnd_open_at1(i_23333, cont, res, _ctx);
}

kk_box_t kk_std_core_foldr1(kk_std_core__list xs, kk_function_t f, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, f : (a, a) -> <exn|e> a) -> <exn|e> a */ 
  kk_std_core__list xs0_23331 = kk_std_core_reverse(xs, _ctx); /*list<15855>*/;
  if (kk_std_core__is_Cons(xs0_23331)) {
    struct kk_std_core_Cons* _con26381 = kk_std_core__as_Cons(xs0_23331);
    kk_box_t x = _con26381->head;
    kk_std_core__list xx = _con26381->tail;
    if (kk_std_core__list_is_unique(xs0_23331)) {
      kk_std_core__list_free(xs0_23331);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs0_23331, _ctx);
    }
    return kk_std_core_foldl(xx, x, f, _ctx);
  }
  kk_function_drop(f, _ctx);
  size_t i_23333;
  kk_std_core_hnd__htag _x26382 = kk_std_core_hnd__htag_dup(kk_std_core__tag_exn); /*std/core/hnd/htag<.hnd-exn>*/
  i_23333 = kk_std_core_hnd__evv_index(_x26382, _ctx); /*std/core/hnd/ev-index*/
  kk_evv_t w = kk_evv_swap_create1(i_23333,kk_context()); /*std/core/hnd/evv<<exn|15856>>*/;
  kk_box_t y;
  kk_std_core__exception exn_23170;
  kk_string_t _x26383;
  kk_define_string_literal(, _s26384, 33, "unexpected Nil in std/core/foldl1")
  _x26383 = kk_string_dup(_s26384); /*string*/
  kk_std_core__exception_info _x26385;
  kk_std_core_types__optional _x26386 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
  if (kk_std_core_types__is_Optional(_x26386)) {
    kk_box_t _box_x24489 = _x26386._cons.Optional.value;
    kk_std_core__exception_info _info_12803 = kk_std_core__exception_info_unbox(_box_x24489, NULL);
    _x26385 = _info_12803; /*exception-info*/
    goto _match26387;
  }
  _x26385 = kk_std_core__new_ExnError(_ctx); /*exception-info*/
  _match26387: ;
  exn_23170 = kk_std_core__new_Exception(_x26383, _x26385, _ctx); /*exception*/
  kk_std_core_hnd__ev ev_23171;
  size_t _x26389 = ((size_t)0); /*size_t*/
  ev_23171 = kk_evv_at(_x26389,kk_context()); /*std/core/hnd/ev<.hnd-exn>*/
  struct kk_std_core_hnd_Ev* _con26390 = kk_std_core_hnd__as_Ev(ev_23171);
  kk_std_core_hnd__marker m = _con26390->marker;
  kk_box_t _box_x24490 = _con26390->hnd;
  kk_std_core__hnd_exn h = kk_std_core__hnd_exn_unbox(_box_x24490, NULL);
  kk_std_core__hnd_exn_dup(h);
  kk_std_core_hnd__clause1 _match_25236;
  struct kk_std_core__Hnd_exn* _con26392 = kk_std_core__as_Hnd_exn(h);
  kk_std_core_hnd__clause1 except_throw_exn = _con26392->except_throw_exn;
  if (kk_std_core__hnd_exn_is_unique(h)) {
    kk_std_core__hnd_exn_free(h);
  }
  else {
    kk_std_core_hnd__clause1_dup(except_throw_exn);
    kk_std_core__hnd_exn_decref(h, _ctx);
  }
  _match_25236 = except_throw_exn; /*forall<a> std/core/hnd/clause1<exception,a,.hnd-exn,3528,3529>*/
  kk_function_t _fun_unbox_x24494 = _match_25236.clause;
  y = kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_box_t, kk_context_t*), _fun_unbox_x24494, (_fun_unbox_x24494, m, ev_23171, kk_std_core__exception_box(exn_23170, _ctx), _ctx)); /*15855*/
  kk_unit_t __ = kk_Unit;
  kk_evv_set(w,kk_context());
  if (kk_yielding(kk_context())) {
    kk_box_drop(y, _ctx);
    return kk_std_core_hnd_yield_cont(kk_std_core_new_foldr1_fun26393(i_23333, _ctx), _ctx);
  }
  return y;
}
 
// monadic lift

kk_unit_t kk_std_core__mlift22890_op(kk_function_t action, kk_integer_t end, kk_integer_t i, kk_unit_t wild__, kk_context_t* _ctx) { /* forall<e> (action : (int) -> e (), end : int, i : int, wild_ : ()) -> e () */ 
  kk_integer_t i0_22766 = kk_std_core_inc(i, _ctx); /*int*/;
  kk_std_core__lift22737_for(action, end, i0_22766, _ctx); return kk_Unit;
}
 
// lift


// lift anonymous function
struct kk_std_core__lift22737_for_fun26399__t {
  struct kk_function_s _base;
  kk_function_t action0;
  kk_integer_t end0;
  kk_integer_t i0;
};
static kk_box_t kk_std_core__lift22737_for_fun26399(kk_function_t _fself, kk_box_t _b_24502, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift22737_for_fun26399(kk_function_t action0, kk_integer_t end0, kk_integer_t i0, kk_context_t* _ctx) {
  struct kk_std_core__lift22737_for_fun26399__t* _self = kk_function_alloc_as(struct kk_std_core__lift22737_for_fun26399__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift22737_for_fun26399, kk_context());
  _self->action0 = action0;
  _self->end0 = end0;
  _self->i0 = i0;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift22737_for_fun26399(kk_function_t _fself, kk_box_t _b_24502, kk_context_t* _ctx) {
  struct kk_std_core__lift22737_for_fun26399__t* _self = kk_function_as(struct kk_std_core__lift22737_for_fun26399__t*, _fself);
  kk_function_t action0 = _self->action0; /* (int) -> 15857 () */
  kk_integer_t end0 = _self->end0; /* int */
  kk_integer_t i0 = _self->i0; /* int */
  kk_drop_match(_self, {kk_function_dup(action0);kk_integer_dup(end0);kk_integer_dup(i0);}, {}, _ctx)
  kk_unit_t _x26400 = kk_Unit;
  kk_unit_t _x26401 = kk_Unit;
  kk_unit_unbox(_b_24502);
  kk_std_core__mlift22890_op(action0, end0, i0, _x26401, _ctx);
  return kk_unit_box(_x26400);
}

kk_unit_t kk_std_core__lift22737_for(kk_function_t action0, kk_integer_t end0, kk_integer_t i0, kk_context_t* _ctx) { /* forall<e> (action : (int) -> e (), end : int, i : int) -> e () */ 
  kk__tailcall: ;
  bool _match_25233;
  kk_integer_t _x26394 = kk_integer_dup(i0); /*int*/
  kk_integer_t _x26395 = kk_integer_dup(end0); /*int*/
  _match_25233 = kk_integer_lte(_x26394,_x26395,kk_context()); /*bool*/
  if (_match_25233) {
    kk_unit_t x_23337 = kk_Unit;
    kk_function_t _x26397 = kk_function_dup(action0); /*(int) -> 15857 ()*/
    kk_integer_t _x26396 = kk_integer_dup(i0); /*int*/
    kk_function_call(kk_unit_t, (kk_function_t, kk_integer_t, kk_context_t*), _x26397, (_x26397, _x26396, _ctx));
    if (kk_yielding(kk_context())) {
      kk_box_t _x26398 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift22737_for_fun26399(action0, end0, i0, _ctx), _ctx); /*3991*/
      kk_unit_unbox(_x26398); return kk_Unit;
    }
    kk_integer_t i0_227660 = kk_std_core_inc(i0, _ctx); /*int*/;
    { // tailcall
      i0 = i0_227660;
      goto kk__tailcall;
    }
  }
  kk_function_drop(action0, _ctx);
  kk_integer_drop(end0, _ctx);
  kk_integer_drop(i0, _ctx);
  kk_Unit; return kk_Unit;
}
 
// monadic lift

kk_std_core_types__maybe kk_std_core__mlift22891_op(kk_function_t action, kk_integer_t end, kk_integer_t i, kk_std_core_types__maybe _y_179, kk_context_t* _ctx) { /* forall<a,e> (action : (int) -> e maybe<a>, end : int, i : int, maybe<a>) -> e maybe<a> */ 
  if (kk_std_core_types__is_Nothing(_y_179)) {
    kk_integer_t i0_22768 = kk_std_core_inc(i, _ctx); /*int*/;
    return kk_std_core__lift22738_for_while(action, end, i0_22768, _ctx);
  }
  kk_box_t x = _y_179._cons.Just.value;
  kk_function_drop(action, _ctx);
  kk_integer_drop(end, _ctx);
  kk_integer_drop(i, _ctx);
  return kk_std_core_types__new_Just(x, _ctx);
}
 
// lift


// lift anonymous function
struct kk_std_core__lift22738_for_while_fun26407__t {
  struct kk_function_s _base;
  kk_function_t action0;
  kk_integer_t end0;
  kk_integer_t i0;
};
static kk_box_t kk_std_core__lift22738_for_while_fun26407(kk_function_t _fself, kk_box_t _b_24506, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift22738_for_while_fun26407(kk_function_t action0, kk_integer_t end0, kk_integer_t i0, kk_context_t* _ctx) {
  struct kk_std_core__lift22738_for_while_fun26407__t* _self = kk_function_alloc_as(struct kk_std_core__lift22738_for_while_fun26407__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift22738_for_while_fun26407, kk_context());
  _self->action0 = action0;
  _self->end0 = end0;
  _self->i0 = i0;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift22738_for_while_fun26407(kk_function_t _fself, kk_box_t _b_24506, kk_context_t* _ctx) {
  struct kk_std_core__lift22738_for_while_fun26407__t* _self = kk_function_as(struct kk_std_core__lift22738_for_while_fun26407__t*, _fself);
  kk_function_t action0 = _self->action0; /* (int) -> 15910 maybe<15909> */
  kk_integer_t end0 = _self->end0; /* int */
  kk_integer_t i0 = _self->i0; /* int */
  kk_drop_match(_self, {kk_function_dup(action0);kk_integer_dup(end0);kk_integer_dup(i0);}, {}, _ctx)
  kk_std_core_types__maybe _x26408;
  kk_std_core_types__maybe _x26409 = kk_std_core_types__maybe_unbox(_b_24506, _ctx); /*maybe<15909>*/
  _x26408 = kk_std_core__mlift22891_op(action0, end0, i0, _x26409, _ctx); /*maybe<15909>*/
  return kk_std_core_types__maybe_box(_x26408, _ctx);
}

kk_std_core_types__maybe kk_std_core__lift22738_for_while(kk_function_t action0, kk_integer_t end0, kk_integer_t i0, kk_context_t* _ctx) { /* forall<a,e> (action : (int) -> e maybe<a>, end : int, i : int) -> e maybe<a> */ 
  kk__tailcall: ;
  bool _match_25231;
  kk_integer_t _x26402 = kk_integer_dup(i0); /*int*/
  kk_integer_t _x26403 = kk_integer_dup(end0); /*int*/
  _match_25231 = kk_integer_lte(_x26402,_x26403,kk_context()); /*bool*/
  if (_match_25231) {
    kk_std_core_types__maybe x_23341;
    kk_function_t _x26405 = kk_function_dup(action0); /*(int) -> 15910 maybe<15909>*/
    kk_integer_t _x26404 = kk_integer_dup(i0); /*int*/
    x_23341 = kk_function_call(kk_std_core_types__maybe, (kk_function_t, kk_integer_t, kk_context_t*), _x26405, (_x26405, _x26404, _ctx)); /*maybe<15909>*/
    if (kk_yielding(kk_context())) {
      kk_std_core_types__maybe_drop(x_23341, _ctx);
      kk_box_t _x26406 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift22738_for_while_fun26407(action0, end0, i0, _ctx), _ctx); /*3991*/
      return kk_std_core_types__maybe_unbox(_x26406, _ctx);
    }
    if (kk_std_core_types__is_Nothing(x_23341)) {
      kk_integer_t i0_227680 = kk_std_core_inc(i0, _ctx); /*int*/;
      { // tailcall
        i0 = i0_227680;
        goto kk__tailcall;
      }
    }
    kk_box_t x0 = x_23341._cons.Just.value;
    kk_integer_drop(i0, _ctx);
    kk_integer_drop(end0, _ctx);
    kk_function_drop(action0, _ctx);
    return kk_std_core_types__new_Just(x0, _ctx);
  }
  kk_function_drop(action0, _ctx);
  kk_integer_drop(end0, _ctx);
  kk_integer_drop(i0, _ctx);
  return kk_std_core_types__new_Nothing(_ctx);
}
 
// monadic lift

kk_unit_t kk_std_core__mlift22892_foreach_indexed(kk_ref_t i, kk_integer_t _y_187, kk_context_t* _ctx) { /* forall<h,e> (i : local-var<h,int>, int) -> <local<h>|e> () */ 
  kk_integer_t _b_24512_24510 = kk_integer_add(_y_187,(kk_integer_from_small(1)),kk_context()); /*int*/;
  (kk_ref_set(i,(kk_integer_box(_b_24512_24510)),kk_context())); return kk_Unit;
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift22893_foreach_indexed_fun26413__t {
  struct kk_function_s _base;
  kk_ref_t i;
};
static kk_box_t kk_std_core__mlift22893_foreach_indexed_fun26413(kk_function_t _fself, kk_box_t _b_24516, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift22893_foreach_indexed_fun26413(kk_ref_t i, kk_context_t* _ctx) {
  struct kk_std_core__mlift22893_foreach_indexed_fun26413__t* _self = kk_function_alloc_as(struct kk_std_core__mlift22893_foreach_indexed_fun26413__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift22893_foreach_indexed_fun26413, kk_context());
  _self->i = i;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift22893_foreach_indexed_fun26413(kk_function_t _fself, kk_box_t _b_24516, kk_context_t* _ctx) {
  struct kk_std_core__mlift22893_foreach_indexed_fun26413__t* _self = kk_function_as(struct kk_std_core__mlift22893_foreach_indexed_fun26413__t*, _fself);
  kk_ref_t i = _self->i; /* local-var<16098,int> */
  kk_drop_match(_self, {kk_ref_dup(i);}, {}, _ctx)
  kk_unit_t _x26414 = kk_Unit;
  kk_integer_t _x26415 = kk_integer_unbox(_b_24516); /*int*/
  kk_std_core__mlift22892_foreach_indexed(i, _x26415, _ctx);
  return kk_unit_box(_x26414);
}

kk_unit_t kk_std_core__mlift22893_foreach_indexed(kk_ref_t i, kk_unit_t wild__, kk_context_t* _ctx) { /* forall<h,e> (i : local-var<h,int>, wild_ : ()) -> <local<h>|e> () */ 
  kk_integer_t x_23345;
  kk_box_t _x26410;
  kk_ref_t _x26411 = kk_ref_dup(i); /*local-var<16098,int>*/
  _x26410 = (kk_ref_get(_x26411,kk_context())); /*228*/
  x_23345 = kk_integer_unbox(_x26410); /*int*/
  if (kk_yielding(kk_context())) {
    kk_integer_drop(x_23345, _ctx);
    kk_box_t _x26412 = kk_std_core_hnd_yield_extend(kk_std_core__new_mlift22893_foreach_indexed_fun26413(i, _ctx), _ctx); /*3991*/
    kk_unit_unbox(_x26412); return kk_Unit;
  }
  kk_std_core__mlift22892_foreach_indexed(i, x_23345, _ctx); return kk_Unit;
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift22894_foreach_indexed_fun26417__t {
  struct kk_function_s _base;
  kk_ref_t i;
};
static kk_box_t kk_std_core__mlift22894_foreach_indexed_fun26417(kk_function_t _fself, kk_box_t _b_24520, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift22894_foreach_indexed_fun26417(kk_ref_t i, kk_context_t* _ctx) {
  struct kk_std_core__mlift22894_foreach_indexed_fun26417__t* _self = kk_function_alloc_as(struct kk_std_core__mlift22894_foreach_indexed_fun26417__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift22894_foreach_indexed_fun26417, kk_context());
  _self->i = i;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift22894_foreach_indexed_fun26417(kk_function_t _fself, kk_box_t _b_24520, kk_context_t* _ctx) {
  struct kk_std_core__mlift22894_foreach_indexed_fun26417__t* _self = kk_function_as(struct kk_std_core__mlift22894_foreach_indexed_fun26417__t*, _fself);
  kk_ref_t i = _self->i; /* local-var<16098,int> */
  kk_drop_match(_self, {kk_ref_dup(i);}, {}, _ctx)
  kk_unit_t _x26418 = kk_Unit;
  kk_unit_t _x26419 = kk_Unit;
  kk_unit_unbox(_b_24520);
  kk_std_core__mlift22893_foreach_indexed(i, _x26419, _ctx);
  return kk_unit_box(_x26418);
}

kk_unit_t kk_std_core__mlift22894_foreach_indexed(kk_function_t action, kk_ref_t i, kk_box_t x, kk_integer_t j, kk_context_t* _ctx) { /* forall<h,a,e> (action : (int, a) -> e (), i : local-var<h,int>, x : a, j : int) -> <local<h>|e> () */ 
  kk_unit_t x0_23347 = kk_Unit;
  kk_function_call(kk_unit_t, (kk_function_t, kk_integer_t, kk_box_t, kk_context_t*), action, (action, j, x, _ctx));
  if (kk_yielding(kk_context())) {
    kk_box_t _x26416 = kk_std_core_hnd_yield_extend(kk_std_core__new_mlift22894_foreach_indexed_fun26417(i, _ctx), _ctx); /*3991*/
    kk_unit_unbox(_x26416); return kk_Unit;
  }
  kk_std_core__mlift22893_foreach_indexed(i, x0_23347, _ctx); return kk_Unit;
}
 
// Invoke `action` for each element of a list, passing also the position of the element.


// lift anonymous function
struct kk_std_core_foreach_indexed_fun26421__t {
  struct kk_function_s _base;
  kk_function_t action;
  kk_ref_t loc;
};
static kk_unit_t kk_std_core_foreach_indexed_fun26421(kk_function_t _fself, kk_box_t x, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_indexed_fun26421(kk_function_t action, kk_ref_t loc, kk_context_t* _ctx) {
  struct kk_std_core_foreach_indexed_fun26421__t* _self = kk_function_alloc_as(struct kk_std_core_foreach_indexed_fun26421__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_foreach_indexed_fun26421, kk_context());
  _self->action = action;
  _self->loc = loc;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_foreach_indexed_fun26425__t {
  struct kk_function_s _base;
  kk_function_t action;
  kk_ref_t loc;
  kk_box_t x;
};
static kk_box_t kk_std_core_foreach_indexed_fun26425(kk_function_t _fself, kk_box_t _b_24528, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_indexed_fun26425(kk_function_t action, kk_ref_t loc, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_foreach_indexed_fun26425__t* _self = kk_function_alloc_as(struct kk_std_core_foreach_indexed_fun26425__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_foreach_indexed_fun26425, kk_context());
  _self->action = action;
  _self->loc = loc;
  _self->x = x;
  return &_self->_base;
}

static kk_box_t kk_std_core_foreach_indexed_fun26425(kk_function_t _fself, kk_box_t _b_24528, kk_context_t* _ctx) {
  struct kk_std_core_foreach_indexed_fun26425__t* _self = kk_function_as(struct kk_std_core_foreach_indexed_fun26425__t*, _fself);
  kk_function_t action = _self->action; /* (int, 16104) -> 16105 () */
  kk_ref_t loc = _self->loc; /* local-var<global,int> */
  kk_box_t x = _self->x; /* 16104 */
  kk_drop_match(_self, {kk_function_dup(action);kk_ref_dup(loc);kk_box_dup(x);}, {}, _ctx)
  kk_unit_t _x26426 = kk_Unit;
  kk_integer_t _x26427 = kk_integer_unbox(_b_24528); /*int*/
  kk_std_core__mlift22894_foreach_indexed(action, loc, x, _x26427, _ctx);
  return kk_unit_box(_x26426);
}
static kk_unit_t kk_std_core_foreach_indexed_fun26421(kk_function_t _fself, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_foreach_indexed_fun26421__t* _self = kk_function_as(struct kk_std_core_foreach_indexed_fun26421__t*, _fself);
  kk_function_t action = _self->action; /* (int, 16104) -> 16105 () */
  kk_ref_t loc = _self->loc; /* local-var<global,int> */
  kk_drop_match(_self, {kk_function_dup(action);kk_ref_dup(loc);}, {}, _ctx)
  kk_integer_t x0_23354;
  kk_box_t _x26422;
  kk_ref_t _x26423 = kk_ref_dup(loc); /*local-var<global,int>*/
  _x26422 = (kk_ref_get(_x26423,kk_context())); /*228*/
  x0_23354 = kk_integer_unbox(_x26422); /*int*/
  if (kk_yielding(kk_context())) {
    kk_integer_drop(x0_23354, _ctx);
    kk_box_t _x26424 = kk_std_core_hnd_yield_extend(kk_std_core_new_foreach_indexed_fun26425(action, loc, x, _ctx), _ctx); /*3991*/
    return kk_unit_unbox(_x26424);
  }
  return kk_std_core__mlift22894_foreach_indexed(action, loc, x, x0_23354, _ctx);
}

kk_unit_t kk_std_core_foreach_indexed(kk_std_core__list xs, kk_function_t action, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, action : (int, a) -> e ()) -> e () */ 
  kk_ref_t loc = kk_ref_alloc((kk_integer_box(kk_integer_from_small(0))),kk_context()); /*local-var<global,int>*/;
  kk_unit_t res = kk_Unit;
  kk_function_t _x26420;
  kk_ref_dup(loc);
  _x26420 = kk_std_core_new_foreach_indexed_fun26421(action, loc, _ctx); /*(x : 16104) -> <local<global>|16105> ()*/
  kk_std_core_foreach(xs, _x26420, _ctx);
  kk_box_t _x26428 = kk_std_core_hnd_prompt_local_var(loc, kk_unit_box(res), _ctx); /*9788*/
  kk_unit_unbox(_x26428); return kk_Unit;
}
 
// Invoke a function `f` for each element in a vector `v`


// lift anonymous function
struct kk_std_core_foreach_indexed_fun26431__t_1 {
  struct kk_function_s _base;
  kk_function_t f;
  kk_vector_t v;
};
static kk_unit_t kk_std_core_foreach_indexed_fun26431_1(kk_function_t _fself, size_t i, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_foreach_indexed_fun26431_1(kk_function_t f, kk_vector_t v, kk_context_t* _ctx) {
  struct kk_std_core_foreach_indexed_fun26431__t_1* _self = kk_function_alloc_as(struct kk_std_core_foreach_indexed_fun26431__t_1, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_foreach_indexed_fun26431_1, kk_context());
  _self->f = f;
  _self->v = v;
  return &_self->_base;
}

static kk_unit_t kk_std_core_foreach_indexed_fun26431_1(kk_function_t _fself, size_t i, kk_context_t* _ctx) {
  struct kk_std_core_foreach_indexed_fun26431__t_1* _self = kk_function_as(struct kk_std_core_foreach_indexed_fun26431__t_1*, _fself);
  kk_function_t f = _self->f; /* (16143, int) -> 16144 () */
  kk_vector_t v = _self->v; /* vector<16143> */
  kk_drop_match(_self, {kk_function_dup(f);kk_vector_dup(v);}, {}, _ctx)
  kk_box_t x_23361 = kk_vector_at(v,i); /*16143*/;
  kk_integer_t _x26432 = kk_integer_from_size_t(i,kk_context()); /*int*/
  return kk_function_call(kk_unit_t, (kk_function_t, kk_box_t, kk_integer_t, kk_context_t*), f, (f, x_23361, _x26432, _ctx));
}

kk_unit_t kk_std_core_foreach_indexed_1(kk_vector_t v, kk_function_t f, kk_context_t* _ctx) { /* forall<a,e> (v : vector<a>, f : (a, int) -> e ()) -> e () */ 
  size_t start_23358 = ((size_t)0); /*size_t*/;
  size_t end_23359;
  size_t _x26429;
  kk_vector_t _x26430 = kk_vector_dup(v); /*vector<16143>*/
  _x26429 = kk_vector_len(_x26430); /*size_t*/
  end_23359 = kk_std_core_decr_1(_x26429, _ctx); /*size_t*/
  kk_std_core__lift22732_forz(kk_std_core_new_foreach_indexed_fun26431_1(f, v, _ctx), end_23359, start_23358, _ctx); return kk_Unit;
}
 
// Return the head of list if the list is not empty.

kk_std_core_types__maybe kk_std_core_head_1(kk_std_core__list xs, kk_context_t* _ctx) { /* forall<a> (xs : list<a>) -> maybe<a> */ 
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con26440 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con26440->head;
    kk_std_core__list _pat0 = _con26440->tail;
    if (kk_std_core__list_is_unique(xs)) {
      kk_std_core__list_drop(_pat0, _ctx);
      kk_std_core__list_free(xs);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_decref(xs, _ctx);
    }
    return kk_std_core_types__new_Just(x, _ctx);
  }
  return kk_std_core_types__new_Nothing(_ctx);
}
 
// Return the head of list if the list is not empty, or use `default` otherwise

kk_box_t kk_std_core_head_2(kk_std_core__list xs, kk_box_t default0, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, default : a) -> a */ 
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con26441 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con26441->head;
    kk_std_core__list _pat0 = _con26441->tail;
    if (kk_std_core__list_is_unique(xs)) {
      kk_std_core__list_drop(_pat0, _ctx);
      kk_std_core__list_free(xs);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_decref(xs, _ctx);
    }
    kk_box_drop(default0, _ctx);
    return x;
  }
  return default0;
}
 
// Return the first character of a string as a string (or the empty string)

kk_string_t kk_std_core_head_3(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> string */ 
  kk_std_core__sslice _x26442;
  kk_std_core__sslice slice0 = kk_std_core_first1(s, _ctx); /*sslice*/;
  bool _match_25225;
  kk_integer_t _x26443;
  kk_std_core_types__optional _x26444 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
  if (kk_std_core_types__is_Optional(_x26444)) {
    kk_box_t _box_x24535 = _x26444._cons.Optional.value;
    kk_integer_t _n_13060 = kk_integer_unbox(_box_x24535);
    _x26443 = _n_13060; /*int*/
    goto _match26445;
  }
  _x26443 = kk_integer_from_small(1); /*int*/
  _match26445: ;
  _match_25225 = kk_integer_eq(_x26443,(kk_integer_from_small(1)),kk_context()); /*bool*/
  if (_match_25225) {
    _x26442 = slice0; /*sslice*/
  }
  else {
    kk_integer_t _x26447;
    kk_integer_t _x26448;
    kk_std_core_types__optional _x26449 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
    if (kk_std_core_types__is_Optional(_x26449)) {
      kk_box_t _box_x24536 = _x26449._cons.Optional.value;
      kk_integer_t _n_130600 = kk_integer_unbox(_box_x24536);
      _x26448 = _n_130600; /*int*/
      goto _match26450;
    }
    _x26448 = kk_integer_from_small(1); /*int*/
    _match26450: ;
    _x26447 = kk_integer_sub(_x26448,(kk_integer_from_small(1)),kk_context()); /*int*/
    _x26442 = kk_std_core_extend(slice0, _x26447, _ctx); /*sslice*/
  }
  return kk_std_core_string_3(_x26442, _ctx);
}
extern kk_std_core_types__maybe kk_std_core_head_char_fun26453(kk_function_t _fself, kk_char_t _b_24537, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  return kk_std_core_types__new_Just(kk_char_box(_b_24537, _ctx), _ctx);
}

kk_integer_t kk_std_core_index_of_acc(kk_std_core__list xs, kk_function_t pred, kk_integer_t idx, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, pred : (a) -> bool, idx : int) -> int */ 
  kk__tailcall: ;
  if (kk_std_core__is_Nil(xs)) {
    kk_integer_drop(idx, _ctx);
    kk_function_drop(pred, _ctx);
    return kk_integer_sub((kk_integer_from_small(0)),(kk_integer_from_small(1)),kk_context());
  }
  struct kk_std_core_Cons* _con26454 = kk_std_core__as_Cons(xs);
  kk_box_t x = _con26454->head;
  kk_std_core__list xx = _con26454->tail;
  if (kk_std_core__list_is_unique(xs)) {
    kk_std_core__list_free(xs);
  }
  else {
    kk_box_dup(x);
    kk_std_core__list_dup(xx);
    kk_std_core__list_decref(xs, _ctx);
  }
  bool _match_25224;
  kk_function_t _x26455 = kk_function_dup(pred); /*(16723) -> bool*/
  _match_25224 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), _x26455, (_x26455, x, _ctx)); /*bool*/
  if (_match_25224) {
    kk_function_drop(pred, _ctx);
    kk_std_core__list_drop(xx, _ctx);
    return idx;
  }
  { // tailcall
    kk_integer_t _x26456 = kk_integer_add(idx,(kk_integer_from_small(1)),kk_context()); /*int*/
    xs = xx;
    idx = _x26456;
    goto kk__tailcall;
  }
}
 
// Return the list without its last element.
// Return an empty list for an empty list.

kk_std_core__list kk_std_core__ctail_init(kk_std_core__list xs, kk_std_core_types__ctail _acc, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, ctail<list<a>>) -> list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con26457 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con26457->head;
    kk_std_core__list xx = _con26457->tail;
    if (kk_std_core__is_Cons(xx)) {
      struct kk_std_core_Cons* _con26458 = kk_std_core__as_Cons(xx);
      if (kk_std_core__list_is_unique(xs)) {
        kk_std_core__list_free(xs);
      }
      else {
        kk_box_dup(x);
        kk_std_core__list_dup(xx);
        kk_std_core__list_decref(xs, _ctx);
      }
      kk_reuse_t _ru_25123;
      kk_std_core__list _x26459 = kk_std_core__list_dup(xx); /*list<16760>*/
      _ru_25123 = kk_std_core__list_dropn_reuse(_x26459, ((int32_t)2), _ctx); /*reuse*/
      kk_std_core__list _ctail_22824 = kk_std_core__list_hole(); /*list<16760>*/;
      kk_std_core__list _ctail_22825 = kk_std_core__new_Cons(_ru_25123, x, _ctail_22824, _ctx); /*list<16760>*/;
      { // tailcall
        kk_std_core_types__ctail _x26460;
        kk_box_t* field_23372 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_22825)->tail)); /*cfield<list<16760>>*/;
        if (kk_std_core_types__is_CTail(_acc)) {
          kk_box_t _box_x24544 = _acc._cons.CTail._field1;
          kk_box_t* last0 = _acc._cons.CTail._field2;
          kk_std_core__list head0 = kk_std_core__list_unbox(_box_x24544, NULL);
          kk_unit_t __ = kk_Unit;
          *(last0) = kk_std_core__list_box(_ctail_22825, _ctx);
          _x26460 = kk_std_core_types__new_CTail(kk_std_core__list_box(head0, _ctx), field_23372, _ctx); /*ctail<58>*/
          goto _match26461;
        }
        _x26460 = kk_std_core_types__new_CTail(kk_std_core__list_box(_ctail_22825, _ctx), field_23372, _ctx); /*ctail<58>*/
        _match26461: ;
        xs = xx;
        _acc = _x26460;
        goto kk__tailcall;
      }
    }
  }
  kk_std_core__list_drop(xs, _ctx);
  if (kk_std_core_types__is_CTail(_acc)) {
    kk_box_t _box_x24553 = _acc._cons.CTail._field1;
    kk_box_t* last00 = _acc._cons.CTail._field2;
    kk_std_core__list head00 = kk_std_core__list_unbox(_box_x24553, NULL);
    kk_unit_t __0 = kk_Unit;
    *(last00) = kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx);
    return head00;
  }
  return kk_std_core__new_Nil(_ctx);
}
 
// Return the list without its last element.
// Return an empty list for an empty list.

kk_std_core__list kk_std_core_init(kk_std_core__list xs0, kk_context_t* _ctx) { /* forall<a> (xs : list<a>) -> list<a> */ 
  return kk_std_core__ctail_init(xs0, kk_std_core_types__new_CTailNil(_ctx), _ctx);
}
 
// Is the character an ASCII hexa-decimal digit ?

bool kk_std_core_is_hex_digit(kk_char_t c, kk_context_t* _ctx) { /* (c : char) -> bool */ 
  bool _match_25215 = kk_std_core_is_digit(c, _ctx); /*bool*/;
  if (_match_25215) {
    return true;
  }
  bool _match_25216;
  bool _match_25218 = (c >= ('a')); /*bool*/;
  if (_match_25218) {
    _match_25216 = (c <= ('f')); /*bool*/
  }
  else {
    _match_25216 = false; /*bool*/
  }
  if (_match_25216) {
    return true;
  }
  bool _match_25217 = (c >= ('A')); /*bool*/;
  if (_match_25217) {
    return (c <= ('F'));
  }
  return false;
}
 
// Tests if a character is an element of `" \t\n\r"`

bool kk_std_core_is_white(kk_char_t c, kk_context_t* _ctx) { /* (c : char) -> bool */ 
  bool _match_25212 = (c == (' ')); /*bool*/;
  if (_match_25212) {
    return true;
  }
  bool _match_25213 = (c == 0x0009); /*bool*/;
  if (_match_25213) {
    return true;
  }
  bool _match_25214 = (c == 0x000A); /*bool*/;
  if (_match_25214) {
    return true;
  }
  return (c == 0x000D);
}
 
// Return the last element of a list (or `Nothing` for the empty list)

kk_std_core_types__maybe kk_std_core_last(kk_std_core__list xs, kk_context_t* _ctx) { /* forall<a> (xs : list<a>) -> maybe<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con26470 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con26470->head;
    kk_std_core__list _pat0 = _con26470->tail;
    if (kk_std_core__is_Nil(_pat0)) {
      if (kk_std_core__list_is_unique(xs)) {
        if (kk_std_core__list_is_unique(_pat0)) {
          kk_std_core__list_free(_pat0);
        }
        else {
          kk_std_core__list_decref(_pat0, _ctx);
        }
        kk_std_core__list_free(xs);
      }
      else {
        kk_box_dup(x);
        kk_std_core__list_decref(xs, _ctx);
      }
      return kk_std_core_types__new_Just(x, _ctx);
    }
  }
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con26471 = kk_std_core__as_Cons(xs);
    kk_box_t _pat2 = _con26471->head;
    kk_std_core__list xx = _con26471->tail;
    if (kk_std_core__list_is_unique(xs)) {
      kk_box_drop(_pat2, _ctx);
      kk_std_core__list_free(xs);
    }
    else {
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    { // tailcall
      xs = xx;
      goto kk__tailcall;
    }
  }
  return kk_std_core_types__new_Nothing(_ctx);
}
 
// Return the last element of a list (or `default` for the empty list)

kk_box_t kk_std_core_last_1(kk_std_core__list xs, kk_box_t default0, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, default : a) -> a */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con26472 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con26472->head;
    kk_std_core__list _pat0 = _con26472->tail;
    if (kk_std_core__is_Nil(_pat0)) {
      if (kk_std_core__list_is_unique(xs)) {
        if (kk_std_core__list_is_unique(_pat0)) {
          kk_std_core__list_free(_pat0);
        }
        else {
          kk_std_core__list_decref(_pat0, _ctx);
        }
        kk_std_core__list_free(xs);
      }
      else {
        kk_box_dup(x);
        kk_std_core__list_decref(xs, _ctx);
      }
      kk_box_drop(default0, _ctx);
      return x;
    }
  }
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con26473 = kk_std_core__as_Cons(xs);
    kk_box_t _pat2 = _con26473->head;
    kk_std_core__list xx = _con26473->tail;
    if (kk_std_core__list_is_unique(xs)) {
      kk_box_drop(_pat2, _ctx);
      kk_std_core__list_free(xs);
    }
    else {
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    { // tailcall
      xs = xx;
      goto kk__tailcall;
    }
  }
  return default0;
}
 
// O(`n`). The last `n` (default = `1`) characters in a string

kk_std_core__sslice kk_std_core_last_2(kk_string_t s, kk_std_core_types__optional n, kk_context_t* _ctx) { /* (s : string, n : optional<int>) -> sslice */ 
  kk_std_core__sslice slice0 = kk_std_core_last1(s, _ctx); /*sslice*/;
  bool _match_25211;
  kk_integer_t _x26474;
  if (kk_std_core_types__is_Optional(n)) {
    kk_box_t _box_x24562 = n._cons.Optional.value;
    kk_integer_t _n_17702 = kk_integer_unbox(_box_x24562);
    kk_integer_dup(_n_17702);
    _x26474 = _n_17702; /*int*/
    goto _match26475;
  }
  _x26474 = kk_integer_from_small(1); /*int*/
  _match26475: ;
  _match_25211 = kk_integer_eq(_x26474,(kk_integer_from_small(1)),kk_context()); /*bool*/
  if (_match_25211) {
    kk_std_core_types__optional_drop(n, _ctx);
    return slice0;
  }
  kk_std_core__sslice _x26477;
  kk_integer_t _x26478;
  kk_integer_t _x26479;
  if (kk_std_core_types__is_Optional(n)) {
    kk_box_t _box_x24563 = n._cons.Optional.value;
    kk_integer_t _n_177020 = kk_integer_unbox(_box_x24563);
    kk_integer_dup(_n_177020);
    _x26479 = _n_177020; /*int*/
    goto _match26480;
  }
  _x26479 = kk_integer_from_small(1); /*int*/
  _match26480: ;
  _x26478 = kk_integer_sub((kk_integer_from_small(1)),_x26479,kk_context()); /*int*/
  _x26477 = kk_std_core_advance(slice0, _x26478, _ctx); /*sslice*/
  kk_integer_t _x26482;
  kk_integer_t _x26483;
  if (kk_std_core_types__is_Optional(n)) {
    kk_box_t _box_x24564 = n._cons.Optional.value;
    kk_integer_t _n_177021 = kk_integer_unbox(_box_x24564);
    _x26483 = _n_177021; /*int*/
    goto _match26484;
  }
  _x26483 = kk_integer_from_small(1); /*int*/
  _match26484: ;
  _x26482 = kk_integer_sub(_x26483,(kk_integer_from_small(1)),kk_context()); /*int*/
  return kk_std_core_extend(_x26477, _x26482, _ctx);
}
 
// Take the first `n` elements of a list (or fewer if the list is shorter than `n`)

kk_std_core__list kk_std_core__ctail_take(kk_std_core__list xs, kk_integer_t n, kk_std_core_types__ctail _acc, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, n : int, ctail<list<a>>) -> list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con26486 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con26486->head;
    kk_std_core__list xx = _con26486->tail;
    kk_integer_t _x26487 = kk_integer_dup(n); /*int*/
    if (kk_integer_gt(_x26487,(kk_integer_from_small(0)),kk_context())) {
      kk_reuse_t _ru_25128 = kk_reuse_null; /*reuse*/;
      if (kk_std_core__list_is_unique(xs)) {
        _ru_25128 = (kk_std_core__list_reuse(xs));
      }
      else {
        kk_box_dup(x);
        kk_std_core__list_dup(xx);
        kk_std_core__list_decref(xs, _ctx);
        _ru_25128 = kk_reuse_null;
      }
      kk_std_core__list _ctail_22826 = kk_std_core__list_hole(); /*list<17886>*/;
      kk_std_core__list _ctail_22827;
      bool _x26488 = _ru_25128!=NULL; /*bool*/
      if (_x26488) {
        struct kk_std_core_Cons* _con26489 = (struct kk_std_core_Cons*)_ru_25128;
        _con26489->tail = _ctail_22826;
        _ctail_22827 = kk_std_core__base_Cons(_con26489); /*list<17886>*/
      }
      else {
        _ctail_22827 = kk_std_core__new_Cons(kk_reuse_null, x, _ctail_22826, _ctx); /*list<17886>*/
      }
      { // tailcall
        kk_integer_t _x26490 = kk_integer_sub(n,(kk_integer_from_small(1)),kk_context()); /*int*/
        kk_std_core_types__ctail _x26491;
        kk_box_t* field_23378 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_22827)->tail)); /*cfield<list<17886>>*/;
        if (kk_std_core_types__is_CTail(_acc)) {
          kk_box_t _box_x24571 = _acc._cons.CTail._field1;
          kk_box_t* last0 = _acc._cons.CTail._field2;
          kk_std_core__list head0 = kk_std_core__list_unbox(_box_x24571, NULL);
          kk_unit_t __ = kk_Unit;
          *(last0) = kk_std_core__list_box(_ctail_22827, _ctx);
          _x26491 = kk_std_core_types__new_CTail(kk_std_core__list_box(head0, _ctx), field_23378, _ctx); /*ctail<58>*/
          goto _match26492;
        }
        _x26491 = kk_std_core_types__new_CTail(kk_std_core__list_box(_ctail_22827, _ctx), field_23378, _ctx); /*ctail<58>*/
        _match26492: ;
        xs = xx;
        n = _x26490;
        _acc = _x26491;
        goto kk__tailcall;
      }
    }
  }
  kk_integer_drop(n, _ctx);
  kk_std_core__list_drop(xs, _ctx);
  if (kk_std_core_types__is_CTail(_acc)) {
    kk_box_t _box_x24580 = _acc._cons.CTail._field1;
    kk_box_t* last00 = _acc._cons.CTail._field2;
    kk_std_core__list head00 = kk_std_core__list_unbox(_box_x24580, NULL);
    kk_unit_t __0 = kk_Unit;
    *(last00) = kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx);
    return head00;
  }
  return kk_std_core__new_Nil(_ctx);
}
 
// Take the first `n` elements of a list (or fewer if the list is shorter than `n`)

kk_std_core__list kk_std_core_take(kk_std_core__list xs0, kk_integer_t n0, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, n : int) -> list<a> */ 
  return kk_std_core__ctail_take(xs0, n0, kk_std_core_types__new_CTailNil(_ctx), _ctx);
}
 
// split a list at position `n`

kk_std_core_types__tuple2_ kk_std_core_split(kk_std_core__list xs, kk_integer_t n, kk_context_t* _ctx) { /* forall<a> (xs : list<a>, n : int) -> (list<a>, list<a>) */ 
  kk_std_core__list _b_24591_24589;
  kk_std_core__list _x26495 = kk_std_core__list_dup(xs); /*list<17906>*/
  kk_integer_t _x26496 = kk_integer_dup(n); /*int*/
  _b_24591_24589 = kk_std_core_take(_x26495, _x26496, _ctx); /*list<17906>*/
  kk_std_core__list _b_24592_24590 = kk_std_core_drop(xs, n, _ctx); /*list<17906>*/;
  return kk_std_core_types__new_dash__lp__comma__rp_(kk_std_core__list_box(_b_24591_24589, _ctx), kk_std_core__list_box(_b_24592_24590, _ctx), _ctx);
}
 
// Lookup the first element satisfying some predicate


// lift anonymous function
struct kk_std_core_lookup_fun26500__t {
  struct kk_function_s _base;
  kk_function_t pred;
};
static kk_std_core_types__maybe kk_std_core_lookup_fun26500(kk_function_t _fself, kk_box_t _b_24595, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_lookup_fun26500(kk_function_t pred, kk_context_t* _ctx) {
  struct kk_std_core_lookup_fun26500__t* _self = kk_function_alloc_as(struct kk_std_core_lookup_fun26500__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_lookup_fun26500, kk_context());
  _self->pred = pred;
  return &_self->_base;
}

static kk_std_core_types__maybe kk_std_core_lookup_fun26500(kk_function_t _fself, kk_box_t _b_24595, kk_context_t* _ctx) {
  struct kk_std_core_lookup_fun26500__t* _self = kk_function_as(struct kk_std_core_lookup_fun26500__t*, _fself);
  kk_function_t pred = _self->pred; /* (18279) -> bool */
  kk_drop_match(_self, {kk_function_dup(pred);}, {}, _ctx)
  bool _match_25208;
  kk_box_t _x26501;
  kk_std_core_types__tuple2_ _match_25210;
  kk_box_t _x26502 = kk_box_dup(_b_24595); /*13716*/
  _match_25210 = kk_std_core_types__tuple2__unbox(_x26502, _ctx); /*(18279, 18280)*/
  kk_box_t _x = _match_25210.fst;
  kk_box_dup(_x);
  kk_std_core_types__tuple2__drop(_match_25210, _ctx);
  _x26501 = _x; /*18279*/
  _match_25208 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), pred, (pred, _x26501, _ctx)); /*bool*/
  if (_match_25208) {
    kk_box_t _x26503;
    kk_std_core_types__tuple2_ _match_25209 = kk_std_core_types__tuple2__unbox(_b_24595, _ctx); /*(18279, 18280)*/;
    kk_box_t _x0 = _match_25209.snd;
    kk_box_dup(_x0);
    kk_std_core_types__tuple2__drop(_match_25209, _ctx);
    _x26503 = _x0; /*18280*/
    return kk_std_core_types__new_Just(_x26503, _ctx);
  }
  kk_box_drop(_b_24595, _ctx);
  return kk_std_core_types__new_Nothing(_ctx);
}

kk_std_core_types__maybe kk_std_core_lookup(kk_std_core__list xs, kk_function_t pred, kk_context_t* _ctx) { /* forall<a,b> (xs : list<(a, b)>, pred : (a) -> bool) -> maybe<b> */ 
  return kk_std_core_foreach_while(xs, kk_std_core_new_lookup_fun26500(pred, _ctx), _ctx);
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift22895_op(kk_box_t _y_194, kk_std_core__list _y_195, kk_context_t* _ctx) { /* forall<a,e> (a, list<a>) -> e list<a> */ 
  return kk_std_core__new_Cons(kk_reuse_null, _y_194, _y_195, _ctx);
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift22896_op_fun26505__t {
  struct kk_function_s _base;
  kk_box_t _y_1940;
};
static kk_box_t kk_std_core__mlift22896_op_fun26505(kk_function_t _fself, kk_box_t _b_24600, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift22896_op_fun26505(kk_box_t _y_1940, kk_context_t* _ctx) {
  struct kk_std_core__mlift22896_op_fun26505__t* _self = kk_function_alloc_as(struct kk_std_core__mlift22896_op_fun26505__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift22896_op_fun26505, kk_context());
  _self->_y_1940 = _y_1940;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift22896_op_fun26505(kk_function_t _fself, kk_box_t _b_24600, kk_context_t* _ctx) {
  struct kk_std_core__mlift22896_op_fun26505__t* _self = kk_function_as(struct kk_std_core__mlift22896_op_fun26505__t*, _fself);
  kk_box_t _y_1940 = _self->_y_1940; /* 18282 */
  kk_drop_match(_self, {kk_box_dup(_y_1940);}, {}, _ctx)
  kk_std_core__list _x26506;
  kk_std_core__list _x26507 = kk_std_core__list_unbox(_b_24600, _ctx); /*list<18282>*/
  _x26506 = kk_std_core__mlift22895_op(_y_1940, _x26507, _ctx); /*list<18282>*/
  return kk_std_core__list_box(_x26506, _ctx);
}

kk_std_core__list kk_std_core__mlift22896_op(kk_function_t f, kk_integer_t i, kk_std_core__list yy, kk_box_t _y_1940, kk_context_t* _ctx) { /* forall<a,b,e> (f : (idx : int, value : a) -> e b, i : int, yy : list<a>, b) -> e list<b> */ 
  kk_integer_t i0_22771 = kk_integer_add(i,(kk_integer_from_small(1)),kk_context()); /*int*/;
  kk_std_core__list x_23388 = kk_std_core__lift22739_map_indexed(f, yy, i0_22771, _ctx); /*list<18282>*/;
  if (kk_yielding(kk_context())) {
    kk_std_core__list_drop(x_23388, _ctx);
    kk_box_t _x26504 = kk_std_core_hnd_yield_extend(kk_std_core__new_mlift22896_op_fun26505(_y_1940, _ctx), _ctx); /*3991*/
    return kk_std_core__list_unbox(_x26504, _ctx);
  }
  return kk_std_core__mlift22895_op(_y_1940, x_23388, _ctx);
}
 
// lift


// lift anonymous function
struct kk_std_core__lift22739_map_indexed_fun26512__t {
  struct kk_function_s _base;
  kk_function_t f0;
  kk_integer_t i0;
  kk_std_core__list yy0;
};
static kk_box_t kk_std_core__lift22739_map_indexed_fun26512(kk_function_t _fself, kk_box_t _b_24604, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift22739_map_indexed_fun26512(kk_function_t f0, kk_integer_t i0, kk_std_core__list yy0, kk_context_t* _ctx) {
  struct kk_std_core__lift22739_map_indexed_fun26512__t* _self = kk_function_alloc_as(struct kk_std_core__lift22739_map_indexed_fun26512__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift22739_map_indexed_fun26512, kk_context());
  _self->f0 = f0;
  _self->i0 = i0;
  _self->yy0 = yy0;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift22739_map_indexed_fun26512(kk_function_t _fself, kk_box_t _b_24604, kk_context_t* _ctx) {
  struct kk_std_core__lift22739_map_indexed_fun26512__t* _self = kk_function_as(struct kk_std_core__lift22739_map_indexed_fun26512__t*, _fself);
  kk_function_t f0 = _self->f0; /* (idx : int, value : 18281) -> 18283 18282 */
  kk_integer_t i0 = _self->i0; /* int */
  kk_std_core__list yy0 = _self->yy0; /* list<18281> */
  kk_drop_match(_self, {kk_function_dup(f0);kk_integer_dup(i0);kk_std_core__list_dup(yy0);}, {}, _ctx)
  kk_std_core__list _x26513 = kk_std_core__mlift22896_op(f0, i0, yy0, _b_24604, _ctx); /*list<18282>*/
  return kk_std_core__list_box(_x26513, _ctx);
}


// lift anonymous function
struct kk_std_core__lift22739_map_indexed_fun26515__t {
  struct kk_function_s _base;
  kk_box_t x_23390;
};
static kk_box_t kk_std_core__lift22739_map_indexed_fun26515(kk_function_t _fself, kk_box_t _b_24606, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift22739_map_indexed_fun26515(kk_box_t x_23390, kk_context_t* _ctx) {
  struct kk_std_core__lift22739_map_indexed_fun26515__t* _self = kk_function_alloc_as(struct kk_std_core__lift22739_map_indexed_fun26515__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift22739_map_indexed_fun26515, kk_context());
  _self->x_23390 = x_23390;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift22739_map_indexed_fun26515(kk_function_t _fself, kk_box_t _b_24606, kk_context_t* _ctx) {
  struct kk_std_core__lift22739_map_indexed_fun26515__t* _self = kk_function_as(struct kk_std_core__lift22739_map_indexed_fun26515__t*, _fself);
  kk_box_t x_23390 = _self->x_23390; /* 18282 */
  kk_drop_match(_self, {kk_box_dup(x_23390);}, {}, _ctx)
  kk_std_core__list _x26516;
  kk_std_core__list _x26517 = kk_std_core__list_unbox(_b_24606, _ctx); /*list<18282>*/
  _x26516 = kk_std_core__mlift22895_op(x_23390, _x26517, _ctx); /*list<18282>*/
  return kk_std_core__list_box(_x26516, _ctx);
}

kk_std_core__list kk_std_core__lift22739_map_indexed(kk_function_t f0, kk_std_core__list ys, kk_integer_t i0, kk_context_t* _ctx) { /* forall<a,b,e> (f : (idx : int, value : a) -> e b, ys : list<a>, i : int) -> e list<b> */ 
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con26508 = kk_std_core__as_Cons(ys);
    kk_box_t y = _con26508->head;
    kk_std_core__list yy0 = _con26508->tail;
    kk_reuse_t _ru_25129 = kk_reuse_null; /*reuse*/;
    if (kk_std_core__list_is_unique(ys)) {
      _ru_25129 = (kk_std_core__list_reuse(ys));
    }
    else {
      kk_box_dup(y);
      kk_std_core__list_dup(yy0);
      kk_std_core__list_decref(ys, _ctx);
      _ru_25129 = kk_reuse_null;
    }
    kk_box_t x_23390;
    kk_function_t _x26510 = kk_function_dup(f0); /*(idx : int, value : 18281) -> 18283 18282*/
    kk_integer_t _x26509 = kk_integer_dup(i0); /*int*/
    x_23390 = kk_function_call(kk_box_t, (kk_function_t, kk_integer_t, kk_box_t, kk_context_t*), _x26510, (_x26510, _x26509, y, _ctx)); /*18282*/
    if (kk_yielding(kk_context())) {
      kk_reuse_drop(_ru_25129, _ctx);
      kk_box_drop(x_23390, _ctx);
      kk_box_t _x26511 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift22739_map_indexed_fun26512(f0, i0, yy0, _ctx), _ctx); /*3991*/
      return kk_std_core__list_unbox(_x26511, _ctx);
    }
    kk_integer_t i0_227710 = kk_integer_add(i0,(kk_integer_from_small(1)),kk_context()); /*int*/;
    kk_std_core__list x0_23394 = kk_std_core__lift22739_map_indexed(f0, yy0, i0_227710, _ctx); /*list<18282>*/;
    if (kk_yielding(kk_context())) {
      kk_reuse_drop(_ru_25129, _ctx);
      kk_std_core__list_drop(x0_23394, _ctx);
      kk_box_t _x26514 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift22739_map_indexed_fun26515(x_23390, _ctx), _ctx); /*3991*/
      return kk_std_core__list_unbox(_x26514, _ctx);
    }
    return kk_std_core__new_Cons(_ru_25129, x_23390, x0_23394, _ctx);
  }
  kk_function_drop(f0, _ctx);
  kk_integer_drop(i0, _ctx);
  return kk_std_core__new_Nil(_ctx);
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift22897_op(kk_box_t _y_198, kk_std_core__list _y_199, kk_context_t* _ctx) { /* forall<a,e> (a, list<a>) -> e list<a> */ 
  return kk_std_core__new_Cons(kk_reuse_null, _y_198, _y_199, _ctx);
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift22898_op_fun26519__t {
  struct kk_function_s _base;
  kk_box_t _y_1980;
};
static kk_box_t kk_std_core__mlift22898_op_fun26519(kk_function_t _fself, kk_box_t _b_24612, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift22898_op_fun26519(kk_box_t _y_1980, kk_context_t* _ctx) {
  struct kk_std_core__mlift22898_op_fun26519__t* _self = kk_function_alloc_as(struct kk_std_core__mlift22898_op_fun26519__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift22898_op_fun26519, kk_context());
  _self->_y_1980 = _y_1980;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift22898_op_fun26519(kk_function_t _fself, kk_box_t _b_24612, kk_context_t* _ctx) {
  struct kk_std_core__mlift22898_op_fun26519__t* _self = kk_function_as(struct kk_std_core__mlift22898_op_fun26519__t*, _fself);
  kk_box_t _y_1980 = _self->_y_1980; /* 18365 */
  kk_drop_match(_self, {kk_box_dup(_y_1980);}, {}, _ctx)
  kk_std_core__list _x26520;
  kk_std_core__list _x26521 = kk_std_core__list_unbox(_b_24612, _ctx); /*list<18365>*/
  _x26520 = kk_std_core__mlift22897_op(_y_1980, _x26521, _ctx); /*list<18365>*/
  return kk_std_core__list_box(_x26520, _ctx);
}

kk_std_core__list kk_std_core__mlift22898_op(kk_function_t f, kk_integer_t i, kk_std_core__list yy, kk_box_t _y_1980, kk_context_t* _ctx) { /* forall<a,b,e> (f : (idx : int, value : a, rest : list<a>) -> e b, i : int, yy : list<a>, b) -> e list<b> */ 
  kk_integer_t i0_22775 = kk_integer_add(i,(kk_integer_from_small(1)),kk_context()); /*int*/;
  kk_std_core__list x_23398 = kk_std_core__lift22740_map_indexed_peek(f, yy, i0_22775, _ctx); /*list<18365>*/;
  if (kk_yielding(kk_context())) {
    kk_std_core__list_drop(x_23398, _ctx);
    kk_box_t _x26518 = kk_std_core_hnd_yield_extend(kk_std_core__new_mlift22898_op_fun26519(_y_1980, _ctx), _ctx); /*3991*/
    return kk_std_core__list_unbox(_x26518, _ctx);
  }
  return kk_std_core__mlift22897_op(_y_1980, x_23398, _ctx);
}
 
// lift


// lift anonymous function
struct kk_std_core__lift22740_map_indexed_peek_fun26527__t {
  struct kk_function_s _base;
  kk_function_t f0;
  kk_integer_t i0;
  kk_std_core__list yy0;
};
static kk_box_t kk_std_core__lift22740_map_indexed_peek_fun26527(kk_function_t _fself, kk_box_t _b_24616, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift22740_map_indexed_peek_fun26527(kk_function_t f0, kk_integer_t i0, kk_std_core__list yy0, kk_context_t* _ctx) {
  struct kk_std_core__lift22740_map_indexed_peek_fun26527__t* _self = kk_function_alloc_as(struct kk_std_core__lift22740_map_indexed_peek_fun26527__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift22740_map_indexed_peek_fun26527, kk_context());
  _self->f0 = f0;
  _self->i0 = i0;
  _self->yy0 = yy0;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift22740_map_indexed_peek_fun26527(kk_function_t _fself, kk_box_t _b_24616, kk_context_t* _ctx) {
  struct kk_std_core__lift22740_map_indexed_peek_fun26527__t* _self = kk_function_as(struct kk_std_core__lift22740_map_indexed_peek_fun26527__t*, _fself);
  kk_function_t f0 = _self->f0; /* (idx : int, value : 18364, rest : list<18364>) -> 18366 18365 */
  kk_integer_t i0 = _self->i0; /* int */
  kk_std_core__list yy0 = _self->yy0; /* list<18364> */
  kk_drop_match(_self, {kk_function_dup(f0);kk_integer_dup(i0);kk_std_core__list_dup(yy0);}, {}, _ctx)
  kk_std_core__list _x26528 = kk_std_core__mlift22898_op(f0, i0, yy0, _b_24616, _ctx); /*list<18365>*/
  return kk_std_core__list_box(_x26528, _ctx);
}


// lift anonymous function
struct kk_std_core__lift22740_map_indexed_peek_fun26530__t {
  struct kk_function_s _base;
  kk_box_t x_23400;
};
static kk_box_t kk_std_core__lift22740_map_indexed_peek_fun26530(kk_function_t _fself, kk_box_t _b_24618, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift22740_map_indexed_peek_fun26530(kk_box_t x_23400, kk_context_t* _ctx) {
  struct kk_std_core__lift22740_map_indexed_peek_fun26530__t* _self = kk_function_alloc_as(struct kk_std_core__lift22740_map_indexed_peek_fun26530__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift22740_map_indexed_peek_fun26530, kk_context());
  _self->x_23400 = x_23400;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift22740_map_indexed_peek_fun26530(kk_function_t _fself, kk_box_t _b_24618, kk_context_t* _ctx) {
  struct kk_std_core__lift22740_map_indexed_peek_fun26530__t* _self = kk_function_as(struct kk_std_core__lift22740_map_indexed_peek_fun26530__t*, _fself);
  kk_box_t x_23400 = _self->x_23400; /* 18365 */
  kk_drop_match(_self, {kk_box_dup(x_23400);}, {}, _ctx)
  kk_std_core__list _x26531;
  kk_std_core__list _x26532 = kk_std_core__list_unbox(_b_24618, _ctx); /*list<18365>*/
  _x26531 = kk_std_core__mlift22897_op(x_23400, _x26532, _ctx); /*list<18365>*/
  return kk_std_core__list_box(_x26531, _ctx);
}

kk_std_core__list kk_std_core__lift22740_map_indexed_peek(kk_function_t f0, kk_std_core__list ys, kk_integer_t i0, kk_context_t* _ctx) { /* forall<a,b,e> (f : (idx : int, value : a, rest : list<a>) -> e b, ys : list<a>, i : int) -> e list<b> */ 
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con26522 = kk_std_core__as_Cons(ys);
    kk_box_t y = _con26522->head;
    kk_std_core__list yy0 = _con26522->tail;
    kk_reuse_t _ru_25130 = kk_reuse_null; /*reuse*/;
    if (kk_std_core__list_is_unique(ys)) {
      _ru_25130 = (kk_std_core__list_reuse(ys));
    }
    else {
      kk_box_dup(y);
      kk_std_core__list_dup(yy0);
      kk_std_core__list_decref(ys, _ctx);
      _ru_25130 = kk_reuse_null;
    }
    kk_box_t x_23400;
    kk_function_t _x26525 = kk_function_dup(f0); /*(idx : int, value : 18364, rest : list<18364>) -> 18366 18365*/
    kk_integer_t _x26523 = kk_integer_dup(i0); /*int*/
    kk_std_core__list _x26524 = kk_std_core__list_dup(yy0); /*list<18364>*/
    x_23400 = kk_function_call(kk_box_t, (kk_function_t, kk_integer_t, kk_box_t, kk_std_core__list, kk_context_t*), _x26525, (_x26525, _x26523, y, _x26524, _ctx)); /*18365*/
    if (kk_yielding(kk_context())) {
      kk_reuse_drop(_ru_25130, _ctx);
      kk_box_drop(x_23400, _ctx);
      kk_box_t _x26526 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift22740_map_indexed_peek_fun26527(f0, i0, yy0, _ctx), _ctx); /*3991*/
      return kk_std_core__list_unbox(_x26526, _ctx);
    }
    kk_integer_t i0_227750 = kk_integer_add(i0,(kk_integer_from_small(1)),kk_context()); /*int*/;
    kk_std_core__list x0_23404 = kk_std_core__lift22740_map_indexed_peek(f0, yy0, i0_227750, _ctx); /*list<18365>*/;
    if (kk_yielding(kk_context())) {
      kk_reuse_drop(_ru_25130, _ctx);
      kk_std_core__list_drop(x0_23404, _ctx);
      kk_box_t _x26529 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift22740_map_indexed_peek_fun26530(x_23400, _ctx), _ctx); /*3991*/
      return kk_std_core__list_unbox(_x26529, _ctx);
    }
    return kk_std_core__new_Cons(_ru_25130, x_23400, x0_23404, _ctx);
  }
  kk_function_drop(f0, _ctx);
  kk_integer_drop(i0, _ctx);
  return kk_std_core__new_Nil(_ctx);
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift22899_op(kk_std_core_types__ctail _acc, kk_function_t action, kk_std_core__list xx, kk_std_core_types__maybe _y_202, kk_context_t* _ctx) { /* forall<a,b,e> (ctail<list<b>>, action : (a) -> e maybe<b>, xx : list<a>, maybe<b>) -> e list<b> */ 
  if (kk_std_core_types__is_Just(_y_202)) {
    kk_box_t y = _y_202._cons.Just.value;
    kk_std_core__list _ctail_22828 = kk_std_core__list_hole(); /*list<18488>*/;
    kk_std_core__list _ctail_22829 = kk_std_core__new_Cons(kk_reuse_null, y, _ctail_22828, _ctx); /*list<18488>*/;
    kk_std_core_types__ctail _x26533;
    kk_box_t* field_23410 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_22829)->tail)); /*cfield<list<18488>>*/;
    if (kk_std_core_types__is_CTail(_acc)) {
      kk_box_t _box_x24629 = _acc._cons.CTail._field1;
      kk_box_t* last0 = _acc._cons.CTail._field2;
      kk_std_core__list head0 = kk_std_core__list_unbox(_box_x24629, NULL);
      kk_unit_t __ = kk_Unit;
      *(last0) = kk_std_core__list_box(_ctail_22829, _ctx);
      _x26533 = kk_std_core_types__new_CTail(kk_std_core__list_box(head0, _ctx), field_23410, _ctx); /*ctail<58>*/
      goto _match26534;
    }
    _x26533 = kk_std_core_types__new_CTail(kk_std_core__list_box(_ctail_22829, _ctx), field_23410, _ctx); /*ctail<58>*/
    _match26534: ;
    return kk_std_core__ctail_map_while(xx, action, _x26533, _ctx);
  }
  kk_function_drop(action, _ctx);
  kk_std_core__list_drop(xx, _ctx);
  if (kk_std_core_types__is_CTail(_acc)) {
    kk_box_t _box_x24638 = _acc._cons.CTail._field1;
    kk_box_t* last00 = _acc._cons.CTail._field2;
    kk_std_core__list head00 = kk_std_core__list_unbox(_box_x24638, NULL);
    kk_unit_t __0 = kk_Unit;
    *(last00) = kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx);
    return head00;
  }
  return kk_std_core__new_Nil(_ctx);
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift22900_op_fun26537__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t y0;
};
static kk_std_core__list kk_std_core__mlift22900_op_fun26537(kk_function_t _fself, kk_std_core__list _ctail_22831, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift22900_op_fun26537(kk_function_t _accm, kk_box_t y0, kk_context_t* _ctx) {
  struct kk_std_core__mlift22900_op_fun26537__t* _self = kk_function_alloc_as(struct kk_std_core__mlift22900_op_fun26537__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift22900_op_fun26537, kk_context());
  _self->_accm = _accm;
  _self->y0 = y0;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift22900_op_fun26537(kk_function_t _fself, kk_std_core__list _ctail_22831, kk_context_t* _ctx) {
  struct kk_std_core__mlift22900_op_fun26537__t* _self = kk_function_as(struct kk_std_core__mlift22900_op_fun26537__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<18488>) -> list<18488> */
  kk_box_t y0 = _self->y0; /* 18488 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(y0);}, {}, _ctx)
  kk_std_core__list _x26538 = kk_std_core__new_Cons(kk_reuse_null, y0, _ctail_22831, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x26538, _ctx));
}

kk_std_core__list kk_std_core__mlift22900_op(kk_function_t _accm, kk_function_t action0, kk_std_core__list xx0, kk_std_core_types__maybe _y_206, kk_context_t* _ctx) { /* forall<a,b,e> ((list<b>) -> list<b>, action : (a) -> e maybe<b>, xx : list<a>, maybe<b>) -> e list<b> */ 
  if (kk_std_core_types__is_Just(_y_206)) {
    kk_box_t y0 = _y_206._cons.Just.value;
    return kk_std_core__ctailm_map_while(xx0, action0, kk_std_core__new_mlift22900_op_fun26537(_accm, y0, _ctx), _ctx);
  }
  kk_function_drop(action0, _ctx);
  kk_std_core__list_drop(xx0, _ctx);
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, kk_std_core__new_Nil(_ctx), _ctx));
}
 
// Invoke `action` on each element of a list while `action` returns `Just`


// lift anonymous function
struct kk_std_core__ctail_map_while_fun26543__t {
  struct kk_function_s _base;
  kk_function_t action1;
  kk_std_core__list xx1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_map_while_fun26543(kk_function_t _fself, kk_box_t _b_24653, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_map_while_fun26543(kk_function_t action1, kk_std_core__list xx1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_map_while_fun26543__t* _self = kk_function_alloc_as(struct kk_std_core__ctail_map_while_fun26543__t, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_map_while_fun26543, kk_context());
  _self->action1 = action1;
  _self->xx1 = xx1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_map_while_fun26543(kk_function_t _fself, kk_box_t _b_24653, kk_context_t* _ctx) {
  struct kk_std_core__ctail_map_while_fun26543__t* _self = kk_function_as(struct kk_std_core__ctail_map_while_fun26543__t*, _fself);
  kk_function_t action1 = _self->action1; /* (18487) -> 18489 maybe<18488> */
  kk_std_core__list xx1 = _self->xx1; /* list<18487> */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<18488>> */
  kk_drop_match(_self, {kk_function_dup(action1);kk_std_core__list_dup(xx1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  kk_std_core__list _x26544;
  kk_std_core_types__maybe _x26545 = kk_std_core_types__maybe_unbox(_b_24653, _ctx); /*maybe<18488>*/
  _x26544 = kk_std_core__mlift22899_op(_acc0, action1, xx1, _x26545, _ctx); /*list<18488>*/
  return kk_std_core__list_box(_x26544, _ctx);
}

kk_std_core__list kk_std_core__ctail_map_while(kk_std_core__list xs, kk_function_t action1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, action : (a) -> e maybe<b>, ctail<list<b>>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Nil(xs)) {
    kk_function_drop(action1, _ctx);
    if (kk_std_core_types__is_CTail(_acc0)) {
      kk_box_t _box_x24647 = _acc0._cons.CTail._field1;
      kk_box_t* last2 = _acc0._cons.CTail._field2;
      kk_std_core__list head1 = kk_std_core__list_unbox(_box_x24647, NULL);
      kk_unit_t __1 = kk_Unit;
      *(last2) = kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx);
      return head1;
    }
    return kk_std_core__new_Nil(_ctx);
  }
  struct kk_std_core_Cons* _con26540 = kk_std_core__as_Cons(xs);
  kk_box_t x = _con26540->head;
  kk_std_core__list xx1 = _con26540->tail;
  kk_reuse_t _ru_25131 = kk_reuse_null; /*reuse*/;
  if (kk_std_core__list_is_unique(xs)) {
    _ru_25131 = (kk_std_core__list_reuse(xs));
  }
  else {
    kk_box_dup(x);
    kk_std_core__list_dup(xx1);
    kk_std_core__list_decref(xs, _ctx);
    _ru_25131 = kk_reuse_null;
  }
  kk_std_core_types__maybe x0_23415;
  kk_function_t _x26541 = kk_function_dup(action1); /*(18487) -> 18489 maybe<18488>*/
  x0_23415 = kk_function_call(kk_std_core_types__maybe, (kk_function_t, kk_box_t, kk_context_t*), _x26541, (_x26541, x, _ctx)); /*maybe<18488>*/
  if (kk_yielding(kk_context())) {
    kk_reuse_drop(_ru_25131, _ctx);
    kk_std_core_types__maybe_drop(x0_23415, _ctx);
    kk_box_t _x26542 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_map_while_fun26543(action1, xx1, _acc0, _ctx), _ctx); /*3991*/
    return kk_std_core__list_unbox(_x26542, _ctx);
  }
  if (kk_std_core_types__is_Just(x0_23415)) {
    kk_box_t y1 = x0_23415._cons.Just.value;
    kk_std_core__list _ctail_228280 = kk_std_core__list_hole(); /*list<18488>*/;
    kk_std_core__list _ctail_228290 = kk_std_core__new_Cons(_ru_25131, y1, _ctail_228280, _ctx); /*list<18488>*/;
    { // tailcall
      kk_std_core_types__ctail _x26546;
      kk_box_t* field_23421 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_228290)->tail)); /*cfield<list<18488>>*/;
      if (kk_std_core_types__is_CTail(_acc0)) {
        kk_box_t _box_x24660 = _acc0._cons.CTail._field1;
        kk_box_t* last01 = _acc0._cons.CTail._field2;
        kk_std_core__list head01 = kk_std_core__list_unbox(_box_x24660, NULL);
        kk_unit_t __00 = kk_Unit;
        *(last01) = kk_std_core__list_box(_ctail_228290, _ctx);
        _x26546 = kk_std_core_types__new_CTail(kk_std_core__list_box(head01, _ctx), field_23421, _ctx); /*ctail<58>*/
        goto _match26547;
      }
      _x26546 = kk_std_core_types__new_CTail(kk_std_core__list_box(_ctail_228290, _ctx), field_23421, _ctx); /*ctail<58>*/
      _match26547: ;
      xs = xx1;
      _acc0 = _x26546;
      goto kk__tailcall;
    }
  }
  kk_reuse_drop(_ru_25131, _ctx);
  kk_function_drop(action1, _ctx);
  kk_std_core__list_drop(xx1, _ctx);
  if (kk_std_core_types__is_CTail(_acc0)) {
    kk_box_t _box_x24669 = _acc0._cons.CTail._field1;
    kk_box_t* last10 = _acc0._cons.CTail._field2;
    kk_std_core__list head10 = kk_std_core__list_unbox(_box_x24669, NULL);
    kk_unit_t __10 = kk_Unit;
    *(last10) = kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx);
    return head10;
  }
  return kk_std_core__new_Nil(_ctx);
}
 
// Invoke `action` on each element of a list while `action` returns `Just`


// lift anonymous function
struct kk_std_core__ctailm_map_while_fun26553__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t action2;
  kk_std_core__list xx2;
};
static kk_box_t kk_std_core__ctailm_map_while_fun26553(kk_function_t _fself, kk_box_t _b_24681, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_map_while_fun26553(kk_function_t _accm0, kk_function_t action2, kk_std_core__list xx2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_map_while_fun26553__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_map_while_fun26553__t, 4, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_map_while_fun26553, kk_context());
  _self->_accm0 = _accm0;
  _self->action2 = action2;
  _self->xx2 = xx2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_map_while_fun26553(kk_function_t _fself, kk_box_t _b_24681, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_map_while_fun26553__t* _self = kk_function_as(struct kk_std_core__ctailm_map_while_fun26553__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<18488>) -> list<18488> */
  kk_function_t action2 = _self->action2; /* (18487) -> 18489 maybe<18488> */
  kk_std_core__list xx2 = _self->xx2; /* list<18487> */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(action2);kk_std_core__list_dup(xx2);}, {}, _ctx)
  kk_std_core__list _x26554;
  kk_std_core_types__maybe _x26555 = kk_std_core_types__maybe_unbox(_b_24681, _ctx); /*maybe<18488>*/
  _x26554 = kk_std_core__mlift22900_op(_accm0, action2, xx2, _x26555, _ctx); /*list<18488>*/
  return kk_std_core__list_box(_x26554, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_map_while_fun26557__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t y2;
};
static kk_std_core__list kk_std_core__ctailm_map_while_fun26557(kk_function_t _fself, kk_std_core__list _ctail_228310, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_map_while_fun26557(kk_function_t _accm0, kk_box_t y2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_map_while_fun26557__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_map_while_fun26557__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_map_while_fun26557, kk_context());
  _self->_accm0 = _accm0;
  _self->y2 = y2;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_map_while_fun26557(kk_function_t _fself, kk_std_core__list _ctail_228310, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_map_while_fun26557__t* _self = kk_function_as(struct kk_std_core__ctailm_map_while_fun26557__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<18488>) -> list<18488> */
  kk_box_t y2 = _self->y2; /* 18488 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(y2);}, {}, _ctx)
  kk_std_core__list _x26558 = kk_std_core__new_Cons(kk_reuse_null, y2, _ctail_228310, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x26558, _ctx));
}

kk_std_core__list kk_std_core__ctailm_map_while(kk_std_core__list xs0, kk_function_t action2, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, action : (a) -> e maybe<b>, (list<b>) -> list<b>) -> e list<b> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Nil(xs0)) {
    kk_function_drop(action2, _ctx);
    return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, kk_std_core__new_Nil(_ctx), _ctx));
  }
  struct kk_std_core_Cons* _con26550 = kk_std_core__as_Cons(xs0);
  kk_box_t x0 = _con26550->head;
  kk_std_core__list xx2 = _con26550->tail;
  if (kk_std_core__list_is_unique(xs0)) {
    kk_std_core__list_free(xs0);
  }
  else {
    kk_box_dup(x0);
    kk_std_core__list_dup(xx2);
    kk_std_core__list_decref(xs0, _ctx);
  }
  kk_std_core_types__maybe x0_23424;
  kk_function_t _x26551 = kk_function_dup(action2); /*(18487) -> 18489 maybe<18488>*/
  x0_23424 = kk_function_call(kk_std_core_types__maybe, (kk_function_t, kk_box_t, kk_context_t*), _x26551, (_x26551, x0, _ctx)); /*maybe<18488>*/
  if (kk_yielding(kk_context())) {
    kk_std_core_types__maybe_drop(x0_23424, _ctx);
    kk_box_t _x26552 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_map_while_fun26553(_accm0, action2, xx2, _ctx), _ctx); /*3991*/
    return kk_std_core__list_unbox(_x26552, _ctx);
  }
  if (kk_std_core_types__is_Just(x0_23424)) {
    kk_box_t y2 = x0_23424._cons.Just.value;
    { // tailcall
      kk_function_t _x26556 = kk_std_core__new_ctailm_map_while_fun26557(_accm0, y2, _ctx); /*(list<18488>) -> list<18488>*/
      xs0 = xx2;
      _accm0 = _x26556;
      goto kk__tailcall;
    }
  }
  kk_function_drop(action2, _ctx);
  kk_std_core__list_drop(xx2, _ctx);
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, kk_std_core__new_Nil(_ctx), _ctx));
}
 
// Invoke `action` on each element of a list while `action` returns `Just`


// lift anonymous function
struct kk_std_core_map_while_fun26559__t {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core_map_while_fun26559(kk_function_t _fself, kk_std_core__list _ctail_22830, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_map_while_fun26559(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_map_while_fun26559, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core_map_while_fun26559(kk_function_t _fself, kk_std_core__list _ctail_22830, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  return _ctail_22830;
}

kk_std_core__list kk_std_core_map_while(kk_std_core__list xs1, kk_function_t action3, kk_context_t* _ctx) { /* forall<a,b,e> (xs : list<a>, action : (a) -> e maybe<b>) -> e list<b> */ 
  bool _match_25199 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_25199) {
    return kk_std_core__ctail_map_while(xs1, action3, kk_std_core_types__new_CTailNil(_ctx), _ctx);
  }
  return kk_std_core__ctailm_map_while(xs1, action3, kk_std_core_new_map_while_fun26559(_ctx), _ctx);
}
 
// Return the maximum of two integers

kk_integer_t kk_std_core_max(kk_integer_t i, kk_integer_t j, kk_context_t* _ctx) { /* (i : int, j : int) -> int */ 
  bool _match_25198;
  kk_integer_t _x26560 = kk_integer_dup(i); /*int*/
  kk_integer_t _x26561 = kk_integer_dup(j); /*int*/
  _match_25198 = kk_integer_gte(_x26560,_x26561,kk_context()); /*bool*/
  if (_match_25198) {
    kk_integer_drop(j, _ctx);
    return i;
  }
  kk_integer_drop(i, _ctx);
  return j;
}
 
// Returns the largest element of a list of integers (or `default` (=`0`) for the empty list)


// lift anonymous function
struct kk_std_core_maximum_fun26566__t {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_maximum_fun26566(kk_function_t _fself, kk_box_t _b_24689, kk_box_t _b_24690, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_maximum_fun26566(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_maximum_fun26566, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_maximum_fun26566(kk_function_t _fself, kk_box_t _b_24689, kk_box_t _b_24690, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  kk_integer_t _x26567;
  kk_integer_t _x26568 = kk_integer_unbox(_b_24689); /*int*/
  kk_integer_t _x26569 = kk_integer_unbox(_b_24690); /*int*/
  _x26567 = kk_std_core_max(_x26568, _x26569, _ctx); /*int*/
  return kk_integer_box(_x26567);
}

kk_integer_t kk_std_core_maximum(kk_std_core__list xs, kk_std_core_types__optional default0, kk_context_t* _ctx) { /* (xs : list<int>, default : optional<int>) -> int */ 
  if (kk_std_core__is_Nil(xs)) {
    if (kk_std_core_types__is_Optional(default0)) {
      kk_box_t _box_x24684 = default0._cons.Optional.value;
      kk_integer_t _default_18561 = kk_integer_unbox(_box_x24684);
      return _default_18561;
    }
    return kk_integer_from_small(0);
  }
  struct kk_std_core_Cons* _con26563 = kk_std_core__as_Cons(xs);
  kk_box_t _box_x24685 = _con26563->head;
  kk_std_core__list xx = _con26563->tail;
  kk_integer_t x = kk_integer_unbox(_box_x24685);
  if (kk_std_core__list_is_unique(xs)) {
    kk_std_core__list_free(xs);
  }
  else {
    kk_integer_dup(x);
    kk_std_core__list_dup(xx);
    kk_std_core__list_decref(xs, _ctx);
  }
  kk_std_core_types__optional_drop(default0, _ctx);
  kk_box_t _x26565 = kk_std_core_foldl(xx, kk_integer_box(x), kk_std_core_new_maximum_fun26566(_ctx), _ctx); /*15726*/
  return kk_integer_unbox(_x26565);
}
 
// Returns the largest element of a list of doubles (or `0` for the empty list)


// lift anonymous function
struct kk_std_core_maximum_fun26573__t_1 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_maximum_fun26573_1(kk_function_t _fself, kk_box_t _b_24698, kk_box_t _b_24699, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_maximum_fun26573_1(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_maximum_fun26573_1, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_maximum_fun26573_1(kk_function_t _fself, kk_box_t _b_24698, kk_box_t _b_24699, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  double _x26574;
  double _x26575 = kk_double_unbox(_b_24698, _ctx); /*double*/
  double _x26576 = kk_double_unbox(_b_24699, _ctx); /*double*/
  _x26574 = kk_std_core_max_1(_x26575, _x26576, _ctx); /*double*/
  return kk_double_box(_x26574, _ctx);
}

double kk_std_core_maximum_1(kk_std_core__list xs, kk_context_t* _ctx) { /* (xs : list<double>) -> double */ 
  if (kk_std_core__is_Nil(xs)) {
    return 0x0p+0;
  }
  struct kk_std_core_Cons* _con26570 = kk_std_core__as_Cons(xs);
  kk_box_t _box_x24694 = _con26570->head;
  kk_std_core__list xx = _con26570->tail;
  double x = kk_double_unbox(_box_x24694, NULL);
  if (kk_std_core__list_is_unique(xs)) {
    kk_box_drop(_box_x24694, _ctx);
    kk_std_core__list_free(xs);
  }
  else {
    kk_std_core__list_dup(xx);
    kk_std_core__list_decref(xs, _ctx);
  }
  kk_box_t _x26572 = kk_std_core_foldl(xx, kk_double_box(x, _ctx), kk_std_core_new_maximum_fun26573_1(_ctx), _ctx); /*15726*/
  return kk_double_unbox(_x26572, _ctx);
}
 
// Convert a list to a `:maybe` type, using `Nothing` for an empty list, and otherwise `Just` on the head element.
// Note: this is just `head`.

kk_std_core_types__maybe kk_std_core_maybe_3(kk_std_core__list xs, kk_context_t* _ctx) { /* forall<a> (xs : list<a>) -> maybe<a> */ 
  if (kk_std_core__is_Nil(xs)) {
    return kk_std_core_types__new_Nothing(_ctx);
  }
  struct kk_std_core_Cons* _con26579 = kk_std_core__as_Cons(xs);
  kk_box_t x = _con26579->head;
  kk_std_core__list _pat1 = _con26579->tail;
  if (kk_std_core__list_is_unique(xs)) {
    kk_std_core__list_drop(_pat1, _ctx);
    kk_std_core__list_free(xs);
  }
  else {
    kk_box_dup(x);
    kk_std_core__list_decref(xs, _ctx);
  }
  return kk_std_core_types__new_Just(x, _ctx);
}
 
// Transform an integer to a maybe type, using `Nothing` for `0`

kk_std_core_types__maybe kk_std_core_maybe_5(kk_integer_t i, kk_context_t* _ctx) { /* (i : int) -> maybe<int> */ 
  bool _match_25196;
  kk_integer_t _x26580 = kk_integer_dup(i); /*int*/
  _match_25196 = kk_integer_eq(_x26580,(kk_integer_from_small(0)),kk_context()); /*bool*/
  if (_match_25196) {
    kk_integer_drop(i, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
  return kk_std_core_types__new_Just(kk_integer_box(i), _ctx);
}
 
// Transform a string to a maybe type, using `Nothing` for an empty string

kk_std_core_types__maybe kk_std_core_maybe_6(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> maybe<string> */ 
  bool _match_25195;
  kk_string_t _x26581 = kk_string_dup(s); /*string*/
  _match_25195 = kk_std_core_is_empty_2(_x26581, _ctx); /*bool*/
  if (_match_25195) {
    kk_string_drop(s, _ctx);
    return kk_std_core_types__new_Nothing(_ctx);
  }
  return kk_std_core_types__new_Just(kk_string_box(s), _ctx);
}
 
// Transform a `:null` type to a `:maybe` type. Note that it is not
// always the case that `id(x) == maybe(null(x))` (e.g. when `x = Just(Nothing)`).

kk_std_core_types__maybe kk_std_core_maybe_8(kk_std_core__null n, kk_context_t* _ctx) { /* forall<a> (n : null<a>) -> maybe<a> */ 
  return ((n).ptr == NULL ? kk_std_core_types__new_Nothing(kk_context()) : kk_std_core_types__new_Just(kk_datatype_box(n),kk_context()));
}
 
// Return the minimum of two integers

kk_integer_t kk_std_core_min(kk_integer_t i, kk_integer_t j, kk_context_t* _ctx) { /* (i : int, j : int) -> int */ 
  bool _match_25194;
  kk_integer_t _x26582 = kk_integer_dup(i); /*int*/
  kk_integer_t _x26583 = kk_integer_dup(j); /*int*/
  _match_25194 = kk_integer_lte(_x26582,_x26583,kk_context()); /*bool*/
  if (_match_25194) {
    kk_integer_drop(j, _ctx);
    return i;
  }
  kk_integer_drop(i, _ctx);
  return j;
}
 
// Returns the smallest element of a list of integers (or `default` (=`0`) for the empty list)


// lift anonymous function
struct kk_std_core_minimum_fun26588__t {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_minimum_fun26588(kk_function_t _fself, kk_box_t _b_24714, kk_box_t _b_24715, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_minimum_fun26588(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_minimum_fun26588, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_minimum_fun26588(kk_function_t _fself, kk_box_t _b_24714, kk_box_t _b_24715, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  kk_integer_t _x26589;
  kk_integer_t _x26590 = kk_integer_unbox(_b_24714); /*int*/
  kk_integer_t _x26591 = kk_integer_unbox(_b_24715); /*int*/
  _x26589 = kk_std_core_min(_x26590, _x26591, _ctx); /*int*/
  return kk_integer_box(_x26589);
}

kk_integer_t kk_std_core_minimum(kk_std_core__list xs, kk_std_core_types__optional default0, kk_context_t* _ctx) { /* (xs : list<int>, default : optional<int>) -> int */ 
  if (kk_std_core__is_Nil(xs)) {
    if (kk_std_core_types__is_Optional(default0)) {
      kk_box_t _box_x24709 = default0._cons.Optional.value;
      kk_integer_t _default_19162 = kk_integer_unbox(_box_x24709);
      return _default_19162;
    }
    return kk_integer_from_small(0);
  }
  struct kk_std_core_Cons* _con26585 = kk_std_core__as_Cons(xs);
  kk_box_t _box_x24710 = _con26585->head;
  kk_std_core__list xx = _con26585->tail;
  kk_integer_t x = kk_integer_unbox(_box_x24710);
  if (kk_std_core__list_is_unique(xs)) {
    kk_std_core__list_free(xs);
  }
  else {
    kk_integer_dup(x);
    kk_std_core__list_dup(xx);
    kk_std_core__list_decref(xs, _ctx);
  }
  kk_std_core_types__optional_drop(default0, _ctx);
  kk_box_t _x26587 = kk_std_core_foldl(xx, kk_integer_box(x), kk_std_core_new_minimum_fun26588(_ctx), _ctx); /*15726*/
  return kk_integer_unbox(_x26587);
}
 
// Returns the smallest element of a list of doubles (or `0` for the empty list)


// lift anonymous function
struct kk_std_core_minimum_fun26595__t_1 {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_minimum_fun26595_1(kk_function_t _fself, kk_box_t _b_24723, kk_box_t _b_24724, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_minimum_fun26595_1(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_minimum_fun26595_1, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_minimum_fun26595_1(kk_function_t _fself, kk_box_t _b_24723, kk_box_t _b_24724, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  double _x26596;
  double _x26597 = kk_double_unbox(_b_24723, _ctx); /*double*/
  double _x26598 = kk_double_unbox(_b_24724, _ctx); /*double*/
  _x26596 = kk_std_core_min_1(_x26597, _x26598, _ctx); /*double*/
  return kk_double_box(_x26596, _ctx);
}

double kk_std_core_minimum_1(kk_std_core__list xs, kk_context_t* _ctx) { /* (xs : list<double>) -> double */ 
  if (kk_std_core__is_Nil(xs)) {
    return 0x0p+0;
  }
  struct kk_std_core_Cons* _con26592 = kk_std_core__as_Cons(xs);
  kk_box_t _box_x24719 = _con26592->head;
  kk_std_core__list xx = _con26592->tail;
  double x = kk_double_unbox(_box_x24719, NULL);
  if (kk_std_core__list_is_unique(xs)) {
    kk_box_drop(_box_x24719, _ctx);
    kk_std_core__list_free(xs);
  }
  else {
    kk_std_core__list_dup(xx);
    kk_std_core__list_decref(xs, _ctx);
  }
  kk_box_t _x26594 = kk_std_core_foldl(xx, kk_double_box(x, _ctx), kk_std_core_new_minimum_fun26595_1(_ctx), _ctx); /*15726*/
  return kk_double_unbox(_x26594, _ctx);
}
 
// Disable tracing completely.

kk_unit_t kk_std_core_notrace(kk_context_t* _ctx) { /* () -> (st<global>) () */ 
  kk_ref_t _b_24731_24729 = kk_ref_alloc((kk_bool_box(true)),kk_context()); /*ref<global,bool>*/;
  kk_ref_set(_b_24731_24729,(kk_bool_box(false)),kk_context()); return kk_Unit;
}
 
// Transform a `:maybe` type to a `:null` type (using `null` for `Nothing`).

kk_std_core__null kk_std_core_null(kk_std_core_types__maybe x, kk_context_t* _ctx) { /* forall<a> (x : maybe<a>) -> null<a> */ 
  return (kk_std_core_types__is_Nothing(x) ? kk_datatype_from_ptr(NULL) : kk_datatype_unbox((x)._cons.Just.value));
}

kk_std_core__null kk_std_core_null_const;
 
// Left-align a string to width `width`  using `fill`  (default is a space) to fill on the right.

kk_string_t kk_std_core_pad_right(kk_string_t s, kk_integer_t width, kk_std_core_types__optional fill, kk_context_t* _ctx) { /* (s : string, width : int, fill : optional<char>) -> string */ 
  size_t w = kk_std_core_size__t(width, _ctx); /*size_t*/;
  size_t n;
  kk_string_t _x26602 = kk_string_dup(s); /*string*/
  n = kk_string_len(_x26602,kk_context()); /*size_t*/
  bool _match_25192 = (w <= n); /*bool*/;
  if (_match_25192) {
    kk_std_core_types__optional_drop(fill, _ctx);
    return s;
  }
  kk_string_t _x26603;
  kk_string_t _x26604;
  kk_char_t _x26605;
  if (kk_std_core_types__is_Optional(fill)) {
    kk_box_t _box_x24734 = fill._cons.Optional.value;
    kk_char_t _fill_19741 = kk_char_unbox(_box_x24734, NULL);
    _x26605 = _fill_19741; /*char*/
    goto _match26606;
  }
  _x26605 = ' '; /*char*/
  _match26606: ;
  _x26604 = kk_std_core_string(_x26605, _ctx); /*string*/
  size_t _x26608 = (w - n); /*size_t*/
  _x26603 = kk_std_core_repeatz(_x26604, _x26608, _ctx); /*string*/
  return kk_std_core__lp__plus__6_rp_(s, _x26603, _ctx);
}
 
// Is `pre`  a prefix of `s`? If so, returns a slice
// of `s` following `pre` up to the end of `s`.

kk_std_core_types__maybe kk_std_core_starts_with(kk_string_t s, kk_string_t pre, kk_context_t* _ctx) { /* (s : string, pre : string) -> maybe<sslice> */ 
  bool _match_25191;
  kk_string_t _x26609 = kk_string_dup(s); /*string*/
  kk_string_t _x26610 = kk_string_dup(pre); /*string*/
  _match_25191 = kk_string_starts_with(_x26609,_x26610,kk_context()); /*bool*/
  if (_match_25191) {
    kk_std_core__sslice _b_24736_24735;
    kk_string_t _x26611 = kk_string_dup(s); /*string*/
    size_t _x26612;
    kk_string_t _x26613 = kk_string_dup(pre); /*string*/
    _x26612 = kk_string_len(_x26613,kk_context()); /*size_t*/
    size_t _x26614;
    size_t _x26615 = kk_string_len(s,kk_context()); /*size_t*/
    size_t _x26616 = kk_string_len(pre,kk_context()); /*size_t*/
    _x26614 = (_x26615 - _x26616); /*size_t*/
    _b_24736_24735 = kk_std_core__new_Sslice(_x26611, _x26612, _x26614, _ctx); /*sslice*/
    return kk_std_core_types__new_Just(kk_std_core__sslice_box(_b_24736_24735, _ctx), _ctx);
  }
  kk_string_drop(pre, _ctx);
  kk_string_drop(s, _ctx);
  return kk_std_core_types__new_Nothing(_ctx);
}
 
// Trim off a substring `sub` while `s` starts with that string.

kk_string_t kk_std_core_trim_left_1(kk_string_t s, kk_string_t sub, kk_context_t* _ctx) { /* (s : string, sub : string) -> string */ 
  kk__tailcall: ;
  bool _match_25189;
  kk_string_t _x26617 = kk_string_dup(sub); /*string*/
  _match_25189 = kk_std_core_is_empty_2(_x26617, _ctx); /*bool*/
  if (_match_25189) {
    kk_string_drop(sub, _ctx);
    return s;
  }
  kk_std_core_types__maybe _match_25190;
  kk_string_t _x26618 = kk_string_dup(s); /*string*/
  kk_string_t _x26619 = kk_string_dup(sub); /*string*/
  _match_25190 = kk_std_core_starts_with(_x26618, _x26619, _ctx); /*maybe<sslice>*/
  if (kk_std_core_types__is_Just(_match_25190)) {
    kk_box_t _box_x24737 = _match_25190._cons.Just.value;
    kk_std_core__sslice slice0 = kk_std_core__sslice_unbox(_box_x24737, NULL);
    kk_string_drop(s, _ctx);
    { // tailcall
      kk_string_t _x26621 = kk_std_core_string_3(slice0, _ctx); /*string*/
      s = _x26621;
      goto kk__tailcall;
    }
  }
  kk_string_drop(sub, _ctx);
  return s;
}
 
// Trim off a substring `sub` while `s` ends with that string.

kk_string_t kk_std_core_trim_right_1(kk_string_t s, kk_string_t sub, kk_context_t* _ctx) { /* (s : string, sub : string) -> string */ 
  kk__tailcall: ;
  bool _match_25187;
  kk_string_t _x26622 = kk_string_dup(sub); /*string*/
  _match_25187 = kk_std_core_is_empty_2(_x26622, _ctx); /*bool*/
  if (_match_25187) {
    kk_string_drop(sub, _ctx);
    return s;
  }
  kk_std_core_types__maybe _match_25188;
  kk_string_t _x26623 = kk_string_dup(s); /*string*/
  kk_string_t _x26624 = kk_string_dup(sub); /*string*/
  _match_25188 = kk_std_core_ends_with(_x26623, _x26624, _ctx); /*maybe<sslice>*/
  if (kk_std_core_types__is_Just(_match_25188)) {
    kk_box_t _box_x24738 = _match_25188._cons.Just.value;
    kk_std_core__sslice slice0 = kk_std_core__sslice_unbox(_box_x24738, NULL);
    kk_string_drop(s, _ctx);
    { // tailcall
      kk_string_t _x26626 = kk_std_core_string_3(slice0, _ctx); /*string*/
      s = _x26626;
      goto kk__tailcall;
    }
  }
  kk_string_drop(sub, _ctx);
  return s;
}
 
// Parse an integer using `parseInt`. If an illegal digit character is encountered the
// `default` value is returned. An empty string will also result in `default`.

kk_integer_t kk_std_core_parse_int_default(kk_string_t s, kk_std_core_types__optional default0, kk_std_core_types__optional hex, kk_context_t* _ctx) { /* (s : string, default : optional<int>, hex : optional<bool>) -> int */ 
  bool _match_25186;
  kk_string_t _x26632 = kk_string_dup(s); /*string*/
  _match_25186 = kk_std_core_is_empty_2(_x26632, _ctx); /*bool*/
  if (_match_25186) {
    kk_std_core_types__optional_drop(hex, _ctx);
    kk_string_drop(s, _ctx);
    if (kk_std_core_types__is_Optional(default0)) {
      kk_box_t _box_x24740 = default0._cons.Optional.value;
      kk_integer_t _default_20166 = kk_integer_unbox(_box_x24740);
      return _default_20166;
    }
    return kk_integer_from_small(0);
  }
  kk_std_core_types__maybe m_23428;
  kk_std_core_types__optional hex0_23431;
  kk_box_t _x26634;
  bool _x26635;
  if (kk_std_core_types__is_Optional(hex)) {
    kk_box_t _box_x24741 = hex._cons.Optional.value;
    bool _hex_20170 = kk_bool_unbox(_box_x24741);
    _x26635 = _hex_20170; /*bool*/
    goto _match26636;
  }
  _x26635 = false; /*bool*/
  _match26636: ;
  _x26634 = kk_bool_box(_x26635); /*108*/
  hex0_23431 = kk_std_core_types__new_Optional(_x26634, _ctx); /*optional<bool>*/
  kk_string_t _x26638 = kk_std_core_trim(s, _ctx); /*string*/
  bool _x26639;
  if (kk_std_core_types__is_Optional(hex0_23431)) {
    kk_box_t _box_x24744 = hex0_23431._cons.Optional.value;
    bool _hex_20154 = kk_bool_unbox(_box_x24744);
    _x26639 = _hex_20154; /*bool*/
    goto _match26640;
  }
  _x26639 = false; /*bool*/
  _match26640: ;
  m_23428 = kk_std_core_xparse_int(_x26638, _x26639, _ctx); /*maybe<int>*/
  if (kk_std_core_types__is_Nothing(m_23428)) {
    if (kk_std_core_types__is_Optional(default0)) {
      kk_box_t _box_x24745 = default0._cons.Optional.value;
      kk_integer_t _default_201660 = kk_integer_unbox(_box_x24745);
      return _default_201660;
    }
    return kk_integer_from_small(0);
  }
  kk_box_t _box_x24746 = m_23428._cons.Just.value;
  kk_integer_t x = kk_integer_unbox(_box_x24746);
  kk_std_core_types__optional_drop(default0, _ctx);
  return x;
}
 
// monadic lift

kk_std_core_types__tuple2_ kk_std_core__mlift22901_partition_acc(kk_std_core__list acc1, kk_std_core__list acc2, kk_function_t pred, kk_box_t x, kk_std_core__list xx, bool _y_215, kk_context_t* _ctx) { /* forall<a,e> (acc1 : list<a>, acc2 : list<a>, pred : (a) -> e bool, x : a, xx : list<a>, bool) -> e (list<a>, list<a>) */ 
  if (_y_215) {
    kk_std_core__list _x26644 = kk_std_core__new_Cons(kk_reuse_null, x, acc1, _ctx); /*list<61>*/
    return kk_std_core_partition_acc(xx, pred, _x26644, acc2, _ctx);
  }
  kk_std_core__list _x26645 = kk_std_core__new_Cons(kk_reuse_null, x, acc2, _ctx); /*list<61>*/
  return kk_std_core_partition_acc(xx, pred, acc1, _x26645, _ctx);
}


// lift anonymous function
struct kk_std_core_partition_acc_fun26650__t {
  struct kk_function_s _base;
  kk_std_core__list acc10;
  kk_std_core__list acc20;
  kk_function_t pred0;
  kk_box_t x0;
  kk_std_core__list xx0;
};
static kk_box_t kk_std_core_partition_acc_fun26650(kk_function_t _fself, kk_box_t _b_24750, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_partition_acc_fun26650(kk_std_core__list acc10, kk_std_core__list acc20, kk_function_t pred0, kk_box_t x0, kk_std_core__list xx0, kk_context_t* _ctx) {
  struct kk_std_core_partition_acc_fun26650__t* _self = kk_function_alloc_as(struct kk_std_core_partition_acc_fun26650__t, 6, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_partition_acc_fun26650, kk_context());
  _self->acc10 = acc10;
  _self->acc20 = acc20;
  _self->pred0 = pred0;
  _self->x0 = x0;
  _self->xx0 = xx0;
  return &_self->_base;
}

static kk_box_t kk_std_core_partition_acc_fun26650(kk_function_t _fself, kk_box_t _b_24750, kk_context_t* _ctx) {
  struct kk_std_core_partition_acc_fun26650__t* _self = kk_function_as(struct kk_std_core_partition_acc_fun26650__t*, _fself);
  kk_std_core__list acc10 = _self->acc10; /* list<20302> */
  kk_std_core__list acc20 = _self->acc20; /* list<20302> */
  kk_function_t pred0 = _self->pred0; /* (20302) -> 20303 bool */
  kk_box_t x0 = _self->x0; /* 20302 */
  kk_std_core__list xx0 = _self->xx0; /* list<20302> */
  kk_drop_match(_self, {kk_std_core__list_dup(acc10);kk_std_core__list_dup(acc20);kk_function_dup(pred0);kk_box_dup(x0);kk_std_core__list_dup(xx0);}, {}, _ctx)
  kk_std_core_types__tuple2_ _x26651;
  bool _x26652 = kk_bool_unbox(_b_24750); /*bool*/
  _x26651 = kk_std_core__mlift22901_partition_acc(acc10, acc20, pred0, x0, xx0, _x26652, _ctx); /*(list<20302>, list<20302>)*/
  return kk_std_core_types__tuple2__box(_x26651, _ctx);
}

kk_std_core_types__tuple2_ kk_std_core_partition_acc(kk_std_core__list xs, kk_function_t pred0, kk_std_core__list acc10, kk_std_core__list acc20, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, pred : (a) -> e bool, acc1 : list<a>, acc2 : list<a>) -> e (list<a>, list<a>) */ 
  kk__tailcall: ;
  if (kk_std_core__is_Nil(xs)) {
    kk_function_drop(pred0, _ctx);
    kk_std_core__list _b_24751_24747 = kk_std_core_reverse(acc10, _ctx); /*list<20302>*/;
    kk_std_core__list _b_24752_24748 = kk_std_core_reverse(acc20, _ctx); /*list<20302>*/;
    return kk_std_core_types__new_dash__lp__comma__rp_(kk_std_core__list_box(_b_24751_24747, _ctx), kk_std_core__list_box(_b_24752_24748, _ctx), _ctx);
  }
  struct kk_std_core_Cons* _con26646 = kk_std_core__as_Cons(xs);
  kk_box_t x0 = _con26646->head;
  kk_std_core__list xx0 = _con26646->tail;
  kk_reuse_t _ru_25138 = kk_reuse_null; /*reuse*/;
  if (kk_std_core__list_is_unique(xs)) {
    _ru_25138 = (kk_std_core__list_reuse(xs));
  }
  else {
    kk_box_dup(x0);
    kk_std_core__list_dup(xx0);
    kk_std_core__list_decref(xs, _ctx);
    _ru_25138 = kk_reuse_null;
  }
  bool x0_23432;
  kk_function_t _x26648 = kk_function_dup(pred0); /*(20302) -> 20303 bool*/
  kk_box_t _x26647 = kk_box_dup(x0); /*20302*/
  x0_23432 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), _x26648, (_x26648, _x26647, _ctx)); /*bool*/
  if (kk_yielding(kk_context())) {
    kk_reuse_drop(_ru_25138, _ctx);
    kk_box_t _x26649 = kk_std_core_hnd_yield_extend(kk_std_core_new_partition_acc_fun26650(acc10, acc20, pred0, x0, xx0, _ctx), _ctx); /*3991*/
    return kk_std_core_types__tuple2__unbox(_x26649, _ctx);
  }
  if (x0_23432) {
    { // tailcall
      kk_std_core__list _x26653;
      bool _x26654 = _ru_25138!=NULL; /*bool*/
      if (_x26654) {
        struct kk_std_core_Cons* _con26655 = (struct kk_std_core_Cons*)_ru_25138;
        _con26655->tail = acc10;
        _x26653 = kk_std_core__base_Cons(_con26655); /*list<61>*/
      }
      else {
        _x26653 = kk_std_core__new_Cons(kk_reuse_null, x0, acc10, _ctx); /*list<61>*/
      }
      xs = xx0;
      acc10 = _x26653;
      goto kk__tailcall;
    }
  }
  { // tailcall
    kk_std_core__list _x26656;
    bool _x26657 = _ru_25138!=NULL; /*bool*/
    if (_x26657) {
      struct kk_std_core_Cons* _con26658 = (struct kk_std_core_Cons*)_ru_25138;
      _con26658->tail = acc20;
      _x26656 = kk_std_core__base_Cons(_con26658); /*list<61>*/
    }
    else {
      _x26656 = kk_std_core__new_Cons(kk_reuse_null, x0, acc20, _ctx); /*list<61>*/
    }
    xs = xx0;
    acc20 = _x26656;
    goto kk__tailcall;
  }
}
 
// redirect `print` and `println` calls to a specified function.


// lift anonymous function
struct kk_std_core_print_redirect_fun26659__t {
  struct kk_function_s _base;
  kk_function_t print0;
};
static kk_box_t kk_std_core_print_redirect_fun26659(kk_function_t _fself, kk_box_t _b_24758, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_print_redirect_fun26659(kk_function_t print0, kk_context_t* _ctx) {
  struct kk_std_core_print_redirect_fun26659__t* _self = kk_function_alloc_as(struct kk_std_core_print_redirect_fun26659__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_print_redirect_fun26659, kk_context());
  _self->print0 = print0;
  return &_self->_base;
}

static kk_box_t kk_std_core_print_redirect_fun26659(kk_function_t _fself, kk_box_t _b_24758, kk_context_t* _ctx) {
  struct kk_std_core_print_redirect_fun26659__t* _self = kk_function_as(struct kk_std_core_print_redirect_fun26659__t*, _fself);
  kk_function_t print0 = _self->print0; /* (msg : string) -> console () */
  kk_drop_match(_self, {kk_function_dup(print0);}, {}, _ctx)
  kk_unit_t _x26660 = kk_Unit;
  kk_string_t _x26661 = kk_string_unbox(_b_24758); /*string*/
  kk_function_call(kk_unit_t, (kk_function_t, kk_string_t, kk_context_t*), print0, (print0, _x26661, _ctx));
  return kk_unit_box(_x26660);
}

kk_unit_t kk_std_core_print_redirect(kk_function_t print0, kk_context_t* _ctx) { /* (print : (msg : string) -> console ()) -> io () */ 
  kk_std_core_types__maybe _b_24762_24760 = kk_std_core_types__new_Just(kk_function_box(kk_std_core_new_print_redirect_fun26659(print0, _ctx)), _ctx); /*maybe<(msg : string) -> console ()>*/;
  kk_ref_t _x26662 = kk_ref_dup(kk_std_core_redirect); /*ref<global,maybe<(string) -> console ()>>*/
  kk_ref_set(_x26662,(kk_std_core_types__maybe_box(_b_24762_24760, _ctx)),kk_context()); return kk_Unit;
}
extern bool kk_std_core_remove_fun26663(kk_function_t _fself, kk_box_t x, kk_context_t* _ctx) {
  struct kk_std_core_remove_fun26663__t* _self = kk_function_as(struct kk_std_core_remove_fun26663__t*, _fself);
  kk_function_t pred = _self->pred; /* (20373) -> bool */
  kk_drop_match(_self, {kk_function_dup(pred);}, {}, _ctx)
  bool _x26664 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), pred, (pred, x, _ctx)); /*bool*/
  return !(_x26664);
}
extern kk_unit_t kk_std_core_repeat_fun26666_1(kk_function_t _fself, kk_integer_t i, kk_context_t* _ctx) {
  struct kk_std_core_repeat_fun26666__t_1* _self = kk_function_as(struct kk_std_core_repeat_fun26666__t_1*, _fself);
  kk_function_t action = _self->action; /* () -> 20392 () */
  kk_drop_match(_self, {kk_function_dup(action);}, {}, _ctx)
  kk_integer_drop(i, _ctx);
  return kk_function_call(kk_unit_t, (kk_function_t, kk_context_t*), action, (action, _ctx));
}
 
// Create a list of `n`  repeated elementes `x`

kk_std_core__list kk_std_core__ctail_replicate(kk_box_t x, kk_integer_t n, kk_std_core_types__ctail _acc, kk_context_t* _ctx) { /* forall<a> (x : a, n : int, ctail<list<a>>) -> list<a> */ 
  kk__tailcall: ;
  bool _match_25184;
  kk_integer_t _x26667 = kk_integer_dup(n); /*int*/
  _match_25184 = kk_integer_gt(_x26667,(kk_integer_from_small(0)),kk_context()); /*bool*/
  if (_match_25184) {
    kk_std_core__list _ctail_22832 = kk_std_core__list_hole(); /*list<20449>*/;
    kk_std_core__list _ctail_22833;
    kk_box_t _x26668 = kk_box_dup(x); /*20449*/
    _ctail_22833 = kk_std_core__new_Cons(kk_reuse_null, _x26668, _ctail_22832, _ctx); /*list<20449>*/
    { // tailcall
      kk_integer_t _x26669 = kk_std_core_dec(n, _ctx); /*int*/
      kk_std_core_types__ctail _x26670;
      kk_box_t* field_23441 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_22833)->tail)); /*cfield<list<20449>>*/;
      if (kk_std_core_types__is_CTail(_acc)) {
        kk_box_t _box_x24770 = _acc._cons.CTail._field1;
        kk_box_t* last0 = _acc._cons.CTail._field2;
        kk_std_core__list head0 = kk_std_core__list_unbox(_box_x24770, NULL);
        kk_unit_t __ = kk_Unit;
        *(last0) = kk_std_core__list_box(_ctail_22833, _ctx);
        _x26670 = kk_std_core_types__new_CTail(kk_std_core__list_box(head0, _ctx), field_23441, _ctx); /*ctail<58>*/
        goto _match26671;
      }
      _x26670 = kk_std_core_types__new_CTail(kk_std_core__list_box(_ctail_22833, _ctx), field_23441, _ctx); /*ctail<58>*/
      _match26671: ;
      n = _x26669;
      _acc = _x26670;
      goto kk__tailcall;
    }
  }
  kk_integer_drop(n, _ctx);
  kk_box_drop(x, _ctx);
  if (kk_std_core_types__is_CTail(_acc)) {
    kk_box_t _box_x24779 = _acc._cons.CTail._field1;
    kk_box_t* last00 = _acc._cons.CTail._field2;
    kk_std_core__list head00 = kk_std_core__list_unbox(_box_x24779, NULL);
    kk_unit_t __0 = kk_Unit;
    *(last00) = kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx);
    return head00;
  }
  return kk_std_core__new_Nil(_ctx);
}
 
// Create a list of `n`  repeated elementes `x`

kk_std_core__list kk_std_core_replicate(kk_box_t x0, kk_integer_t n0, kk_context_t* _ctx) { /* forall<a> (x : a, n : int) -> list<a> */ 
  return kk_std_core__ctail_replicate(x0, n0, kk_std_core_types__new_CTailNil(_ctx), _ctx);
}

kk_string_t kk_std_core_show_tuple(kk_std_core_types__tuple2_ x, kk_function_t showfst, kk_function_t showsnd, kk_context_t* _ctx) { /* forall<a,b> (x : (a, b), showfst : (a) -> string, showsnd : (b) -> string) -> string */ 
  kk_string_t _x26674;
  kk_string_t _x26675;
  kk_string_t _x26676;
  kk_string_t _x26677;
  kk_define_string_literal(, _s26678, 1, "(")
  _x26677 = kk_string_dup(_s26678); /*string*/
  kk_string_t _x26679;
  kk_box_t _x26680;
  kk_box_t _x = x.fst;
  kk_box_dup(_x);
  _x26680 = _x; /*21629*/
  _x26679 = kk_function_call(kk_string_t, (kk_function_t, kk_box_t, kk_context_t*), showfst, (showfst, _x26680, _ctx)); /*string*/
  _x26676 = kk_std_core__lp__plus__6_rp_(_x26677, _x26679, _ctx); /*string*/
  kk_string_t _x26681;
  kk_define_string_literal(, _s26682, 1, ",")
  _x26681 = kk_string_dup(_s26682); /*string*/
  _x26675 = kk_std_core__lp__plus__6_rp_(_x26676, _x26681, _ctx); /*string*/
  kk_string_t _x26683;
  kk_box_t _x26684;
  kk_box_t _x0 = x.snd;
  kk_box_dup(_x0);
  kk_std_core_types__tuple2__drop(x, _ctx);
  _x26684 = _x0; /*21630*/
  _x26683 = kk_function_call(kk_string_t, (kk_function_t, kk_box_t, kk_context_t*), showsnd, (showsnd, _x26684, _ctx)); /*string*/
  _x26674 = kk_std_core__lp__plus__6_rp_(_x26675, _x26683, _ctx); /*string*/
  kk_string_t _x26685;
  kk_define_string_literal(, _s26686, 1, ")")
  _x26685 = kk_string_dup(_s26686); /*string*/
  return kk_std_core__lp__plus__6_rp_(_x26674, _x26685, _ctx);
}
 
// monadic lift

kk_std_core_types__tuple2_ kk_std_core__mlift22902_op(kk_std_core__list acc, kk_function_t predicate, kk_box_t y, kk_std_core__list ys, kk_std_core__list yy, bool _y_223, kk_context_t* _ctx) { /* forall<a,e> (acc : list<a>, predicate : (a) -> e bool, y : a, ys : list<a>, yy : list<a>, bool) -> e (list<a>, list<a>) */ 
  if (_y_223) {
    kk_std_core__list_drop(ys, _ctx);
    kk_std_core__list acc0_22779 = kk_std_core__new_Cons(kk_reuse_null, y, acc, _ctx); /*list<21631>*/;
    return kk_std_core__lift22741_span(predicate, yy, acc0_22779, _ctx);
  }
  kk_function_drop(predicate, _ctx);
  kk_box_drop(y, _ctx);
  kk_std_core__list_drop(yy, _ctx);
  kk_std_core__list _b_24790_24788 = kk_std_core_reverse(acc, _ctx); /*list<21631>*/;
  return kk_std_core_types__new_dash__lp__comma__rp_(kk_std_core__list_box(_b_24790_24788, _ctx), kk_std_core__list_box(ys, _ctx), _ctx);
}
 
// lift


// lift anonymous function
struct kk_std_core__lift22741_span_fun26692__t {
  struct kk_function_s _base;
  kk_std_core__list acc0;
  kk_function_t predicate0;
  kk_box_t y0;
  kk_std_core__list ys0;
  kk_std_core__list yy0;
};
static kk_box_t kk_std_core__lift22741_span_fun26692(kk_function_t _fself, kk_box_t _b_24793, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift22741_span_fun26692(kk_std_core__list acc0, kk_function_t predicate0, kk_box_t y0, kk_std_core__list ys0, kk_std_core__list yy0, kk_context_t* _ctx) {
  struct kk_std_core__lift22741_span_fun26692__t* _self = kk_function_alloc_as(struct kk_std_core__lift22741_span_fun26692__t, 6, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift22741_span_fun26692, kk_context());
  _self->acc0 = acc0;
  _self->predicate0 = predicate0;
  _self->y0 = y0;
  _self->ys0 = ys0;
  _self->yy0 = yy0;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift22741_span_fun26692(kk_function_t _fself, kk_box_t _b_24793, kk_context_t* _ctx) {
  struct kk_std_core__lift22741_span_fun26692__t* _self = kk_function_as(struct kk_std_core__lift22741_span_fun26692__t*, _fself);
  kk_std_core__list acc0 = _self->acc0; /* list<21631> */
  kk_function_t predicate0 = _self->predicate0; /* (21631) -> 21632 bool */
  kk_box_t y0 = _self->y0; /* 21631 */
  kk_std_core__list ys0 = _self->ys0; /* list<21631> */
  kk_std_core__list yy0 = _self->yy0; /* list<21631> */
  kk_drop_match(_self, {kk_std_core__list_dup(acc0);kk_function_dup(predicate0);kk_box_dup(y0);kk_std_core__list_dup(ys0);kk_std_core__list_dup(yy0);}, {}, _ctx)
  kk_std_core_types__tuple2_ _x26693;
  bool _x26694 = kk_bool_unbox(_b_24793); /*bool*/
  _x26693 = kk_std_core__mlift22902_op(acc0, predicate0, y0, ys0, yy0, _x26694, _ctx); /*(list<21631>, list<21631>)*/
  return kk_std_core_types__tuple2__box(_x26693, _ctx);
}

kk_std_core_types__tuple2_ kk_std_core__lift22741_span(kk_function_t predicate0, kk_std_core__list ys0, kk_std_core__list acc0, kk_context_t* _ctx) { /* forall<a,e> (predicate : (a) -> e bool, ys : list<a>, acc : list<a>) -> e (list<a>, list<a>) */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys0)) {
    struct kk_std_core_Cons* _con26687 = kk_std_core__as_Cons(ys0);
    kk_box_t y0 = _con26687->head;
    kk_std_core__list yy0 = _con26687->tail;
    kk_box_dup(y0);
    kk_std_core__list_dup(yy0);
    kk_reuse_t _ru_25139;
    kk_std_core__list _x26688 = kk_std_core__list_dup(ys0); /*list<21631>*/
    _ru_25139 = kk_std_core__list_dropn_reuse(_x26688, ((int32_t)2), _ctx); /*reuse*/
    bool x_23446;
    kk_function_t _x26690 = kk_function_dup(predicate0); /*(21631) -> 21632 bool*/
    kk_box_t _x26689 = kk_box_dup(y0); /*21631*/
    x_23446 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), _x26690, (_x26690, _x26689, _ctx)); /*bool*/
    if (kk_yielding(kk_context())) {
      kk_reuse_drop(_ru_25139, _ctx);
      kk_box_t _x26691 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift22741_span_fun26692(acc0, predicate0, y0, ys0, yy0, _ctx), _ctx); /*3991*/
      return kk_std_core_types__tuple2__unbox(_x26691, _ctx);
    }
    if (x_23446) {
      kk_std_core__list_dropn(ys0, ((int32_t)2), _ctx);
      kk_std_core__list acc0_227790;
      bool _x26695 = _ru_25139!=NULL; /*bool*/
      if (_x26695) {
        struct kk_std_core_Cons* _con26696 = (struct kk_std_core_Cons*)_ru_25139;
        _con26696->tail = acc0;
        acc0_227790 = kk_std_core__base_Cons(_con26696); /*list<21631>*/
      }
      else {
        acc0_227790 = kk_std_core__new_Cons(kk_reuse_null, y0, acc0, _ctx); /*list<21631>*/
      }
      { // tailcall
        ys0 = yy0;
        acc0 = acc0_227790;
        goto kk__tailcall;
      }
    }
    kk_reuse_drop(_ru_25139, _ctx);
    kk_function_drop(predicate0, _ctx);
    kk_box_drop(y0, _ctx);
    kk_std_core__list_drop(yy0, _ctx);
    kk_std_core__list _b_24800_24794 = kk_std_core_reverse(acc0, _ctx); /*list<21631>*/;
    return kk_std_core_types__new_dash__lp__comma__rp_(kk_std_core__list_box(_b_24800_24794, _ctx), kk_std_core__list_box(ys0, _ctx), _ctx);
  }
  kk_function_drop(predicate0, _ctx);
  kk_std_core__list _b_24802_24796 = kk_std_core_reverse(acc0, _ctx); /*list<21631>*/;
  return kk_std_core_types__new_dash__lp__comma__rp_(kk_std_core__list_box(_b_24802_24796, _ctx), kk_std_core__list_box(ys0, _ctx), _ctx);
}
 
// Return the sum of a list of integers


// lift anonymous function
struct kk_std_core_sum_fun26698__t {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_sum_fun26698(kk_function_t _fself, kk_box_t _b_24807, kk_box_t _b_24808, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_sum_fun26698(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_sum_fun26698, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_sum_fun26698(kk_function_t _fself, kk_box_t _b_24807, kk_box_t _b_24808, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  kk_integer_t _x26699;
  kk_integer_t _x26700 = kk_integer_unbox(_b_24807); /*int*/
  kk_integer_t _x26701 = kk_integer_unbox(_b_24808); /*int*/
  _x26699 = kk_integer_add(_x26700,_x26701,kk_context()); /*int*/
  return kk_integer_box(_x26699);
}

kk_integer_t kk_std_core_sum(kk_std_core__list xs, kk_context_t* _ctx) { /* (xs : list<int>) -> int */ 
  kk_box_t _x26697 = kk_std_core_foldl(xs, kk_integer_box(kk_integer_from_small(0)), kk_std_core_new_sum_fun26698(_ctx), _ctx); /*15726*/
  return kk_integer_unbox(_x26697);
}
 
// Return the tail of list. Returns the empty list if `xs` is empty.

kk_std_core__list kk_std_core_tail_1(kk_std_core__list xs, kk_context_t* _ctx) { /* forall<a> (xs : list<a>) -> list<a> */ 
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con26702 = kk_std_core__as_Cons(xs);
    kk_box_t _pat0 = _con26702->head;
    kk_std_core__list xx = _con26702->tail;
    if (kk_std_core__list_is_unique(xs)) {
      kk_box_drop(_pat0, _ctx);
      kk_std_core__list_free(xs);
    }
    else {
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    return xx;
  }
  return kk_std_core__new_Nil(_ctx);
}
 
// Return the tail of a string (or the empty string)

kk_string_t kk_std_core_tail_2(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> string */ 
  kk_std_core__sslice _x26703;
  kk_std_core__sslice slice_23450;
  kk_std_core__sslice slice0 = kk_std_core_first1(s, _ctx); /*sslice*/;
  bool _match_25180;
  kk_integer_t _x26704;
  kk_std_core_types__optional _x26705 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
  if (kk_std_core_types__is_Optional(_x26705)) {
    kk_box_t _box_x24814 = _x26705._cons.Optional.value;
    kk_integer_t _n_13060 = kk_integer_unbox(_box_x24814);
    _x26704 = _n_13060; /*int*/
    goto _match26706;
  }
  _x26704 = kk_integer_from_small(1); /*int*/
  _match26706: ;
  _match_25180 = kk_integer_eq(_x26704,(kk_integer_from_small(1)),kk_context()); /*bool*/
  if (_match_25180) {
    slice_23450 = slice0; /*sslice*/
  }
  else {
    kk_integer_t _x26708;
    kk_integer_t _x26709;
    kk_std_core_types__optional _x26710 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
    if (kk_std_core_types__is_Optional(_x26710)) {
      kk_box_t _box_x24815 = _x26710._cons.Optional.value;
      kk_integer_t _n_130600 = kk_integer_unbox(_box_x24815);
      _x26709 = _n_130600; /*int*/
      goto _match26711;
    }
    _x26709 = kk_integer_from_small(1); /*int*/
    _match26711: ;
    _x26708 = kk_integer_sub(_x26709,(kk_integer_from_small(1)),kk_context()); /*int*/
    slice_23450 = kk_std_core_extend(slice0, _x26708, _ctx); /*sslice*/
  }
  kk_string_t s0 = slice_23450.str;
  size_t start0 = slice_23450.start;
  size_t len0 = slice_23450.len;
  kk_string_dup(s0);
  kk_std_core__sslice_drop(slice_23450, _ctx);
  kk_string_t _x26713 = kk_string_dup(s0); /*string*/
  size_t _x26714 = (start0 + len0); /*size_t*/
  size_t _x26715;
  size_t _x26716 = kk_string_len(s0,kk_context()); /*size_t*/
  size_t _x26717 = (start0 + len0); /*size_t*/
  _x26715 = (_x26716 - _x26717); /*size_t*/
  _x26703 = kk_std_core__new_Sslice(_x26713, _x26714, _x26715, _ctx); /*sslice*/
  return kk_std_core_string_3(_x26703, _ctx);
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift22903_op(kk_std_core_types__ctail _acc, kk_function_t predicate, kk_box_t x, kk_std_core__list xx, bool _y_228, kk_context_t* _ctx) { /* forall<a,e> (ctail<list<a>>, predicate : (a) -> e bool, x : a, xx : list<a>, bool) -> e list<a> */ 
  if (_y_228) {
    kk_std_core__list _ctail_22834 = kk_std_core__list_hole(); /*list<21842>*/;
    kk_std_core__list _ctail_22835 = kk_std_core__new_Cons(kk_reuse_null, x, _ctail_22834, _ctx); /*list<21842>*/;
    kk_std_core_types__ctail _x26718;
    kk_box_t* field_23455 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_22835)->tail)); /*cfield<list<21842>>*/;
    if (kk_std_core_types__is_CTail(_acc)) {
      kk_box_t _box_x24822 = _acc._cons.CTail._field1;
      kk_box_t* last0 = _acc._cons.CTail._field2;
      kk_std_core__list head0 = kk_std_core__list_unbox(_box_x24822, NULL);
      kk_unit_t __ = kk_Unit;
      *(last0) = kk_std_core__list_box(_ctail_22835, _ctx);
      _x26718 = kk_std_core_types__new_CTail(kk_std_core__list_box(head0, _ctx), field_23455, _ctx); /*ctail<58>*/
      goto _match26719;
    }
    _x26718 = kk_std_core_types__new_CTail(kk_std_core__list_box(_ctail_22835, _ctx), field_23455, _ctx); /*ctail<58>*/
    _match26719: ;
    return kk_std_core__ctail_take_while(xx, predicate, _x26718, _ctx);
  }
  kk_function_drop(predicate, _ctx);
  kk_box_drop(x, _ctx);
  kk_std_core__list_drop(xx, _ctx);
  if (kk_std_core_types__is_CTail(_acc)) {
    kk_box_t _box_x24831 = _acc._cons.CTail._field1;
    kk_box_t* last00 = _acc._cons.CTail._field2;
    kk_std_core__list head00 = kk_std_core__list_unbox(_box_x24831, NULL);
    kk_unit_t __0 = kk_Unit;
    *(last00) = kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx);
    return head00;
  }
  return kk_std_core__new_Nil(_ctx);
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift22904_op_fun26722__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t x0;
};
static kk_std_core__list kk_std_core__mlift22904_op_fun26722(kk_function_t _fself, kk_std_core__list _ctail_22837, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift22904_op_fun26722(kk_function_t _accm, kk_box_t x0, kk_context_t* _ctx) {
  struct kk_std_core__mlift22904_op_fun26722__t* _self = kk_function_alloc_as(struct kk_std_core__mlift22904_op_fun26722__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift22904_op_fun26722, kk_context());
  _self->_accm = _accm;
  _self->x0 = x0;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift22904_op_fun26722(kk_function_t _fself, kk_std_core__list _ctail_22837, kk_context_t* _ctx) {
  struct kk_std_core__mlift22904_op_fun26722__t* _self = kk_function_as(struct kk_std_core__mlift22904_op_fun26722__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<21842>) -> list<21842> */
  kk_box_t x0 = _self->x0; /* 21842 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(x0);}, {}, _ctx)
  kk_std_core__list _x26723 = kk_std_core__new_Cons(kk_reuse_null, x0, _ctail_22837, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x26723, _ctx));
}

kk_std_core__list kk_std_core__mlift22904_op(kk_function_t _accm, kk_function_t predicate0, kk_box_t x0, kk_std_core__list xx0, bool _y_232, kk_context_t* _ctx) { /* forall<a,e> ((list<a>) -> list<a>, predicate : (a) -> e bool, x : a, xx : list<a>, bool) -> e list<a> */ 
  if (_y_232) {
    return kk_std_core__ctailm_take_while(xx0, predicate0, kk_std_core__new_mlift22904_op_fun26722(_accm, x0, _ctx), _ctx);
  }
  kk_function_drop(predicate0, _ctx);
  kk_box_drop(x0, _ctx);
  kk_std_core__list_drop(xx0, _ctx);
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, kk_std_core__new_Nil(_ctx), _ctx));
}
 
// Keep only those initial elements that satisfy `predicate`


// lift anonymous function
struct kk_std_core__ctail_take_while_fun26728__t {
  struct kk_function_s _base;
  kk_function_t predicate1;
  kk_box_t x1;
  kk_std_core__list xx1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_take_while_fun26728(kk_function_t _fself, kk_box_t _b_24841, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_take_while_fun26728(kk_function_t predicate1, kk_box_t x1, kk_std_core__list xx1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_take_while_fun26728__t* _self = kk_function_alloc_as(struct kk_std_core__ctail_take_while_fun26728__t, 6, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_take_while_fun26728, kk_context());
  _self->predicate1 = predicate1;
  _self->x1 = x1;
  _self->xx1 = xx1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_take_while_fun26728(kk_function_t _fself, kk_box_t _b_24841, kk_context_t* _ctx) {
  struct kk_std_core__ctail_take_while_fun26728__t* _self = kk_function_as(struct kk_std_core__ctail_take_while_fun26728__t*, _fself);
  kk_function_t predicate1 = _self->predicate1; /* (21842) -> 21843 bool */
  kk_box_t x1 = _self->x1; /* 21842 */
  kk_std_core__list xx1 = _self->xx1; /* list<21842> */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<21842>> */
  kk_drop_match(_self, {kk_function_dup(predicate1);kk_box_dup(x1);kk_std_core__list_dup(xx1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  kk_std_core__list _x26729;
  bool _x26730 = kk_bool_unbox(_b_24841); /*bool*/
  _x26729 = kk_std_core__mlift22903_op(_acc0, predicate1, x1, xx1, _x26730, _ctx); /*list<21842>*/
  return kk_std_core__list_box(_x26729, _ctx);
}

kk_std_core__list kk_std_core__ctail_take_while(kk_std_core__list xs, kk_function_t predicate1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, predicate : (a) -> e bool, ctail<list<a>>) -> e list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con26724 = kk_std_core__as_Cons(xs);
    kk_box_t x1 = _con26724->head;
    kk_std_core__list xx1 = _con26724->tail;
    kk_reuse_t _ru_25141 = kk_reuse_null; /*reuse*/;
    if (kk_std_core__list_is_unique(xs)) {
      _ru_25141 = (kk_std_core__list_reuse(xs));
    }
    else {
      kk_box_dup(x1);
      kk_std_core__list_dup(xx1);
      kk_std_core__list_decref(xs, _ctx);
      _ru_25141 = kk_reuse_null;
    }
    bool x0_23458;
    kk_function_t _x26726 = kk_function_dup(predicate1); /*(21842) -> 21843 bool*/
    kk_box_t _x26725 = kk_box_dup(x1); /*21842*/
    x0_23458 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), _x26726, (_x26726, _x26725, _ctx)); /*bool*/
    if (kk_yielding(kk_context())) {
      kk_reuse_drop(_ru_25141, _ctx);
      kk_box_t _x26727 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_take_while_fun26728(predicate1, x1, xx1, _acc0, _ctx), _ctx); /*3991*/
      return kk_std_core__list_unbox(_x26727, _ctx);
    }
    if (x0_23458) {
      kk_std_core__list _ctail_228340 = kk_std_core__list_hole(); /*list<21842>*/;
      kk_std_core__list _ctail_228350;
      bool _x26731 = _ru_25141!=NULL; /*bool*/
      if (_x26731) {
        struct kk_std_core_Cons* _con26732 = (struct kk_std_core_Cons*)_ru_25141;
        _con26732->tail = _ctail_228340;
        _ctail_228350 = kk_std_core__base_Cons(_con26732); /*list<21842>*/
      }
      else {
        _ctail_228350 = kk_std_core__new_Cons(kk_reuse_null, x1, _ctail_228340, _ctx); /*list<21842>*/
      }
      { // tailcall
        kk_std_core_types__ctail _x26733;
        kk_box_t* field_23464 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_228350)->tail)); /*cfield<list<21842>>*/;
        if (kk_std_core_types__is_CTail(_acc0)) {
          kk_box_t _box_x24848 = _acc0._cons.CTail._field1;
          kk_box_t* last2 = _acc0._cons.CTail._field2;
          kk_std_core__list head1 = kk_std_core__list_unbox(_box_x24848, NULL);
          kk_unit_t __1 = kk_Unit;
          *(last2) = kk_std_core__list_box(_ctail_228350, _ctx);
          _x26733 = kk_std_core_types__new_CTail(kk_std_core__list_box(head1, _ctx), field_23464, _ctx); /*ctail<58>*/
          goto _match26734;
        }
        _x26733 = kk_std_core_types__new_CTail(kk_std_core__list_box(_ctail_228350, _ctx), field_23464, _ctx); /*ctail<58>*/
        _match26734: ;
        xs = xx1;
        _acc0 = _x26733;
        goto kk__tailcall;
      }
    }
    kk_reuse_drop(_ru_25141, _ctx);
    kk_function_drop(predicate1, _ctx);
    kk_box_drop(x1, _ctx);
    kk_std_core__list_drop(xx1, _ctx);
    if (kk_std_core_types__is_CTail(_acc0)) {
      kk_box_t _box_x24857 = _acc0._cons.CTail._field1;
      kk_box_t* last01 = _acc0._cons.CTail._field2;
      kk_std_core__list head01 = kk_std_core__list_unbox(_box_x24857, NULL);
      kk_unit_t __00 = kk_Unit;
      *(last01) = kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx);
      return head01;
    }
    return kk_std_core__new_Nil(_ctx);
  }
  kk_function_drop(predicate1, _ctx);
  if (kk_std_core_types__is_CTail(_acc0)) {
    kk_box_t _box_x24862 = _acc0._cons.CTail._field1;
    kk_box_t* last10 = _acc0._cons.CTail._field2;
    kk_std_core__list head10 = kk_std_core__list_unbox(_box_x24862, NULL);
    kk_unit_t __10 = kk_Unit;
    *(last10) = kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx);
    return head10;
  }
  return kk_std_core__new_Nil(_ctx);
}
 
// Keep only those initial elements that satisfy `predicate`


// lift anonymous function
struct kk_std_core__ctailm_take_while_fun26742__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t predicate2;
  kk_box_t x2;
  kk_std_core__list xx2;
};
static kk_box_t kk_std_core__ctailm_take_while_fun26742(kk_function_t _fself, kk_box_t _b_24874, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_take_while_fun26742(kk_function_t _accm0, kk_function_t predicate2, kk_box_t x2, kk_std_core__list xx2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_take_while_fun26742__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_take_while_fun26742__t, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_take_while_fun26742, kk_context());
  _self->_accm0 = _accm0;
  _self->predicate2 = predicate2;
  _self->x2 = x2;
  _self->xx2 = xx2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_take_while_fun26742(kk_function_t _fself, kk_box_t _b_24874, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_take_while_fun26742__t* _self = kk_function_as(struct kk_std_core__ctailm_take_while_fun26742__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<21842>) -> list<21842> */
  kk_function_t predicate2 = _self->predicate2; /* (21842) -> 21843 bool */
  kk_box_t x2 = _self->x2; /* 21842 */
  kk_std_core__list xx2 = _self->xx2; /* list<21842> */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(predicate2);kk_box_dup(x2);kk_std_core__list_dup(xx2);}, {}, _ctx)
  kk_std_core__list _x26743;
  bool _x26744 = kk_bool_unbox(_b_24874); /*bool*/
  _x26743 = kk_std_core__mlift22904_op(_accm0, predicate2, x2, xx2, _x26744, _ctx); /*list<21842>*/
  return kk_std_core__list_box(_x26743, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_take_while_fun26746__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t x2;
};
static kk_std_core__list kk_std_core__ctailm_take_while_fun26746(kk_function_t _fself, kk_std_core__list _ctail_228370, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_take_while_fun26746(kk_function_t _accm0, kk_box_t x2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_take_while_fun26746__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_take_while_fun26746__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_take_while_fun26746, kk_context());
  _self->_accm0 = _accm0;
  _self->x2 = x2;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_take_while_fun26746(kk_function_t _fself, kk_std_core__list _ctail_228370, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_take_while_fun26746__t* _self = kk_function_as(struct kk_std_core__ctailm_take_while_fun26746__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<21842>) -> list<21842> */
  kk_box_t x2 = _self->x2; /* 21842 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(x2);}, {}, _ctx)
  kk_std_core__list _x26747 = kk_std_core__new_Cons(kk_reuse_null, x2, _ctail_228370, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x26747, _ctx));
}

kk_std_core__list kk_std_core__ctailm_take_while(kk_std_core__list xs0, kk_function_t predicate2, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, predicate : (a) -> e bool, (list<a>) -> list<a>) -> e list<a> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs0)) {
    struct kk_std_core_Cons* _con26738 = kk_std_core__as_Cons(xs0);
    kk_box_t x2 = _con26738->head;
    kk_std_core__list xx2 = _con26738->tail;
    if (kk_std_core__list_is_unique(xs0)) {
      kk_std_core__list_free(xs0);
    }
    else {
      kk_box_dup(x2);
      kk_std_core__list_dup(xx2);
      kk_std_core__list_decref(xs0, _ctx);
    }
    bool x0_23469;
    kk_function_t _x26740 = kk_function_dup(predicate2); /*(21842) -> 21843 bool*/
    kk_box_t _x26739 = kk_box_dup(x2); /*21842*/
    x0_23469 = kk_function_call(bool, (kk_function_t, kk_box_t, kk_context_t*), _x26740, (_x26740, _x26739, _ctx)); /*bool*/
    if (kk_yielding(kk_context())) {
      kk_box_t _x26741 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_take_while_fun26742(_accm0, predicate2, x2, xx2, _ctx), _ctx); /*3991*/
      return kk_std_core__list_unbox(_x26741, _ctx);
    }
    if (x0_23469) {
      { // tailcall
        kk_function_t _x26745 = kk_std_core__new_ctailm_take_while_fun26746(_accm0, x2, _ctx); /*(list<21842>) -> list<21842>*/
        xs0 = xx2;
        _accm0 = _x26745;
        goto kk__tailcall;
      }
    }
    kk_function_drop(predicate2, _ctx);
    kk_box_drop(x2, _ctx);
    kk_std_core__list_drop(xx2, _ctx);
    return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, kk_std_core__new_Nil(_ctx), _ctx));
  }
  kk_function_drop(predicate2, _ctx);
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, kk_std_core__new_Nil(_ctx), _ctx));
}
 
// Keep only those initial elements that satisfy `predicate`


// lift anonymous function
struct kk_std_core_take_while_fun26748__t {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core_take_while_fun26748(kk_function_t _fself, kk_std_core__list _ctail_22836, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_take_while_fun26748(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_take_while_fun26748, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core_take_while_fun26748(kk_function_t _fself, kk_std_core__list _ctail_22836, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  return _ctail_22836;
}

kk_std_core__list kk_std_core_take_while(kk_std_core__list xs1, kk_function_t predicate3, kk_context_t* _ctx) { /* forall<a,e> (xs : list<a>, predicate : (a) -> e bool) -> e list<a> */ 
  bool _match_25177 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_25177) {
    return kk_std_core__ctail_take_while(xs1, predicate3, kk_std_core_types__new_CTailNil(_ctx), _ctx);
  }
  return kk_std_core__ctailm_take_while(xs1, predicate3, kk_std_core_new_take_while_fun26748(_ctx), _ctx);
}

kk_unit_t kk_std_core_todo(kk_string_t message0, kk_context_t* _ctx) { /* (message : string) -> () */ 
  kk_std_core_types__optional info_23475 = kk_std_core_types__new_Optional(kk_std_core__exception_info_box(kk_std_core__new_ExnTodo(_ctx), _ctx), _ctx); /*optional<exception-info>*/;
  kk_std_core__exception exn_23476;
  kk_std_core__exception_info _x26749;
  if (kk_std_core_types__is_Optional(info_23475)) {
    kk_box_t _box_x24879 = info_23475._cons.Optional.value;
    kk_std_core__exception_info _info_12803 = kk_std_core__exception_info_unbox(_box_x24879, NULL);
    _x26749 = _info_12803; /*exception-info*/
    goto _match26750;
  }
  _x26749 = kk_std_core__new_ExnError(_ctx); /*exception-info*/
  _match26750: ;
  exn_23476 = kk_std_core__new_Exception(message0, _x26749, _ctx); /*exception*/
  kk_std_core_hnd__ev ev_23477;
  size_t _x26752 = ((size_t)0); /*size_t*/
  ev_23477 = kk_evv_at(_x26752,kk_context()); /*std/core/hnd/ev<.hnd-exn>*/
  kk_box_t _x26753;
  struct kk_std_core_hnd_Ev* _con26754 = kk_std_core_hnd__as_Ev(ev_23477);
  kk_std_core_hnd__marker m = _con26754->marker;
  kk_box_t _box_x24880 = _con26754->hnd;
  kk_std_core__hnd_exn h = kk_std_core__hnd_exn_unbox(_box_x24880, NULL);
  kk_std_core__hnd_exn_dup(h);
  kk_std_core_hnd__clause1 _match_25176;
  struct kk_std_core__Hnd_exn* _con26756 = kk_std_core__as_Hnd_exn(h);
  kk_std_core_hnd__clause1 except_throw_exn = _con26756->except_throw_exn;
  if (kk_std_core__hnd_exn_is_unique(h)) {
    kk_std_core__hnd_exn_free(h);
  }
  else {
    kk_std_core_hnd__clause1_dup(except_throw_exn);
    kk_std_core__hnd_exn_decref(h, _ctx);
  }
  _match_25176 = except_throw_exn; /*forall<a> std/core/hnd/clause1<exception,a,.hnd-exn,3528,3529>*/
  kk_function_t _fun_unbox_x24884 = _match_25176.clause;
  _x26753 = kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_box_t, kk_context_t*), _fun_unbox_x24884, (_fun_unbox_x24884, m, ev_23477, kk_std_core__exception_box(exn_23476, _ctx), _ctx)); /*53*/
  kk_unit_unbox(_x26753); return kk_Unit;
}
 
// Trace a message used for debug purposes.
// The behaviour is system dependent. On a browser and node it uses
// `console.log`  by default.
// Disabled if `notrace` is called.

kk_unit_t kk_std_core_trace(kk_string_t message0, kk_context_t* _ctx) { /* (message : string) -> () */ 
  bool _match_25175;
  kk_ref_t _b_24893_24892 = kk_ref_alloc((kk_bool_box(true)),kk_context()); /*ref<global,bool>*/;
  kk_box_t _x26757 = kk_ref_get(_b_24893_24892,kk_context()); /*179*/
  _match_25175 = kk_bool_unbox(_x26757); /*bool*/
  if (_match_25175) {
    kk_std_core_xtrace(message0, _ctx); return kk_Unit;
  }
  kk_string_drop(message0, _ctx);
  kk_Unit; return kk_Unit;
}

kk_unit_t kk_std_core_trace_any(kk_string_t message0, kk_box_t x, kk_context_t* _ctx) { /* forall<a> (message : string, x : a) -> () */ 
  bool _match_25174;
  kk_ref_t _b_24897_24896 = kk_ref_alloc((kk_bool_box(true)),kk_context()); /*ref<global,bool>*/;
  kk_box_t _x26758 = kk_ref_get(_b_24897_24896,kk_context()); /*179*/
  _match_25174 = kk_bool_unbox(_x26758); /*bool*/
  if (_match_25174) {
    kk_std_core_xtrace_any(message0, x, _ctx); return kk_Unit;
  }
  kk_string_drop(message0, _ctx);
  kk_box_drop(x, _ctx);
  kk_Unit; return kk_Unit;
}
 
// Truncate a string to `count` characters.

kk_string_t kk_std_core_truncate(kk_string_t s, kk_integer_t count, kk_context_t* _ctx) { /* (s : string, count : int) -> string */ 
  kk_std_core__sslice _x26759;
  kk_std_core__sslice _x26760;
  kk_std_core__sslice slice0 = kk_std_core_first1(s, _ctx); /*sslice*/;
  bool _match_25171;
  kk_integer_t _x26761;
  kk_std_core_types__optional _x26762 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
  if (kk_std_core_types__is_Optional(_x26762)) {
    kk_box_t _box_x24899 = _x26762._cons.Optional.value;
    kk_integer_t _n_13060 = kk_integer_unbox(_box_x24899);
    _x26761 = _n_13060; /*int*/
    goto _match26763;
  }
  _x26761 = kk_integer_from_small(1); /*int*/
  _match26763: ;
  _match_25171 = kk_integer_eq(_x26761,(kk_integer_from_small(1)),kk_context()); /*bool*/
  if (_match_25171) {
    _x26760 = slice0; /*sslice*/
  }
  else {
    kk_integer_t _x26765;
    kk_integer_t _x26766;
    kk_std_core_types__optional _x26767 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
    if (kk_std_core_types__is_Optional(_x26767)) {
      kk_box_t _box_x24900 = _x26767._cons.Optional.value;
      kk_integer_t _n_130600 = kk_integer_unbox(_box_x24900);
      _x26766 = _n_130600; /*int*/
      goto _match26768;
    }
    _x26766 = kk_integer_from_small(1); /*int*/
    _match26768: ;
    _x26765 = kk_integer_sub(_x26766,(kk_integer_from_small(1)),kk_context()); /*int*/
    _x26760 = kk_std_core_extend(slice0, _x26765, _ctx); /*sslice*/
  }
  kk_integer_t _x26770 = kk_integer_sub(count,(kk_integer_from_small(1)),kk_context()); /*int*/
  _x26759 = kk_std_core_extend(_x26760, _x26770, _ctx); /*sslice*/
  return kk_std_core_string_3(_x26759, _ctx);
}
 
// Return a default value when an exception is raised


// lift anonymous function
struct kk_std_core_try_default_fun26773__t {
  struct kk_function_s _base;
  kk_box_t value;
};
static kk_box_t kk_std_core_try_default_fun26773(kk_function_t _fself, kk_std_core_hnd__marker _b_24902, kk_std_core_hnd__ev _b_24903, kk_box_t _b_24904, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_default_fun26773(kk_box_t value, kk_context_t* _ctx) {
  struct kk_std_core_try_default_fun26773__t* _self = kk_function_alloc_as(struct kk_std_core_try_default_fun26773__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_try_default_fun26773, kk_context());
  _self->value = value;
  return &_self->_base;
}



// lift anonymous function
struct kk_std_core_try_default_fun26774__t {
  struct kk_function_s _base;
  kk_box_t value;
};
static kk_box_t kk_std_core_try_default_fun26774(kk_function_t _fself, kk_function_t __k, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_default_fun26774(kk_box_t value, kk_context_t* _ctx) {
  struct kk_std_core_try_default_fun26774__t* _self = kk_function_alloc_as(struct kk_std_core_try_default_fun26774__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_try_default_fun26774, kk_context());
  _self->value = value;
  return &_self->_base;
}

static kk_box_t kk_std_core_try_default_fun26774(kk_function_t _fself, kk_function_t __k, kk_context_t* _ctx) {
  struct kk_std_core_try_default_fun26774__t* _self = kk_function_as(struct kk_std_core_try_default_fun26774__t*, _fself);
  kk_box_t value = _self->value; /* 22087 */
  kk_drop_match(_self, {kk_box_dup(value);}, {}, _ctx)
  kk_function_drop(__k, _ctx);
  return value;
}
static kk_box_t kk_std_core_try_default_fun26773(kk_function_t _fself, kk_std_core_hnd__marker _b_24902, kk_std_core_hnd__ev _b_24903, kk_box_t _b_24904, kk_context_t* _ctx) {
  struct kk_std_core_try_default_fun26773__t* _self = kk_function_as(struct kk_std_core_try_default_fun26773__t*, _fself);
  kk_box_t value = _self->value; /* 22087 */
  kk_drop_match(_self, {kk_box_dup(value);}, {}, _ctx)
  kk_std_core_hnd__ev_dropn(_b_24903, ((int32_t)3), _ctx);
  kk_box_drop(_b_24904, _ctx);
  return kk_std_core_hnd_yield_to_final(_b_24902, kk_std_core_new_try_default_fun26774(value, _ctx), _ctx);
}


// lift anonymous function
struct kk_std_core_try_default_fun26775__t {
  struct kk_function_s _base;
};
static kk_box_t kk_std_core_try_default_fun26775(kk_function_t _fself, kk_box_t _x, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_try_default_fun26775(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_try_default_fun26775, _ctx)
  return kk_function_dup(_fself);
}

static kk_box_t kk_std_core_try_default_fun26775(kk_function_t _fself, kk_box_t _x, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  return _x;
}

kk_box_t kk_std_core_try_default(kk_box_t value, kk_function_t action, kk_context_t* _ctx) { /* forall<a,e> (value : a, action : () -> <exn|e> a) -> e a */ 
  kk_std_core__hnd_exn _x26771;
  kk_std_core_hnd__clause1 _x26772 = kk_std_core_hnd__new_Clause1(kk_std_core_new_try_default_fun26773(value, _ctx), _ctx); /*std/core/hnd/clause1<52,53,54,55,56>*/
  _x26771 = kk_std_core__new_Hnd_exn(kk_reuse_null, _x26772, _ctx); /*.hnd-exn<11,12>*/
  return kk_std_core__handle_exn(((int32_t)0), _x26771, kk_std_core_new_try_default_fun26775(_ctx), action, _ctx);
}
 
// Returns a unique integer (modulo 32-bits).

kk_integer_t kk_std_core_unique(kk_context_t* _ctx) { /* () -> ndet int */ 
  kk_integer_t u;
  kk_box_t _x26776;
  kk_ref_t _x26777 = kk_ref_dup(kk_std_core_unique_count); /*ref<global,int>*/
  _x26776 = kk_ref_get(_x26777,kk_context()); /*179*/
  u = kk_integer_unbox(_x26776); /*int*/
  kk_unit_t __ = kk_Unit;
  kk_integer_t _b_24914_24912;
  kk_integer_t _x26778 = kk_integer_dup(u); /*int*/
  _b_24914_24912 = kk_integer_add(_x26778,(kk_integer_from_small(1)),kk_context()); /*int*/
  kk_ref_t _x26779 = kk_ref_dup(kk_std_core_unique_count); /*ref<global,int>*/
  kk_ref_set(_x26779,(kk_integer_box(_b_24914_24912)),kk_context());
  return u;
}
 
// Get the value of the `Just` constructor or raise an exception

kk_box_t kk_std_core_unjust(kk_std_core_types__maybe m, kk_context_t* _ctx) { /* forall<a> (m : maybe<a>) -> exn a */ 
  if (kk_std_core_types__is_Just(m)) {
    kk_box_t x = m._cons.Just.value;
    return x;
  }
  kk_std_core__exception exn_23489;
  kk_string_t _x26780;
  kk_define_string_literal(, _s26781, 37, "unexpected Nothing in std/core/unjust")
  _x26780 = kk_string_dup(_s26781); /*string*/
  kk_std_core__exception_info _x26782;
  kk_std_core_types__optional _x26783 = kk_std_core_types__new_None(_ctx); /*forall<a> optional<a>*/
  if (kk_std_core_types__is_Optional(_x26783)) {
    kk_box_t _box_x24915 = _x26783._cons.Optional.value;
    kk_std_core__exception_info _info_12803 = kk_std_core__exception_info_unbox(_box_x24915, NULL);
    _x26782 = _info_12803; /*exception-info*/
    goto _match26784;
  }
  _x26782 = kk_std_core__new_ExnError(_ctx); /*exception-info*/
  _match26784: ;
  exn_23489 = kk_std_core__new_Exception(_x26780, _x26782, _ctx); /*exception*/
  kk_std_core_hnd__ev ev_23490;
  size_t _x26786 = ((size_t)0); /*size_t*/
  ev_23490 = kk_evv_at(_x26786,kk_context()); /*std/core/hnd/ev<.hnd-exn>*/
  struct kk_std_core_hnd_Ev* _con26787 = kk_std_core_hnd__as_Ev(ev_23490);
  kk_std_core_hnd__marker m0 = _con26787->marker;
  kk_box_t _box_x24916 = _con26787->hnd;
  kk_std_core__hnd_exn h = kk_std_core__hnd_exn_unbox(_box_x24916, NULL);
  kk_std_core__hnd_exn_dup(h);
  kk_std_core_hnd__clause1 _match_25169;
  struct kk_std_core__Hnd_exn* _con26789 = kk_std_core__as_Hnd_exn(h);
  kk_std_core_hnd__clause1 except_throw_exn = _con26789->except_throw_exn;
  if (kk_std_core__hnd_exn_is_unique(h)) {
    kk_std_core__hnd_exn_free(h);
  }
  else {
    kk_std_core_hnd__clause1_dup(except_throw_exn);
    kk_std_core__hnd_exn_decref(h, _ctx);
  }
  _match_25169 = except_throw_exn; /*forall<a> std/core/hnd/clause1<exception,a,.hnd-exn,3528,3529>*/
  kk_function_t _fun_unbox_x24920 = _match_25169.clause;
  return kk_function_call(kk_box_t, (kk_function_t, kk_std_core_hnd__marker, kk_std_core_hnd__ev, kk_box_t, kk_context_t*), _fun_unbox_x24920, (_fun_unbox_x24920, m0, ev_23490, kk_std_core__exception_box(exn_23489, _ctx), _ctx));
}
 
// Join a list of strings with newlines

kk_string_t kk_std_core_unlines(kk_std_core__list xs, kk_context_t* _ctx) { /* (xs : list<string>) -> string */ 
  if (kk_std_core__is_Nil(xs)) {
    return kk_string_empty();
  }
  struct kk_std_core_Cons* _con26791 = kk_std_core__as_Cons(xs);
  kk_box_t _box_x24927 = _con26791->head;
  kk_std_core__list xx = _con26791->tail;
  kk_string_t x = kk_string_unbox(_box_x24927);
  if (kk_std_core__list_is_unique(xs)) {
    kk_std_core__list_free(xs);
  }
  else {
    kk_string_dup(x);
    kk_std_core__list_dup(xx);
    kk_std_core__list_decref(xs, _ctx);
  }
  kk_string_t _x26793;
  kk_define_string_literal(, _s26794, 1, "\n")
  _x26793 = kk_string_dup(_s26794); /*string*/
  return kk_std_core__lift22731_joinsep(_x26793, xx, x, _ctx);
}
 
// lift

kk_std_core_types__tuple2_ kk_std_core__lift22742_unzip(kk_std_core__list ys, kk_std_core__list acc1, kk_std_core__list acc2, kk_context_t* _ctx) { /* forall<a,b> (ys : list<(a, b)>, acc1 : list<a>, acc2 : list<b>) -> (list<a>, list<b>) */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(ys)) {
    struct kk_std_core_Cons* _con26795 = kk_std_core__as_Cons(ys);
    kk_box_t _box_x24928 = _con26795->head;
    kk_std_core__list xx = _con26795->tail;
    kk_std_core_types__tuple2_ _pat0 = kk_std_core_types__tuple2__unbox(_box_x24928, NULL);
    kk_box_t x = _pat0.fst;
    kk_box_t y = _pat0.snd;
    kk_reuse_t _ru_25148 = kk_reuse_null; /*reuse*/;
    if (kk_std_core__list_is_unique(ys)) {
      kk_box_dup(x);
      kk_box_dup(y);
      kk_box_drop(_box_x24928, _ctx);
      _ru_25148 = (kk_std_core__list_reuse(ys));
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx);
      kk_box_dup(y);
      kk_std_core__list_decref(ys, _ctx);
      _ru_25148 = kk_reuse_null;
    }
    { // tailcall
      kk_std_core__list _x26797 = kk_std_core__new_Cons(_ru_25148, x, acc1, _ctx); /*list<61>*/
      kk_std_core__list _x26798 = kk_std_core__new_Cons(kk_reuse_null, y, acc2, _ctx); /*list<61>*/
      ys = xx;
      acc1 = _x26797;
      acc2 = _x26798;
      goto kk__tailcall;
    }
  }
  kk_std_core__list _b_24931_24929 = kk_std_core_reverse(acc1, _ctx); /*list<22249>*/;
  kk_std_core__list _b_24932_24930 = kk_std_core_reverse(acc2, _ctx); /*list<22250>*/;
  return kk_std_core_types__new_dash__lp__comma__rp_(kk_std_core__list_box(_b_24931_24929, _ctx), kk_std_core__list_box(_b_24932_24930, _ctx), _ctx);
}
 
// Convert a string to a vector of characters.

kk_vector_t kk_std_core_vector_1(kk_string_t s, kk_context_t* _ctx) { /* (s : string) -> vector<char> */ 
  return kk_string_to_chars(s,kk_context());
}
extern kk_box_t kk_std_core_vector_fun26800_2(kk_function_t _fself, size_t __i, kk_context_t* _ctx) {
  struct kk_std_core_vector_fun26800__t_2* _self = kk_function_as(struct kk_std_core_vector_fun26800__t_2*, _fself);
  kk_box_t default0 = _self->default0; /* 22328 */
  kk_drop_match(_self, {kk_box_dup(default0);}, {}, _ctx)
  return default0;
}
extern kk_box_t kk_std_core_vector_init_fun26803(kk_function_t _fself, size_t i, kk_context_t* _ctx) {
  struct kk_std_core_vector_init_fun26803__t* _self = kk_function_as(struct kk_std_core_vector_init_fun26803__t*, _fself);
  kk_function_t f = _self->f; /* (int) -> 22416 */
  kk_drop_match(_self, {kk_function_dup(f);}, {}, _ctx)
  kk_integer_t _x26804 = kk_integer_from_size_t(i,kk_context()); /*int*/
  return kk_function_call(kk_box_t, (kk_function_t, kk_integer_t, kk_context_t*), f, (f, _x26804, _ctx));
}
 
// monadic lift

kk_unit_t kk_std_core__mlift22905_while(kk_function_t action, kk_function_t predicate, kk_unit_t wild__, kk_context_t* _ctx) { /* forall<e> (action : () -> <div|e> (), predicate : () -> <div|e> bool, wild_ : ()) -> <div|e> () */ 
  kk_std_core_while(predicate, action, _ctx); return kk_Unit;
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift22906_while_fun26807__t {
  struct kk_function_s _base;
  kk_function_t action0;
  kk_function_t predicate0;
};
static kk_box_t kk_std_core__mlift22906_while_fun26807(kk_function_t _fself, kk_box_t _b_24934, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift22906_while_fun26807(kk_function_t action0, kk_function_t predicate0, kk_context_t* _ctx) {
  struct kk_std_core__mlift22906_while_fun26807__t* _self = kk_function_alloc_as(struct kk_std_core__mlift22906_while_fun26807__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift22906_while_fun26807, kk_context());
  _self->action0 = action0;
  _self->predicate0 = predicate0;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift22906_while_fun26807(kk_function_t _fself, kk_box_t _b_24934, kk_context_t* _ctx) {
  struct kk_std_core__mlift22906_while_fun26807__t* _self = kk_function_as(struct kk_std_core__mlift22906_while_fun26807__t*, _fself);
  kk_function_t action0 = _self->action0; /* () -> <div|22437> () */
  kk_function_t predicate0 = _self->predicate0; /* () -> <div|22437> bool */
  kk_drop_match(_self, {kk_function_dup(action0);kk_function_dup(predicate0);}, {}, _ctx)
  kk_unit_t _x26808 = kk_Unit;
  kk_unit_t _x26809 = kk_Unit;
  kk_unit_unbox(_b_24934);
  kk_std_core__mlift22905_while(action0, predicate0, _x26809, _ctx);
  return kk_unit_box(_x26808);
}

kk_unit_t kk_std_core__mlift22906_while(kk_function_t action0, kk_function_t predicate0, bool _y_248, kk_context_t* _ctx) { /* forall<e> (action : () -> <div|e> (), predicate : () -> <div|e> bool, bool) -> <div|e> () */ 
  if (_y_248) {
    kk_unit_t x_23495 = kk_Unit;
    kk_function_t _x26805 = kk_function_dup(action0); /*() -> <div|22437> ()*/
    kk_function_call(kk_unit_t, (kk_function_t, kk_context_t*), _x26805, (_x26805, _ctx));
    if (kk_yielding(kk_context())) {
      kk_box_t _x26806 = kk_std_core_hnd_yield_extend(kk_std_core__new_mlift22906_while_fun26807(action0, predicate0, _ctx), _ctx); /*3991*/
      kk_unit_unbox(_x26806); return kk_Unit;
    }
    kk_std_core__mlift22905_while(action0, predicate0, x_23495, _ctx); return kk_Unit;
  }
  kk_function_drop(action0, _ctx);
  kk_function_drop(predicate0, _ctx);
  kk_Unit; return kk_Unit;
}
 
// The `while` fun executes `action`  as long as `pred`  is `true`.


// lift anonymous function
struct kk_std_core_while_fun26812__t {
  struct kk_function_s _base;
  kk_function_t action1;
  kk_function_t predicate1;
};
static kk_box_t kk_std_core_while_fun26812(kk_function_t _fself, kk_box_t _b_24938, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_while_fun26812(kk_function_t action1, kk_function_t predicate1, kk_context_t* _ctx) {
  struct kk_std_core_while_fun26812__t* _self = kk_function_alloc_as(struct kk_std_core_while_fun26812__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_while_fun26812, kk_context());
  _self->action1 = action1;
  _self->predicate1 = predicate1;
  return &_self->_base;
}

static kk_box_t kk_std_core_while_fun26812(kk_function_t _fself, kk_box_t _b_24938, kk_context_t* _ctx) {
  struct kk_std_core_while_fun26812__t* _self = kk_function_as(struct kk_std_core_while_fun26812__t*, _fself);
  kk_function_t action1 = _self->action1; /* () -> <div|22437> () */
  kk_function_t predicate1 = _self->predicate1; /* () -> <div|22437> bool */
  kk_drop_match(_self, {kk_function_dup(action1);kk_function_dup(predicate1);}, {}, _ctx)
  kk_unit_t _x26813 = kk_Unit;
  bool _x26814 = kk_bool_unbox(_b_24938); /*bool*/
  kk_std_core__mlift22906_while(action1, predicate1, _x26814, _ctx);
  return kk_unit_box(_x26813);
}


// lift anonymous function
struct kk_std_core_while_fun26817__t {
  struct kk_function_s _base;
  kk_function_t action1;
  kk_function_t predicate1;
};
static kk_box_t kk_std_core_while_fun26817(kk_function_t _fself, kk_box_t _b_24940, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_while_fun26817(kk_function_t action1, kk_function_t predicate1, kk_context_t* _ctx) {
  struct kk_std_core_while_fun26817__t* _self = kk_function_alloc_as(struct kk_std_core_while_fun26817__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_while_fun26817, kk_context());
  _self->action1 = action1;
  _self->predicate1 = predicate1;
  return &_self->_base;
}

static kk_box_t kk_std_core_while_fun26817(kk_function_t _fself, kk_box_t _b_24940, kk_context_t* _ctx) {
  struct kk_std_core_while_fun26817__t* _self = kk_function_as(struct kk_std_core_while_fun26817__t*, _fself);
  kk_function_t action1 = _self->action1; /* () -> <div|22437> () */
  kk_function_t predicate1 = _self->predicate1; /* () -> <div|22437> bool */
  kk_drop_match(_self, {kk_function_dup(action1);kk_function_dup(predicate1);}, {}, _ctx)
  kk_unit_t _x26818 = kk_Unit;
  kk_unit_t _x26819 = kk_Unit;
  kk_unit_unbox(_b_24940);
  kk_std_core__mlift22905_while(action1, predicate1, _x26819, _ctx);
  return kk_unit_box(_x26818);
}

kk_unit_t kk_std_core_while(kk_function_t predicate1, kk_function_t action1, kk_context_t* _ctx) { /* forall<e> (predicate : () -> <div|e> bool, action : () -> <div|e> ()) -> <div|e> () */ 
  kk__tailcall: ;
  bool x_23497;
  kk_function_t _x26810 = kk_function_dup(predicate1); /*() -> <div|22437> bool*/
  x_23497 = kk_function_call(bool, (kk_function_t, kk_context_t*), _x26810, (_x26810, _ctx)); /*bool*/
  if (kk_yielding(kk_context())) {
    kk_box_t _x26811 = kk_std_core_hnd_yield_extend(kk_std_core_new_while_fun26812(action1, predicate1, _ctx), _ctx); /*3991*/
    kk_unit_unbox(_x26811); return kk_Unit;
  }
  if (x_23497) {
    kk_unit_t x0_23501 = kk_Unit;
    kk_function_t _x26815 = kk_function_dup(action1); /*() -> <div|22437> ()*/
    kk_function_call(kk_unit_t, (kk_function_t, kk_context_t*), _x26815, (_x26815, _ctx));
    if (kk_yielding(kk_context())) {
      kk_box_t _x26816 = kk_std_core_hnd_yield_extend(kk_std_core_new_while_fun26817(action1, predicate1, _ctx), _ctx); /*3991*/
      kk_unit_unbox(_x26816); return kk_Unit;
    }
    { // tailcall
      goto kk__tailcall;
    }
  }
  kk_function_drop(action1, _ctx);
  kk_function_drop(predicate1, _ctx);
  kk_Unit; return kk_Unit;
}
 
// Zip two lists together by pairing the corresponding elements.
// The returned list is only as long as the smallest input list.

kk_std_core__list kk_std_core__ctail_zip(kk_std_core__list xs, kk_std_core__list ys, kk_std_core_types__ctail _acc, kk_context_t* _ctx) { /* forall<a,b> (xs : list<a>, ys : list<b>, ctail<list<(a, b)>>) -> list<(a, b)> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con26820 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con26820->head;
    kk_std_core__list xx = _con26820->tail;
    if (kk_std_core__list_is_unique(xs)) {
      kk_std_core__list_free(xs);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx);
      kk_std_core__list_decref(xs, _ctx);
    }
    if (kk_std_core__is_Cons(ys)) {
      struct kk_std_core_Cons* _con26821 = kk_std_core__as_Cons(ys);
      kk_box_t y = _con26821->head;
      kk_std_core__list yy = _con26821->tail;
      kk_reuse_t _ru_25150 = kk_reuse_null; /*reuse*/;
      if (kk_std_core__list_is_unique(ys)) {
        _ru_25150 = (kk_std_core__list_reuse(ys));
      }
      else {
        kk_box_dup(y);
        kk_std_core__list_dup(yy);
        kk_std_core__list_decref(ys, _ctx);
        _ru_25150 = kk_reuse_null;
      }
      kk_std_core_types__tuple2_ _ctail_22838 = kk_std_core_types__new_dash__lp__comma__rp_(x, y, _ctx); /*(22480, 22481)*/;
      kk_std_core__list _ctail_22839 = kk_std_core__list_hole(); /*list<(22480, 22481)>*/;
      kk_std_core__list _ctail_22840 = kk_std_core__new_Cons(_ru_25150, kk_std_core_types__tuple2__box(_ctail_22838, _ctx), _ctail_22839, _ctx); /*list<(22480, 22481)>*/;
      { // tailcall
        kk_std_core_types__ctail _x26822;
        kk_box_t* field_23507 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_22840)->tail)); /*cfield<list<(22480, 22481)>>*/;
        if (kk_std_core_types__is_CTail(_acc)) {
          kk_box_t _box_x24955 = _acc._cons.CTail._field1;
          kk_box_t* last0 = _acc._cons.CTail._field2;
          kk_std_core__list head0 = kk_std_core__list_unbox(_box_x24955, NULL);
          kk_unit_t __ = kk_Unit;
          *(last0) = kk_std_core__list_box(_ctail_22840, _ctx);
          _x26822 = kk_std_core_types__new_CTail(kk_std_core__list_box(head0, _ctx), field_23507, _ctx); /*ctail<58>*/
          goto _match26823;
        }
        _x26822 = kk_std_core_types__new_CTail(kk_std_core__list_box(_ctail_22840, _ctx), field_23507, _ctx); /*ctail<58>*/
        _match26823: ;
        xs = xx;
        ys = yy;
        _acc = _x26822;
        goto kk__tailcall;
      }
    }
    kk_box_drop(x, _ctx);
    kk_std_core__list_drop(xx, _ctx);
    if (kk_std_core_types__is_CTail(_acc)) {
      kk_box_t _box_x24964 = _acc._cons.CTail._field1;
      kk_box_t* last00 = _acc._cons.CTail._field2;
      kk_std_core__list head00 = kk_std_core__list_unbox(_box_x24964, NULL);
      kk_unit_t __0 = kk_Unit;
      *(last00) = kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx);
      return head00;
    }
    return kk_std_core__new_Nil(_ctx);
  }
  kk_std_core__list_drop(ys, _ctx);
  if (kk_std_core_types__is_CTail(_acc)) {
    kk_box_t _box_x24969 = _acc._cons.CTail._field1;
    kk_box_t* last10 = _acc._cons.CTail._field2;
    kk_std_core__list head1 = kk_std_core__list_unbox(_box_x24969, NULL);
    kk_unit_t __1 = kk_Unit;
    *(last10) = kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx);
    return head1;
  }
  return kk_std_core__new_Nil(_ctx);
}
 
// Zip two lists together by pairing the corresponding elements.
// The returned list is only as long as the smallest input list.

kk_std_core__list kk_std_core_zip(kk_std_core__list xs0, kk_std_core__list ys0, kk_context_t* _ctx) { /* forall<a,b> (xs : list<a>, ys : list<b>) -> list<(a, b)> */ 
  return kk_std_core__ctail_zip(xs0, ys0, kk_std_core_types__new_CTailNil(_ctx), _ctx);
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift22907_op(kk_std_core_types__ctail _acc, kk_function_t f, kk_std_core__list xx, kk_std_core__list yy, kk_box_t _ctail_22841, kk_context_t* _ctx) { /* forall<a,b,c,e> (ctail<list<c>>, f : (a, b) -> e c, xx : list<a>, yy : list<b>, c) -> e list<c> */ 
  kk_std_core__list _ctail_22842 = kk_std_core__list_hole(); /*list<22528>*/;
  kk_std_core__list _ctail_22843 = kk_std_core__new_Cons(kk_reuse_null, _ctail_22841, _ctail_22842, _ctx); /*list<22528>*/;
  kk_std_core_types__ctail _x26827;
  kk_box_t* field_23514 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_22843)->tail)); /*cfield<list<22528>>*/;
  if (kk_std_core_types__is_CTail(_acc)) {
    kk_box_t _box_x24984 = _acc._cons.CTail._field1;
    kk_box_t* last0 = _acc._cons.CTail._field2;
    kk_std_core__list head0 = kk_std_core__list_unbox(_box_x24984, NULL);
    kk_unit_t __ = kk_Unit;
    *(last0) = kk_std_core__list_box(_ctail_22843, _ctx);
    _x26827 = kk_std_core_types__new_CTail(kk_std_core__list_box(head0, _ctx), field_23514, _ctx); /*ctail<58>*/
    goto _match26828;
  }
  _x26827 = kk_std_core_types__new_CTail(kk_std_core__list_box(_ctail_22843, _ctx), field_23514, _ctx); /*ctail<58>*/
  _match26828: ;
  return kk_std_core__ctail_zipwith(xx, yy, f, _x26827, _ctx);
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift22908_op_fun26830__t {
  struct kk_function_s _base;
  kk_function_t _accm;
  kk_box_t _ctail_22846;
};
static kk_std_core__list kk_std_core__mlift22908_op_fun26830(kk_function_t _fself, kk_std_core__list _ctail_22845, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift22908_op_fun26830(kk_function_t _accm, kk_box_t _ctail_22846, kk_context_t* _ctx) {
  struct kk_std_core__mlift22908_op_fun26830__t* _self = kk_function_alloc_as(struct kk_std_core__mlift22908_op_fun26830__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift22908_op_fun26830, kk_context());
  _self->_accm = _accm;
  _self->_ctail_22846 = _ctail_22846;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__mlift22908_op_fun26830(kk_function_t _fself, kk_std_core__list _ctail_22845, kk_context_t* _ctx) {
  struct kk_std_core__mlift22908_op_fun26830__t* _self = kk_function_as(struct kk_std_core__mlift22908_op_fun26830__t*, _fself);
  kk_function_t _accm = _self->_accm; /* (list<22528>) -> list<22528> */
  kk_box_t _ctail_22846 = _self->_ctail_22846; /* 22528 */
  kk_drop_match(_self, {kk_function_dup(_accm);kk_box_dup(_ctail_22846);}, {}, _ctx)
  kk_std_core__list _x26831 = kk_std_core__new_Cons(kk_reuse_null, _ctail_22846, _ctail_22845, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm, (_accm, _x26831, _ctx));
}

kk_std_core__list kk_std_core__mlift22908_op(kk_function_t _accm, kk_function_t f0, kk_std_core__list xx0, kk_std_core__list yy0, kk_box_t _ctail_22846, kk_context_t* _ctx) { /* forall<a,b,c,e> ((list<c>) -> list<c>, f : (a, b) -> e c, xx : list<a>, yy : list<b>, c) -> e list<c> */ 
  return kk_std_core__ctailm_zipwith(xx0, yy0, f0, kk_std_core__new_mlift22908_op_fun26830(_accm, _ctail_22846, _ctx), _ctx);
}
 
// Zip two lists together by apply a function `f` to all corresponding elements.
// The returned list is only as long as the smallest input list.


// lift anonymous function
struct kk_std_core__ctail_zipwith_fun26836__t {
  struct kk_function_s _base;
  kk_function_t f1;
  kk_std_core__list xx1;
  kk_std_core__list yy1;
  kk_std_core_types__ctail _acc0;
};
static kk_box_t kk_std_core__ctail_zipwith_fun26836(kk_function_t _fself, kk_box_t _b_24998, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctail_zipwith_fun26836(kk_function_t f1, kk_std_core__list xx1, kk_std_core__list yy1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) {
  struct kk_std_core__ctail_zipwith_fun26836__t* _self = kk_function_alloc_as(struct kk_std_core__ctail_zipwith_fun26836__t, 6, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctail_zipwith_fun26836, kk_context());
  _self->f1 = f1;
  _self->xx1 = xx1;
  _self->yy1 = yy1;
  _self->_acc0 = _acc0;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctail_zipwith_fun26836(kk_function_t _fself, kk_box_t _b_24998, kk_context_t* _ctx) {
  struct kk_std_core__ctail_zipwith_fun26836__t* _self = kk_function_as(struct kk_std_core__ctail_zipwith_fun26836__t*, _fself);
  kk_function_t f1 = _self->f1; /* (22526, 22527) -> 22529 22528 */
  kk_std_core__list xx1 = _self->xx1; /* list<22526> */
  kk_std_core__list yy1 = _self->yy1; /* list<22527> */
  kk_std_core_types__ctail _acc0 = _self->_acc0; /* ctail<list<22528>> */
  kk_drop_match(_self, {kk_function_dup(f1);kk_std_core__list_dup(xx1);kk_std_core__list_dup(yy1);kk_std_core_types__ctail_dup(_acc0);}, {}, _ctx)
  kk_std_core__list _x26837 = kk_std_core__mlift22907_op(_acc0, f1, xx1, yy1, _b_24998, _ctx); /*list<22528>*/
  return kk_std_core__list_box(_x26837, _ctx);
}

kk_std_core__list kk_std_core__ctail_zipwith(kk_std_core__list xs, kk_std_core__list ys, kk_function_t f1, kk_std_core_types__ctail _acc0, kk_context_t* _ctx) { /* forall<a,b,c,e> (xs : list<a>, ys : list<b>, f : (a, b) -> e c, ctail<list<c>>) -> e list<c> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con26832 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con26832->head;
    kk_std_core__list xx1 = _con26832->tail;
    if (kk_std_core__list_is_unique(xs)) {
      kk_std_core__list_free(xs);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx1);
      kk_std_core__list_decref(xs, _ctx);
    }
    if (kk_std_core__is_Cons(ys)) {
      struct kk_std_core_Cons* _con26833 = kk_std_core__as_Cons(ys);
      kk_box_t y = _con26833->head;
      kk_std_core__list yy1 = _con26833->tail;
      kk_reuse_t _ru_25152 = kk_reuse_null; /*reuse*/;
      if (kk_std_core__list_is_unique(ys)) {
        _ru_25152 = (kk_std_core__list_reuse(ys));
      }
      else {
        kk_box_dup(y);
        kk_std_core__list_dup(yy1);
        kk_std_core__list_decref(ys, _ctx);
        _ru_25152 = kk_reuse_null;
      }
      kk_box_t x0_23515;
      kk_function_t _x26834 = kk_function_dup(f1); /*(22526, 22527) -> 22529 22528*/
      x0_23515 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_context_t*), _x26834, (_x26834, x, y, _ctx)); /*22528*/
      if (kk_yielding(kk_context())) {
        kk_reuse_drop(_ru_25152, _ctx);
        kk_box_drop(x0_23515, _ctx);
        kk_box_t _x26835 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctail_zipwith_fun26836(f1, xx1, yy1, _acc0, _ctx), _ctx); /*3991*/
        return kk_std_core__list_unbox(_x26835, _ctx);
      }
      kk_std_core__list _ctail_228420 = kk_std_core__list_hole(); /*list<22528>*/;
      kk_std_core__list _ctail_228430 = kk_std_core__new_Cons(_ru_25152, x0_23515, _ctail_228420, _ctx); /*list<22528>*/;
      { // tailcall
        kk_std_core_types__ctail _x26838;
        kk_box_t* field_23521 = (kk_box_t*)((&kk_std_core__as_Cons(_ctail_228430)->tail)); /*cfield<list<22528>>*/;
        if (kk_std_core_types__is_CTail(_acc0)) {
          kk_box_t _box_x25005 = _acc0._cons.CTail._field1;
          kk_box_t* last2 = _acc0._cons.CTail._field2;
          kk_std_core__list head1 = kk_std_core__list_unbox(_box_x25005, NULL);
          kk_unit_t __0 = kk_Unit;
          *(last2) = kk_std_core__list_box(_ctail_228430, _ctx);
          _x26838 = kk_std_core_types__new_CTail(kk_std_core__list_box(head1, _ctx), field_23521, _ctx); /*ctail<58>*/
          goto _match26839;
        }
        _x26838 = kk_std_core_types__new_CTail(kk_std_core__list_box(_ctail_228430, _ctx), field_23521, _ctx); /*ctail<58>*/
        _match26839: ;
        xs = xx1;
        ys = yy1;
        _acc0 = _x26838;
        goto kk__tailcall;
      }
    }
    kk_function_drop(f1, _ctx);
    kk_box_drop(x, _ctx);
    kk_std_core__list_drop(xx1, _ctx);
    if (kk_std_core_types__is_CTail(_acc0)) {
      kk_box_t _box_x25014 = _acc0._cons.CTail._field1;
      kk_box_t* last00 = _acc0._cons.CTail._field2;
      kk_std_core__list head00 = kk_std_core__list_unbox(_box_x25014, NULL);
      kk_unit_t __00 = kk_Unit;
      *(last00) = kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx);
      return head00;
    }
    return kk_std_core__new_Nil(_ctx);
  }
  kk_function_drop(f1, _ctx);
  kk_std_core__list_drop(ys, _ctx);
  if (kk_std_core_types__is_CTail(_acc0)) {
    kk_box_t _box_x25019 = _acc0._cons.CTail._field1;
    kk_box_t* last10 = _acc0._cons.CTail._field2;
    kk_std_core__list head10 = kk_std_core__list_unbox(_box_x25019, NULL);
    kk_unit_t __1 = kk_Unit;
    *(last10) = kk_std_core__list_box(kk_std_core__new_Nil(_ctx), _ctx);
    return head10;
  }
  return kk_std_core__new_Nil(_ctx);
}
 
// Zip two lists together by apply a function `f` to all corresponding elements.
// The returned list is only as long as the smallest input list.


// lift anonymous function
struct kk_std_core__ctailm_zipwith_fun26847__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_function_t f2;
  kk_std_core__list xx2;
  kk_std_core__list yy2;
};
static kk_box_t kk_std_core__ctailm_zipwith_fun26847(kk_function_t _fself, kk_box_t _b_25031, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_zipwith_fun26847(kk_function_t _accm0, kk_function_t f2, kk_std_core__list xx2, kk_std_core__list yy2, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_zipwith_fun26847__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_zipwith_fun26847__t, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_zipwith_fun26847, kk_context());
  _self->_accm0 = _accm0;
  _self->f2 = f2;
  _self->xx2 = xx2;
  _self->yy2 = yy2;
  return &_self->_base;
}

static kk_box_t kk_std_core__ctailm_zipwith_fun26847(kk_function_t _fself, kk_box_t _b_25031, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_zipwith_fun26847__t* _self = kk_function_as(struct kk_std_core__ctailm_zipwith_fun26847__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<22528>) -> list<22528> */
  kk_function_t f2 = _self->f2; /* (22526, 22527) -> 22529 22528 */
  kk_std_core__list xx2 = _self->xx2; /* list<22526> */
  kk_std_core__list yy2 = _self->yy2; /* list<22527> */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_function_dup(f2);kk_std_core__list_dup(xx2);kk_std_core__list_dup(yy2);}, {}, _ctx)
  kk_std_core__list _x26848 = kk_std_core__mlift22908_op(_accm0, f2, xx2, yy2, _b_25031, _ctx); /*list<22528>*/
  return kk_std_core__list_box(_x26848, _ctx);
}


// lift anonymous function
struct kk_std_core__ctailm_zipwith_fun26850__t {
  struct kk_function_s _base;
  kk_function_t _accm0;
  kk_box_t x0_23526;
};
static kk_std_core__list kk_std_core__ctailm_zipwith_fun26850(kk_function_t _fself, kk_std_core__list _ctail_228450, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_ctailm_zipwith_fun26850(kk_function_t _accm0, kk_box_t x0_23526, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_zipwith_fun26850__t* _self = kk_function_alloc_as(struct kk_std_core__ctailm_zipwith_fun26850__t, 3, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__ctailm_zipwith_fun26850, kk_context());
  _self->_accm0 = _accm0;
  _self->x0_23526 = x0_23526;
  return &_self->_base;
}

static kk_std_core__list kk_std_core__ctailm_zipwith_fun26850(kk_function_t _fself, kk_std_core__list _ctail_228450, kk_context_t* _ctx) {
  struct kk_std_core__ctailm_zipwith_fun26850__t* _self = kk_function_as(struct kk_std_core__ctailm_zipwith_fun26850__t*, _fself);
  kk_function_t _accm0 = _self->_accm0; /* (list<22528>) -> list<22528> */
  kk_box_t x0_23526 = _self->x0_23526; /* 22528 */
  kk_drop_match(_self, {kk_function_dup(_accm0);kk_box_dup(x0_23526);}, {}, _ctx)
  kk_std_core__list _x26851 = kk_std_core__new_Cons(kk_reuse_null, x0_23526, _ctail_228450, _ctx); /*list<61>*/
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, _x26851, _ctx));
}

kk_std_core__list kk_std_core__ctailm_zipwith(kk_std_core__list xs0, kk_std_core__list ys0, kk_function_t f2, kk_function_t _accm0, kk_context_t* _ctx) { /* forall<a,b,c,e> (xs : list<a>, ys : list<b>, f : (a, b) -> e c, (list<c>) -> list<c>) -> e list<c> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Cons(xs0)) {
    struct kk_std_core_Cons* _con26843 = kk_std_core__as_Cons(xs0);
    kk_box_t x0 = _con26843->head;
    kk_std_core__list xx2 = _con26843->tail;
    if (kk_std_core__list_is_unique(xs0)) {
      kk_std_core__list_free(xs0);
    }
    else {
      kk_box_dup(x0);
      kk_std_core__list_dup(xx2);
      kk_std_core__list_decref(xs0, _ctx);
    }
    if (kk_std_core__is_Cons(ys0)) {
      struct kk_std_core_Cons* _con26844 = kk_std_core__as_Cons(ys0);
      kk_box_t y0 = _con26844->head;
      kk_std_core__list yy2 = _con26844->tail;
      if (kk_std_core__list_is_unique(ys0)) {
        kk_std_core__list_free(ys0);
      }
      else {
        kk_box_dup(y0);
        kk_std_core__list_dup(yy2);
        kk_std_core__list_decref(ys0, _ctx);
      }
      kk_box_t x0_23526;
      kk_function_t _x26845 = kk_function_dup(f2); /*(22526, 22527) -> 22529 22528*/
      x0_23526 = kk_function_call(kk_box_t, (kk_function_t, kk_box_t, kk_box_t, kk_context_t*), _x26845, (_x26845, x0, y0, _ctx)); /*22528*/
      if (kk_yielding(kk_context())) {
        kk_box_drop(x0_23526, _ctx);
        kk_box_t _x26846 = kk_std_core_hnd_yield_extend(kk_std_core__new_ctailm_zipwith_fun26847(_accm0, f2, xx2, yy2, _ctx), _ctx); /*3991*/
        return kk_std_core__list_unbox(_x26846, _ctx);
      }
      { // tailcall
        kk_function_t _x26849 = kk_std_core__new_ctailm_zipwith_fun26850(_accm0, x0_23526, _ctx); /*(list<22528>) -> list<22528>*/
        xs0 = xx2;
        ys0 = yy2;
        _accm0 = _x26849;
        goto kk__tailcall;
      }
    }
    kk_function_drop(f2, _ctx);
    kk_box_drop(x0, _ctx);
    kk_std_core__list_drop(xx2, _ctx);
    return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, kk_std_core__new_Nil(_ctx), _ctx));
  }
  kk_function_drop(f2, _ctx);
  kk_std_core__list_drop(ys0, _ctx);
  return kk_function_call(kk_std_core__list, (kk_function_t, kk_std_core__list, kk_context_t*), _accm0, (_accm0, kk_std_core__new_Nil(_ctx), _ctx));
}
 
// Zip two lists together by apply a function `f` to all corresponding elements.
// The returned list is only as long as the smallest input list.


// lift anonymous function
struct kk_std_core_zipwith_fun26852__t {
  struct kk_function_s _base;
};
static kk_std_core__list kk_std_core_zipwith_fun26852(kk_function_t _fself, kk_std_core__list _ctail_22844, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_zipwith_fun26852(kk_context_t* _ctx) {
  kk_define_static_function(_fself, kk_std_core_zipwith_fun26852, _ctx)
  return kk_function_dup(_fself);
}

static kk_std_core__list kk_std_core_zipwith_fun26852(kk_function_t _fself, kk_std_core__list _ctail_22844, kk_context_t* _ctx) {
  KK_UNUSED(_fself);
  return _ctail_22844;
}

kk_std_core__list kk_std_core_zipwith(kk_std_core__list xs1, kk_std_core__list ys1, kk_function_t f3, kk_context_t* _ctx) { /* forall<a,b,c,e> (xs : list<a>, ys : list<b>, f : (a, b) -> e c) -> e list<c> */ 
  bool _match_25163 = kk_std_core_hnd__evv_is_affine(_ctx); /*bool*/;
  if (_match_25163) {
    return kk_std_core__ctail_zipwith(xs1, ys1, f3, kk_std_core_types__new_CTailNil(_ctx), _ctx);
  }
  return kk_std_core__ctailm_zipwith(xs1, ys1, f3, kk_std_core_new_zipwith_fun26852(_ctx), _ctx);
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift22909_zipwith_acc(kk_std_core__list acc, kk_function_t f, kk_integer_t i, kk_std_core__list xx, kk_std_core__list yy, kk_box_t _y_263, kk_context_t* _ctx) { /* forall<e,a,b,c> (acc : list<b>, f : (int, a, c) -> e b, i : int, xx : list<a>, yy : list<c>, b) -> e list<b> */ 
  kk_integer_t _x26853 = kk_integer_add(i,(kk_integer_from_small(1)),kk_context()); /*int*/
  kk_std_core__list _x26854 = kk_std_core__new_Cons(kk_reuse_null, _y_263, acc, _ctx); /*list<61>*/
  return kk_std_core_zipwith_acc(f, _x26853, _x26854, xx, yy, _ctx);
}


// lift anonymous function
struct kk_std_core_zipwith_acc_fun26860__t {
  struct kk_function_s _base;
  kk_std_core__list acc0;
  kk_function_t f0;
  kk_integer_t i0;
  kk_std_core__list xx0;
  kk_std_core__list yy0;
};
static kk_box_t kk_std_core_zipwith_acc_fun26860(kk_function_t _fself, kk_box_t _b_25035, kk_context_t* _ctx);
static kk_function_t kk_std_core_new_zipwith_acc_fun26860(kk_std_core__list acc0, kk_function_t f0, kk_integer_t i0, kk_std_core__list xx0, kk_std_core__list yy0, kk_context_t* _ctx) {
  struct kk_std_core_zipwith_acc_fun26860__t* _self = kk_function_alloc_as(struct kk_std_core_zipwith_acc_fun26860__t, 6, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core_zipwith_acc_fun26860, kk_context());
  _self->acc0 = acc0;
  _self->f0 = f0;
  _self->i0 = i0;
  _self->xx0 = xx0;
  _self->yy0 = yy0;
  return &_self->_base;
}

static kk_box_t kk_std_core_zipwith_acc_fun26860(kk_function_t _fself, kk_box_t _b_25035, kk_context_t* _ctx) {
  struct kk_std_core_zipwith_acc_fun26860__t* _self = kk_function_as(struct kk_std_core_zipwith_acc_fun26860__t*, _fself);
  kk_std_core__list acc0 = _self->acc0; /* list<22541> */
  kk_function_t f0 = _self->f0; /* (int, 22539, 22549) -> 22536 22541 */
  kk_integer_t i0 = _self->i0; /* int */
  kk_std_core__list xx0 = _self->xx0; /* list<22539> */
  kk_std_core__list yy0 = _self->yy0; /* list<22549> */
  kk_drop_match(_self, {kk_std_core__list_dup(acc0);kk_function_dup(f0);kk_integer_dup(i0);kk_std_core__list_dup(xx0);kk_std_core__list_dup(yy0);}, {}, _ctx)
  kk_std_core__list _x26861 = kk_std_core__mlift22909_zipwith_acc(acc0, f0, i0, xx0, yy0, _b_25035, _ctx); /*list<22541>*/
  return kk_std_core__list_box(_x26861, _ctx);
}

kk_std_core__list kk_std_core_zipwith_acc(kk_function_t f0, kk_integer_t i0, kk_std_core__list acc0, kk_std_core__list xs, kk_std_core__list ys, kk_context_t* _ctx) { /* forall<a,b,c,e> ((int, a, b) -> e c, int, list<c>, list<a>, list<b>) -> e list<c> */ 
  kk__tailcall: ;
  if (kk_std_core__is_Nil(xs)) {
    kk_function_drop(f0, _ctx);
    kk_integer_drop(i0, _ctx);
    kk_std_core__list_drop(ys, _ctx);
    return kk_std_core_reverse(acc0, _ctx);
  }
  struct kk_std_core_Cons* _con26855 = kk_std_core__as_Cons(xs);
  kk_box_t x = _con26855->head;
  kk_std_core__list xx0 = _con26855->tail;
  if (kk_std_core__list_is_unique(xs)) {
    kk_std_core__list_free(xs);
  }
  else {
    kk_box_dup(x);
    kk_std_core__list_dup(xx0);
    kk_std_core__list_decref(xs, _ctx);
  }
  if (kk_std_core__is_Nil(ys)) {
    kk_function_drop(f0, _ctx);
    kk_integer_drop(i0, _ctx);
    kk_box_drop(x, _ctx);
    kk_std_core__list_drop(xx0, _ctx);
    return kk_std_core_reverse(acc0, _ctx);
  }
  struct kk_std_core_Cons* _con26856 = kk_std_core__as_Cons(ys);
  kk_box_t y = _con26856->head;
  kk_std_core__list yy0 = _con26856->tail;
  kk_reuse_t _ru_25156 = kk_reuse_null; /*reuse*/;
  if (kk_std_core__list_is_unique(ys)) {
    _ru_25156 = (kk_std_core__list_reuse(ys));
  }
  else {
    kk_box_dup(y);
    kk_std_core__list_dup(yy0);
    kk_std_core__list_decref(ys, _ctx);
    _ru_25156 = kk_reuse_null;
  }
  kk_box_t x0_23530;
  kk_function_t _x26858 = kk_function_dup(f0); /*(int, 22539, 22549) -> 22536 22541*/
  kk_integer_t _x26857 = kk_integer_dup(i0); /*int*/
  x0_23530 = kk_function_call(kk_box_t, (kk_function_t, kk_integer_t, kk_box_t, kk_box_t, kk_context_t*), _x26858, (_x26858, _x26857, x, y, _ctx)); /*22541*/
  if (kk_yielding(kk_context())) {
    kk_reuse_drop(_ru_25156, _ctx);
    kk_box_drop(x0_23530, _ctx);
    kk_box_t _x26859 = kk_std_core_hnd_yield_extend(kk_std_core_new_zipwith_acc_fun26860(acc0, f0, i0, xx0, yy0, _ctx), _ctx); /*3991*/
    return kk_std_core__list_unbox(_x26859, _ctx);
  }
  { // tailcall
    kk_integer_t _x26862 = kk_integer_add(i0,(kk_integer_from_small(1)),kk_context()); /*int*/
    kk_std_core__list _x26863 = kk_std_core__new_Cons(_ru_25156, x0_23530, acc0, _ctx); /*list<61>*/
    i0 = _x26862;
    acc0 = _x26863;
    xs = xx0;
    ys = yy0;
    goto kk__tailcall;
  }
}
 
// monadic lift

kk_std_core__list kk_std_core__mlift22910_op(kk_box_t _y_267, kk_std_core__list _y_268, kk_context_t* _ctx) { /* forall<a,e> (a, list<a>) -> e list<a> */ 
  return kk_std_core__new_Cons(kk_reuse_null, _y_267, _y_268, _ctx);
}
 
// monadic lift


// lift anonymous function
struct kk_std_core__mlift22911_op_fun26865__t {
  struct kk_function_s _base;
  kk_box_t _y_2670;
};
static kk_box_t kk_std_core__mlift22911_op_fun26865(kk_function_t _fself, kk_box_t _b_25039, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_mlift22911_op_fun26865(kk_box_t _y_2670, kk_context_t* _ctx) {
  struct kk_std_core__mlift22911_op_fun26865__t* _self = kk_function_alloc_as(struct kk_std_core__mlift22911_op_fun26865__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__mlift22911_op_fun26865, kk_context());
  _self->_y_2670 = _y_2670;
  return &_self->_base;
}

static kk_box_t kk_std_core__mlift22911_op_fun26865(kk_function_t _fself, kk_box_t _b_25039, kk_context_t* _ctx) {
  struct kk_std_core__mlift22911_op_fun26865__t* _self = kk_function_as(struct kk_std_core__mlift22911_op_fun26865__t*, _fself);
  kk_box_t _y_2670 = _self->_y_2670; /* 22627 */
  kk_drop_match(_self, {kk_box_dup(_y_2670);}, {}, _ctx)
  kk_std_core__list _x26866;
  kk_std_core__list _x26867 = kk_std_core__list_unbox(_b_25039, _ctx); /*list<22627>*/
  _x26866 = kk_std_core__mlift22910_op(_y_2670, _x26867, _ctx); /*list<22627>*/
  return kk_std_core__list_box(_x26866, _ctx);
}

kk_std_core__list kk_std_core__mlift22911_op(kk_function_t f, kk_integer_t i, kk_std_core__list xx, kk_std_core__list yy, kk_box_t _y_2670, kk_context_t* _ctx) { /* forall<a,b,c,e> (f : (int, a, b) -> e c, i : int, xx : list<a>, yy : list<b>, c) -> e list<c> */ 
  kk_integer_t i0_22782 = kk_integer_add(i,(kk_integer_from_small(1)),kk_context()); /*int*/;
  kk_std_core__list x_23534 = kk_std_core__lift22743_zipwith_indexed(f, i0_22782, xx, yy, _ctx); /*list<22627>*/;
  if (kk_yielding(kk_context())) {
    kk_std_core__list_drop(x_23534, _ctx);
    kk_box_t _x26864 = kk_std_core_hnd_yield_extend(kk_std_core__new_mlift22911_op_fun26865(_y_2670, _ctx), _ctx); /*3991*/
    return kk_std_core__list_unbox(_x26864, _ctx);
  }
  return kk_std_core__mlift22910_op(_y_2670, x_23534, _ctx);
}
 
// lift


// lift anonymous function
struct kk_std_core__lift22743_zipwith_indexed_fun26873__t {
  struct kk_function_s _base;
  kk_function_t f0;
  kk_integer_t i0;
  kk_std_core__list xx0;
  kk_std_core__list yy0;
};
static kk_box_t kk_std_core__lift22743_zipwith_indexed_fun26873(kk_function_t _fself, kk_box_t _b_25043, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift22743_zipwith_indexed_fun26873(kk_function_t f0, kk_integer_t i0, kk_std_core__list xx0, kk_std_core__list yy0, kk_context_t* _ctx) {
  struct kk_std_core__lift22743_zipwith_indexed_fun26873__t* _self = kk_function_alloc_as(struct kk_std_core__lift22743_zipwith_indexed_fun26873__t, 5, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift22743_zipwith_indexed_fun26873, kk_context());
  _self->f0 = f0;
  _self->i0 = i0;
  _self->xx0 = xx0;
  _self->yy0 = yy0;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift22743_zipwith_indexed_fun26873(kk_function_t _fself, kk_box_t _b_25043, kk_context_t* _ctx) {
  struct kk_std_core__lift22743_zipwith_indexed_fun26873__t* _self = kk_function_as(struct kk_std_core__lift22743_zipwith_indexed_fun26873__t*, _fself);
  kk_function_t f0 = _self->f0; /* (int, 22625, 22626) -> 22628 22627 */
  kk_integer_t i0 = _self->i0; /* int */
  kk_std_core__list xx0 = _self->xx0; /* list<22625> */
  kk_std_core__list yy0 = _self->yy0; /* list<22626> */
  kk_drop_match(_self, {kk_function_dup(f0);kk_integer_dup(i0);kk_std_core__list_dup(xx0);kk_std_core__list_dup(yy0);}, {}, _ctx)
  kk_std_core__list _x26874 = kk_std_core__mlift22911_op(f0, i0, xx0, yy0, _b_25043, _ctx); /*list<22627>*/
  return kk_std_core__list_box(_x26874, _ctx);
}


// lift anonymous function
struct kk_std_core__lift22743_zipwith_indexed_fun26876__t {
  struct kk_function_s _base;
  kk_box_t x0_23536;
};
static kk_box_t kk_std_core__lift22743_zipwith_indexed_fun26876(kk_function_t _fself, kk_box_t _b_25045, kk_context_t* _ctx);
static kk_function_t kk_std_core__new_lift22743_zipwith_indexed_fun26876(kk_box_t x0_23536, kk_context_t* _ctx) {
  struct kk_std_core__lift22743_zipwith_indexed_fun26876__t* _self = kk_function_alloc_as(struct kk_std_core__lift22743_zipwith_indexed_fun26876__t, 2, _ctx);
  _self->_base.fun = kk_cfun_ptr_box(&kk_std_core__lift22743_zipwith_indexed_fun26876, kk_context());
  _self->x0_23536 = x0_23536;
  return &_self->_base;
}

static kk_box_t kk_std_core__lift22743_zipwith_indexed_fun26876(kk_function_t _fself, kk_box_t _b_25045, kk_context_t* _ctx) {
  struct kk_std_core__lift22743_zipwith_indexed_fun26876__t* _self = kk_function_as(struct kk_std_core__lift22743_zipwith_indexed_fun26876__t*, _fself);
  kk_box_t x0_23536 = _self->x0_23536; /* 22627 */
  kk_drop_match(_self, {kk_box_dup(x0_23536);}, {}, _ctx)
  kk_std_core__list _x26877;
  kk_std_core__list _x26878 = kk_std_core__list_unbox(_b_25045, _ctx); /*list<22627>*/
  _x26877 = kk_std_core__mlift22910_op(x0_23536, _x26878, _ctx); /*list<22627>*/
  return kk_std_core__list_box(_x26877, _ctx);
}

kk_std_core__list kk_std_core__lift22743_zipwith_indexed(kk_function_t f0, kk_integer_t i0, kk_std_core__list xs, kk_std_core__list ys, kk_context_t* _ctx) { /* forall<a,b,c,e> (f : (int, a, b) -> e c, i : int, xs : list<a>, ys : list<b>) -> e list<c> */ 
  if (kk_std_core__is_Cons(xs)) {
    struct kk_std_core_Cons* _con26868 = kk_std_core__as_Cons(xs);
    kk_box_t x = _con26868->head;
    kk_std_core__list xx0 = _con26868->tail;
    if (kk_std_core__list_is_unique(xs)) {
      kk_std_core__list_free(xs);
    }
    else {
      kk_box_dup(x);
      kk_std_core__list_dup(xx0);
      kk_std_core__list_decref(xs, _ctx);
    }
    if (kk_std_core__is_Cons(ys)) {
      struct kk_std_core_Cons* _con26869 = kk_std_core__as_Cons(ys);
      kk_box_t y = _con26869->head;
      kk_std_core__list yy0 = _con26869->tail;
      kk_reuse_t _ru_25158 = kk_reuse_null; /*reuse*/;
      if (kk_std_core__list_is_unique(ys)) {
        _ru_25158 = (kk_std_core__list_reuse(ys));
      }
      else {
        kk_box_dup(y);
        kk_std_core__list_dup(yy0);
        kk_std_core__list_decref(ys, _ctx);
        _ru_25158 = kk_reuse_null;
      }
      kk_box_t x0_23536;
      kk_function_t _x26871 = kk_function_dup(f0); /*(int, 22625, 22626) -> 22628 22627*/
      kk_integer_t _x26870 = kk_integer_dup(i0); /*int*/
      x0_23536 = kk_function_call(kk_box_t, (kk_function_t, kk_integer_t, kk_box_t, kk_box_t, kk_context_t*), _x26871, (_x26871, _x26870, x, y, _ctx)); /*22627*/
      if (kk_yielding(kk_context())) {
        kk_reuse_drop(_ru_25158, _ctx);
        kk_box_drop(x0_23536, _ctx);
        kk_box_t _x26872 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift22743_zipwith_indexed_fun26873(f0, i0, xx0, yy0, _ctx), _ctx); /*3991*/
        return kk_std_core__list_unbox(_x26872, _ctx);
      }
      kk_integer_t i0_227820 = kk_integer_add(i0,(kk_integer_from_small(1)),kk_context()); /*int*/;
      kk_std_core__list x1_23540 = kk_std_core__lift22743_zipwith_indexed(f0, i0_227820, xx0, yy0, _ctx); /*list<22627>*/;
      if (kk_yielding(kk_context())) {
        kk_reuse_drop(_ru_25158, _ctx);
        kk_std_core__list_drop(x1_23540, _ctx);
        kk_box_t _x26875 = kk_std_core_hnd_yield_extend(kk_std_core__new_lift22743_zipwith_indexed_fun26876(x0_23536, _ctx), _ctx); /*3991*/
        return kk_std_core__list_unbox(_x26875, _ctx);
      }
      return kk_std_core__new_Cons(_ru_25158, x0_23536, x1_23540, _ctx);
    }
    kk_function_drop(f0, _ctx);
    kk_integer_drop(i0, _ctx);
    kk_box_drop(x, _ctx);
    kk_std_core__list_drop(xx0, _ctx);
    return kk_std_core__new_Nil(_ctx);
  }
  kk_function_drop(f0, _ctx);
  kk_integer_drop(i0, _ctx);
  kk_std_core__list_drop(ys, _ctx);
  return kk_std_core__new_Nil(_ctx);
}

// initialization
void kk_std_core__init(kk_context_t* _ctx){
  static bool _initialized = false;
  if (_initialized) return;
  _initialized = true;
  kk_std_core_types__init(_ctx);
  kk_std_core_hnd__init(_ctx);
  {
    kk_string_t _x25426;
    kk_define_string_literal(, _s25427, 8, "exn.core")
    _x25426 = kk_string_dup(_s25427); /*string*/
    kk_std_core__tag_exn = kk_std_core_hnd__new_Htag(_x25426, _ctx); /*std/core/hnd/htag<.hnd-exn>*/
  }
  {
    kk_std_core_redirect = kk_ref_alloc((kk_std_core_types__maybe_box(kk_std_core_types__new_Nothing(_ctx), _ctx)),kk_context()); /*ref<global,maybe<(string) -> console ()>>*/
  }
  {
    kk_std_core_maxListStack = kk_integer_from_small(50); /*int*/
  }
  {
    kk_std_core_trace_enabled = kk_ref_alloc((kk_bool_box(true)),kk_context()); /*ref<global,bool>*/
  }
  {
    kk_std_core_unique_count = kk_ref_alloc((kk_integer_box(kk_integer_from_small(0))),kk_context()); /*ref<global,int>*/
  }
  {
    kk_string_t _x26226 = kk_string_empty(); /*string*/
    size_t _x26228 = ((size_t)0); /*size_t*/
    size_t _x26229 = ((size_t)0); /*size_t*/
    kk_std_core_empty = kk_std_core__new_Sslice(_x26226, _x26228, _x26229, _ctx); /*sslice*/
  }
  {
    kk_std_core_null_const = kk_std_core_null(kk_std_core_types__new_Nothing(_ctx), _ctx); /*forall<a> null<a>*/
  }
}
